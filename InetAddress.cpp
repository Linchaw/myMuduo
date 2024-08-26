#include "InetAddress.h"

InetAddress::InetAddress()
{
    std::memset(&m_addr, 0, sizeof(m_addr));
}
InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    std::memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) <= 0)
    {
        throw std::invalid_argument("Invalid IP address format");
    }
}

void InetAddress::setaddr(sockaddr_in addr)
{
    m_addr = addr;
}


std::string InetAddress::toIp() const
{
    char buf[64] = {0};
    if (inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf)) == nullptr)
    {
        return "Invalid IP address";
    }
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    if (inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf)) == nullptr)
    {
        return "Invalid IP address";
    }
    uint16_t port = ntohs(m_addr.sin_port);
    return std::string(buf) + ":" + std::to_string(port);
}

uint16_t InetAddress::toPort() const
{
    return ntohs(m_addr.sin_port);
}