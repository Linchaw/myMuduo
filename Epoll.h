#ifndef EPOLL_H
#define EPOLL_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <vector>

class Channel;

class Epoll
{
private:
    static const int MAX_NUM = 100;
    int m_fd;
    epoll_event m_evs[MAX_NUM];

public:
    Epoll();
    ~Epoll();
    
    void updateChannel(Channel *ch);
    void removeChannel(Channel *ch);
    
    std::vector<Channel *> loop(int timeout = -1);
};

#endif //  EPOLL_H