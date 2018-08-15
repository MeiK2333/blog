#include "request.h"

#include "logger.h"

Request::Request(char* buffer) {
    using namespace std;
    string body = buffer;
    string line;

    /* 解析 method, uri 以及 protocol */
    line = SplitLine(body);
    int pos, len;
    pos = line.find(" ");
    len = line.length();
    method_ = line.substr(0, pos);
    line = line.substr(pos + 1, len);

    pos = line.find(" ");
    len = line.length();
    uri_ = line.substr(0, pos);
    line = line.substr(pos + 1, len);

    protocol_ = line;

    /* 解析 headers 信息 */
    while ((line = SplitLine(body)) != "") {
        string key, value;
        pos = line.find(": ");
        len = line.length();
        key = line.substr(0, pos);
        value = line.substr(pos + 2, len);
        headers_[key] = value;
    }

    body_ = body;
}

Request::~Request() {}

std::string Request::GetMethod() { return method_; }

std::string Request::GetUri() { return uri_; }

std::string Request::GetProtocol() { return protocol_; }

std::string Request::GetHeader(std::string key) { return headers_[key]; }

/**
 * 以行为单位分割字符串
 * */
std::string Request::SplitLine(std::string& str) {
    int len;
    if ((len = str.find("\r\n")) != 0) {
        std::string str1 = str.substr(0, len);
        str = str.substr(len + 2, str.length());
        return str1;
    }
    return "";
}