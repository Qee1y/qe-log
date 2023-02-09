#include <iostream>

#include "src/log.h"
#include "src/ptime.h"

int main()
{
    using qe::log::Logger;
    ptime time;  //计时

    Logger::GetInstance()->LogMax(4096);

    int cnt = 0;
    Logger::GetInstance()->LogOpen("test1.txt", 0);
    for(int i = 0; i < 15000; i++)
    {
        debug("Hello world");
        info("getinstance !");
        warn("warn d123dadadd");
        error("dwofkaowfafk");
    }
    Logger::GetInstance()->LogClose();
    
    
    cnt = 0;
    Logger::GetInstance()->LogOpen("test2.txt", 1024);
    for(int i = 0; i < 15000; i++)
    {
        debug("Hello world", cnt++);
        info("getinstance !", cnt++);
        warn("warn d123dadadd",  cnt++);
        error("dwofkaowfafk", cnt++);
    }
    Logger::GetInstance()->LogClose();
    return 0;
}