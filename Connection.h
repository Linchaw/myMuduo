#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include <memory>
#include <atomic>

#include "TimeStamp.h"
#include "Buffer.h"

class EventLoop;
class Channel;
class Socket;
class Connection;

using spconn = std::shared_ptr<Connection>;

class Connection: public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* m_lp;                        // 别人的传过来的

    // 自己的
    std::unique_ptr<Channel> m_ch;
    std::unique_ptr<Socket> m_sock;

    Buffer m_inputBuf;
    Buffer m_outputBuf;
    std::atomic_bool m_disconnect;

    TimeStamp m_last_read_time;


    std::function<void(spconn, std::string&)> m_onmsgcallback;
    std::function<void(spconn)> m_finmsgcallback;
    std::function<void(spconn)> m_errorcallback;
    std::function<void(spconn)> m_closecallback;

public:
    Connection(EventLoop* lp, std::unique_ptr<Socket> sock);
    ~Connection();

    void handlemessage();
    void writeCallback();
    void errorCallback();
    void closeCallback();

    void sendmsg(const char* data, size_t size);
    void sendmsg_ori(const char *data, size_t size); 
    int fd() const;
    std::string IpPort() const;
    bool timeout(time_t now, int gap=10);
    TimeStamp last_activate_time() const { return m_last_read_time; };


    void setOnMsgCallback(std::function<void(spconn, std::string&)> fn){m_onmsgcallback = std::move(fn);}
    void setFinMsgCallback(std::function<void(spconn)> fn){m_finmsgcallback = std::move(fn);}
    void setErrorCallback(std::function<void(spconn)> fn){m_errorcallback = std::move(fn);}
    void setCloseCallback(std::function<void(spconn)> fn){m_closecallback = std::move(fn);}
};

#endif