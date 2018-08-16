# blog

从零开始构建 WEB 服务器

## 使用方法

```shell
cd src && make
sudo ./blog ../blog.conf
```

```shell
curl http://127.0.0.1/index.html
```

## 重构中

出于学习的目的, 我准备使用我的 C with STL 的 C++ 技术来重构本项目.

## 特性 (已完成的功能)

- 配置文件解析
- 单例模式打印日志
- 为每个 work 进程绑定一个 CPU 核心, 以减少系统调度产生的影响
- 父进程创建套接字, 子进程监听套接字事件
- epoll 异步监听连接 + 回调通知
- 红黑树维持描述符与其对应的缓冲区

## TODO (并非紧急但日后要做的)

- 按照 [Google 开源项目风格指南](http://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/) 重新整理一遍结构
- 现在按值传递 config 的方式导致 epoll 之后的函数无法获得这些配置, 可以考虑将 config 也修改为单例模式

## 已知却未能解决的问题

- 无法稳定的获得请求数据已经传输完毕的事件
- 请求传输结束之前对方结束传输, 会导致莫名的异常
- 在多次的 epoll 事件传递与连接事件处理中丢失了很多关键的信息, 是项目结构的问题

## 开发环境

- VSCode + VIM 插件
- MacOS
- Docker && Ubuntu

## 参考

- UNIX 环境高级编程
- [Nginx](http://nginx.org/)
- [TinyWeb](https://github.com/GeneralSandman/TinyWeb)