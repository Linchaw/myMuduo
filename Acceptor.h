#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include <string>
#include <cstdint>

#include "Socket.h"
#include "Channel.h"

class Channel;
class EventLoop;
class Socket;


class Acceptor
{
private:
    EventLoop* m_lp;                        // 别人的传过来的
    std::function<void(std::unique_ptr<Socket>)> m_newconnectioncb;
    
    Socket m_sock;
    Channel m_servch;

public:
    Acceptor(EventLoop* lp, const std::string& ip, uint16_t port);
    ~Acceptor() = default;
    
    void newconnection();
    void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn){m_newconnectioncb = std::move(fn);};
};

#endif