## github团队开发中常用指令

### 1. 新建分支

`git branch [branch name]`

### 2. 切换分支

`git checkout [branch]`

### 3. 撤销 git add

`git reset HEAD`

### 4. 查看工作区状况

`git status`

### 5. 撤销提交

`git commit -m"fix:fix user service bugs." | git reset --soft HEAD^`

### 5.1 commit 规范

github 的 commit 应该包含四个部分： type scope subject body

分别表示提交修改的类型、提交修改的范围、提交修改的简短描述和提交修改的详细描述

其中 type 和 subject 是必须的。

**type有一下几种：**

**feat:** 新功能

**fix:** 修复 bug

**docs:** 文档改变

**style:** 代码格式改变

**refactor:** 某个已有功能重构

**perf:** 性能优化

**test:** 新增加测试/测试用例

**revert:** 撤销上一次 commit

**chore:** 改变构建流程、或者增加依赖库、工具等

**init:** 新建库

**build:** 改变了 build 工具

**subject的格式：**

简短描述，不超过50个字。多用动词开头，首字母小写，不加句号。

**一个例子：**

`fix:fix login part of user service`

### 6. 查看提交记录

`git log`

### 7. 回到以前版本

`git checkout [version_hash]`

版本哈希值通过 `git log` 查看。

### 8. 提pr

这个通过网页版实现，提pr的标题要说明进行了哪些修改。

### 9. 拉取上游分支最新修改并合并

`git pull`

一般默认为拉取 `master` 分支。自动拉取，自动合并。

### 10. 拉取远程仓库最新修改，但不合并

`git fetch <remote>`

`fetch` 只会将数据下再到本地仓库，并不会自动合并或者修改当前工作。必须手动合并。

### 11. 管理远程仓库
                                   
`git remote` 查看远程仓库。其中 `origin` 为`clone` 后的默认远程仓库名
 
`git remote add <shortname> <url>` 来添加远程仓库，其中 `<shortname>` 为远程仓库简写，此后都用此名字代替这个远程仓库。
 
`git remote show <shortname>` 可以查看远程仓库信息。

`git remote rename <oldname> <newname>` 改名

`git remote remove/rm <shortname>` 删除远程分支

### 12. 打标签 

我们可以给仓库历史中某一个版本打上标签，以示重要。

git 的标签分为两种：轻量标签的和附注标签

前者像一个不会改变的分支，它只是某一个特定提交的引用。

后者是存储是 git 数据库中一个完整的对象。是可以被校验的。

>其中包括打标签者的名字，电子邮件，日期时间，此外还有一个标签信息。还可以使用GPG签名并验证

`git tag` 列出标签

`git tag -l "v1.8.5*"` 按照特定的某是查找标签。按照模式查找要加上选项 `-l/--list` 但一般的不需要。

`git tag -a v1.4 -m "my version 1.4"` 创建附注标签，指定`-a` 选项即可，然后 `-m` 后输入信息。

`git show v1.4` 查看某一个标签的信息。

`git tag v1.4` 没有任何选项，就是轻量标签。

`git tag -a v1.2 <hash>` 可以对过去版本打标签，`<hash> 为版本hash值，通过`git log`查看。

`git push origin v1.5` 向远程仓库推特定标签。标签不会自动推，必须手动。

`git push origin --tags` 向远程仓库推所有标签。`git push`没有分辨两种标签的选项。

`git tag -d v1.4` 删除本地标签，但远程并没有删除

`git push origin :refs/tags/v1.4` 删除远程标签

`git push origin --delete <tagname>` 删除远程标签，同上

`git checkout v1.4 / git checkout -b v1.4` 分别是查看 `v1.4` 版本的仓库和新建一个分支，此分支的仓库是`1.4`版本的。第一条指令执行后，会处于头指针分离状态。

### 13. 别名

`git config --global alias.ci commit` `alias.<name>` 其中的`name`就是别名，后面的就是命令。这条指令执行后，`git ci` 等价于 `git commit`。

`git config --global alias.visual '|gitk'` 这样可以设置执行外部命令。

>更多相关，请看 `git <verb> --help 或 git help <verb>`
