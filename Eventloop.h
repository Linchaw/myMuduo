#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoll.h"
#include "Channel.h"
#include "Connection.h"

#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

class EventLoop
{
private:
    std::unique_ptr<Epoll> m_ep;
    bool m_listen_el;
    std::function<void(EventLoop*)> m_timeoutcallback;
    pid_t m_pid;
    std::queue<std::function<void()>> m_taskqueue; 
    std::mutex m_mtx;
    int m_efd;  
    std::unique_ptr<Channel> m_wakeupch;
    int m_timeout;
    int m_timerfd; 
    std::unique_ptr<Channel> m_timerch;
    std::mutex m_conmtx;
    std::map<int, spconn> m_conns;
    std::atomic_bool m_stop;

public:
    EventLoop(bool listen_el = false);
    ~EventLoop(); 

    // std::unique_ptr<Epoll>& ep();
    Epoll* ep();
    void run();
    void Stop();
    void setTimeoutCallback(std::function<void(EventLoop*)> fn) { m_timeoutcallback = std::move(fn); }
    pid_t pid();
    void enqueueTask(std::function<void()> fn);
    void wakeup();
    void HandleWakeup();
    void HandleTimer();
    void HandleNewConn(spconn conn);
    void HandleRemConn();
};

#endif //EVENTLOOP_H