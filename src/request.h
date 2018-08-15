#ifndef REQUEST_H
#define REQUEST_H

#include <map>
#include <string>

class Request {
   public:
    Request(char *);
    ~Request();

    std::string GetMethod();
    std::string GetUri();
    std::string GetProtocol();
    std::string GetBody();
    std::string GetHeader(std::string);

   private:
    std::string method_;
    std::string uri_;
    std::string protocol_;
    std::string body_;
    std::map<std::string, std::string> headers_;

    std::string SplitLine(std::string &);
};

#endif