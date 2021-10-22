## grpc 与 HTTP/2

#### grpc 简介

gRPC 是由 Google 开发，基于 HTTP/2 的一个 rpc 框架。

gRPC 在 HTTP/2 的基础上引入了：多通道（channels）、RPC（remote procedure calls） 和 消息（messages） 三个概念

每个通道可能有多个 RPC，而每个RPC可能有多个消息。

> gRPC 通常有四种模式，unary，client streaming，server streaming 以及 bidirectional streaming，对于底层 HTTP/2 来说，它们都是流，并且仍然分 request 和 response。--- https://pingcap.com/zh/blog/grpc

#### grpc 和 HTTP/2 的联系

多通道是 gRPC 的一个核心概念。

HTTP/2 中的流，使得在单个连接上可以并发发送请求。

而 gRPC 的多通道概念，就是开启多个 HTTP/2 链接，并且在多个链接上通过流并发发送请求。

在实际使用中，gRPC 都已经封装好了，我们只需要函数调用就可以轻松使用。而在这之下，是复杂的一套过程，要去管理多个 HTTP/2 连接，去判断这些连接的状态、处理错误等。

通过封装，多通道代表了一条虚拟的连上另一端端点的连接，实际上底下是有复数条 HTTP/2 连接的。

rpc 和 连接有关，实际上是纯HTTP/2流（以 HTTP/2 协议发送）。

消息与rpc相关联，并以HTTP/2数据帧的形式发送。消息的分层在数据帧之上。根据消息的大小，一个消息可跨越多个帧发送，帧也可以发送多个流。

除此之外，grpc 还提连接管理，生成一个连接池，定期更新。

#### grpc request

gRPC 的 request 通常包含：

1. Request-Headers

   Request-Headers 直接使用的 HTTP/2 headers，在 HEADERS 和 CONTINUATION frame 里面派发。

   定义的 header 主要有 Call-Definition 以及 Custom-Metadata：

   * Call-Definition 里面包括 Method（其实就是用的 HTTP/2 的 POST），Content-Type 等。

   * 而 Custom-Metadata 则是应用层自定义的任意 key-value，key 不建议使用 `grpc-`开头，因为这是为 gRPC 后续自己保留的。

2. 0 或者多个 Length-Prefixed-Message

   Length-Prefixed-Message 主要在 DATA frame 里面派发，它有一个 Compressed flag 用来表示改 message 是否压缩，如果为 1，表示该 message 采用了压缩，而压缩算法定义在 header 里面的 Message-Encoding 里面。然后后面跟着四字节的 message length 以及实际的 message。

3. EOS

   EOS（end-of-stream） 会在最后的 DATA frame 里面带上了 `END_STREAM`这个 flag。用来表示 stream 不会在发送任何数据，可以关闭了。

#### grpc response

Response 主要包含：

1. Response-Headers

   Response-Headers 主要包括 HTTP-Status，Content-Type 以及 Custom-Metadata 等。

2. 0 或者多个 Length-Prefixed-Message 以及 Trailers

   Trailers 包括了 Status 以及 0 或者多个 Custom-Metadata。

3. 如果遇到了错误，也可以直接返回 Trailers-Only。

   Trailers-Only 也有 HTTP-Status ，Content-Type 和 Trailers。

HTTP-Status 就是我们通常的 HTTP 200，301，400 这些，很通用就不再解释。Status 也就是 gRPC 的 status， 而 Status-Message 则是 gRPC 的 message。Status-Message 采用了 Percent-Encoded 的编码方式。

如果在最后收到的 HEADERS frame 里面，带上了 Trailers，并且有 `END_STREAM`这个 flag，那么就意味着 response 的 EOS。