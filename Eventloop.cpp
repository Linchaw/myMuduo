#include "Eventloop.h"
#include "Channel.h"
#include "Logger.h"

int creat_timefd(int sec=5)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    itimerspec timeout;
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = sec;  // 定时器的间隔时间
    timeout.it_interval.tv_nsec = 0;
    timerfd_settime(tfd, 0,&timeout, 0);
    return tfd;
}

EventLoop::EventLoop(bool listen_el)
    : m_ep(std::make_unique<Epoll>()),
      m_listen_el(listen_el),
      m_efd(eventfd(0, EFD_NONBLOCK)),
      m_wakeupch(std::make_unique<Channel>(this, m_efd)),
      m_timeout(5),
      m_timerfd(creat_timefd(m_timeout)),
      m_timerch(std::make_unique<Channel>(this, m_timerfd)),
      m_stop(false)
{
    m_wakeupch->setReadCallback([this]()
                                { this->HandleWakeup(); });
    m_wakeupch->enablereading();
    m_timerch->setReadCallback([this]()
                               { this->HandleTimer();});
    m_timerch->enablereading();
}

EventLoop::~EventLoop()
{}

// std::unique_ptr<Epoll> &EventLoop::ep()
// {
//     return m_ep;
// }

Epoll* EventLoop::ep()
{
    return m_ep.get();
}

void EventLoop::run()
{
    m_pid = syscall(SYS_gettid);
    Logger::instance().log("main loop start", 3);
    while (!m_stop)
    {
        std::cout << "-------main loop-------" << std::endl;
        std::vector<Channel *> channels = m_ep->loop(10 * 1000);
        if (channels.empty())
        {
            m_timeoutcallback(this);
            continue;
        }
        for (auto ch : channels)
            ch->handleEvent();
    }
}

void EventLoop::Stop()
{
    Logger::instance().log("EventLoop::Stop()");
    m_stop = true;
    wakeup();
}

pid_t EventLoop::pid()
{
    return m_pid;
}

void EventLoop::enqueueTask(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_taskqueue.push(std::move(fn));
    }
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    if (write(m_efd, &one, sizeof(one)) != sizeof(one))
        Logger::instance().log("EventLoop::wakeup()", 1);
}

void EventLoop::HandleWakeup()
{
    uint64_t one = 1;
    if (read(m_efd, &one, sizeof(one)) != sizeof(one))
    {
        Logger::instance().log("EventLoop::HandleWakeup(): read error", 1);
        return; // 可能需要根据错误情况退出或处理
    }

    std::function<void()> fn;
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            if (m_taskqueue.empty())
                break;

            fn = std::move(m_taskqueue.front());
            m_taskqueue.pop();
        }

        try
        {
            fn(); // 执行任务
        }
        catch (const std::exception &e)
        {
            Logger::instance().log("EventLoop::HandleWakeup(): exception during task execution", 1);
            Logger::instance().log(e.what());
        }
        catch (...)
        {
            Logger::instance().log("EventLoop::HandleWakeup(): unknown exception during task execution", 1);
        }
    }
}

void EventLoop::HandleTimer()
{
    Logger::instance().log("EventLoop::HandleTimer()");
    uint64_t expirations;
    if (read(m_timerfd, &expirations, sizeof(expirations)) != sizeof(expirations)) 
        perror("timerfd read");
    if(!m_listen_el)
        HandleRemConn();
}

void EventLoop::HandleNewConn(spconn conn)
{
    Logger::instance().log("EventLoop::HandleNewConn");
    std::lock_guard<std::mutex> lock(m_conmtx);
    m_conns[conn->fd()] = conn;
}

void EventLoop::HandleRemConn()
{
    Logger::instance().log("EventLoop::HandleRemConn");
    time_t now = time(NULL);
    for (auto it = m_conns.begin(); it != m_conns.end(); /* no increment here */) {
        if (it->second->timeout(now)) {
            it->second->closeCallback();
            {
                std::lock_guard<std::mutex> lock(m_conmtx);
                it = m_conns.erase(it);  // erase returns the next valid iterator
            }
        } else {
            ++it;
        }
    }
}

