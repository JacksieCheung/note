## csapp.h 和 csapp.c

关于头文件 csapp.h 这里有必要说明一下

这应该是作者自己封装的头文件，我们的系统是没有的。

所以为了顺利运行书中的代码，就有记录如何把这个头文件弄下来。

* http://csapp.cs.cmu.edu/public/code.html 点这里打开官网

* 找到 csapp.h 和 csapp.c

* 复制这两个文件到本地 c 语言头文件库
  不知道怎么找的可以用 `gcc -v` 查看带 include 字样的那个路径
  mac 下的路径是 /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/

* 编译的时候只要加上 -Ipthread 即可
  如：`gcc main.c -Ipthread`
