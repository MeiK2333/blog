| 文件 | 作用 |
| - | - |
| [main.cc](main.cc) | 程序入口 |
| [config](config.h) | 配置相关 |
| [logger](logger.h) | 日志相关 |
| [master](master.h) | 监听端口并创建 work 进程 |
| [work](work.h) | 工作进程, 主要的工作都在此完成 |
| [event](event.h) | 创建异步事件监听并触发回调事件 |
| [utils](utils.h) | 一些工具函数 |
| [read_buffer](read_buffer.h) | 自己实现的 buffer |
| [read_buffer_tree](read_buffer_tree.h) | 利用红黑树实现的方便的 buffer 操作 |
| [request](request.h) | 请求相关 |
| [response](response.h) | 响应相关 |
| [produce](produce.h) | 请求处理流程 |