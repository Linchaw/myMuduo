#include "TcpServer.h"

#include "Eventloop.h"
#include "Socket.h"
#include "Logger.h"

TcpServer::TcpServer(const std::string &ip, uint16_t port, size_t threadsnum)
    : m_lp(std::make_unique<EventLoop>(true)),
      m_threadnum(threadsnum),
      m_threadpool(m_threadnum),
      m_acc(m_lp.get(), ip, port)
{
    Logger::instance().log("TcpServer::TcpServer");
    m_lp->setTimeoutCallback([this](EventLoop *lp)
                             { this->epolltimeout(lp); });
    m_acc.setnewconnectioncb([this](std::unique_ptr<Socket> clisock)
                             { this->newconnection(std::move(clisock)); });

    for (size_t i = 0; i < m_threadnum; i++)
    {
        auto& sub_loop = m_sublp.emplace_back(std::make_unique<EventLoop>());
        sub_loop->setTimeoutCallback([this](EventLoop *lp)
                                    { this->epolltimeout(lp); });
    }

}

TcpServer::~TcpServer()
{
    Logger::instance().log("TcpServer::~TcpServer");
}

void TcpServer::Start()
{
    Logger::instance().log("TcpServer::start");
    for (size_t i = 0; i < m_threadnum; i++)
    {
        m_threadpool.enqueueTask([this, i]()
                                 { m_sublp[i]->run(); });
    }
    m_lp->run();
}

void TcpServer::Stop()
{
    Logger::instance().log("TcpServer::stop");
    m_lp->Stop();
    for (size_t i = 0; i < m_threadnum; i++)
        m_sublp[i]->Stop();
    m_threadpool.Stop();
}

void TcpServer::newconnection(std::unique_ptr<Socket> clisock)
{
    Logger::instance().log("TcpServer::newconnection");

    int tmp = clisock->fd() % m_threadnum;
    spconn conn = std::make_shared<Connection>(m_sublp[tmp].get(), std::move(clisock));
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_conns[conn->fd()] = conn;
    }
    m_sublp[tmp]->HandleNewConn(conn);
    conn->setOnMsgCallback([this](spconn conn, std::string &msg)
                           { this->handlemessage(conn, msg); });
    conn->setErrorCallback([this](spconn conn)
                           { this->errorconnection(conn); });
    conn->setCloseCallback([this](spconn conn)
                           { this->closeconnection(conn); });
    conn->setFinMsgCallback([this](spconn conn)
                            { this->sendfinish(conn); });
    if (m_newconncallback)
        m_newconncallback(conn);
}

void TcpServer::errorconnection(spconn conn)
{
    Logger::instance().log("TcpServer::errorconnection", 1);
    if (m_errorconncallback)
        m_errorconncallback(conn);
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_conns.erase(conn->fd());
    }
}

void TcpServer::closeconnection(spconn conn)
{
    Logger::instance().log("TcpServer::closeconnection", 1);
    if (m_closeconncallback)
        m_closeconncallback(conn);
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_conns.erase(conn->fd());
    }
}

void TcpServer::handlemessage(spconn conn, std::string &msg)
{
    Logger::instance().log("TcpServer::handlemessage");

    m_handlemsgcallback(conn, msg);
}

void TcpServer::sendfinish(spconn conn)
{
    Logger::instance().log("TcpServer::sendfinish");
    if (m_finsendcallback)
        m_finsendcallback(conn);
}

void TcpServer::epolltimeout(EventLoop *lp)
{
    Logger::instance().log("TcpServer::epolltimeout", 3);
    if (m_timeoutcallback)
        m_timeoutcallback(lp);
}