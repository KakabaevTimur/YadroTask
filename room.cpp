#include "room.hpp"

Room::Room(std::filesystem::path inFilePath)
{
    this->inFile.open(inFilePath);

    if (!this->inFile.is_open())
    {
        logStatistics = false;
        throw std::runtime_error{"Failed to open file"};
    }

    std::size_t countTables;

    const std::regex countTablesRegex{"^[1-9]\\d*$"};
    auto matched = parse(countTablesRegex);
    countTables = std::stoull(matched[0]);

    this->tables.resize(countTables);

    const std::regex timesRegex{"^([0-1]\\d|2[0-3]):([0-5]\\d) ([0-1]\\d|2[0-3]):([0-5]\\d)$"};  // for line like "09:00 19:00"
    matched = parse(timesRegex);
    this->startTime = std::stoll(matched[1]) * 60 + std::stoll(matched[2]);
    this->endTime = std::stoll(matched[3]) * 60 + std::stoll(matched[4]);

    if (this->startTime > this->endTime)
    {
        // Print last wrong line here if start time is greater then end time
        std::cout << formatTime(startTime) << ' ' << formatTime(endTime) << std::endl;
        logStatistics = false;
        throw std::runtime_error{""};
    }

    std::cout << formatTime(this->startTime) << std::endl;

    const std::regex& costRegex = countTablesRegex;
    matched = parse(costRegex);
    this->cost = std::stoull(matched[0]);
}

Room::~Room()
{
    if (logStatistics)
    {
        for (auto& tableSessions: tables)
        {
            if (tableSessions.size() > 0 && tableSessions.back().endTime == 0)
            {
                tableSessions.back().endTime = endTime;
                clientsQueue.push_back(tableSessions.back().client);
            }
        }

        std::sort(clientsQueue.begin(), clientsQueue.end());

        for (const auto& client: clientsQueue)
        {
            logEvent(endTime, client, 11);
        }

        std::cout << formatTime(endTime) << std::endl;

        for (size_t tableId = 0; tableId < tables.size(); tableId++)
        {
            size_t totalTableTime{};
            size_t tableSum{};

            for (const auto& session: tables[tableId])
            {
                const size_t sessionTime = session.endTime - session.startTime;
                totalTableTime += sessionTime;
                tableSum += std::ceil(static_cast<double>(sessionTime) / 60) * cost;
            }

            std::cout << std::format("{} {} {}\n", tableId + 1, tableSum, formatTime(totalTableTime));
        }
    }
}

std::string Room::formatTime(std::size_t time)
{
    return std::format("{:0>2}:{:0>2}", time / 60, time % 60);
}

void Room::logEvent(std::size_t time, std::string message, std::size_t eventId = 13)
{
    std::cout << std::format("{} {} {}\n", formatTime(time), eventId, message);
}

std::smatch Room::parse(const std::regex& regex)
{
    static std::string line;
    if (!std::getline(inFile, line))
    {
        // For last line of input return empty match
        return {};
    }
    std::smatch match;
    if (!std::regex_search(line, match, regex))
    {
        std::cout << line;
        logStatistics = false;
        throw std::format_error{"Failed to parse with provided regex"};
    }
    return match;
}

void Room::clientArrive(std::size_t time, const std::string& client)
{
    if (time < startTime || time > endTime)
    {
        logEvent(time, "NotOpenYet");
        return;
    }

    const bool clientInClub = clients.contains(client);
    const bool clientInQueue = std::find(clientsQueue.begin(), clientsQueue.end(), client) != clientsQueue.end();
    const bool clientAtTheTable = std::find_if(tables.begin(), tables.end(), [client](const auto& tableSessions)
    {
        return tableSessions.size() > 0 && tableSessions.back().client == client && tableSessions.back().endTime == 0;
    }) != tables.end();

    if (clientInClub || clientInQueue || clientAtTheTable)
    {
        logEvent(time, "YouShallNotPass");
        return;
    }

    clients.emplace(client);
}

