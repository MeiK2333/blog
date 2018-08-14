#include "read_buffer.h"

#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include "logger.h"

ReadBuffer::ReadBuffer(int fd) {
    this->fd = fd;
    this->offset = 0;
    this->buffer = (char *)malloc(MAXBUFFER * sizeof(char *));
    this->size = MAXBUFFER;
}

ReadBuffer::~ReadBuffer() { free(this->buffer); }

/**
 * 从描述符中读取数据并存入缓冲区
 * */
int ReadBuffer::Read() {
    if (this->offset >= this->size) {
        this->resize(this->size * 2);
    }
    int len;
    len = read(this->fd, this->buffer + offset, MAXBUFFER);
    if (len > 0) {
        offset += len;
    }
    return len;
}

/**
 * 判断 read 是否完成
 * */
bool ReadBuffer::End() {
    return strcmp(this->buffer + offset - 4, "\r\n\r\n") == 0;
}

/**
 * 扩大 buffer
 * */
void ReadBuffer::resize(int size) {
    char *tmp = (char *)realloc(this->buffer, size * sizeof(char *));
    if (tmp) {
        this->buffer = tmp;
        this->size = size;
    } else {
        Logger::WARNING("resize failure");
    }
}