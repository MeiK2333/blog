#ifndef BLOG_READ_BUFFER_H
#define BLOG_READ_BUFFER_H

class ReadBuffer {
   public:
    ReadBuffer(int);
    ~ReadBuffer();
    int Read();
    char *buffer;
    bool End();

   private:
    int fd;
    int offset;
    int size;

    void resize(int);
};

#endif