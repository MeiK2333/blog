#include "produce.h"

Produce::Produce(char *request_buff) {
    request_ = new Request(request_buff);
    response_ = new Response();
}

Produce::~Produce() {
    delete request_;
    delete response_;
}

Response *Produce::Make() {
    if (request_->GetMethod() == "GET") {
    } else { /* 暂时无法处理其他类型的请求 */
    }
    return response_;
}