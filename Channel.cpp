#include "Channel.h"

#include "Eventloop.h"
#include "Logger.h"

Channel::Channel(EventLoop* lp, int fd) 
: m_lp(lp), 
m_fd(fd), 
m_inpoll(false), 
m_events(0), 
m_revents(0) 
{

}

void Channel::enablereading()
{
    Logger::instance().log("Channel::enablereading-"+std::to_string(m_fd));
    m_events |= EPOLLIN;
    m_lp->ep()->updateChannel(this);
}

void Channel::disablereading()
{
    Logger::instance().log("Channel::disablereading-"+std::to_string(m_fd));
    m_events &= ~EPOLLIN;
    m_lp->ep()->updateChannel(this);
}

void Channel::enablewriting()
{
    Logger::instance().log("Channel::enablewriting-"+std::to_string(m_fd));
    m_events |= EPOLLOUT;
    m_lp->ep()->updateChannel(this);
}

void Channel::disablewriting()
{
    Logger::instance().log("Channel::disablewriting-"+std::to_string(m_fd));
    m_events &= ~EPOLLOUT;
    m_lp->ep()->updateChannel(this);
}

void Channel::disableall()
{
    Logger::instance().log("Channel::disableall-"+std::to_string(m_fd));
    m_events = 0;
    m_lp->ep()->updateChannel(this);
}

void Channel::remove()
{
    Logger::instance().log("Channel::remove-"+std::to_string(m_fd));
    m_lp->ep()->removeChannel(this);
}

void Channel::handleEvent()
{
    if(m_revents & (EPOLLIN | EPOLLPRI))
    {
        Logger::instance().log("Channel::handleEvent-EPOLLIN | EPOLLPRI-"+std::to_string(m_fd));
        m_readcallback();
    }
    else if(m_revents & EPOLLHUP)  // closed 
    {
        Logger::instance().log("Channel::handleEvent-EPOLLHUP-"+std::to_string(m_fd));
        m_closeCallback();
    }

    else if(m_revents & EPOLLOUT)
    {
        Logger::instance().log("Channel::handleEvent-EPOLLOUT-"+std::to_string(m_fd));
        m_writeCallback();
    }
    else{
        Logger::instance().log("Channel::handleEvent-other-"+std::to_string(m_fd));
        m_errorCallback();
    }
}