#pragma once

#include <fstream>
#include <algorithm>
#include <iostream>
#include <format>
#include <filesystem>
#include <vector>
#include <deque>
#include <regex>
#include <cmath>

#include "session.hpp"

class Room
{
    std::size_t startTime;  // times in minutes
    std::size_t endTime;
    std::size_t cost;
    std::ifstream inFile;
    std::deque<std::string> clients;
    std::vector<std::vector<Session>> tables;
    bool logStatistics = true;

    static constexpr std::size_t UnfinishedSessionEndTime = 0;

    static std::string formatTime(std::size_t time);

    static void logEvent(std::size_t time, std::string message, std::size_t eventId);

    std::smatch parse(const std::regex&);

    void clientArrive(std::size_t time, std::string client);

    void clientSat(std::size_t time, std::string client, std::size_t tableId);

    void clientWait(std::size_t time, std::string client);

    void clientLeft(std::size_t time, std::string client);

public:
    Room(std::filesystem::path inFilePath);

    ~Room();

    void start();
};

