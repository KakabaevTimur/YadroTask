#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <vector>
#include <deque>
#include <regex>

struct Session
{
    std::string client;
    std::size_t startTime;
    std::size_t endTime;
};

class Room
{
    std::size_t startTime;
    std::size_t endTime;
    std::size_t cost;
    std::deque<std::string> clients;
    std::vector<std::vector<Session>> tables;

    static constexpr std::size_t UnfinishedSessionEndTime = 0;

    static std::string formatTime(std::size_t time)
    {
        return std::format("{:0>2}:{:0>2}", time / 60, time % 60);
    }

    static void logEvent(std::size_t time, std::string message, std::size_t eventId = 13)
    {
        std::cout << std::format("{} {} {}\n", formatTime(time), eventId, message);
    }

public:
    Room(std::size_t startTime, std::size_t endTime, std::size_t countTables, std::size_t cost)
        : startTime{startTime}, endTime{endTime}, tables{countTables}, cost{cost}
    {
        std::cout << formatTime(startTime) << std::endl;
    }

    ~Room()
    {
        for (auto& tableSessions: tables)
        {
            if (tableSessions.size() > 0 && tableSessions.back().endTime == 0)
            {
                tableSessions.back().endTime = endTime;
                clients.push_back(tableSessions.back().client);
            }
        }

        std::sort(clients.begin(), clients.end());

        for (const auto& client: clients)
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
                const size_t time = session.endTime - session.startTime;
                totalTableTime += time;
                tableSum += std::ceil(static_cast<double>(time) / 60) * cost;
            }

            std::cout << std::format("{} {} {}\n", tableId + 1, tableSum, formatTime(totalTableTime));
        }
    }

    void clientArrive(std::size_t time, std::string client)
    {
        if (time < startTime || time > endTime)
        {
            logEvent(time, "NotOpenYet");
            return;
        }

        if (std::find(clients.begin(), clients.end(), client) != clients.end())
        {
            logEvent(time, "YouShallNotPass");
            return;
        }

        clients.push_back(client);
    }

    void clientSat(std::size_t time, std::string client, std::size_t tableId)
    {
        tableId--; // fixing index of table

        if (tableId > tables.size())
        {
            std::exit(1);
        }

        bool clientIsFirstInQueue = (clients.size() > 0 && clients.front() == client);

        const auto tableIterator = std::find_if(tables.begin(), tables.end(), [client](const auto& tableSessions)
        {
            return tableSessions.size() > 0 && tableSessions.back().client == client && tableSessions.back().endTime == 0;
        });

        bool clientIsSitting = (tableIterator != tables.end());

        if (!clientIsFirstInQueue && !clientIsSitting)
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
            if (clients.size() > 0)
            {
                tableIterator->push_back(Session{clients.front(), time, UnfinishedSessionEndTime});
                clients.pop_front();
                const std::size_t freedTableId = std::distance(tables.begin(), tableIterator);
                logEvent(time, std::format("{} {}", client, freedTableId + 1), 12);
            }
        }

        tables[tableId].push_back(Session{client, time, UnfinishedSessionEndTime});

        clients.pop_front();
    }

    void clientWait(std::size_t time, std::string client)
    {
        bool hasFreeTable = std::any_of(tables.begin(), tables.end(), [](const auto& tableSessions)
        {
            return tableSessions.size() == 0 || tableSessions.back().endTime != 0;
        });

        if (hasFreeTable)
        {
            logEvent(time, "ICanWaitNoLonger!");
            return;
        }

        if (clients.size() > tables.size())
        {
            clients.erase(std::find(clients.begin(), clients.end(), client));

            logEvent(time, client, 11);
        }
    }

    void clientLeft(std::size_t time, std::string client)
    {
        const auto clientIterator = std::find(clients.begin(), clients.end(), client);

        const auto tableIterator = std::find_if(tables.begin(), tables.end(), [client](const auto& tableSessions)
        {
            return tableSessions.size() > 0 && tableSessions.back().client == client;
        });

        bool clientIsInQueue = (clientIterator != clients.end());
        bool clientIsSitting = (tableIterator != tables.end());

        if (!clientIsInQueue && !clientIsSitting)
        {
            logEvent(time, "ClientUnknown");
            return;
        }

        if (clientIsSitting)
        {
            tableIterator->back().endTime = time;

            if (clients.size() > 0)
            {
                tableIterator->push_back(Session{clients.front(), time, UnfinishedSessionEndTime});
                clients.pop_front();
                const std::size_t freedTableId = std::distance(tables.begin(), tableIterator);
                logEvent(time, std::format("{} {}", client, freedTableId + 1), 12);
            }
        }
        else
        {
            clients.erase(clientIterator);
        }
    }
};

std::smatch parse(std::ifstream& infile, const std::regex& regex)
{
    static std::string line;
    if (!std::getline(infile, line))
    {
        return {};
    }
    std::smatch match;
    if (!std::regex_search(line, match, regex))
    {
        std::cout << line;
        std::exit(1);
    }
    return match;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
	{
		return 1;
	}

    std::ifstream infile(argv[1]);

    std::size_t countTables;
    std::size_t startTime; //in minutes
    std::size_t endTime; //in minutes
    long long cost;

    std::string line;

    const std::regex countTablesRegex{"([1-9]\\d*)"};
    auto matched = parse(infile, countTablesRegex);
    countTables = std::stoull(matched[0]);

    const std::regex timesRegex{"([0-1]\\d|2[0-3]):([0-5]\\d) ([0-1]\\d|2[0-3]):([0-5]\\d)"};  // for line like "09:00 19:00"
    matched = parse(infile, timesRegex);
    startTime = std::stoll(matched[1]) * 60 + std::stoll(matched[2]);
    endTime = std::stoll(matched[3]) * 60 + std::stoll(matched[4]);
    if (startTime > endTime)
    {
        return 1;
    }

    const std::regex& costRegex = countTablesRegex;
    matched = parse(infile, costRegex);
    cost = std::stoull(matched[1]);
    if (cost < 0)
    {
        return 1;
    }

    Room room{startTime, endTime, countTables, static_cast<std::size_t>(cost)};

    std::size_t time{};
    std::size_t eventId{};
    std::string name;
    std::size_t tableId{};

    const std::regex eventRegex{"([0-1]\\d|2[0-3]):([0-5]\\d) (?:(?:(2) ((?:[a-z]|\\d|_|-)+) ([1-9]\\d*))|(?:(1|3|4) ((?:[a-z]|\\d|_|-)+)))"};
    matched = parse(infile, eventRegex);
    for (; matched.size() > 0; matched = parse(infile, eventRegex))
    {
        std::cout << matched[0] << std::endl;

        std::size_t currentTime = std::stoll(matched[1]) * 60 + std::stoll(matched[2]);
        if (currentTime < time)
        {
            std::exit(1);
        }
        else
        {
            time = currentTime;
        }

        if (!matched[3].str().empty())
        {
            eventId = std::stoll(matched[3]);
        }
        else
        {
            eventId = std::stoll(matched[6]);
        }

        if (eventId != 2)
        {
            name = matched[7];
        }
        else
        {
            name = matched[4];
            tableId = std::stoll(matched[5]);
        }

        switch (eventId)
        {
        case 1:
            room.clientArrive(time, name);
            break;
        case 2:
            room.clientSat(time, name, tableId);
            break;
        case 3:
            room.clientWait(time, name);
            break;
        case 4:
            room.clientLeft(time, name);
            break;
        default:
            break;
        }
    }
}