# qe-log 
   简介: qe-log是一个简单c++日志记录库，特性方面,线程安全，异步IO以及同步,性能方面只能说一般般.日志使用的单例模式，以及引入生产者与消费者概念，下面细说。  
   日志功能:
      异步IO
      同步
      日志回滚
      线程安全
      崩溃堆栈(未实现)

## 性能
     


## 单例模式
#### 日志为什么为什么要设计为单例模式?  
     减少系统资源开销，由于单例只保留一个对象  
     提高创建速度，每次都获取已经存在的对象因此提高创建速度  
     全局共享对象，由于单例在系统中只存在一个对象实例，因此任何地方使用此对象都是一个对象
     
#### 实例:
      Logger::GetInstance()->LogOpen("test1.txt", 0);   //每次调用使用的都是一个对象
      
      //立马初始化对象实例，线程安全
      Logger *Logger::m_instance = new (std::nothrow) Logger(); 
    

## 生产者与消费者
#### 为什么有生产者消费者模式需求?
           在多线程可能会出现消费者数据非常慢，而生产者效率取决于消费者，生产者必须等消费消费完数据继续生产，为了平衡
       诞生出了生产者和消费者模型，为了生产者与消费者数据间的平衡，需要一个缓冲区存储数据。
#### 生产者与消费者特点
           保证生产者不会在缓冲区满的时候继续向缓冲区放入数据，而消费者也不会在缓冲区空的时候，消耗数据
       当缓冲区满的时候，生产者会进入休眠状态，当下次消费者开始消耗缓冲区的数据时，生产者才会被唤醒，开始往缓冲区中添加数据；当缓冲区空的时候，消费者也会进入休        眠状态，直到生产者往缓冲区中添加数据时才会被唤醒
        生产者:
            for (int i = 0; i < num; i++)
            {
               static int bh = 1;
               message = to_string(bh++) + "号";
               m_q.push(message);   //产生数据
            }
            m_code.notify_one(); // 唤醒当前条件变量阻塞的线程
        消费者:
            while (m_q.empty())
                  m_code.wait(lock); // 如果生产者没有生产数据，就等待生产者产生数据并唤醒
            message = m_q.front();
            m_q.pop();
         
      
