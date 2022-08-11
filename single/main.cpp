#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <functional>
#include <string>

#include "reactor.h"

#define PROGRAM_ERROR   -1
#define PROGRAM_NORMAL   0

std::function<void()> stop;

void signHandler(int signo)
{
    printf("\nrevice signo=%d, exit\n", signo);
    if (stop)
    {
        stop();
    }
}

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
    printf("listen ip = %s port = %d\n", ip.c_str(), port);

    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signHandler);
    //signal(SIGKILL, signHandler); // 不能处理或者忽略以及接受后清理
    signal(SIGTERM, signHandler);

    simplereactor::Reactor reactor;
    if (!reactor.create(ip.c_str(), port))
    {
        printf("reactor create error\n");
        return 0;
    }
    stop = [&reactor]() {
        reactor.stop();
    };
    reactor.loop();

    return 0;
}