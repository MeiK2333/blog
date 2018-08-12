#ifndef BLOG_READ_BUFFER_H
#define BLOG_READ_BUFFER_H

#define MAXBUFFER 1024

class ReadBuffer {
   public:
    ReadBuffer(int);
    ~ReadBuffer();
    int readFd();

   private:
    int fd;
    int offset;
    int size;
    char *buffer;

    void resize(int);
};

#endif