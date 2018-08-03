#ifndef REQUEST_H
#define REQUEST_H

#include <map>
#include <string>

class Request {
   public:
   private:
    int requestSock;
    std::string method;
    std::string uri;
    std::map<std::string, std::string> headers;
};

#endif