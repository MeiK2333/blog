#include "response.h"

#include <sys/mman.h>
#include <cstring>

#include "logger.h"

Response::Response() {
    /* 如果这个 status code 没有被改变, 说明服务器内部发生了错误 */
    status_code_ = 500;
    this->buf = NULL;
    this->offset = this->len = this->header_len = 0;
}

Response::~Response() {
    if (this->buf != NULL) {
        munmap(this->buf, this->len);
        // free(this->buf);
    }
}

/**
 * 设置状态码
 * */
void Response::SetStatusCode(int code) { status_code_ = code; }

/**
 * 获得状态码
 * */
int Response::GetStatusCode() { return status_code_; }

/**
 * 设置 header 信息
 * */
void Response::SetHeader(std::string key, std::string value) {
    headers_[key] = value;
}

/**
 * 获得 header 信息
 * */
std::string Response::GetHeader(std::string key) { return headers_[key]; }

/**
 * 将 map 中的 headers 拼接为字符串并根据不同的 status code 给定不同的返回信息
 * */
void Response::MakeHeader() {
    switch (status_code_) {
        case 200:
            sprintf(header_buf, "HTTP/1.0 200 OK\r\n");
            break;
        case 206:
            sprintf(header_buf, "HTTP/1.0 206 Partial Content\r\n");
            break;
        case 304:
            sprintf(header_buf, "HTTP/1.0 304 Not Modified\r\n");
            break;
        case 403:
            sprintf(header_buf, "HTTP/1.0 200 OK\r\n");
            break;
        case 404:
            sprintf(header_buf, "HTTP/1.0 404 NOT FOUND\r\n");
            break;
        case 405:
            sprintf(header_buf, "HTTP/1.0 405 Method Not Allowed\r\n");
            break;
        case 500:
            sprintf(header_buf, "HTTP/1.0 Internal Server Error\r\n");
            break;
    }
    header_len = strlen(header_buf);
    for (auto &it : headers_) {
        sprintf(header_buf + header_len, "%s: %s\r\n", it.first.c_str(),
                it.second.c_str());
        header_len = strlen(header_buf);
    }
    sprintf(header_buf + header_len, "\r\n");
    header_len = strlen(header_buf);
}

void Response::Dump() {
    Logger::DEBUG("Status Code: %d", status_code_);
    for (auto &it : headers_) {
        Logger::DEBUG("%s: %s", it.first.c_str(), it.second.c_str());
    }
}