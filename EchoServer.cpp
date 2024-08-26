#include "EchoServer.h"

#include "Logger.h"

EchoServer::EchoServer(const std::string &ip, uint16_t port, size_t threadnum, size_t workthreadnum)
    : m_tcpserver(ip, port, threadnum),
      m_threadpool(workthreadnum, WorkThreadPool)
{
    Logger::instance().log("EchoServer::EchoServer");
    m_tcpserver.set_newconncallback([this](spconn conn)
                                    { this->HandleNewConn(conn); });
    m_tcpserver.set_errorconncallback([this](spconn conn)
                                      { this->HandleError(conn); });
    m_tcpserver.set_closeconncallback([this](spconn conn)
                                      { this->HandleClose(conn); });
    m_tcpserver.set_finsendcallback([this](spconn conn)
                                    { this->HandleCompleteSend(conn); });
    m_tcpserver.set_handlemsgcallback([this](spconn conn, std::string &msg)
                                      { this->HandleMessage(conn, msg); });
}

EchoServer::~EchoServer()
{
    Stop();
}

void EchoServer::Start()
{
    Logger::instance().log("EchoServer::Start");
    m_tcpserver.Start();
}

void EchoServer::Stop()
{
    Logger::instance().log("EchoServer::Stop");
    m_threadpool.Stop();
    m_tcpserver.Stop();
}

void EchoServer::HandleNewConn(spconn conn)
{
    Logger::instance().log("EchoServer::HandleNewConn");
}

void EchoServer::HandleError(spconn conn)
{
    Logger::instance().log("EchoServer::HandleError--Ip:Port:" + conn->IpPort(), 1);
}

void EchoServer::HandleClose(spconn conn)
{
    Logger::instance().log("EchoServer::HandleClose--Ip:Port:" + conn->IpPort(), 1);
}

void EchoServer::HandleMessage(spconn conn, std::string &msg)
{

    Logger::instance().log("EchoServer::HandleMessage");
    if (m_threadpool.size() > 0)
        m_threadpool.enqueueTask([this, conn, msg]()
                                 { this->DowithMessage(conn, msg); });
    else
        DowithMessage(conn, msg);
}
void EchoServer::HandleCompleteSend(spconn conn)
{
    Logger::instance().log("EchoServer::HandleCompleteSend");
}
void EchoServer::HandleEpollTimeout(EventLoop *lp)
{
    Logger::instance().log("EchoServer::HandleEpollTimeout");
}

void EchoServer::DowithMessage(spconn conn, const std::string &msg)
{
    Logger::instance().log("EchoServer::DowithMessage");
    std::cout << "Received: " << msg << std::endl;
    std::string new_msg = "Reply:" + msg;
    conn->sendmsg(new_msg.data(), new_msg.size());
}