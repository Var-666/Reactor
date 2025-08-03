cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 20)

add_executable(reactor main.cpp Server.cpp Socket.cpp Connection.cpp EventLoop.cpp
        InetAddress.cpp
        Channel.cpp
        Epoller.cpp
        Acceptor.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
)

add_executable(test_client test_client.cpp)