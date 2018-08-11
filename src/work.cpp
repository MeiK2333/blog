#include "work.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

#include "logger.h"

Work::Work(int listenfd) {
    this->listenfd = listenfd;
    this->event = new Event();
}

Work::~Work() { delete this->event; }

void Work::start() {
    this->event->addEvent(this->listenfd, EPOLLIN);

    std::function<void(int)> func;
    func = std::bind(&Work::handleRead, this, std::placeholders::_1);
    this->event->setHandleReadFunc(func);

    this->event->loop();
}

/**
 * 描述符可读的事件
 * */
void Work::handleRead(int fd) {
    /* 判断是否是新的连接 */
    if (fd == this->listenfd) {
        this->handleAccept(fd);
    } else {
    }
}

/**
 * 尝试接收连接并将读取客户端数据的事件添加到 epoll 中
 * */
void Work::handleAccept(int fd) {
    int clientfd;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    clientfd = accept(listenfd, (struct sockaddr *)&clientAddr, &clientAddrLen);

    if (clientfd == -1) {
        Logger::warning("accept failure!");
        return;
    } else {
        Logger::debug("accept a new client: " +
                      std::string(inet_ntoa(clientAddr.sin_addr)) + ":" +
                      std::to_string(clientAddr.sin_port));
        this->event->addEvent(clientfd, EPOLLIN);
    }
}