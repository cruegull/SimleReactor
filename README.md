# 基于LinuxIO多路复用Epoll的简单Reactor模型C++实现

## single 单Reactor单线程
    主线程accept以及分发事件至eventhandle

## single_thread 单Reactor多线程
    主线程accept以及分发事件至eventhandle，线程池处理

## 描述
    基于C11实现，收到消息后回发该条消息
    Cmake后 option -p[监听ip] -d[监听端口]
    test实现了收发测试 option -p[连接ip] -d[连接端口]