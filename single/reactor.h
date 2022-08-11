#ifndef _SIMPLE_REACTOR_REACTOR_H_
#define _SIMPLE_REACTOR_REACTOR_H_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <thread>
#include <vector>
#include <memory>


#include "eventhandle.h"

namespace simplereactor
{
    class Reactor
    {
    private:
        int m_socketfd;
        int m_epollfd;
        bool m_run;
        std::unique_ptr<EventHandle> m_event_handle;

    public:
        Reactor();
        ~Reactor();
        bool create(const char* ip, short port);
        void setConnfdNoBlock(int& conndfd);
        void loop();
        void stop();
    };
    
    Reactor::Reactor():
        m_socketfd(0),
        m_epollfd(0),
        m_run(false),
        m_event_handle(new EventHandle)
    {
    }
    
    Reactor::~Reactor()
    {
        ::close(m_socketfd);
        ::close(m_epollfd); // 内核会自动关闭，但显示关闭最好
        printf("close all fd\n");
    }
    
    bool Reactor::create(const char* ip, short port)
    {
        sockaddr_in service_addr;
        memset(&service_addr, 0, sizeof(service_addr));
        service_addr.sin_family = AF_INET;
        service_addr.sin_addr.s_addr = inet_addr(ip);
        service_addr.sin_port = htons(port);

        if ((m_socketfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) == -1)
        {
            printf("create err:%s(errno=%d)\n", strerror(errno), errno);
            return false;
        }
        
        int on = 1;
        ::setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
        ::setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

        if (::bind(m_socketfd, (sockaddr *)&service_addr, sizeof(service_addr)) == -1)
        {
            printf("bind err:%s(errno=%d)\n", strerror(errno), errno);
            return false;
        }

        if (::listen(m_socketfd, SOMAXCONN) == -1)
        {
            printf("listen err:%s(errno=%d)\n", strerror(errno), errno);
            return false;
        }

        m_epollfd = epoll_create(EPOLL_CLOEXEC);
        struct epoll_event event;
        event.data.fd = m_socketfd;
        event.events = EPOLLIN | EPOLLRDHUP;
        if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_socketfd, &event) == -1)
        {
            printf("epoll_ctl err\n");
            return false;
        }
        m_event_handle->setFd(m_epollfd, m_socketfd);
        printf("Reactor create success\n");
        return true;
    }

    void Reactor::setConnfdNoBlock(int& conndfd)
    {
        int old_flag = ::fcntl(conndfd, F_GETFL, 0);
        int new_flag = old_flag | O_NONBLOCK;
        fcntl(conndfd, F_SETFL, new_flag);
    }

    void Reactor::loop()
    {
        printf("Reactor loop start\n");

        m_run = true;

        const int events_size = 1024;
        struct epoll_event events[events_size];
        memset(events, 0, sizeof(events));

        char buf[1024] = { 0 };
        int buf_size = 1024;
        int read_size = 0;

        for (;;)
        {
            if (!m_run) break;;

            int nready = ::epoll_wait(m_epollfd, events, events_size, 0);
            if (nready <= 0) continue;

            for (int i = 0; i < events_size; ++i)
            {
                if (events[i].data.fd == m_socketfd)
                {
                    struct sockaddr_in client;
                    socklen_t len = sizeof(client);
                    int connfd = 0;
                    if ((connfd = ::accept(m_socketfd, (struct sockaddr *)&client, &len)) == -1) {
                        printf("accept err:%s(errno=%d)\n", strerror(errno), errno);
                        continue;
                    }
                    
                    setConnfdNoBlock(connfd);

                    struct epoll_event event;
                    event.data.fd = connfd;
                    event.events = EPOLLIN;
                    ::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, connfd, &event);
                    printf("connect=%d\n", connfd);
                }
                else
                {
                    m_event_handle->setEvents(events[i]);
                }
            }
            
            m_event_handle->handleEvent();
        }

        printf("Reactor loop stop\n");
    }

    void Reactor::stop()
    {
        m_run = false;
    }
}

#endif