## SSH 和 Telnet

虽然这两种协议服务都可以远程登录另一台机器，**但SSH更安全**！

telnet是明文传送, ssh是加密的且支持压缩

此外ssh服务一般都提供sftp支持，支持文件传送。telnet一般只能通过zmodem等协议传送文件。ssh还可以借助ssh连接建立tcp通道，映射远端或本地的端口，以及转发X到本地X Server等。

使用Telnet这个用来访问远程计算机的TCP/IP协议以控制你的网络设备相当于在离开某个建筑时大喊你的用户名和口令。很快会有人进行监听，并且他们会利用你安全意识的缺乏。传统的网络服务程序如：ftp、pop和telnet在本质上都是不安全的，因为它们在网络上用明文传送口令和数据，别有用心的人非常容易就可以截获这些口令和数据。而且，这些服务程序的安全验证方式也是有其弱点的，就是很容易受到“中间人”（man-in-the-middle）这种方式的攻击。所谓“中间人”的攻击方式，就是“中间人”冒充真正的服务器接收你的传给服务器的数据，然后再冒充你把数据传给真正的服务器。服务器和你之间的数据传送被“中间人”一转手做了手脚之后，就会出现很严重的问题。

SSH是替代Telnet和其他远程控制台管理应用程序的行业标准。SSH命令是加密的并以几种方式进行保密。 
在使用SSH的时候，一个数字证书将认证客户端(你的工作站)和服务器(你的网络设备)之间的连接，并加密受保护的口令。SSH1使用RSA加密密钥，SSH2使用数字签名算法(DSA)密钥保护连接和认证。加密算法包括Blowfish，数据加密标准(DES)，以及三重DES(3DES)。SSH保护并且有助于防止欺骗，“中间人”攻击，以及数据包监听。

通过使用SSH把所有传输的数据进行加密，这样“中间人”这种攻击方式就不可能实现了，而且也能够防止DNS和IP欺骗。还有一个额外的好处就是传输的数据是经过压缩的，所以可以加快传输的速度。SSH有很多功能，它既可以代替telnet，又可以为ftp、pop、甚至ppp提供一个安全的“通道”。

如果从一台linux服务器通过ssh远程登录到另一台机器， 

这种情况通常会在多台服务器的时候用到。 

因为常常会为服务器架设一个防火墙，在外网不能直接访问某一台服务器， 

要能访问，则需要先连接到防火墙外面的一台机器，然后再通过它连接上防火墙内的服务器。 