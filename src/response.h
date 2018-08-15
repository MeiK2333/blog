#ifndef BLOG_RESPONSE_H
#define BLOG_RESPONSE_H

#include <map>
#include <string>

class Response {
   public:
    Response();
    void SetStatusCode(int);
    void SetHeader(std::string, std::string);
    char *buf;
    size_t offset;
    size_t len;
    void Dump();

   private:
    int status_code_;
    std::map<std::string, std::string> headers_;
};

#endif