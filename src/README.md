| 文件 | 作用 |
| - | - |
| main.cpp | 程序入口 |
| config | 配置相关 |
| logger | 日志相关 |
| master | 监听端口并创建 work 进程 |
| work | 工作进程, 主要的工作都在此完成 |
| event | 创建异步事件监听并触发回调事件 |
| utils | 一些工具函数 |
| read_buffer | 自己实现的 buffer |
| read_buffer_tree | 利用红黑树实现的方便的 buffer 操作 |