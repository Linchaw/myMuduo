#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <map>
#include <string>
#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

#include "ThreadPool.h"
#include "Connection.h"
#include "Acceptor.h"


class TcpServer
{
private:
    std::unique_ptr<EventLoop> m_lp;
    std::vector<std::unique_ptr<EventLoop>> m_sublp; 

    size_t m_threadnum;
    ThreadPool m_threadpool;
    
    Acceptor m_acc;
    std::mutex m_mtx;
    std::map<int, spconn> m_conns;

    std::function<void(spconn)> m_newconncallback;
    std::function<void(spconn)> m_errorconncallback;
    std::function<void(spconn)> m_closeconncallback;
    std::function<void(spconn, std::string&)> m_handlemsgcallback;
    std::function<void(spconn)> m_finsendcallback;
    std::function<void(EventLoop*)> m_timeoutcallback;

public:
    TcpServer(const std::string& ip, uint16_t port, size_t threadsnum);
    ~TcpServer();

    void Start();
    void Stop();
    void newconnection(std::unique_ptr<Socket> clisock); 
    void closeconnection(spconn conn);
    void errorconnection(spconn conn);
    void handlemessage(spconn conn, std::string& msg);
    void sendfinish(spconn conn);
    void epolltimeout(EventLoop* lp);

    void set_newconncallback(std::function<void(spconn)> fn) {m_newconncallback = std::move(fn);}
    void set_errorconncallback(std::function<void(spconn)> fn) {m_errorconncallback = std::move(fn);}
    void set_closeconncallback(std::function<void(spconn)> fn) {m_closeconncallback = std::move(fn);}
    void set_handlemsgcallback(std::function<void(spconn, std::string&)> fn) {m_handlemsgcallback = std::move(fn);}
    void set_finsendcallback(std::function<void(spconn)> fn) {m_finsendcallback = std::move(fn);}
    void set_timeoutcallback(std::function<void(EventLoop*)> fn) {m_timeoutcallback = std::move(fn);}
};

#endif // TCPSERVER_H