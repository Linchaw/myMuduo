#include "Acceptor.h"


Acceptor::Acceptor(EventLoop* lp, const std::string& ip, uint16_t port)
        :m_lp(lp), 
        m_sock(createfd()), 
        m_servch(m_lp, m_sock.fd())
{   
    InetAddress server_addr(ip, port);

    m_sock.setreuseaddr(1);
    m_sock.bindInetAddr(server_addr);
    m_sock.listen();
    m_servch.setReadCallback([this](){ this->newconnection(); });
    m_servch.enablereading();
}


void Acceptor::newconnection()
{
    InetAddress clientaddr;
    int conn_fd;
    while ((conn_fd = m_sock.accept(clientaddr)) > 0)
    {
        std::unique_ptr<Socket> clisock = std::make_unique<Socket>(conn_fd);
        clisock->bindAddr(clientaddr);
        m_newconnectioncb(std::move(clisock));
    }
}