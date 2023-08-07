#pragma once

#include <string>

struct Session
{
    std::string client;
    std::size_t startTime;
    std::size_t endTime;
};
