#include "utils.h"

#include <sched.h>
#include <unistd.h>

/**
 * 清除字符串两侧空格
 * */
std::string &trim(std::string &str) {
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

/**
 * 获取当前时间字符串
 * */
std::string getTime() {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return std::string(tmp);
}

/**
 * 获取系统可用的 CPU 核数
 * */
int get_nprocs() { return sysconf(_SC_NPROCESSORS_ONLN); }

/**
 * 为指定的进程绑定指定的 CPU
 * */
void set_affnity(pid_t pid, int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(pid, sizeof(cpuset), &cpuset);
}