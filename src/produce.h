#ifndef BLOG_PRODUCE_H
#define BLOG_PRODUCE_H

#include "request.h"
#include "response.h"

class Produce {
   public:
    Produce(char *);
    ~Produce();
    Response *Make();

   private:
    Request *request_;
    Response *response_;
};

#endif