# unsafe

Go 语言在语言层面封装了指针，给指针的使用加了许多限制，使指针更安全。但这种安全性，有时候会影响性能。go 提供了 unsafe 包，让我们可以像c一样使用指针。

指针是个很有趣的东西…

我们可以看一下一些有趣的例子🤔

```go
package main

import "fmt"

func double(x *int) {
	*x += *x
	x = nil
}

func main() {
	var a = 3
	double(&a)
	fmt.Println(a) // 6
	
	p := &a
	double(p)
	fmt.Println(a, p == nil) // 12 false
}
```

像这个例子，第一个a会输出6，其中 double 函数中的 x = nil 不会造成什么影响。因为 go 所有函数都是值传递，这里只是拷贝了一份a的地址，存到x，改变x的值，a 仍然存在。

## go 语言指针的限制

- 指针不能进行数学运算。指针是存储变量地址的变量+1 让其指向下一个地址。c 语言的数组经常会这样操作。
- 不同类型指针不能相互转化
- 不同类型指针不能用==和!=比较。但可以和 nil 比较
- 不同类型的指针不能互相赋值

