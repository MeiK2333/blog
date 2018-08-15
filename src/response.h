#ifndef BLOG_RESPONSE_H
#define BLOG_RESPONSE_H

#include <map>
#include <string>

class Response {
   public:
    Response();

   private:
    std::string httpStatus_;
    std::map<std::string, std::string> headers_;
};

#endif