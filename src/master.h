#ifndef BLOG_MASTER_H
#define BLOG_MASTER_H

#include "config.h"

class Master {
   public:
    Master(Config *);
    void startup();

   private:
    int socket_bind();

   private:
    Config *config;
    pid_t *children;
};

#endif