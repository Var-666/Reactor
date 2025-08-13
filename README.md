这个 Reactor 模型中每个类的作用，就像是在搭建一个“客服中心”一样。

⸻

🧱 1. Socket —— 电话线路

作用：创建和管理底层 socket，负责打电话、接电话。

想象你要搭建一个客服中心，每条电话线就是一个 Socket。你可以用这条电话线拨打、接听、挂断电话。
•	listen()：开始接听电话
•	accept()：有电话进来，接起它
•	bind()：把电话线绑定到一个具体位置（IP+端口）

⸻

🧭 2. InetAddress —— 地址本

作用：保存 IP 和端口的工具。

这个类就像一本地址本，帮你记录“客户打来的地址”。它把 sockaddr_in 封装成了更好用的类。

⸻

🎛️ 3. Channel —— 电话客服的小面板

作用：描述一个 fd（文件描述符）关注什么事件，并绑定处理回调。

想象每位客服员面前有个控制台，有按钮指示：
•	有人讲话了（读事件 EPOLLIN）
•	可以说话了（写事件 EPOLLOUT）

Channel 就是这个面板，它会告诉系统：“这个 fd 如果读事件来了，叫我执行 readCallback；写事件来了，叫我执行 writeCallback。”

⸻

🧼 4. Epoller —— 前台的调度员（epoll 封装）

作用：监听所有的 Channel，看哪个有事，然后告诉 EventLoop。

就像客服前台调度员不断看着谁的电话灯亮了（epoll_wait），然后告诉主管（EventLoop）去处理。

⸻

🔁 5. EventLoop —— 主管（事件循环）

作用：不断循环，询问 Epoller 哪个 Channel 有事件，调起回调函数。

这就是整个客服中心的大脑，它每隔一段时间就去 epoll 那里问：

“谁有事件发生了？”

然后挨个处理，比如：
•	有电话进来了 → 调用 Acceptor 的 handleRead() → 建立连接
•	有人说话了 → 调用 Connection 的 handleRead() → 收到数据
•	可以说话了 → 调用 Connection 的 handleWrite() → 发数据

⸻

📞 6. Acceptor —— 接线员

作用：专门监听新连接（listen fd），并 accept() 建立新连接。

每次电话打进来（listen fd 可读），Acceptor 就接起来，然后交给一个新的客服（Connection）去处理。

⸻

👩‍💼 7. Connection —— 客服员

作用：处理一个客户端连接，收数据、发数据、断开连接。

每个 Connection 负责一位客户的对话：
•	对方说话 → 触发 handleRead() → 收到数据
•	你要说话 → 调用 send() → 异步发出去
•	客户挂电话 → 触发 handleClose() → 清理资源

里面也包含 Channel，绑定这个连接的 fd，来监听读写事件。

⸻

🧩 8. Server —— 经理（控制一切）

作用：组合 Acceptor 和多个 Connection，协调整个系统。

你不需要每次都手动写创建 Connection、绑定事件、注册回调 —— 这些 Server 帮你做好了：
•	初始化 Acceptor，开始监听端口
•	有连接进来，生成新的 Connection
•	给 Connection 绑定好回调函数（messageCallback / closeCallback）
•	保持所有连接的引用，避免提前销毁

⸻


已实现功能清单
1.	Reactor 基础框架
-	事件循环（EventLoop）
-	事件分发（Epoller）
-	事件通道（Channel）
2.	网络通信模块
-	TCP Socket 封装
-	连接管理（Connection 类）
-	接受新连接（Acceptor）
3.	单线程版本的 Reactor 服务器
-	事件驱动模型实现客户端连接和消息收发
-	读写事件处理与回调机制
4.	多线程支持
-	EventLoopThread 类：为每个线程维护独立的事件循环
-	EventLoopThreadPool 类：线程池管理多个 EventLoopThread
-	服务器多线程分发连接到不同线程处理
5.	连接超时管理
-	独立的 ConnectionTimeoutManager 类
-	定时检测空闲连接并主动关闭
6.	Buffer 类设计与实现
-	高效缓冲区管理，支持零拷贝读写
-	支持从文件描述符读取数据，管理可读/可写空间
7.	协议层拆包粘包处理（策略模式实现）
-	抽象协议接口 IProtocol
-	具体协议实现如 LengthHeaderProtocol（包头 + 包体长度）
-	Codec 上下文类，实现灵活协议切换
-	支持消息完整解析和业务层解耦

⸻


