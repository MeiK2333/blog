#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "urlcode.c"

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: blog/0.1.0\r\n"

void err_exit(const char *);
static void sig_cld(int);
int startup(u_short *);
size_t get_line(int, char *, size_t);
void accept_request(int);
void response_method_not_allowed(int, char *);
void response_not_found(int);
void response_dir_list(int, char *, char *);
void response_file(int, char *);
void content_type(char *, char *);
void cat(int, char *);

/**
 * 将本地文件内容输出到网络套接字
 * */
void cat(int client, char *path) {
    char buf[1024];
    FILE *resource;
    if ((resource = fopen(path, "r")) == NULL) {
        err_exit("fopen error");
    }

    if (fgets(buf, sizeof(buf), resource) != NULL) {
        send(client, buf, strlen(buf), 0);
    }
    fclose(resource);
}

/**
 * 返回文件
 * */
void response_file(int client, char *path) {
    char buf[1024], cbuf[64];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    content_type(path, cbuf);
    sprintf(buf, "Content-type: %s\r\n", cbuf);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    cat(client, path);
}

/**
 * 请求类型不支持
 * */
void response_method_not_allowed(int client, char *method) {
    char buf[1024];

    sprintf(buf, "HTTP/1.0 405 Method Not Allowed\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html;charset=utf-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Allow: GET\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<html><title>405 Method Not Allowed</title>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<body bgcolor=\"white\">\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<center><h1>405 Method Not Allowed</h1></center>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<hr><center>blog/0.1.0</center>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</body></html>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**
 * 返回目录
 * */
void response_dir_list(int client, char *path, char *url) {
    DIR *dp;
    char buf[1024];
    struct dirent *dirp;

    if ((dp = opendir(path)) == NULL) {
        err_exit("opendir error");
    }

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html;charset=utf-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<html><title>%s</title>\r\n", url);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<body bgcolor=\"white\">\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<h1>Index of %s</h1><hr><pre>", url);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<a href=\"../\">../</a>\n");
    send(client, buf, strlen(buf), 0);

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
            continue;
        }
        sprintf(buf, "<a href=\"%s/%s\">%s</a>\n", url, dirp->d_name,
                dirp->d_name);
        send(client, buf, strlen(buf), 0);
    }

    sprintf(buf, "</pre><hr></body></html>\r\n");
    send(client, buf, strlen(buf), 0);

    if (closedir(dp) < 0) {
        err_exit("close dir error");
    }
}

/**
 * 返回 404
 * */
