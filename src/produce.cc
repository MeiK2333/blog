#include "produce.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <cstring>

#include "logger.h"

Produce::Produce(char *request_buff) {
    request_ = new Request(request_buff);
    response_ = new Response();
}

Produce::~Produce() {
    delete request_;
    delete response_;
}

void Produce::Make() {
    if (request_->GetMethod() == "GET") { /* 暂时只能处理静态文件 */
        struct stat st;
        const char *tmp;
        char path[BUFSIZ];

        tmp = request_->GetUri().substr(1, request_->GetUri().length()).c_str();
        strcpy(path, tmp);

        /* 判断文件是否存在 */
        if (stat(path, &st) == -1) {
            response_->SetStatusCode(404);
            return;
        }

        /* 判断文件是否为一个目录 */
        if ((st.st_mode & S_IFMT) == S_IFDIR) {
            /* 无法显示目录(其实应该在配置中设置的...) */
            response_->SetStatusCode(403);
            return;
        }

        /* 判断文件的上次修改时间 */
        time_t timer;
        char buf[BUFSIZ];
        strftime(buf, BUFSIZ, "%a, %d %b %Y %T %Z", localtime(&(st.st_mtime)));
        response_->SetHeader("Last-Modified", std::string(buf));
        /* 如果自上次访问过后没有过修改 */
        if (request_->GetHeader("If-Modified-Since") == std::string(buf)) {
            response_->SetStatusCode(304);
            return;
        }

        int file_size = st.st_size;
        response_->SetHeader("Content-Length", std::to_string(file_size));

        response_->offset = 0;
        response_->len = file_size;
        /* 判断有无 Range */
        if (request_->GetHeader("Range") != "") {
            std::string str1, str2, range = request_->GetHeader("range");
            int pos = range.find("-"), len = range.length();
            str1 = range.substr(0, pos);
            str2 = range.substr(pos + 1, len);
            response_->SetStatusCode(206);
            if (str1 != "") {
                response_->offset = std::stoi(str1);
            }
            if (str2 != "") {
                response_->len = std::stoi(str2);
            }
            response_->SetHeader(
                "Content-Range",
                "bytes " + range + "/" + std::to_string(file_size));
        } else {
            response_->SetStatusCode(200);
            response_->SetHeader("Accept-Ranges", "bytes");
        }
        int file_fd;
        if ((file_fd = open(path, O_RDONLY)) == -1) {
            Logger::DEBUG("open %s failure", path);
            response_->SetStatusCode(403);
            return;
        }
        if ((response_->buf = (char *)mmap(0, response_->len, PROT_READ, 0,
                                           file_fd, 0)) == MAP_FAILED) {
            Logger::WARNING("mmap failure");
            response_->SetStatusCode(500);
        }
    } else { /* 暂时无法处理其他类型的请求 */
        response_->SetStatusCode(405);
        return;
    }
}

Response *Produce::GetResponse() { return response_; }