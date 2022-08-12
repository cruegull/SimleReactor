# 基于LinuxIO多路复用Epoll的简单Reactor模型C++实现

## 描述
    1.single 单Reactor单线程：
        主线程accept以及分发事件至eventhandle
    2.single_thread 单Reactor多线程：
        主线程accept以及分发事件至eventhandle，线程池处理
    3.基于C11实现，收到消息后回发该条消息
    4.Cmake后 option -p[监听ip] -d[监听端口]
    5.test实现了收发测试 option -p[连接ip] -d[连接端口]

## 缺点
    1.未实现包的边界处理，在对一个fd不停收取时会出现粘包现象
    2.没有并发测试