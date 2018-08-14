#include "work.h"

#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <functional>

#include "logger.h"

Work::Work(int listenfd) {
    this->listenfd = listenfd;
    this->event = new Event();
    this->buffer_tree_ = new ReadBufferTree();
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
        int len;
        len = this->buffer_tree_->Find(fd)->reader->Read();
        if (len == -1) {
            Logger::WARNING("read failure!");
            this->event->deleteEvent(fd, EPOLLIN);
        } else if (len == 0 || len != MAXBUFFER) {
            this->event->deleteEvent(fd, EPOLLIN);
            write(fd,
                  "HTTP/1.0 200 OK\r\nContent-Type: "
                  "text/html;charset=utf-8\r\n\r\nHello World!\r\n",
                  70);
            close(fd);
        }
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
        Logger::WARNING("accept failure!");
        return;
    } else {
        Logger::DEBUG("accept a new client: %s:%d",
                      inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
        /* 添加描述符监听, 插入 buffer_tree 中 */
        this->event->addEvent(clientfd, EPOLLIN);
        this->buffer_tree_->Insert(clientfd);
    }
}