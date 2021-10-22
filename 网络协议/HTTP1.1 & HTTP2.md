## HTTP/1.1 & HTTP/2

### HTTP/1.1

目前最广泛使用的版本就是这个版本。

HTTP/1.1 是1999 年六月发布的，距今已经20年有余。

> But in some areas it’s beginning to show its age; for the most part, in that the designers weren’t building for the scale at which HTTP/1.1 would be used and the astonishing amount of traffic that it would come to handle. --- *Jean de Klerk, Developer Program Engineer, Google*

#### HTTP/1.1 特性

HTTP/1.1 默认所有连接都是长连接，即允许HTTP设备在事务处理结束之后将TCP连接保持在打开的状态，以便未来的HTTP请求**重用现在的连接**。而在以前的HTTP/1.0版本，长连接需要请求头 Connection: Keep-Alive。

HTTP/1.1 服务端按照队列的**FIFO**形式处理过来的 HTTP 请求。（处理，包括接受请求、处理和返回响应）

这是因为创建HTTP连接的**开销很大**。必须先建立TCP连接，然后使用TLS保护该连接，交换头和设置，等等。HTTP/1.1将连接视为长期存在的、可重用的对象，从而简化了这个过程。HTTP/1.1连接保持空闲状态，以便通过现有的空闲连接发送到相同目的地的新请求。虽然连接重用可以缓解这个问题，但是**一个连接一次只能处理一个请求**——它们是1:1耦合的。如果有一个大的消息正在发送，**新的请求必须等待它的完成**。

### HTTP/2

HTTP/2 2015年五月发布，目前使用还非常少。

> HTTP/2 improves upon HTTP/1.1’s design in a number of ways, perhaps most significantly in providing a semantic mapping over connections --- *Jean de Klerk, Developer Program Engineer, Google*
>
> HTTP 2.0最大的特点：不会改动HTTP 的语义，HTTP 方法、状态码、URI 及首部字段，等等这些核心概念上一如往常，却能致力于突破上一代标准的性能限制，改进传输性能，实现低延迟和高吞吐量。而之所以叫2.0，是在于新增的二进制分帧层。在二进制分帧层上， HTTP 2.0 会将所有传输的信息分割为更小的消息和帧，并对它们采用二进制格式的编码 。--- https://zhuanlan.zhihu.com/p/37387316

HTTP/2 在 HTTP/1.1 的基础上推出了流的概念，让请求得以并发发送。（利用I/O多路复用）

所谓的流，其实就是指把每个请求分成一段一段更小的帧。连接仍然只有一条，但可以乱序把这些帧发出去。这其实和cpu线程抢占时间片是很像的，就是并发执行。通过并发执行，可以有效提高利用率，解决发一个很长的请求，别的短请求只能等待的问题。（HTTP/1.1中只能多开链接或者等待）

#### HTTP/2 流控制

并发发送请求片段会有个问题，如果某一个请求片段把接收方缓冲区塞满了，那么别的帧就会受到影响。并发的效率也会大打折扣。为了解决这个问题，HTTP/2 引入流控制。

服务端给每个请求分配一个缓冲区，每个帧到了服务端，就只能放到对应缓冲区中。如果缓冲区满了，就只能停止发送。发送端也可以发一个特殊的帧，请求额外缓冲区。

#### HTTP/2 智能代理

这个没看太懂，大概是因为HTTP/2 流实现并发的特点，使得连接在开始时，能有较好的流量。（因为TCP要慢开始）

...

### 总结

> HTTP/2 has many advantages over HTTP/1.1 that dramatically reduce the network cost of large-scale, real-time systems. Streams present one of the biggest flexibility and performance improvements that users will see, but HTTP/2 also provides semantics around graceful close (see: [GOAWAY](https://tools.ietf.org/html/rfc7540#section-6.8)), header compression, server push, pinging, stream priority, and more.---  *Jean de Klerk, Developer Program Engineer, Google*

### 参考

* https://www.cncf.io/blog/2018/07/03/http-2-smarter-at-scale/
* https://zhuanlan.zhihu.com/p/37387316
* 另外有HTTP的官网，深入了解建议研究：https://httpwg.org/