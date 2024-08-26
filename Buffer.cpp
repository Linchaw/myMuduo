#include "Buffer.h"

#include <cstring>
#include <iostream>
// #include <cstdint>
// #include <arpa/inet.h>

Buffer::Buffer(buffer_type type):m_type(type)
{
    
}

Buffer::~Buffer()
{

}

void Buffer::append(const char *data, size_t size)
{
    m_buf.append(data, size);
}

void Buffer::appendx(const char *data, size_t size)
{
    if (data == nullptr || size == 0)
    {
        throw std::invalid_argument("Invalid data or size");
    }
    switch (m_type)
    {
    case fixed_buf:
        m_buf.append(data, size);
        break;
    case head_buf:
    {
        // std::uint32_t size_network_order = htonl(static_cast<std::uint32_t>(size));
        // m_buf.append(reinterpret_cast<char*>(&size_network_order), sizeof(size_network_order));
        m_buf.append((char *)(&size), 4);
        m_buf.append(data, size);
        break;
    }
    case http_buf:
        m_buf.append(data, size);
        m_buf.append("\r\n\r\n");
        break;
    default:
        throw std::logic_error("Unknown buffer type");
    }
}

bool Buffer::getdata(std::string &str)
{
    if(m_buf.empty()) return false;
    str.assign(m_buf.data(), m_buf.size());
    m_buf.clear();
    return true;
}

bool Buffer::getdatax(std::string &str)
{
    switch (m_type)
    {
    case fixed_buf:
        return getdata(str);
    case head_buf:
        // std::cout << "Buffer::getdatax" << m_buf.size() << std::endl;
        if (m_buf.size() < 4)
            return false;
        int len;
        memcpy(&len, m_buf.data(), 4);
            if (len < 0 || (int)m_buf.size() < len + 4)
                return false;
        str.assign(m_buf.data() + 4, len); // 提取消息
        m_buf.erase(0, len + 4);
        break;
    case http_buf:
        size_t pos = m_buf.find("\r\n\r\n");
        if (pos == std::string::npos)
            return false;
        str.assign(m_buf,0,pos+4);
        m_buf.erase(0, pos + 4);
        break;
    }
    return true;
}

size_t Buffer::size()
{
    return m_buf.size();
}

bool Buffer::empty()
{
    return m_buf.empty();
}

const char *Buffer::data()
{
    return m_buf.data();
}
void Buffer::clear()
{
    m_buf.clear();
}

void Buffer::erase(size_t pos, size_t size)
{
    m_buf.erase(pos, size);
}