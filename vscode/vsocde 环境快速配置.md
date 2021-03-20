## vscode 环境快速配置

为了预防时不时的环境更换，特意写了这样一个备忘录。

* vscode 官网下

* 字体和窗口根据个人习惯调整（settings.json）

  ```json
  {
    "editor.fontSize": 16,
    "window.zoomLevel": 1
  }
  ```

### 1. go 环境

* go 官网下

* go 环境变量的代理设置成 https://goproxy.cn,direct

* vscode 搜索插件 go 并 install

* 在 vscode 用快捷键 ctrl+shift+p(ubuntu)/command+shift+p(mac) 键入 go:install/update tools

* 全部选项勾选，安装(虽然全部装了，但有些插件挺难受的)

* 首选项，设置，输入go 配置 setting.json，如图：

  ```json
  {
    "go.formatTool": "goimports",
    "go.inferGopath": false,
    "go.lintOnSave": "file"
  }
  ```

**下面我会根据我个人的使用习惯调整插件：**

* 打开设置搜索 go

* Docs Tool 选择 godoc。

* Format Tool 选择 goimports 这个上面也设置了。

  选择 goimports 需要额外安装插件

  ```shell
  export GO111MODULE=on
  go get -v golang.org/x/tools/cmd/goimports
  ```

* Lint On Save 选择 file

* Lint Tool 选择 golint（别选 static-check，傻逼玩意，识别不了导入的变量）

  如果选择 golint，需要下载 golint

  ```shell
  export GO111MODULE=on
  go get -v golang.org/x/lint/golint
  ```

* Use Language Server 取消勾选

  这个条条框框太多了，看着烦，不建议使用

### 2. 生产力插件

* vim 插件

  这是我个人的习惯，vim 快捷键多，分为插入模式和保护模式也能有效保护代码。直接搜这个插件就可。

  设置一下粘贴板和系统共用：

  ```json
  {
    "vim.useSystemClipboard": true,
  }
  ```

* vibrancy （可能会导致卡顿）

  mac 下的透明插件，装了之后自动形成毛玻璃效果，相当地酷炫。

  在 vscode 以前的版本有一个叫 Custom CSS and JS Loader 的插件也是同样的效果。但是这个插件在版本迭代中产生了 bug，已经无效了。作者重写了一份，同时支持 mac 和 windows，就是这个 vibrancy。相比之前，vibrancy 不用自己导入文件，好用得多。但这个插件也有一个很难受多问题，就是会改动 vscode 的底层文件，让我们的 vscode 顶部老是显示“不支持”（解决方案在下面👇）。而且在 mac 中，设置全屏效果就消失了。有点拉胯。

  如果你用的是 **linux 系统**，我非常**推荐 GlassIt-VSC** 这个插件，透明美化非常好用！（而且不会卡顿）

* background（可能会导致卡顿）

  mac 的透明插件不太好用，所以就有了这个插件作为替代。（这俩可以兼容，可一起用）

  background 也是一个很酷炫的插件。装了之后，默认在右下角显示二次元小人儿，而且每打开一个标签页小人儿还不一样。最重要的是，这个小人儿**可以换成自己喜欢的图片！！！**

  不过这个插件同样也改了我们的配置文件，所以也会显示不支持（解决方案还在下面👇）。

* fix vscode checksum

  上面说到两个插件都改了我们的源文件，造成顶部显示 不支持 字样，很是煞风景。

  于是就有大佬开发了这样一个插件去去掉那些字。

  安装之后，command+shift+p (mac) / ctrl+shift+p (linux)，键入 `Fix checksum:Apply` 再重启就好了。

* ToDo Tree

  非常好用的注释插件，建议弄一个。

  只要在代码里打上 ```// TODO: ...```的注释，就可以快速定位。多用于记录待修改和未来修改的地方。

  不过有时候也会识别一些奇奇怪怪的代码，比如说 protoc 自动生成的那些。

* GitLens

  一个和 git 有关的代码。可以直观的看到哪一行代码是谁修改的以及时间。

  找人背锅这种，非常好用。

**总的 setting.json 结果：**

```json
{
  "go.formatTool": "goimports",
  "go.inferGopath": false,
  "go.lintOnSave": "file",
  "go.goroot": "/usr/local/go",
  "go.gopath": "/Users/jacksie/go",
  "vim.useSystemClipboard": true,
  "terminal.integrated.rendererType": "dom",
  "vscode_vibrancy.opacity": -1,
  "todo-tree.tree.showScanModeButton": false,
  "editor.fontSize": 16,
  "window.zoomLevel": 1,
  "editor.suggestSelection": "first",
  "vsintellicode.modify.editor.suggestSelection": "automaticallyOverrodeDefaultValue",
  "java.semanticHighlighting.enabled": true,
  "files.exclude": {
    "**/.classpath": true,
    "**/.project": true,
    "**/.settings": true,
    "**/.factorypath": true
  },
  "java.home": "/Library/Java/JavaVirtualMachines/jdk1.8.0_281.jdk/Contents/Home",
  "java.requirements.JDK11Warning": false,
  "python.pythonPath": "/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Versions/3.8/lib/python3.8",
  "workbench.editorAssociations": [
    {
      "viewType": "jupyter.notebook.ipynb",
      "filenamePattern": "*.ipynb"
    }
  ],
  "python.languageServer": "Microsoft",
  "editor.formatOnSave": true,
  "go.lintTool": "golint",
  "go.useLanguageServer": false
}
```