void response_not_found(int client) {
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html;charset=utf-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<html><title>404 Not Found</title>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<body bgcolor=\"white\">\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<center><h1>404 Not Found</h1></center>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<hr><center>blog/0.1.0</center>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</body></html>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**
 * 获取请求的类型并根据类型对应处理
 * */
void accept_request(int client_sock) {
    char buf[1024];
    char method[512];
    char url[512];
    char path[512];
    size_t size;
    size_t i, j;
    char *status_code;
    struct stat st;

    /* e.g. "GET /index.html HTTP/1.1" */
    size = get_line(client_sock, buf, sizeof(buf));

    i = 0;
    /* 解析 method */
    for (j = 0; !ISspace(buf[i]) && (j < sizeof(method) - 1); i++, j++) {
        method[j] = buf[i];
    }
    method[j] = '\0';

    /* 解析 URL */
    while (ISspace(buf[i]) && (i < size)) {
        i++;
    }
    for (j = 0; !ISspace(buf[i]) && (j < sizeof(url) - 1); i++, j++) {
        url[j] = buf[i];
    }
    url[j] = '\0';

    /* URL 转码 */
    urldecode(url);

    /* 其他头信息没有用 */
    while ((size > 0) && strcmp("\n", buf)) {
        size = get_line(client_sock, buf, sizeof(buf));
    }
    /* 仅支持 GET 请求 */
    if (strcmp(method, "GET")) {
        response_method_not_allowed(client_sock, method);
        status_code = "405 Method Not Allowed";
        close(client_sock);
        printf("%s %s %s\n", method, url, status_code);
        return;
    }

    sprintf(path, "blog%s", url);
    if (path[strlen(path) - 1] == '/') {
        strcat(path, "index.html");
    }
    /* 判断文件是否存在 */
    if (stat(path, &st) == -1) {
        /* 如果不存在 */
        response_not_found(client_sock);
        status_code = "404 Not Found";
    } else {
        /* 如果请求的是一个目录, 则返回其文件列表 */
        if ((st.st_mode & S_IFMT) == S_IFDIR) {
            response_dir_list(client_sock, path, url);
            status_code = "200 OK";
        } else { /* 如果是一个文件, 则根据直接返回文件内容 */
            response_file(client_sock, path);
            status_code = "200 OK";
        }
    }

    close(client_sock);
    printf("%s %s %s\n", method, url, status_code);
}

/**
 * 从网络套接字中读取一行, 返回读取成功的长度
 * */
size_t get_line(int sock, char *buf, size_t size) {
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        if (n > 0) {
            if (c == '\r') {
                /* 查看但不取走 */
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n')) {
                    recv(sock, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';

    return (i);
}

/**
 * 打印(致命的)错误信息并退出
 * */
void err_exit(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

/**
 * 监听子进程变化事件, 清理僵死进程
 * */
static void sig_cld(int signo) {
    if (signal(SIGCHLD, sig_cld) == SIG_ERR) {
        err_exit("signal error");
    }
    pid_t pid;
    if ((pid = wait(NULL)) < 0) {
        err_exit("wait error");
    }
    // printf("PID %d ended\n", pid);
}

/**
 * 设置监听套接字连接, 设置端口监听
 * */
int startup(u_short *port) {
    int httpd = 0;
    int on = 1;
    struct sockaddr_in name;

    if ((httpd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        err_exit("socket error");
    }

    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        err_exit("setsockopt error");
    }

    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        err_exit("bind error");
    }

    if (*port == 0) {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1) {
            err_exit("getsockname error");
        }
        *port = ntohs(name.sin_port);
    }

    if (listen(httpd, 5) < 0) {
        err_exit("listen error");
    }

    return (httpd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }
    pid_t pid = -1;
    u_short port = 0;
    int server_sock = -1;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

    if (signal(SIGCHLD, sig_cld) == SIG_ERR) {
        err_exit("signal error");
    }

    port = atoi(argv[1]);
    /**
     * 设置开始监听端口
     * */
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);

    /**
     * 循环监听连接请求并分发到子进程处理
     * */
    while (1) {
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_name,
                                  &client_name_len)) == -1) {
            err_exit("accept error");
        }

        if ((pid = fork()) < 0) {
            err_exit("fork error");
        } else if (pid == 0) {
            printf("%s ", inet_ntoa(client_name.sin_addr));
            accept_request(client_sock);
            exit(0);
        } else {
            // printf("PID %d start\n", pid);
            close(client_sock);
        }
    }
    return 0;
}

#define check_type(x, y, z, buf) \
    {                            \
        if (strcmp(x, y) == 0) { \
            strcpy(buf, z);      \
            return;              \
        }                        \
    }

/**
 * 查找文件对应的 Content-Type
 * */
