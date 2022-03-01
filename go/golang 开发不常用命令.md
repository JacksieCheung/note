## golang 开发不常用命令

* go build -gcflags -S main.go

  看汇编

* GOSSAFUNC=main go build main.go

  查看汇编的优化过程

* uname -m

  这个不是 go 的指令，这个用来查看机器的指令集

* 编译器的前端和后端

  编译器的前端一般承担着词法分析、语法分析、类型检查和中间代码生成几部分工作，而编译器后端主要负责目标代码的生成和优化，也就是将中间代码翻译成目标机器能够运行的二进制机器码。

* 语言为什么可以自举？

  golang 的编译器，是用 golang 写的。但是有一个问题，golang 如何在没有编译器的情况下写出一个 golang？

  其实，刚开始 golang 编译器是用 c 实现的。我们把这个编译器编译出来，变成一个二进制程序。接着，我们就可以写 golang 了。此时我们只要用 golang 再写一遍编译器，然后用 c 写的编译器，把 golang 写的编译器编译出来，那么就得到了 golang 实现的编译器。这就是我们说的自举。

  https://www.zhihu.com/question/66944175