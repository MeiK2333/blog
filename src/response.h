#ifndef BLOG_RESPONSE_H
#define BLOG_RESPONSE_H

#include <map>
#include <string>

class Response {
   public:
   private:
    int responseSock;
    std::string httpStatus;
    std::map<std::string, std::string> headers;
};

#endif