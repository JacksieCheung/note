## 从源码看 net/http 如何处理请求

```go
func (srv *Server) Serve(l net.Listener) error {
	defer l.Close()
	var tempDelay time.Duration // how long to sleep on accept failure
	for {
		rw, e := l.Accept() // 接收连接
		if e != nil { // have error
			if ne, ok := e.(net.Error); ok && ne.Temporary() { // 类型断言
				if tempDelay == 0 {
					tempDelay = 5 * time.Millisecond // 5ms
				} else {
					tempDelay *= 2 // 如果 延迟不为 0， 则翻倍
				}
				if max := 1 * time.Second; tempDelay > max { // 最长等待时间就是 1s，超过了就限定 1s
					tempDelay = max
				}
				log.Printf("http: Accept error: %v; retrying in %v", e, tempDelay)
				time.Sleep(tempDelay)
				continue
			}
			return e // 类型断言失败，遇到未知错误，直接返回，退出循环等待客户端
		}
		// doesn't have error
		tempDelay = 0 // 等待时间归 0
		
		// 设置超时
		if srv.ReadTimeout != 0 {
			rw.SetReadDeadline(time.Now().Add(srv.ReadTimeout))
		}
		if srv.WriteTimeout != 0 {
			rw.SetWriteDeadline(time.Now().Add(srv.WriteTimeout))
		}

		// 建立新连接
		c, err := srv.newConn(rw)
		if err != nil {
			continue
		}

		// 开启 goroutine
		go c.serve()
	}
	panic("not reached")
}
```

