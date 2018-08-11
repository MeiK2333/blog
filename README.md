# blog

从零开始构建自己的博客系统

## 重构中

出于学习的目的, 我准备使用我的 C with STL 的 C++ 技术来重构本项目.

## 特性 (已完成的功能)

- 配置文件解析
- 单例模式打印日志
- 为每个 work 进程绑定一个 CPU 核心, 以减少系统调度产生的影响
- 父进程创建套接字, 子进程监听套接字事件
- epoll 异步监听连接 + 回调通知

## TODO (并非紧急但日后要做的)

- 优化 Log 的方式 (我个人感觉 `char *` + `format` 的方式更可控, 如 `Logger::info("accept a new client %s:%d", address, port)`, 我还没有习惯 C++ 的那一套)

# 开发环境

- VSCode + VIM 插件
- MacOS
- Docker && Ubuntu

# 参考

- UNIX 环境高级编程
- [Nginx](http://nginx.org/)
- [TinyWeb](https://github.com/GeneralSandman/TinyWeb)