void Room::clientSat(std::size_t time, const std::string& client, std::size_t tableId)
{
    tableId--; // fixing index of table

    if (tableId > tables.size())
    {
        logStatistics = false;
        throw std::runtime_error{""};
    }

    const auto clientQueueIter = std::find(clientsQueue.begin(), clientsQueue.end(), client);
    const bool clientInQueue = clientQueueIter != clientsQueue.end();
    const bool clientInClub = clients.contains(client);

    const auto tableIterator = std::find_if(tables.begin(), tables.end(), [client](const auto& tableSessions)
    {
        return tableSessions.size() > 0 && tableSessions.back().client == client && tableSessions.back().endTime == 0;
    });

    const bool clientIsSitting = (tableIterator != tables.end());

    if (!clientInClub && !clientInQueue && !clientIsSitting)
    {
        logEvent(time, "ClientUnknown");
        return;
    }

    if (tables[tableId].size() > 0 && tables[tableId].back().endTime == 0)
    {
        logEvent(time, "PlaceIsBusy");
        return;
    }

    // Close session if client changes table
    if (clientIsSitting)
    {
        tableIterator->back().endTime = time;
        if (clientsQueue.size() > 0)
        {
            tableIterator->push_back(Session{clientsQueue.front(), time, UnfinishedSessionEndTime});
            clientsQueue.pop_front();
            const std::size_t freedTableId = std::distance(tables.begin(), tableIterator);
            logEvent(time, std::format("{} {}", client, freedTableId + 1), 12);
        }
    }
    else if (clientInClub)
    {
        clients.erase(client);
    }
    else if (clientInQueue)
    {
        clientsQueue.erase(clientQueueIter);
    }

    tables[tableId].push_back(Session{client, time, UnfinishedSessionEndTime});
}

void Room::clientWait(std::size_t time, const std::string& client)
{
    const bool queueIsOverflowed = clientsQueue.size() == tables.size();
    if (queueIsOverflowed)
    {
        logEvent(time, client, 11);  // client leaves
    }
    else
    {
        clientsQueue.push_back(client);
    }

    clients.erase(client);

    const bool hasFreeTable = std::any_of(tables.begin(), tables.end(), [](const auto& tableSessions)
    {
        return tableSessions.size() == 0 || tableSessions.back().endTime != 0;
    });

    if (hasFreeTable)
    {
        logEvent(time, "ICanWaitNoLonger!");
    }
}

void Room::clientLeft(std::size_t time, const std::string& client)
{
    const auto clientIterator = std::find(clientsQueue.begin(), clientsQueue.end(), client);

    const auto tableIterator = std::find_if(tables.begin(), tables.end(), [client](const auto& tableSessions)
    {
        return tableSessions.size() > 0 && tableSessions.back().client == client;
    });

    const bool clientIsInClub = clients.contains(client);
    const bool clientIsInQueue = (clientIterator != clientsQueue.end());
    const bool clientIsSitting = (tableIterator != tables.end());

    if (!clientIsInClub && !clientIsInQueue && !clientIsSitting)
    {
        logEvent(time, "ClientUnknown");
        return;
    }

    if (clientIsSitting)
    {
        tableIterator->back().endTime = time;

        if (clientsQueue.size() > 0)
        {
            tableIterator->push_back(Session{clientsQueue.front(), time, UnfinishedSessionEndTime});
            clientsQueue.pop_front();
            const std::size_t freedTableId = std::distance(tables.begin(), tableIterator);
            logEvent(time, std::format("{} {}", client, freedTableId + 1), 12);
        }
    }
    else if (clientIsInQueue)
    {
        clientsQueue.erase(clientIterator);
    }
    else if (clientIsInClub)
    {
        clients.erase(client);
    }
}

void Room::start()
{
    std::size_t time{};
    std::size_t eventId{};
    std::string clientName;
    std::size_t tableId{};

    const std::regex eventRegex{"^([0-1]\\d|2[0-3]):([0-5]\\d) (?:(?:(2) ((?:[a-z]|\\d|_|-)+) ([1-9]\\d*))|(?:(1|3|4) ((?:[a-z]|\\d|_|-)+)))$"};
    auto matched = parse(eventRegex);
    for (; matched.size() > 0; matched = parse(eventRegex))
    {
        std::cout << matched[0] << std::endl;

        const std::size_t currentTime = std::stoll(matched[1]) * 60 + std::stoll(matched[2]);
        if (currentTime < time)
        {
            logStatistics = false;
            throw std::runtime_error{""};
        }
        else
        {
            time = currentTime;
        }

        if (!matched[3].str().empty())
        {
            eventId = 2;
        }
        else
        {
            eventId = std::stoll(matched[6]);
        }

        if (eventId != 2)
        {
            clientName = matched[7];
        }
        else
        {
            clientName = matched[4];
            tableId = std::stoll(matched[5]);
        }

        switch (eventId)
        {
        case 1:
            clientArrive(time, clientName);
            break;
        case 2:
            clientSat(time, clientName, tableId);
            break;
        case 3:
            clientWait(time, clientName);
            break;
        case 4:
            clientLeft(time, clientName);
            break;
        default:
            break;
        }
    }
}
