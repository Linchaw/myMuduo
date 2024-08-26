#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "Buffer.h"

#define PORT 8080
#define BUFFER_SIZE 128

class Buffer;

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    // 创建socket
    // if ((sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // 清理serv_addr结构体
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 将IP地址转换为二进制形式并赋值给serv_addr.sin_addr
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed: " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }

    int n = 3;
    std::string message;
    for (int i = 0; i < n; i++)
    {
        message = "这是第" + std::to_string(i) + "条消息"; // 一个汉字3字节
        char tmpbuf[BUFFER_SIZE] = {0};
        int len = message.length();
        memcpy(tmpbuf, &len, 4);
        memcpy(tmpbuf + 4, message.c_str(), len);
        send(sock, tmpbuf, len + 4, 0);
        std::cout << "Send:" << message << "-" << message.size() << std::endl;
    }

    std::string msg;
    char tmpbuf[BUFFER_SIZE];
    std::cout << "Prepare to recieve" << std::endl;
    for (int i = 0; i < n; i++) {
        int len=0;
        memset(tmpbuf, 0, sizeof(tmpbuf));  // 正确初始化 tmpbuf

        if (read(sock, &len, 4) > 0) {
            std::cout << "Recieve's len:" << len << std::endl;
            if (len > 0 && len < BUFFER_SIZE && read(sock, tmpbuf, len) == len) {
                msg.assign(tmpbuf, len);
                std::cout << "Recieve: " << msg << std::endl;
            }
        }
        else{
            std::cout << "ERROR--" << len << std::endl;
        }
    }

    sleep(10);

    // 关闭socket
    close(sock);

    return 0;
}
