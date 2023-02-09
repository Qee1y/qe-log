#pragma once

#include <fstream>
#include <thread>
#include <memory>
#include <string>

#include "blockqueue.h"

namespace qe
{
    using qe::bque::BlockQueue;
    using std::string;
    namespace log
    {
// 宏定义，方便用于不同日志类型
#define debug(format, ...) \
    Logger::GetInstance()->Log(Logger::LOGDEBUG, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define info(format, ...) \
    Logger::GetInstance()->Log(Logger::LOGINFO, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define warn(format, ...) \
    Logger::GetInstance()->Log(Logger::LOGWARN, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define error(format, ...) \
    Logger::GetInstance()->Log(Logger::LOGDEBUG, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define fatal(format, ...) \
    Logger::GetInstance()->Log(Logger::LOGFATAL, __LINE__, __FILE__, format, ##__VA_ARGS__)

        class Logger
        {
        public:
            class Tmp
            {
            public:
                Tmp() {}
                ~Tmp()
                {
                    m_instance->GetInstance()->destFun();
                    m_instance->GetInstance()->LogClose();
                    delete m_instance;
                }
            };

            enum Level
            {
                LOGDEBUG = 0,
                LOGINFO,
                LOGWARN,
                LOGERROR,
                LOGFATAL,
                LOGTOTAL
            };

            /// @brief
            /// @return
            static Logger *GetInstance(); // 单例实例对象

            static void flushLogThread() { GetInstance()->asyncWrite(); }

            inline void LogMax(int m) { logMax = m; }
            void Log(Level level, int line, const char *file, const char *format, ...);
            void LogOpen(const char *filename, int max);
            void LogClose();
            void rotate();
            void destFun()
            {
                if (m_wThread && m_wThread->joinable())
                {
                    while (!m_deque->Empty())
                        m_deque->flush();
                }
                m_deque->close();
                m_wThread->join();
            }

            template <class... T>
            string LogFormat(const char *fmt, const T &...t); // 字符串格式化
        private:
            string m_filename;
            std::ofstream m_out;
            static const char *m_level[LOGTOTAL];
            static Logger *m_instance; // 唯一实例对象
            unsigned long logMax = 0;  // 日志最大条目
            unsigned long logLen = 0;  // 日志当前长度
            std::mutex m_mtx;
            bool isAsync; // 是否开启异步

            std::unique_ptr<BlockQueue<string>> m_deque; // 阻塞队列
            std::unique_ptr<std::thread> m_wThread;      // 写线程
        private:
            // 单例吗模式-禁止外部访问
            Logger() {}
            ~Logger() {}
            Logger(const Logger &other) = delete;
            Logger &operator=(const Logger &other) = delete;

            // 异步写入
            void asyncWrite();
        };
    } // namespace name
}