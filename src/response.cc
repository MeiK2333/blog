#include "response.h"

#include "logger.h"

Response::Response() {
    /* 如果这个 status code 没有被改变, 说明服务器内部发生了错误 */
    status_code_ = 500;
    this->buf = NULL;
    this->offset = this->len = 0;
}

Response::~Response() {
    if (this->buf != NULL) {
        free(this->buf);
    }
}

/**
 * 设置状态码
 * */
void Response::SetStatusCode(int code) { status_code_ = code; }

/**
 * 设置 header 信息
 * */
void Response::SetHeader(std::string key, std::string value) {
    headers_[key] = value;
}

void Response::Dump() {
    Logger::DEBUG("Status Code: %d", status_code_);
    for (auto &it : headers_) {
        Logger::DEBUG("%s: %s", it.first.c_str(), it.second.c_str());
    }
}