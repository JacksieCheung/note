## redis list 阅读笔记

* 使用

  链表键、发布订阅、慢查询、监视器、保存多个客户端状态信息

* 特点

  双端、无环、带头尾指针（可以直接访问首尾节点）、带长度属性（直接得到长度）、多态(void* 函数)

  ```c
  typedef struct list {
  
      // 表头节点
      listNode *head;
  
      // 表尾节点
      listNode *tail;
  
      // 节点值复制函数
      void *(*dup)(void *ptr);
  
      // 节点值释放函数
      void (*free)(void *ptr);
  
      // 节点值对比函数
      int (*match)(void *ptr, void *key);
  
      // 链表所包含的节点数量
      unsigned long len;
  
  } list;
  ```

  三个函数指针可以分别指向处理不同类型的函数，从而实现多态

### 从源码看

redis 的 list 比较容易看懂，头文件 adlist.h 中有三个结构，分别是 list头节点、list 节点 和 list 迭代器

list 头节点也就是上面那个代码，记录list的信息。list节点也比较简单，就是正常的双端链表，前后指针+节点值。

迭代器有迭代到的节点和迭代方向两个成员

```c
typedef struct listIter {

    // 当前迭代到的节点
    listNode *next;

    // 迭代的方向
    int direction;

} listIter;

```

在一般的链表操作，我们可能会新建一个指针，然后用这个指针迭代。（因为头指针不能用于迭代）

所以迭代器就是将这个新建的指针封装起来，专门用于迭代。cpp 中也有这个概念，而且标准库有直接封装好的。

值得一提的是，adlist.h 里面将不少对链表的直接操作，封装成了函数宏。也就是我们在用代码操作链表的时候，不是直接对链表进行操作，而是调用函数。这省去了无意义的重复操作，同时隐藏了取址操作。这让人在写代码的过程中，可以不去看指针，而把注意力放在代码的逻辑上。

```c
#define listLength(l) ((l)->len)
// 返回给定链表的表头节点
// T = O(1)
#define listFirst(l) ((l)->head)
// 返回给定链表的表尾节点
// T = O(1)
#define listLast(l) ((l)->tail)
// ......
```

adlist 就和常规操作链表差不多。不过没看到给那三个函数指针赋值的地方。