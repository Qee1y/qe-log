#pragma once

#include <condition_variable>
#include <memory>
#include <deque>
#include <mutex>

namespace qe
{
    namespace bque
    {
        using std::lock_guard;
        using std::mutex;

        template <class T>
        class BlockQueue // 阻塞队列
        {
        public:
            explicit BlockQueue(size_t max = 1000);
            ~BlockQueue();

            void close();
            void flush();
            void clear();
            void PushBack(const T &item);
            void PushFront(const T &item);

            bool Empty();
            bool full();

            bool pop(T &item);
            bool pop(T &item, int timeout);

            T Fornt();
            T Back();

            size_t GetSize();
            size_t GetCapacity();

        private:
            std::deque<T> m_deq;
            size_t m_max; // 容器大小
            mutex m_mtx;

            bool isClose;

            /*线程条件变量 生产者和消费者
            详细: https://zhuanlan.zhihu.com/p/224054283*/
            std::condition_variable m_consumer, m_producer;
        };

        template <class T>
        BlockQueue<T>::BlockQueue(size_t max) : m_max(max), isClose(false)
        {
            if (m_max < 0)
                throw std::logic_error("error");
        }

        template <class T>
        BlockQueue<T>::BlockQueue::~BlockQueue()
        {
            close();
        }

        template <class T>
        void BlockQueue<T>::BlockQueue::close()
        {
            {
                lock_guard<std::mutex> lock(m_mtx);
                m_deq.clear();
                isClose = true;
            }
            m_producer.notify_all();
            m_consumer.notify_all();
        }

        template <class T>
        void BlockQueue<T>::flush()
        {
            m_consumer.notify_one();
        }

        template <class T>
        void BlockQueue<T>::clear()
        {
            lock_guard<std::mutex> lock(m_mtx);
            m_deq.clear();
        }

        template <class T>
        T BlockQueue<T>::Fornt()
        {
            lock_guard<std::mutex> lock(m_mtx) return m_deq.front();
        }

        template <class T>
        T BlockQueue<T>::Back()
        {
            lock_guard<std::mutex> lock(m_mtx) return m_deq.back();
        }

        template <class T>
        size_t BlockQueue<T>::GetSize()
        {
            lock_guard<std::mutex> lock(m_mtx) return m_deq.size();
        }

        template <class T>
        void BlockQueue<T>::PushBack(const T &item)
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            while (m_deq.size() >= m_max)
            {
                m_producer.wait(lock); // 阻塞当前线程，直到条件变量被唤醒
            }
            m_deq.push_back(item);
            m_consumer.notify_one();
        }

        template <class T>
        void BlockQueue<T>::PushFront(const T &item)
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            while (m_deq.size() >= max)
            {
                m_producer.wait(lock); // 阻塞当前线程，直到条件变量被唤醒
            }
            m_deq.push_front(item);
            m_consumer.notify_one();
        }

        template <class T>
        bool BlockQueue<T>::Empty()
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_max == 0;
        }

        template <class T>
        bool BlockQueue<T>::full()
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_deq.size() >= m_max;
        }

        template <class T>
        bool BlockQueue<T>::pop(T &item)
        {
            std::unique_lock<mutex> lock(m_mtx);
            while(m_deq.empty())
            {
                m_consumer.wait(lock);
                if(isClose)
                    return false;
            }
            item = m_deq.front();
            m_deq.pop_front();
            m_producer.notify_one();
            return true;
        }
    }
}