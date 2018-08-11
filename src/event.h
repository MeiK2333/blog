#ifndef BLOG_EVENT_H
#define BLOG_EVENT_H

#include <sys/epoll.h>
#include <functional>

#define EPOLLEVENTS 10000

class Event {
   public:
    Event();
    ~Event();
    void loop();

   public:
    void setHandleReadFunc(std::function<void(int)>);
    void setHandleWriteFunc(std::function<void(int)>);
    void setHandleErrorFunc(std::function<void(int)>);
    void setHandleCloseFunc(std::function<void(int)>);

    void addEvent(int, int);
    void modifyEvent(int, int);
    void deleteEvent(int, int);

   private:
    void handleEvent(epoll_event*, int);

   private:
    bool errorFlag;
    int epollfd;

    std::function<void(int)> handleRead;
    std::function<void(int)> handleWrite;
    std::function<void(int)> handleError;
    std::function<void(int)> handleClose;
};

#endif