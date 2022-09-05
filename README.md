# SimpleReactor

Linux Epoll 单线程和多线程网络Reactor模型

## 描述
简单的网络通信Reactor，实现了收到消息回发该条消息

## 技术
- Epoll多路复用IO
- 事件循环处理触发事件
- single 单Reactor单线程：
  主线程accept以及分发事件至eventhandle
- single_thread 单Reactor多线程：
  主线程accept以及分发事件至eventhandle，线程池处理
- C11新特性

## 使用
- cmake
- ./SimpleReactor.o -p[ip] -d[port]
- ./SimpleReactorThread.o -p[ip] -d[port]
- ./test.o -p[ip] -d[port]

## 后续改进点
- 包的协议化，防止粘包
- 对scoket封装继续