#include <string>
#include <signal.h>

#include "EchoServer.h"
#include "Logger.h"

EchoServer* echoserver;
void sig_handler(int signo)
{
    delete echoserver;
    Logger::instance().log("receive signal:" + std::to_string(signo));
    exit(0);
}

int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    std::string ip = "127.0.0.1"; 
    echoserver = new EchoServer(ip,8080,1UL,0UL);
    echoserver->Start();
    return 0;
}