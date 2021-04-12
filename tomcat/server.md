## tomcat 部署方式

### 1. 将 html 文件夹直接拖到 tomcat 对应的 webapp 目录下

访问的时候直接对应 /webapp/ 后面的路径，加到 localhost/8080/ 后面

### 2. 修改 conf 文件夹下的 server.xml

在文件中，host 标签里面加下如下语句：

```xml
<Context docBase="文件夹的路径" path="/文件在服务器中的访问路径"/>
```

然后浏览器访问时，直接访问 path 对应的虚拟路径就可以了。

### 3. 在 Catlina 文件夹下的 localhost 文件夹新增 xml 文件

第三种是比较推荐的部署方式。如题，直接新增一个 xml 文件

并在文件中加上如下语句：
```xml
<Context docBase="文件夹的路径"/>
```

就可以了。

浏览器访问需要通过 /localhost:8080/xml的文件名/html 文件 来访问。

* 注意路径是 xml 文件名后直接跟 html 文件，html 文件夹忽略。
