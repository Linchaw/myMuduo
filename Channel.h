#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <functional>
#include <memory>
#include <sys/epoll.h>

class EventLoop;

#define BUFFER_SIZE 1024

class Channel
{
private:
    EventLoop* m_lp;

    const int m_fd;
    bool m_inpoll;

    uint32_t m_events;
    uint32_t m_revents;

    std::function<void()> m_readcallback;
    std::function<void()> m_writeCallback;
    std::function<void()> m_closeCallback;
    std::function<void()> m_errorCallback;

public:
    Channel(EventLoop* lp, int fd);
    ~Channel() = default;

    int fd() { return m_fd; }
    bool inpoll() { return m_inpoll; }
    uint32_t events() { return m_events; }
    uint32_t revents() { return m_revents; }
    void setet() { m_events |= EPOLLET; }
    void setinpoll(bool on = true) { m_inpoll = on; }
    void set_revents(uint32_t revt) { m_revents = revt; }

    void handleEvent();

    void setReadCallback(std::function<void()> fn) { m_readcallback = std::move(fn); }
    void setWriteCallback(std::function<void()> fn) { m_writeCallback = std::move(fn); }
    void setCloseCallback(std::function<void()> fn) { m_closeCallback = std::move(fn); }
    void setErrorCallback(std::function<void()> fn) { m_errorCallback = std::move(fn); }

    void enablereading();
    void disablereading();
    void enablewriting();
    void disablewriting();
    void disableall();
    void remove();
};

#endif