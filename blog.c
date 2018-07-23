#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

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
    printf("PID %d ended\n", pid);
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
    int server_socket = -1;
    int client_socket = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

    if (signal(SIGCHLD, sig_cld) == SIG_ERR) {
        err_exit("signal error");
    }

    port = atoi(argv[1]);
    /**
     * 设置开始监听端口
     * */
    server_socket = startup(&port);
    printf("httpd running on port %d\n", port);

    /**
     * 循环监听连接请求并分发到子进程处理
     * */
    while (1) {
        if ((client_socket =
                 accept(server_socket, (struct sockaddr *)&client_name,
                        &client_name_len)) == -1) {
            err_exit("accept error");
        }

        if ((pid = fork()) < 0) {
            err_exit("fork error");
        } else if (pid == 0) {
            printf("%s\n", inet_ntoa(client_name.sin_addr));
            
            exit(0);
        } else {
            printf("PID %d start\n", pid);
        }
    }
    return 0;
}