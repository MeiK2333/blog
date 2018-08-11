#include "event.h"

#include <unistd.h>

#include "logger.h"

Event::Event() {
    /* 在较新版本内核的 Linux 中, 这个参数毫无意义 */
    this->epollfd = epoll_create(2333);
    this->errorFlag = false;
}

Event::~Event() { close(this->epollfd); }

/**
 * 开始 epoll 循环
 * */
void Event::loop() {
    struct epoll_event events[EPOLLEVENTS];
    int len;
    while (!errorFlag) {
        len = epoll_wait(this->epollfd, events, EPOLLEVENTS, -1);
        this->handleEvent(events, len);
    }
    close(this->epollfd);
}

/**
 * 判断事件类型并触发对应函数
 * */
void Event::handleEvent(epoll_event* events, int len) {
    int i, fd;
    for (i = 0; i < len; i++) {
        fd = events[i].data.fd;
        /* 判断事件类型 */
        if (events[i].events & EPOLLIN) {
            this->handleRead(fd);
        } else if (events[i].events & EPOLLOUT) {
            this->handleWrite(fd);
        } else if (events[i].events & EPOLLRDHUP) {
            this->handleClose(fd);
        } else if (events[i].events & EPOLLERR) {
            this->handleError(fd);
        } else {
            this->handleError(fd);
            Logger::error("Unknown epoll type " +
                          std::to_string(events[i].events));
        }
    }
}

/**
 * 设置对应事件的回调函数
 * */
void Event::setHandleReadFunc(std::function<void(int)> handleRead) {
    this->handleRead = handleRead;
}
void Event::setHandleWriteFunc(std::function<void(int)> handleWrite) {
    this->handleWrite = handleWrite;
}
void Event::setHandleErrorFunc(std::function<void(int)> handleError) {
    this->handleError = handleError;
}
void Event::setHandleCloseFunc(std::function<void(int)> handleClose) {
    this->handleClose = handleClose;
}

/**
 * 为 epoll 添加、修改或者删除指定描述符的监听
 * */
void Event::addEvent(int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    epoll_ctl(this->epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Event::modifyEvent(int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    epoll_ctl(this->epollfd, EPOLL_CTL_MOD, fd, &ev);
}
void Event::deleteEvent(int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    epoll_ctl(this->epollfd, EPOLL_CTL_DEL, fd, &ev);
}