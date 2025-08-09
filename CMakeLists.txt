cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(reactor
        src/main.cpp
        src/Server.cpp
        src/Socket.cpp
        src/Connection.cpp
        src/EventLoop.cpp
        src/InetAddress.cpp
        src/Channel.cpp
        src/Epoller.cpp
        src/Acceptor.cpp
        src/EventLoopThread.cpp
        src/EventLoopThreadPool.cpp
        src/ConnectionTimeoutManager.cpp
        src/Timer.cpp
        src/TimerQueue.cpp
        src/Buffer.cpp
        include/protocol/IProtocol.h
        src/LengthHeaderProtocol.cpp
        src/Codec.cpp
)

add_executable(test_client test_client.cpp
        src/Buffer.cpp
        src/Codec.cpp
        src/LengthHeaderProtocol.cpp)