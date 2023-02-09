#include "ptime.h"

ptime::ptime() 
{ 
    start = std::chrono::system_clock::now(); 
}

ptime::~ptime() 
{ 
    end =  std::chrono::system_clock::now();
    dur = end - start;
    float ms = dur.count() * 1000.0f;
    std::cout << "time took: " << ms << " ms\n";
}