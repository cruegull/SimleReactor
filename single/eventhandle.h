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
        void setEvents(struct epoll_event event);
        void handleEvent();
        void handleRead(int fd);
    };
    
    EventHandle::EventHandle():
        m_epollfd(0),
        m_socketfd(0)
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

    void EventHandle::setEvents(struct epoll_event event)
    {
        m_events.emplace_back(event);
    }

    void EventHandle::handleEvent()
    {
        for (auto &i : m_events)
        {
            if (i.events & EPOLLERR)
            {
                // Todo: deal with error
            }
            else if (i.events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            {
                handleRead(i.data.fd);
            }
            else if (i.events & EPOLLOUT)
            {
                // Todo: deal with out
            }
        }
        m_events.clear();
    }

    void EventHandle::handleRead(int fd)
    {
        char buf[1024] = { 0 };
        for (;;)
        {
            ssize_t ret = ::recv(fd, buf, sizeof(buf), 0);
            if (ret > 0)
            {
                printf("recv buf=%s\n", buf);
                ::send(fd, buf, sizeof(buf), 0);
            }
            else if(ret == -1 && errno == EINTR)
            {
                continue;
            }
            else if(ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            {
                break;
            }
            else if(ret == 0)
            {
                printf("close fd=%d\n", fd);
                ::close(fd);
                break;
            }
        }
    }
    
}

#endif