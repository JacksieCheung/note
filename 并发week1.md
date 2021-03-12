## 并发基础

### 1. go 关键词

go 语言里面用 go 关键词开启并发，go 后面跟着的必须是一个函数调用，也可以是匿名函数的调用。

下面是书上的第一个示例，跑次数不同，data的结果也不同。

```go
package main

import (
        "fmt"
        // "time"
)

func main() {
        var data int
        go func() {
                data++
        }()

        // time.Sleep(1*time.Second)
        fmt.Printf("data is %d\n",data)
}
```

其实输出 0 还是输出 1 是 ```data++``` 和```printf```之间的运行顺序决定的。

go 关键词开启了子 goroutine 之后，就会和主 goroutine赛跑，要是主 goroutine 先运行了，程序就会自动退出，子 goroutine 就会自动被回收。我们看到的就是 0 。而在我的计算机上，大多数情况都是输出 0 。说明在这个情景下，主 goroutine 大多数情况下都比子 goroutine 快。

我们当然可以用 ```time.Sleep```函数使主 goroutine 阻塞，以等待子 goroutine 的完成。但是这只是增加了成功率，而并非逻辑正确的。所以我们需要别的方法，来使并发可控。

### 2. 进程、线程、协程

