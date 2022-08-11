#ifndef _SIMPLE_REACTOR_EVENTHANDLE_H_
#define _SIMPLE_REACTOR_EVENTHANDLE_H_

#include <vector>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

namespace simplereactor
{
    class EventHandle
    {
    private:
        int m_epollfd;
        int m_socketfd;
        std::vector<struct epoll_event> m_events;

    public:
        EventHandle();
        ~EventHandle();
        void setFd(const int& epollfd, const int& socketfd);
        void setEvents(struct epoll_event& event);
        void handleEvent();
    };
    
    EventHandle::EventHandle()
    {
    }
    
    EventHandle::~EventHandle()
    {
    }

    void EventHandle::setFd(const int& epollfd, const int& socketfd)
    {
        m_epollfd = epollfd;
        m_socketfd = socketfd;
    }

    void EventHandle::setEvents(struct epoll_event& event)
    {
        m_events.emplace_back(event);
    }

    void EventHandle::handleEvent()
    {
        int ret = 0;
        char buf[1024] = { 0 };
        int buf_size = 1024;

        for (auto &i : m_events)
        {
            if (i.events & EPOLLERR)
            {
                // Todo: deal with error
            }
            if (i.events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            {
                ret = ::recv(i.data.fd, buf, buf_size, 0);
                if (ret > 0)
                {
                    printf("recv buf=%s\n", buf);
                    ::send(i.data.fd, buf, buf_size, 0);
                    // Todo: 做好包的边界处理防止粘包，拆包
                }
                else
                {
                    ::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, i.data.fd, nullptr);
                    ::close(i.data.fd);
                    printf("close connfd=%d\n", i.data.fd);
                }
            }
            if (i.events & EPOLLOUT)
            {
                // Todo: deal with out
            }
        }
        
        m_events.clear();
    }
    
}

#endif