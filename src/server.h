#ifndef BLOG_SERVER_H
#define BLOG_SERVER_H

#include "config.h"

class Server {
   public:
    Server(Config* config);

   private:
    Config* config;
};

#endif