# pygame 入门

* 为什么选择 pygame ?

  其一是因为 python 语法简单，容易入门

  其二是因为 pygame 封装仍然比较底层，可以更好的了解游戏如何运行。

  ~~其三是学长推荐~~

* pygame 特点?

  直接看 readme 就好了。https://www.pygame.org/wiki/about

  大概就是 pygame 封装了 sdl 库，封装了对鼠标键盘等一系列外设的调用。

## 1. pygame 开发游戏的基础

### 1.1 基本结构：事件循环

```python
import sys
import pygame


def run_game():
    # 初始化游戏并创建一个屏幕对象
    pygame.init()
    screen = pygame.display.set_mode((1200, 800))
    pygame.display.set_caption("Alien Invasion")

    # 开始游戏的主循环,事件循环
    while True:
        # 监视键盘和鼠标事件，获取事件
        for event in pygame.event.get():
            # 玩家点击右上角关闭按钮
            if event.type == pygame.QUIT:
                sys.exit()
                
        # 对游戏对象做修改，包括图片重新绘制等...

    		# 让最近绘制的更改可见，刷新屏幕
    		pygame.display.flip()


run_game()
```

简单地来讲，在```while```之前的语句都是游戏开始前的初始化语句。

而```while```里面的语句，是游戏的主逻辑。

在游戏主逻辑里，我们要干三件事：

* 获取事件（pygame 把一系列事件封装成了队列）

* 修改游戏对象

  游戏对象包括玩家控制的东西、不受控制的东西、反正就是各种组成游戏的东西。

  修改可以是修改坐标轴让对象移动，也可以是删除让它消失。但每次修改参数后需要重新调用```blit```等函数来重新绘制。

* 刷新屏幕（相当于对上一个步骤做一个保存操作，让玩家能看到结果)

### 1.2 如何更新屏幕？

上面讲了事件循环的基本过程，其中最后一步时更新屏幕调用了 `pygame.display.flip()` 函数来更新全部。

实际上 pygame 还提供了另外一个函数用来更新屏幕：

```python
update(rectangle=None) -> None
update(rectangle_list) -> None
```

`update` 用来更新局部游戏对象，是 `flip` 的优化版本。一次性传入多个对象比分开一次次传效率更高。

当没有参数时，`update` 和 `flip` 的效果一样。

### 1.3 处理游戏的基本图形：矩形

首先 pygame 有在屏幕上建立一个直角坐标系，是倒过来的（据说是笛卡尔坐标系），原点在左上角。

图形可以帮助我们在直角坐标系移动，从而实现游戏对象的移动。图形还能帮我们实现碰撞检测等需求。

pygame 有一个专门的矩形类 Rect，这里就讲讲矩形。

如果自己有用 pygame 开发过小 demo，那一定会遇到下面一个函数：

```python
self.rect = self.image.get_rect()
```

```get_rect()```函数创建返回一个 rect 类的对象。是这个图像的外接矩形。我们要操作一个游戏对象，不管是移动（改变矩形位置后重新将图像绘制在矩形上）、碰撞检测（判断矩形和矩形之间相交）还是提供用户鼠标点击等（检测点击坐标在矩形内），实际上都是操作这个矩形。

> 这里的 self.image 是 pygame 的 surface 类型，具体会在下面介绍

这个 rect 对象，有以下几个属性，改变他们可以改变矩形的位置或者大小：

```python
x,y
top, left, bottom, right
topleft, bottomleft, topright, bottomright
midtop, midleft, midbottom, midright
center, centerx, centery
size, width, height # 修改这三个会改变矩形的大小，其他属性只是改变位置
w,h
```

这里再列举一些平时常用到的函数：

* 让矩形以一定的速度和方向运行起来

  ```python
  move(x, y) -> Rect
  ```

  其中 x 和 y 是向量，有符号。符号表示的是移动的方向。增大 x 和 y 的绝对值能增加速度。

* 检测一个坐标是否在矩形内

  ```python
  collidepoint(x, y) -> bool
  collidepoint((x,y)) -> bool
  ```

  这个函数可以用于实现按钮。

* 检测两个矩形是否相交

  ```python
  colliderect(Rect) -> bool
  ```

  这个函数用于检测两个矩形是否相交

  > 需要注意的是 pygame 矩形的底边和右边不算在矩形内。如果刚好是这两条边相交，则不会被检测到。

* 网址:https://www.pygame.org/docs/ref/rect.html

### 1.4 pygame 中所有图片对象的类：surface

这个类用来表示 pygame 中的所有图片。

当我们调用如下函数导入一张图片时，返回的变量就是一个 surface 的对象：

```python
self.image = pygame.image.load(
            "/Users/jacksie/python/gameStudy/alien_invastion/image/ufo.png")
```

这里的 ```self.image``` 就是一个 surface 对象。

作为玩家，我们看到的游戏界面都是由大大小小的图片组成的，包括整个屏幕。

我们调用 ```pygame.display.set_mode()``` 函数创建一个屏幕时，也是返回一个 surface 对象。

所以 pygame 中游戏的运作过程，实际上就是对 surface 的重新绘制和刷新屏幕。

下面列举一下函数

* 在一个图像上绘制另一个图像

  ```python
  # 常用来将图片绘制到屏幕上 和 将图片绘制到矩形上来更新图片位置
  # dest参数可以是表示blit的左上角位置的一对坐标，也可以是一个Rect
  blit(source, dest, area=None, special_flags=0) -> Rect
  ```

