#pragma once

#include <iostream>
#include <chrono>

struct ptime
{
public:
    ptime();
    ~ptime();
private:
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<float> dur;
};