void content_type(char *path, char *buf) {
    buf[0] = '\0';
    char *type;
    type = path + strlen(path);
    while ((*type) != '.' && (type > path)) {
        type--;
    }

    check_type(type, ".tif", "image/tiff", buf);
    check_type(type, ".001", "application/x-001", buf);
    check_type(type, ".301", "application/x-301", buf);
    check_type(type, ".323", "text/h323", buf);
    check_type(type, ".906", "application/x-906", buf);
    check_type(type, ".907", "drawing/907", buf);
    check_type(type, ".a11", "application/x-a11", buf);
    check_type(type, ".acp", "audio/x-mei-aac", buf);
    check_type(type, ".ai", "application/postscript", buf);
    check_type(type, ".aif", "audio/aiff", buf);
    check_type(type, ".aifc", "audio/aiff", buf);
    check_type(type, ".aiff", "audio/aiff", buf);
    check_type(type, ".anv", "application/x-anv", buf);
    check_type(type, ".asa", "text/asa", buf);
    check_type(type, ".asf", "video/x-ms-asf", buf);
    check_type(type, ".asp", "text/asp", buf);
    check_type(type, ".asx", "video/x-ms-asf", buf);
    check_type(type, ".au", "audio/basic", buf);
    check_type(type, ".avi", "video/avi", buf);
    check_type(type, ".awf", "application/vnd.adobe.workflow", buf);
    check_type(type, ".biz", "text/xml", buf);
    check_type(type, ".bmp", "application/x-bmp", buf);
    check_type(type, ".bot", "application/x-bot", buf);
    check_type(type, ".c4t", "application/x-c4t", buf);
    check_type(type, ".c90", "application/x-c90", buf);
    check_type(type, ".cal", "application/x-cals", buf);
    check_type(type, ".cat", "application/vnd.ms-pki.seccat", buf);
    check_type(type, ".cdf", "application/x-netcdf", buf);
    check_type(type, ".cdr", "application/x-cdr", buf);
    check_type(type, ".cel", "application/x-cel", buf);
    check_type(type, ".cer", "application/x-x509-ca-cert", buf);
    check_type(type, ".cg4", "application/x-g4", buf);
    check_type(type, ".cgm", "application/x-cgm", buf);
    check_type(type, ".cit", "application/x-cit", buf);
    check_type(type, ".class", "java/*", buf);
    check_type(type, ".cml", "text/xml", buf);
    check_type(type, ".cmp", "application/x-cmp", buf);
    check_type(type, ".cmx", "application/x-cmx", buf);
    check_type(type, ".cot", "application/x-cot", buf);
    check_type(type, ".crl", "application/pkix-crl", buf);
    check_type(type, ".crt", "application/x-x509-ca-cert", buf);
    check_type(type, ".csi", "application/x-csi", buf);
    check_type(type, ".css", "text/css", buf);
    check_type(type, ".cut", "application/x-cut", buf);
    check_type(type, ".dbf", "application/x-dbf", buf);
    check_type(type, ".dbm", "application/x-dbm", buf);
    check_type(type, ".dbx", "application/x-dbx", buf);
    check_type(type, ".dcd", "text/xml", buf);
    check_type(type, ".dcx", "application/x-dcx", buf);
    check_type(type, ".der", "application/x-x509-ca-cert", buf);
    check_type(type, ".dgn", "application/x-dgn", buf);
    check_type(type, ".dib", "application/x-dib", buf);
    check_type(type, ".dll", "application/x-msdownload", buf);
    check_type(type, ".doc", "application/msword", buf);
    check_type(type, ".dot", "application/msword", buf);
    check_type(type, ".drw", "application/x-drw", buf);
    check_type(type, ".dtd", "text/xml", buf);
    check_type(type, ".dwf", "Model/vnd.dwf", buf);
    check_type(type, ".dwf", "application/x-dwf", buf);
    check_type(type, ".dwg", "application/x-dwg", buf);
    check_type(type, ".dxb", "application/x-dxb", buf);
    check_type(type, ".dxf", "application/x-dxf", buf);
    check_type(type, ".edn", "application/vnd.adobe.edn", buf);
    check_type(type, ".emf", "application/x-emf", buf);
    check_type(type, ".eml", "message/rfc822", buf);
    check_type(type, ".ent", "text/xml", buf);
    check_type(type, ".epi", "application/x-epi", buf);
    check_type(type, ".eps", "application/x-ps", buf);
    check_type(type, ".eps", "application/postscript", buf);
    check_type(type, ".etd", "application/x-ebx", buf);
    check_type(type, ".exe", "application/x-msdownload", buf);
    check_type(type, ".fax", "image/fax", buf);
    check_type(type, ".fdf", "application/vnd.fdf", buf);
    check_type(type, ".fif", "application/fractals", buf);
    check_type(type, ".fo", "text/xml", buf);
    check_type(type, ".frm", "application/x-frm", buf);
    check_type(type, ".g4", "application/x-g4", buf);
    check_type(type, ".gbr", "application/x-gbr", buf);
    check_type(type, ".", "application/x-", buf);
    check_type(type, ".gif", "image/gif", buf);
    check_type(type, ".gl2", "application/x-gl2", buf);
    check_type(type, ".gp4", "application/x-gp4", buf);
    check_type(type, ".hgl", "application/x-hgl", buf);
    check_type(type, ".hmr", "application/x-hmr", buf);
    check_type(type, ".hpg", "application/x-hpgl", buf);
    check_type(type, ".hpl", "application/x-hpl", buf);
    check_type(type, ".hqx", "application/mac-binhex40", buf);
    check_type(type, ".hrf", "application/x-hrf", buf);
    check_type(type, ".hta", "application/hta", buf);
    check_type(type, ".htc", "text/x-component", buf);
    check_type(type, ".htm", "text/html", buf);
    check_type(type, ".html", "text/html", buf);
    check_type(type, ".htt", "text/webviewhtml", buf);
    check_type(type, ".htx", "text/html", buf);
    check_type(type, ".icb", "application/x-icb", buf);
    check_type(type, ".ico", "image/x-icon", buf);
    check_type(type, ".ico", "application/x-ico", buf);
    check_type(type, ".iff", "application/x-iff", buf);
    check_type(type, ".ig4", "application/x-g4", buf);
    check_type(type, ".igs", "application/x-igs", buf);
    check_type(type, ".iii", "application/x-iphone", buf);
    check_type(type, ".img", "application/x-img", buf);
    check_type(type, ".ins", "application/x-internet-signup", buf);
    check_type(type, ".isp", "application/x-internet-signup", buf);
    check_type(type, ".IVF", "video/x-ivf", buf);
    check_type(type, ".java", "java/*", buf);
    check_type(type, ".jfif", "image/jpeg", buf);
    check_type(type, ".jpe", "image/jpeg", buf);
    check_type(type, ".jpe", "application/x-jpe", buf);
    check_type(type, ".jpeg", "image/jpeg", buf);
    check_type(type, ".jpg", "image/jpeg", buf);
    check_type(type, ".jpg", "application/x-jpg", buf);
    check_type(type, ".js", "application/x-javascript", buf);
    check_type(type, ".jsp", "text/html", buf);
    check_type(type, ".la1", "audio/x-liquid-file", buf);
    check_type(type, ".lar", "application/x-laplayer-reg", buf);
    check_type(type, ".latex", "application/x-latex", buf);
    check_type(type, ".lavs", "audio/x-liquid-secure", buf);
    check_type(type, ".lbm", "application/x-lbm", buf);
    check_type(type, ".lmsff", "audio/x-la-lms", buf);
    check_type(type, ".ls", "application/x-javascript", buf);
    check_type(type, ".ltr", "application/x-ltr", buf);
    check_type(type, ".m1v", "video/x-mpeg", buf);
    check_type(type, ".m2v", "video/x-mpeg", buf);
    check_type(type, ".m3u", "audio/mpegurl", buf);
    check_type(type, ".m4e", "video/mpeg4", buf);
    check_type(type, ".mac", "application/x-mac", buf);
    check_type(type, ".man", "application/x-troff-man", buf);
    check_type(type, ".math", "text/xml", buf);
    check_type(type, ".mdb", "application/msaccess", buf);
    check_type(type, ".mdb", "application/x-mdb", buf);
    check_type(type, ".mfp", "application/x-shockwave-flash", buf);
    check_type(type, ".mht", "message/rfc822", buf);
    check_type(type, ".mhtml", "message/rfc822", buf);
    check_type(type, ".mi", "application/x-mi", buf);
    check_type(type, ".mid", "audio/mid", buf);
    check_type(type, ".midi", "audio/mid", buf);
    check_type(type, ".mil", "application/x-mil", buf);
    check_type(type, ".mml", "text/xml", buf);
    check_type(type, ".mnd", "audio/x-musicnet-download", buf);
    check_type(type, ".mns", "audio/x-musicnet-stream", buf);
    check_type(type, ".mocha", "application/x-javascript", buf);
    check_type(type, ".movie", "video/x-sgi-movie", buf);
    check_type(type, ".mp1", "audio/mp1", buf);
    check_type(type, ".mp2", "audio/mp2", buf);
    check_type(type, ".mp2v", "video/mpeg", buf);
    check_type(type, ".mp3", "audio/mp3", buf);
    check_type(type, ".mp4", "video/mpeg4", buf);
    check_type(type, ".mpa", "video/x-mpg", buf);
    check_type(type, ".mpd", "application/vnd.ms-project", buf);
    check_type(type, ".mpe", "video/x-mpeg", buf);
    check_type(type, ".mpeg", "video/mpg", buf);
    check_type(type, ".mpg", "video/mpg", buf);
    check_type(type, ".mpga", "audio/rn-mpeg", buf);
    check_type(type, ".mpp", "application/vnd.ms-project", buf);
    check_type(type, ".mps", "video/x-mpeg", buf);
    check_type(type, ".mpt", "application/vnd.ms-project", buf);
    check_type(type, ".mpv", "video/mpg", buf);
    check_type(type, ".mpv2", "video/mpeg", buf);
    check_type(type, ".mpw", "application/vnd.ms-project", buf);
    check_type(type, ".mpx", "application/vnd.ms-project", buf);
    check_type(type, ".mtx", "text/xml", buf);
    check_type(type, ".mxp", "application/x-mmxp", buf);
    check_type(type, ".net", "image/pnetvue", buf);
    check_type(type, ".nrf", "application/x-nrf", buf);
    check_type(type, ".nws", "message/rfc822", buf);
    check_type(type, ".odc", "text/x-ms-odc", buf);
    check_type(type, ".out", "application/x-out", buf);
    check_type(type, ".p10", "application/pkcs10", buf);
    check_type(type, ".p12", "application/x-pkcs12", buf);
    check_type(type, ".p7b", "application/x-pkcs7-certificates", buf);
    check_type(type, ".p7c", "application/pkcs7-mime", buf);
    check_type(type, ".p7m", "application/pkcs7-mime", buf);
    check_type(type, ".p7r", "application/x-pkcs7-certreqresp", buf);
    check_type(type, ".p7s", "application/pkcs7-signature", buf);
    check_type(type, ".pc5", "application/x-pc5", buf);
    check_type(type, ".pci", "application/x-pci", buf);
    check_type(type, ".pcl", "application/x-pcl", buf);
    check_type(type, ".pcx", "application/x-pcx", buf);
    check_type(type, ".pdf", "application/pdf", buf);
    check_type(type, ".pdf", "application/pdf", buf);
    check_type(type, ".pdx", "application/vnd.adobe.pdx", buf);
    check_type(type, ".pfx", "application/x-pkcs12", buf);
    check_type(type, ".pgl", "application/x-pgl", buf);
    check_type(type, ".pic", "application/x-pic", buf);
    check_type(type, ".pko", "application/vnd.ms-pki.pko", buf);
    check_type(type, ".pl", "application/x-perl", buf);
    check_type(type, ".plg", "text/html", buf);
    check_type(type, ".pls", "audio/scpls", buf);
    check_type(type, ".plt", "application/x-plt", buf);
    check_type(type, ".png", "image/png", buf);
    check_type(type, ".png", "application/x-png", buf);
    check_type(type, ".pot", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".ppa", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".ppm", "application/x-ppm", buf);
    check_type(type, ".pps", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".ppt", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".ppt", "application/x-ppt", buf);
    check_type(type, ".pr", "application/x-pr", buf);
    check_type(type, ".prf", "application/pics-rules", buf);
    check_type(type, ".prn", "application/x-prn", buf);
    check_type(type, ".prt", "application/x-prt", buf);
    check_type(type, ".ps", "application/x-ps", buf);
    check_type(type, ".ps", "application/postscript", buf);
    check_type(type, ".ptn", "application/x-ptn", buf);
    check_type(type, ".pwz", "application/vnd.ms-powerpoint", buf);
    check_type(type, ".r3t", "text/vnd.rn-realtext3d", buf);
    check_type(type, ".ra", "audio/vnd.rn-realaudio", buf);
    check_type(type, ".ram", "audio/x-pn-realaudio", buf);
    check_type(type, ".ras", "application/x-ras", buf);
    check_type(type, ".rat", "application/rat-file", buf);
    check_type(type, ".rdf", "text/xml", buf);
    check_type(type, ".rec", "application/vnd.rn-recording", buf);
    check_type(type, ".red", "application/x-red", buf);
    check_type(type, ".rgb", "application/x-rgb", buf);
    check_type(type, ".rjs", "application/vnd.rn-realsystem-rjs", buf);
    check_type(type, ".rjt", "application/vnd.rn-realsystem-rjt", buf);
    check_type(type, ".rlc", "application/x-rlc", buf);
    check_type(type, ".rle", "application/x-rle", buf);
    check_type(type, ".rm", "application/vnd.rn-realmedia", buf);
    check_type(type, ".rmf", "application/vnd.adobe.rmf", buf);
    check_type(type, ".rmi", "audio/mid", buf);
    check_type(type, ".rmj", "application/vnd.rn-realsystem-rmj", buf);
    check_type(type, ".rmm", "audio/x-pn-realaudio", buf);
    check_type(type, ".rmp", "application/vnd.rn-rn_music_package", buf);
    check_type(type, ".rms", "application/vnd.rn-realmedia-secure", buf);
    check_type(type, ".rmvb", "application/vnd.rn-realmedia-vbr", buf);
    check_type(type, ".rmx", "application/vnd.rn-realsystem-rmx", buf);
    check_type(type, ".rnx", "application/vnd.rn-realplayer", buf);
    check_type(type, ".rp", "image/vnd.rn-realpix", buf);
    check_type(type, ".rpm", "audio/x-pn-realaudio-plugin", buf);
    check_type(type, ".rsml", "application/vnd.rn-rsml", buf);
    check_type(type, ".rt", "text/vnd.rn-realtext", buf);
    check_type(type, ".rtf", "application/msword", buf);
    check_type(type, ".rtf", "application/x-rtf", buf);
    check_type(type, ".rv", "video/vnd.rn-realvideo", buf);
    check_type(type, ".sam", "application/x-sam", buf);
    check_type(type, ".sat", "application/x-sat", buf);
    check_type(type, ".sdp", "application/sdp", buf);
    check_type(type, ".sdw", "application/x-sdw", buf);
    check_type(type, ".sit", "application/x-stuffit", buf);
    check_type(type, ".slb", "application/x-slb", buf);
    check_type(type, ".sld", "application/x-sld", buf);
    check_type(type, ".slk", "drawing/x-slk", buf);
    check_type(type, ".smi", "application/smil", buf);
    check_type(type, ".smil", "application/smil", buf);
    check_type(type, ".smk", "application/x-smk", buf);
    check_type(type, ".snd", "audio/basic", buf);
    check_type(type, ".sol", "text/plain", buf);
    check_type(type, ".sor", "text/plain", buf);
    check_type(type, ".spc", "application/x-pkcs7-certificates", buf);
    check_type(type, ".spl", "application/futuresplash", buf);
    check_type(type, ".spp", "text/xml", buf);
    check_type(type, ".ssm", "application/streamingmedia", buf);
    check_type(type, ".sst", "application/vnd.ms-pki.certstore", buf);
    check_type(type, ".stl", "application/vnd.ms-pki.stl", buf);
    check_type(type, ".stm", "text/html", buf);
    check_type(type, ".sty", "application/x-sty", buf);
    check_type(type, ".svg", "text/xml", buf);
    check_type(type, ".swf", "application/x-shockwave-flash", buf);
    check_type(type, ".tdf", "application/x-tdf", buf);
    check_type(type, ".tg4", "application/x-tg4", buf);
    check_type(type, ".tga", "application/x-tga", buf);
    check_type(type, ".tif", "image/tiff", buf);
    check_type(type, ".tif", "application/x-tif", buf);
    check_type(type, ".tiff", "image/tiff", buf);
    check_type(type, ".tld", "text/xml", buf);
    check_type(type, ".top", "drawing/x-top", buf);
    check_type(type, ".torrent", "application/x-bittorrent", buf);
    check_type(type, ".tsd", "text/xml", buf);
    check_type(type, ".txt", "text/plain", buf);
    check_type(type, ".uin", "application/x-icq", buf);
    check_type(type, ".uls", "text/iuls", buf);
    check_type(type, ".vcf", "text/x-vcard", buf);
    check_type(type, ".vda", "application/x-vda", buf);
    check_type(type, ".vdx", "application/vnd.visio", buf);
    check_type(type, ".vml", "text/xml", buf);
    check_type(type, ".vpg", "application/x-vpeg005", buf);
    check_type(type, ".vsd", "application/vnd.visio", buf);
    check_type(type, ".vsd", "application/x-vsd", buf);
    check_type(type, ".vss", "application/vnd.visio", buf);
    check_type(type, ".vst", "application/vnd.visio", buf);
    check_type(type, ".vst", "application/x-vst", buf);
    check_type(type, ".vsw", "application/vnd.visio", buf);
    check_type(type, ".vsx", "application/vnd.visio", buf);
    check_type(type, ".vtx", "application/vnd.visio", buf);
    check_type(type, ".vxml", "text/xml", buf);
    check_type(type, ".wav", "audio/wav", buf);
    check_type(type, ".wax", "audio/x-ms-wax", buf);
    check_type(type, ".wb1", "application/x-wb1", buf);
    check_type(type, ".wb2", "application/x-wb2", buf);
    check_type(type, ".wb3", "application/x-wb3", buf);
    check_type(type, ".wbmp", "image/vnd.wap.wbmp", buf);
    check_type(type, ".wiz", "application/msword", buf);
    check_type(type, ".wk3", "application/x-wk3", buf);
    check_type(type, ".wk4", "application/x-wk4", buf);
    check_type(type, ".wkq", "application/x-wkq", buf);
    check_type(type, ".wks", "application/x-wks", buf);
    check_type(type, ".wm", "video/x-ms-wm", buf);
    check_type(type, ".wma", "audio/x-ms-wma", buf);
    check_type(type, ".wmd", "application/x-ms-wmd", buf);
    check_type(type, ".wmf", "application/x-wmf", buf);
    check_type(type, ".wml", "text/vnd.wap.wml", buf);
    check_type(type, ".wmv", "video/x-ms-wmv", buf);
    check_type(type, ".wmx", "video/x-ms-wmx", buf);
    check_type(type, ".wmz", "application/x-ms-wmz", buf);
    check_type(type, ".wp6", "application/x-wp6", buf);
    check_type(type, ".wpd", "application/x-wpd", buf);
    check_type(type, ".wpg", "application/x-wpg", buf);
    check_type(type, ".wpl", "application/vnd.ms-wpl", buf);
    check_type(type, ".wq1", "application/x-wq1", buf);
    check_type(type, ".wr1", "application/x-wr1", buf);
    check_type(type, ".wri", "application/x-wri", buf);
    check_type(type, ".wrk", "application/x-wrk", buf);
    check_type(type, ".ws", "application/x-ws", buf);
    check_type(type, ".ws2", "application/x-ws", buf);
    check_type(type, ".wsc", "text/scriptlet", buf);
    check_type(type, ".wsdl", "text/xml", buf);
    check_type(type, ".wvx", "video/x-ms-wvx", buf);
    check_type(type, ".xdp", "application/vnd.adobe.xdp", buf);
    check_type(type, ".xdr", "text/xml", buf);
    check_type(type, ".xfd", "application/vnd.adobe.xfd", buf);
    check_type(type, ".xfdf", "application/vnd.adobe.xfdf", buf);
    check_type(type, ".xhtml", "text/html", buf);
    check_type(type, ".xls", "application/vnd.ms-excel", buf);
    check_type(type, ".xls", "application/x-xls", buf);
    check_type(type, ".xlw", "application/x-xlw", buf);
    check_type(type, ".xml", "text/xml", buf);
    check_type(type, ".xpl", "audio/scpls", buf);
    check_type(type, ".xq", "text/xml", buf);
    check_type(type, ".xql", "text/xml", buf);
    check_type(type, ".xquery", "text/xml", buf);
    check_type(type, ".xsd", "text/xml", buf);
    check_type(type, ".xsl", "text/xml", buf);
    check_type(type, ".xslt", "text/xml", buf);
    check_type(type, ".xwd", "application/x-xwd", buf);
    check_type(type, ".x_b", "application/x-x_b", buf);
    check_type(type, ".sis", "application/vnd.symbian.install", buf);
    check_type(type, ".sisx", "application/vnd.symbian.install", buf);
    check_type(type, ".x_t", "application/x-x_t", buf);
    check_type(type, ".ipa", "application/vnd.iphone", buf);
    check_type(type, ".apk", "application/vnd.android.package-archive", buf);
    check_type(type, ".xap", "application/x-silverlight-app", buf);

    if (buf[0] == '\0') {
        strcpy(buf, "application/octet-stream");
    }
}