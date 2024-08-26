#include "Connection.h"

#include "Eventloop.h"
#include "Channel.h"
#include "Socket.h"
#include "Logger.h"

Connection::Connection(EventLoop *lp, std::unique_ptr<Socket> sock) : 
m_lp(lp), 
m_sock(std::move(sock)), 
m_inputBuf(head_buf), 
m_outputBuf(head_buf), 
m_disconnect(false)
{
    m_ch = std::make_unique<Channel>(m_lp, m_sock->fd());
    m_ch->setReadCallback([this](){ this->handlemessage(); });
    m_ch->setWriteCallback([this](){ this->writeCallback(); });
    m_ch->setErrorCallback([this](){ this->errorCallback(); });
    m_ch->setCloseCallback([this](){ this->closeCallback(); });
    m_ch->setet();
    m_ch->enablereading();
}

Connection::~Connection()
{
    Logger::instance().log("Connection::DeStruction()");
}

void Connection::handlemessage()
{
    Logger::instance().log("Connection::handlemessage()");

    char buffer[BUFFER_SIZE] = {0};
    while (1)
    {
        ssize_t count = read(fd(), buffer, sizeof(buffer));
        if (count > 0) // 处理读取到的数据
        {
            m_inputBuf.append(buffer, count);
        }
        else if (count == -1 and (errno == EAGAIN || errno == EWOULDBLOCK)) // 数据读取完毕
        {
            std::string msg;
            while (m_inputBuf.getdatax(msg))
            {
                m_last_read_time = TimeStamp::now();
                m_onmsgcallback(shared_from_this(), msg);
            }
            break;
        }
        else if (count == 0) // 客户端关闭连接
        {
            closeCallback();
            break;
        }
        else
        {
            errorCallback();
            break;
        }
    }
}

void Connection::sendmsg(const char *data, size_t size)
{
    Logger::instance().log("Connection::sendmsg");

    if (m_disconnect)
    {
        Logger::instance().log("Connection::sendmsg--m_disconnect");
        return;
    }
    if(m_lp->pid()==syscall(SYS_gettid))
        sendmsg_ori(data, size);
    else
    {
        Logger::instance().log("Connection::sendmsg back", 3);
        std::string dataCopy(data, size); // 拷贝数据到 std::string 中
        m_lp->enqueueTask([this, dataCopy = std::move(dataCopy)]() { sendmsg_ori(dataCopy.data(), dataCopy.size()); });
    }
}

void Connection::sendmsg_ori(const char *data, size_t size)
{
    Logger::instance().log("Connection::sendmsg_ori");
    m_outputBuf.appendx(data, size);
    m_ch->enablewriting();
}

void Connection::writeCallback()
{
    Logger::instance().log("Connection::writeCallback", 3);

    if (m_outputBuf.empty())
    {
        m_ch->disablewriting();
        return;
    }

    int len = ::send(fd(), m_outputBuf.data(), m_outputBuf.size(), 0);
    if (len < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // 套接字发送缓冲区满，这种情况不需要立即处理
            return;
        }
        else if (errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN)
        {
            std::cerr << "Connection error: " << strerror(errno) << std::endl;
            closeCallback();
            // 关闭连接并清理资源
        }
        else
        {
            std::cerr << "send error: " << strerror(errno) << std::endl;
            // 处理其他错误
            errorCallback();
        }
        return;
    }

    if (len > 0)
    {
        m_outputBuf.erase(0, len);
    }

    if (m_outputBuf.empty())
    {
        try
        {
            m_finmsgcallback(shared_from_this());
        }
        catch (const std::bad_weak_ptr &e)
        {
            std::cerr << "bad_weak_ptr: " << e.what() << std::endl;
        }
        m_ch->disablewriting();
    }
}

void Connection::errorCallback()
{
    Logger::instance().log("Connection::errorCallback");

    m_disconnect = true;
    m_ch->remove();
    m_errorcallback(shared_from_this());
}

void Connection::closeCallback()
{
    Logger::instance().log("Connection::closeCallback");

    m_disconnect = true;
    m_ch->remove();
    m_closecallback(shared_from_this());
}

std::string Connection::IpPort() const
{
    return m_sock->toIpPort();
}

int Connection::fd() const
{
    return m_sock->fd();
}

bool Connection::timeout(time_t now, int gap)
{
    return now - m_last_read_time.toint() > gap; 
}