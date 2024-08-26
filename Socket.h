#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "InetAddress.h"
#include "noncopyable.h"

int createfd();

class Socket : noncopyable
{
private:
    const int m_fd;
    std::string m_ip;
    uint16_t  m_port;

public:
    Socket(int fd) : m_fd(fd) {};
    ~Socket() { ::close(m_fd); };
    
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void bindInetAddr(const InetAddress &seraddr);  // 有::bind()函数  用于监听
    void bindAddr(const InetAddress &addr);   //  单纯绑定一个地址
    void listen(int num = 128);
    int accept(InetAddress &addr);
    int fd() { return m_fd; };
    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;
};

#endif // SOCKET_H