#include "Socket.h"

int createfd()
{
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd < 0)
    {
        perror("Create listen socket");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void Socket::setreuseaddr(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Socket::setreuseport(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void Socket::bindInetAddr(const InetAddress &seraddr)
{
    if (bind(m_fd, seraddr.getSockAddr(), sizeof(sockaddr_in)) < 0)
    {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    m_ip = seraddr.toIp();
    m_port = seraddr.toPort();
}

void Socket::bindAddr(const InetAddress &addr)
{
    m_ip = addr.toIp();
    m_port = addr.toPort();
}

void Socket::listen(int num)
{
    if (::listen(m_fd, num) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

int Socket::accept(InetAddress &addr)
{
    sockaddr_in client;
    socklen_t addrlen = sizeof(client);
    int conn_fd = ::accept4(m_fd, (struct sockaddr *)&client, &addrlen, SOCK_NONBLOCK);
    if (conn_fd >= 0)
    {
        addr.setaddr(client);
    }

    if (conn_fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        perror("accept4");
    }

    return conn_fd;
}

std::string Socket::toIp() const
{
    return m_ip;
}
uint16_t Socket::toPort() const
{
    return m_port;
}
std::string Socket::toIpPort() const
{
    return m_ip + ":" + std::to_string(m_port);
}