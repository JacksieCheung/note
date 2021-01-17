## github团队开发中常用命令

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

`git checkout 1fbcb7ea3b43df60c639875d2bb68e20b451059e`

### 8. 提pr

这个通过网页版实现，提pr的标题要说明进行了哪些修改。

>更多相关，请看 `git --help`
