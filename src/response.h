#ifndef BLOG_RESPONSE_H
#define BLOG_RESPONSE_H

#include <map>
#include <string>

class Response {
   public:
    Response();
    ~Response();
    void SetStatusCode(int);
    int GetStatusCode();
    void SetHeader(std::string, std::string);
    std::string GetHeader(std::string);
    char header_buf[BUFSIZ];
    size_t header_len;
    void MakeHeader();
    char *buf;
    size_t offset;
    size_t len;
    void Dump();

   private:
    int status_code_;
    std::map<std::string, std::string> headers_;
};

#endif