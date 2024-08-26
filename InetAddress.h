#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

class InetAddress
{
public:
    InetAddress();
    explicit InetAddress(const std::string& ip, uint16_t port);
    explicit InetAddress(const sockaddr_in &addr) : m_addr(addr) {};
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddrIN() const { return &m_addr; };
    const sockaddr *getSockAddr() const { return reinterpret_cast<const sockaddr *>(&m_addr); };
    void setaddr(sockaddr_in addr);

private:
    sockaddr_in m_addr;
};

#endif // INETADDRESS_H