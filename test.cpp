#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define PROGRAM_ERROR   -1
#define PROGRAM_NORMAL   0

int main(int argc, char* argv[])
{
    int ch = 0;
    std::string ip = "0";
    short port = 0;
    while ((ch = getopt(argc, argv, "p:d:")) != -1)
    {
        switch (ch)
        {
        case 'p':
            ip = optarg;
            break;
        case 'd':
            port = atol(optarg);
            break;
        default:
            printf("options error\n");
            exit(PROGRAM_ERROR);
            break;
        }
    }
    assert(ip != "0");
    assert(port != 0);
    printf("connect ip = %s port = %d\n", ip.c_str(), port);

    int socketfd = 0;
    if ((socketfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        printf("create socket err:%s(errno=%d)\n", strerror(errno), errno);
        return false;
    }

    sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    service_addr.sin_port = htons(port);
    if (::connect(socketfd, (sockaddr*)&service_addr, sizeof(service_addr)) == -1)
    {
        printf("connect socket err:%s(errno=%d)\n", strerror(errno), errno);
        return false;
    }

    ssize_t ret = 0;
    char buf[] = "这是一条测试消息";
    if ((ret = ::send(socketfd, buf, sizeof(buf), 0)) <= 0)
    {
        printf("send buf err:%s\n", buf);
        return false;
    }
    printf("send success:%s\n", buf);
    
    if ((ret = ::recv(socketfd, buf, sizeof(buf), 0)) <= 0)
    {
        printf("recv buf err:%s\n", buf);
        return false;
    }
    printf("recv success:%s\n", buf);

    return 0;
}