#include <sstream>
#include <time.h>

#include "log.h"

using qe::log::Logger;

static Logger::Tmp tmp;

//立马初始化对象实例，线程安全
Logger *Logger::m_instance = new (std::nothrow) Logger(); 

const char* Logger::m_level[Logger::LOGTOTAL] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger *Logger::GetInstance()
{
    return m_instance;
}

void Logger::Log(Level level, int line, const char* filename, const char* format, ...)
{
    if(m_out.fail())
    {
        throw std::logic_error("打开文件失败" + m_filename);
    }

    //日志回滚
    if(logLen > logMax && logMax > 0)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        rotate();
    }

    //时间戳
    time_t ticks = time(nullptr);
    struct tm ptm;
    localtime_s(&ptm, &ticks);
    char timetmp[16] {};
    strftime(timetmp, sizeof(timetmp), "%H:%M:%S", &ptm);
    
    const auto size = snprintf(nullptr, 0, "%s %s %s:%d ", timetmp, m_level[level], format, line);
    if(size > 0)
    {
        string tmp = LogFormat("[%s] [%s] [%s:%d] ", timetmp, m_level[level], format, line);

        //异步 等待线程读取信息
        if(isAsync && m_deque && !m_deque->full()) 
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_deque->PushBack(tmp);
        }
        else    //同步
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_out << tmp << std::endl;
            logLen += size;
        }
    }

}

void Logger::LogOpen(const char* filename, int max)
{
    m_filename = filename;
    m_out.open(m_filename, std::ios::out | std::ios::app);
    if(m_out.fail())
    {
        throw std::logic_error("打开文件失败" + m_filename);
    }

    if(max > 0)   //异步
    {
        isAsync = true;
        if(!m_deque)
        {
            std::unique_ptr<BlockQueue<string>> newDeque (new BlockQueue<string>(max));
            m_deque = std::move(newDeque);
            std::unique_ptr<std::thread> newThread(new std::thread(flushLogThread));
            m_wThread = std::move(newThread);
        }
    }
    else
    {
        isAsync = false;
    }
}

void Logger::LogClose()
{
    m_out.close();
}

void Logger::rotate()
{
    LogClose();

    time_t ticks = time(nullptr);
    struct tm ptm;
    localtime_s(&ptm, &ticks);
    char timetmp[24] {};
    strftime(timetmp, sizeof(timetmp), "%Y-%m-%d %S ms", &ptm);
    strcat_s(timetmp, ".txt");
    LogOpen(timetmp, 1024);
    logLen = 0;
}

void Logger::asyncWrite()
{
    string str {};
    while(m_deque->pop(str))
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        logLen += str.length();
        m_out << str << std::endl;
    }
}

template<class... T>
std::string Logger::LogFormat(const char* fmt, const T&...t)
{
    const auto len = snprintf(nullptr, 0, fmt, t...);
    string r;
    r.resize(static_cast<size_t>(len) + 1);
    snprintf(&r.front(), len + 1, fmt, t...);
    r.resize(static_cast<size_t>(len));
    return r;
}