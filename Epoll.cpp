#include "Epoll.h"

#include "Channel.h"
#include "Logger.h"

Epoll::Epoll() : m_fd(epoll_create1(0))
{
    Logger::instance().log("Epoll::Epoll epoll_fd:"+std::to_string(m_fd));
    if (m_fd < 0)
    {
        perror("Epoll_create1");
        exit(EXIT_FAILURE);
    }
}

Epoll::~Epoll()
{
    close(m_fd);
}

void Epoll::updateChannel(Channel *ch)
{
    epoll_event ev;
    ev.events = ch->events();
    ev.data.ptr = ch;

    if (ch->inpoll())
    {
        Logger::instance().log("Epoll::updateChannel-EPOLL_CTL_MOD fd-"+std::to_string(ch->fd()));
        if (epoll_ctl(m_fd, EPOLL_CTL_MOD, ch->fd(), &ev) < 0)
        {
            perror("epoll_mod");
            exit(EXIT_FAILURE); 
        }
    }
    else
    {
        Logger::instance().log("Epoll::updateChannel-EPOLL_CTL_ADD fd-"+std::to_string(ch->fd())+" m_fd"+std::to_string(m_fd));
        
        if (epoll_ctl(m_fd, EPOLL_CTL_ADD, ch->fd(), &ev) < 0)
        {
            perror("epoll_add");
            exit(EXIT_FAILURE);
        }
        ch->setinpoll(true);
    }
}

void Epoll::removeChannel(Channel *ch)
{
    Logger::instance().log("Epoll::removeChannel");
    if (ch->inpoll())
    {
        if (epoll_ctl(m_fd, EPOLL_CTL_DEL, ch->fd(), 0) < 0)
        {
            perror("epoll_mod: master_fd");
            exit(EXIT_FAILURE);
        }
        ch->setinpoll(false);
    }
}

std::vector<Channel *> Epoll::loop(int timeout)
{
    Logger::instance().log("Epoll::loop");
    std::vector<Channel *> channels;

    int num = epoll_wait(m_fd, m_evs, MAX_NUM, timeout);

    if (num < 0)
    {
        perror("epoll_wait: fail");
        exit(EXIT_FAILURE);
    }

    if (num == 0)
        return channels;

    // 预分配足够的空间以避免多次分配
    channels.reserve(num);

    for (int i = 0; i < num; ++i)
    {
        Channel *ch = static_cast<Channel *>(m_evs[i].data.ptr);
        ch->set_revents(m_evs[i].events);
        channels.push_back(ch);
    }

    return channels;
}