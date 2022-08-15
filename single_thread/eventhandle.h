#ifndef _SIMPLE_REACTOR_EVENTHANDLE_H_
#define _SIMPLE_REACTOR_EVENTHANDLE_H_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <mutex>
#include <condition_variable>

namespace simplereactor
{
    class EventHandle
    {
    private:
        int m_epollfd;
        int m_socketfd;
        bool m_run;
        std::vector<std::thread> m_thread;
        std::mutex m_mtx;
        std::condition_variable m_cond; 
        std::vector<struct epoll_event> m_events;

    public:
        EventHandle();
        ~EventHandle();
        void setEvents(struct epoll_event event);
        void loop(const int& epollfd, const int& socketfd);
        void stop();
        void handleEvent();
        void handleRead(int fd);
    };
    
    EventHandle::EventHandle():
        m_run(false)
    {
    }
    
    EventHandle::~EventHandle()
    {
    }

    void EventHandle::setEvents(struct epoll_event event)
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_events.emplace_back(event);
    }

    void EventHandle::loop(const int& epollfd, const int& socketfd)
    {
        printf("EventHandle loop start\n");

        m_run = true;
        m_epollfd = epollfd;
        m_socketfd = socketfd;

        int cpu_num = std::max(std::thread::hardware_concurrency(), (unsigned int)1);
        m_thread.resize(cpu_num);
        printf("m_thread size=%d\n", static_cast<int>(m_thread.size()));
        for (int i = 0; i < m_thread.size(); ++i)
        {
            m_thread[i] = std::thread([this](){
                for (;;)
                {
                    struct epoll_event event;
                    memset(&event, 0, sizeof(event));

                    {
                        std::unique_lock<std::mutex> lck(m_mtx);
                        while (m_events.empty())
                        {
                            m_cond.wait(lck);
                            if (!m_run)
                            {
                                printf("EventHandle loop stop\n");
                                return;
                            }
                        }
                        event = *m_events.begin();
                        m_events.erase(m_events.begin());
                    }

                    if (event.events & EPOLLERR)
                    {
                        // Todo: deal with error
                    }
                    if (event.events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                    {
                        handleRead(event.data.fd);
                    }
                    if (event.events & EPOLLOUT)
                    {
                        // Todo: deal with out
                    }
                }
            });
        }
    }

    void EventHandle::stop()
    {
        m_run = false;
        m_cond.notify_all();
        for (auto& i : m_thread)
        {
            if (i.joinable())
                i.join();
        }
    }

    void EventHandle::handleEvent()
    {
        if (m_run)
            m_cond.notify_all();
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