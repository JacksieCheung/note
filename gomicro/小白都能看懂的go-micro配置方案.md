### 小白都能看懂的 go-micro 配置方案

之前看的配置教程挺懵逼的，进过几次平台切换和重新配置，终于差不多搞懂了。

要使用 go-micro ，无非需要两个东西：

* protoc 也称为 protobuf 编译器(Protocol Buffer Compiler)

  protoc 安装参照下面这个网址跟着走就好了：

  https://grpc.io/docs/protoc-installation/

* go-micro 插件

  首先将 go mod 设置成 on

  ```shell
  export GO111MODULE=on
  ```

  然后运行 go get 就可以了

  ```shell
  go get -v github.com/micro/protobuf/{proto,protoc-gen-go}
  ```

  我们可以通过 which 指令判断有无安装成功

  ```shell
  which protoc-gen-go
  ```

  如果没有显示，那么还要配一下环境变量。

  加上这个应该就可以了（修改 .bashrc 或者 .zshrc）

  ```shell
  export PATH=$PATH:$GOPATH
  export PATH=$PATH:$GOROOT
  export PATH="$PATH:$(go env GOPATH)/bin"
  ```

**有了这两个，就能通过 protobuf 文件，自动生成我们想要的代码了。**