* 给图片填充颜色

  ```python
  # 如果 rect 参数没指定，则将填充整个 surface
  fill(color, rect=None, special_flags=0) -> Rect
  ```

* 获得 surface 到外切矩形

  ```python
  # 获得一个和图片大小相同的矩形，初始化在坐标(0,0)
  # 也可以以元组传入初始化坐标
  get_rect(**kwargs) -> Rect
  ```

surface 类就只是图片而已，要移动图片，需要先移动它的矩形，再调用 `blit` 函数将图片绘制在矩形上。

### 1.5 pygame 中的基础游戏对象：精灵

上面提到，用矩形和图像就可以处理游戏对象了。而这里 pygame 提供的精灵（sprite）类，是为了让我们更好的分组管理游戏对象。

使用精灵创建游戏对象时，需要创建一个类。这个类继承自精灵类，如下：

```python
class Block(pygame.sprite.Sprite):

    # Constructor. Pass in the color of the block,
    # and its x and y position
    def __init__(self, color, width, height):
       # Call the parent class (Sprite) constructor
       pygame.sprite.Sprite.__init__(self)

       # Create an image of the block, and fill it with a color.
       # This could also be an image loaded from the disk.
       self.image = pygame.Surface([width, height])
       self.image.fill(color)

       # Fetch the rectangle object that has the dimensions of the image
       # Update the position of this object by setting the values of rect.x and rect.y
       self.rect = self.image.get_rect()
```

这个类中，我们需要创建 ```image``` 和 ```rect``` 属性，并且重写 ```update``` 方法。

最好这三个东西都要有。因为有一个我们会用到的类，叫 Group，可以简单地理解为精灵的集合。

Group 类 在调用 ```draw```，```update``` 和一些碰撞检测的函数时，需要用到这三个成员。

我们自己创建的游戏对象，可以从这个类派生出来，而且一般会和 Group 一起用。

下面简单列举一下 ```Group``` 类的方法和常用函数：

* 列出组内所有精灵

  ```python
  sprites() -> sprite_list
  ```

* 组内加入精灵/移除精灵

  ```python
  add(*sprites) -> None
  remove(*sprites) -> None
  ```

* 更新精灵/绘制精灵

  ```python
  update(*args, **kwargs) -> None # 会循环调用组内每个精灵的 update 方法，故需要重写
  draw(Surface) -> None # 会用到每个精灵的 image 和 rect 成员
  ```

* 碰撞检测

  ```python
  # 检测精灵和组碰撞
  spritecollide(sprite, group, dokill, collided = None) -> Sprite_list
  # 检测精灵和精灵碰撞
  collide_rect(left, right) -> bool
  # 检测组和组碰撞
  groupcollide(group1, group2, dokill1, dokill2, collided = None) -> Sprite_dict
  # 检测精灵和精灵碰撞，但是是圆形图形的碰撞
  # 需要精灵有一个可选的 radius 属性和 rect 类的成员
  # 没有 radius 属性时，将根据 rect 类成员生成一个外切圆
  collide_circle(left, right) -> bool
  ```

* 连接https://www.pygame.org/docs/ref/sprite.html

> 需要注意，pygame 中检测碰撞只有矩形和圆形两种，别的多边形虽然可以用 draw 类的函数画出来，但返回的是一个 surface。也就是说画出来的这个图形仍然要使用它的外切矩形来处理。

### Pygame 能开发哪些游戏？

~~这个板块纯属个人吹逼，任何内容没有经过实践验证，切勿当真！~~

从官方文档看来，pygame 比较适合做 2d 游戏，3d 就文档看到的函数应该是做不了的。

那么 2d 游戏也有很多，比如说；

* 2d 横版动作游戏（如街霸，死神大战火影）

  这种游戏我认为是可以做出来的，简单说下思路：

  玩家没按一个键，执行移动或者攻击。

  移动部分需要角色正在移动的图片，分为迈左脚和右脚，每次循环个多少次换一张显示。

  攻击也需要一组图片，没按一个键就放一张攻击的图片。

  这样动作部分就搞定了。那么还有一部分是攻击判定。

  攻击判定我们只需要给这个角色的类加一个攻击判定的 rect 成员，然后与另一个角色的对象做碰撞检测就行了。

* 2d 横版过关游戏（类似马里奥）

  这游戏b站就有教程，实际弄起来应该和上面那个差不多。

  不过这里加入了地图要素，可能要建一些代表地图的类，记录哪个坐标是地面，然后角色移动到对应地方就做检测，嗯，这里纯属扯淡。

* 2d音游（类似于节奏大师）

  感觉音游也是可以做的。

  不过可能需要提前做一个谱子，就是时间轴对应哪条通道落下音符。

  然后我们把这个谱子存到一个对象数组里面。每次判断时间是否到了，然后给对应的通道落下音符。

* 2d 塔防游戏（类似于保卫萝卜）

  明日方舟那种多半做不出来。

  感觉这种涉及地图的游戏还是比较困难的，因为需要在地图的边界做碰撞检测。

  但是我看到有人做出来了，所以做个简单的应该还好。

* ~~galgame~~

  ~~嗯，这个难度在于动画和配音~~

* 2d 第三人称射击游戏（类似于飞机大战）

  这个书上有示例，类似的射击游戏应该都行。