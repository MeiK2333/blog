#ifndef BLOG_WORK_H
#define BLOG_WORK_H

#include "event.h"
#include "read_buffer_tree.h"

class Work {
   public:
    Work(int listenfd);
    ~Work();
    void start();

   public:
    void handleRead(int);
    void handleWrite(int);
    void handleClose(int);
    void handleError(int);
    void handleAccept(int);

   private:
    int listenfd;
    Event* event;
    ReadBufferTree* buffer_tree_;
};

#endif