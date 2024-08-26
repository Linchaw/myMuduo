#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include "TcpServer.h"
#include "Connection.h"
#include "ThreadPool.h"

class EchoServer
{
private:
    TcpServer m_tcpserver;
    size_t m_threadnum;
    ThreadPool m_threadpool;

public:
    EchoServer(const std::string& ip, uint16_t port, size_t threadnum=3, size_t workthreadnum=3);
    ~EchoServer();

    void Start();
    void Stop();
    void HandleNewConn(spconn conn);
    void HandleError(spconn conn);  
    void HandleClose(spconn conn);
    void HandleMessage(spconn conn, std::string& msg);
    void HandleCompleteSend(spconn conn);
    void HandleEpollTimeout(EventLoop* lp);

    void DowithMessage(spconn conn, const std::string& msg);
};

#endif // ECHO_SERVER_H