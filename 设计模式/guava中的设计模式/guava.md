## guava 中的设计模式

### 目录

- [guava 项目介绍](#guava-%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D)
- [Builder](#builder)
  - [使用建造者模式的必要性](#%E4%BD%BF%E7%94%A8%E5%BB%BA%E9%80%A0%E8%80%85%E6%A8%A1%E5%BC%8F%E7%9A%84%E5%BF%85%E8%A6%81%E6%80%A7)
- [Decorater(wrapper)](#decoraterwrapper)
- [Oberserver](#oberserver)
  - [简单介绍](#%E7%AE%80%E5%8D%95%E4%BB%8B%E7%BB%8D)
  - [观察者模式：](#%E8%A7%82%E5%AF%9F%E8%80%85%E6%A8%A1%E5%BC%8F)
  - [抽象：](#%E6%8A%BD%E8%B1%A1)
  - [更具体一点：](#%E6%9B%B4%E5%85%B7%E4%BD%93%E4%B8%80%E7%82%B9)
  - [EventManager](#eventmanager)
  - [GovernanceProperties(服务治理属性类)](#governanceproperties%E6%9C%8D%E5%8A%A1%E6%B2%BB%E7%90%86%E5%B1%9E%E6%80%A7%E7%B1%BB)
  - [GovernanceConfiguration(服务治理配置)](#governanceconfiguration%E6%9C%8D%E5%8A%A1%E6%B2%BB%E7%90%86%E9%85%8D%E7%BD%AE)
  - [UML 图](#uml-%E5%9B%BE)
  - [为什么要用 EventBus（订阅者模式）？](#%E4%B8%BA%E4%BB%80%E4%B9%88%E8%A6%81%E7%94%A8-eventbus%E8%AE%A2%E9%98%85%E8%80%85%E6%A8%A1%E5%BC%8F)
- [Immutable(不可变)](#immutable%E4%B8%8D%E5%8F%AF%E5%8F%98)
  - [定义：](#%E5%AE%9A%E4%B9%89)
  - [为什么要使用不可变对象？](#%E4%B8%BA%E4%BB%80%E4%B9%88%E8%A6%81%E4%BD%BF%E7%94%A8%E4%B8%8D%E5%8F%AF%E5%8F%98%E5%AF%B9%E8%B1%A1)
  - [使用场景：](#%E4%BD%BF%E7%94%A8%E5%9C%BA%E6%99%AF)
- [Strategy(策略)](#strategy%E7%AD%96%E7%95%A5)
- [SimpleFactory(简单工厂模式)](#simplefactory%E7%AE%80%E5%8D%95%E5%B7%A5%E5%8E%82%E6%A8%A1%E5%BC%8F)
- [单例模式](#%E5%8D%95%E4%BE%8B%E6%A8%A1%E5%BC%8F)

### guava 项目介绍

Guava是谷歌开发的一组核心Java库，包括新的集合类型(例如multimap和multiset)、不可变集合、图形库以及用于并发、I/O、散列、缓存、原语、字符串等的实用程序。它广泛应用于谷歌中的大多数Java项目，也被许多其他公司广泛使用。——摘自 readme

如果把 github 中所有 java 项目按照 star 数排序，它排第十二，仅次于spring 框架。

该项目起始于2007年，目前仓库一共有 43.1 k 的 star 数，下图是commit趋势：

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-commit-trend.png) 

可以看到，仍然很活跃。

guava 的仓库非常大，不过我们可以排除测试文件和适配 Androids 的代码。最后计算出来的代码行数一共是 15w 行。

guava 是封装了java的标准库，实际上很多地方都是用的同一种设计模式。比如说 collect 模块中就使用了 27 次装饰者模式。

能力有限，我们小组一共是找了 7 种设计模式。

### Builder

建造者模式的定义是：将一个复杂对象的构造与它的表示分离，使同样的构建过程可以创建不同的表示。

guava 中，有一个模块叫 cache。下面是 cache 的说明：

> cache 翻译过来就是**缓存**的意思。缓存类似于并发安全的map，但不完全相同。最基本的区别是，map将一直保存添加到它的所有元素，直到显式地删除它们。而缓存通常会配置有自动删除的策略，以限制其内存占用。

官方文档中提到:

> Obtaining a `Cache` is done using the `CacheBuilder` **builder pattern** as demonstrated by the example code above, but customizing your cache is the interesting part.

也就是说，guava 中的 cache 的创建，使用了建造者模式。下面看下用户如何使用：

```java
LoadingCache<Key, Graph> graphs = CacheBuilder.newBuilder() // CacheBuilder 就是建造器
       .maximumSize(1000) 
       .expireAfterWrite(10, TimeUnit.MINUTES)
       .removalListener(MY_LISTENER)
       .build( // 最后要调用 build 方法完成建造
           new CacheLoader<Key, Graph>() {
             @Override
             public Graph load(Key key) throws AnyException {
               return createExpensiveGraph(key);
             }
           });
```

可以看到基本结构是 `CacheBuilder.newBuilder().<many many methods...>.build()` 使用了链式调用。

其中 newBuilder 是建造器的创建函数，而build类似于书上写的`getResult`方法，即返回一个建造好了的产品。

我们可以看到，用 `LoadingCache` 这个类型来接收返回的产品。但是 `LoadingCache` 不是一个具体的产品，它是一个接口。具体的产品会根据 `build`是否有参数，返回 `LocalManualCache` 或者 `LocalLoadingCache` 。

其uml图如下：（以上面代码为例）

其中

1. 要建造的产品Product -- LocalManualCache 或者 LocalLoadingCache
2. 抽象的Builder -- Guava 的 cache 不需要多种建造者，故没有抽象 builder
3. Builder的具体实现ConcreteBuilder -- CacheBuilder类
4. Director -- Guava 的 cache 模块也没有导演，取而代之的是用户自己使用链式调用，也可以说用户自己就是导演。

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-builder-uml.png) 

所以 CacheBuilder 中的建造者模式，是根据具体使用场景，进行简化的建造者模式。

#### 使用建造者模式的必要性

在Guava中，构建一个缓存，需要配置n多参数，比如过期时间、淘汰策略、最大缓存大小等等。相应地，Cache类就会包含n多成员变量。我们需要在构造函数中，设置这些成员变量的值，但又不是所有的值都必须设置，设置哪些值由用户来决定。为了满足这个需求，我们就需要定义多个包含不同参数列表的构造函数。

为了避免构造函数的参数列表过长、不同的构造函数过多，我们一般有两种解决方案。其中，一个解决方案是使用Builder模式；另一个方案是先通过无参构造函数创建对象，然后再通过set()方法来逐一设置需要的设置的成员变量。

但是在Guava中，使用Builder模式更合适，因为在真正构造Cache对象的时候，我们必须做一些必要的参数校验，也就是build()函数中前两行代码要做的工作。（如下代码）

```java
public <K1 extends K, V1 extends V> Cache<K1, V1> build() {
  this.checkWeightWithWeigher(); // 这里要做参数校验
  this.checkNonLoadingCache(); // 这里要做参数校验
  return new LocalManualCache(this);
}
private void checkNonLoadingCache() {
  Preconditions.checkState(this.refreshNanos == -1L, "refreshAfterWrite requires a LoadingCache");
}
private void checkWeightWithWeigher() {
  if (this.weigher == null) {
    Preconditions.checkState(this.maximumWeight == -1L, "maximumWeight requires weigher");
  } else if (this.strictParsing) {
    Preconditions.checkState(this.maximumWeight != -1L, "weigher requires maximumWeight");
  } else if (this.maximumWeight == -1L) {
    logger.log(Level.WARNING, "ignoring weigher specified without maximumWeight");
  }
}
```

第一个函数检查了数据清除权重，如果这个参数不合法，就进行数据的释放回收。第二个函数检查了允许最大的缓存条目数，参数不合法也会导致无法进行数据的释放回收。如果采用无参默认构造函数加set()方法的方案，这两个校验就无处安放了。而不经过校验，创建的Cache对象有可能是不合法、不可用的。

### Decorater(wrapper)

这个设计模式在 guava 的 collect 模块。collect 模块是 guava 对 JDK 集合生态系统的扩展。这部分是 guava 最成熟和最受欢迎的部分。

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-collect-info.png) 

**有时我们需要编写自己的集合扩展。也许是想在将元素添加到列表时添加特殊行为**，像打印日志、维护特殊的变量等。显然，要给类添加功能，用装饰者模式比单纯的子类继承更好。**而 guava 在给我们提供实现装饰模式的扩展工具——forwarding 装饰器。**

guava 的 collect 文件夹下，有 27 个 forwarding 开头的文件，每个文件里面都有一个以 forwarding 前缀命名的抽象类。这些类，实际上就是装饰模式中的**抽象装饰类**。

可以选一个 ForwardingCollection 来看一下uml图：

> 以 ForwardingCollection、AddLoggingCollection、和 ArrayList 为例

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-decorater-uml.png) 

下面是 ForwardingCollection 的源码。（抽象装饰类的源码）

```java
@GwtCompatible
@ElementTypesAreNonnullByDefault
public abstract class ForwardingCollection<E extends @Nullable Object> extends ForwardingObject
    implements Collection<E> {
  // 可以看到这个抽象类继承自 forwardingObject 并且实现了 Collection 接口。
  // forwardingObject 是 guava 定义的抽象类，规定了 guava 中的抽象装饰器必须具有 delegate 抽象函数。
	// Collection 接口就是 java 标准库的接口。
  /** Constructor for use by subclasses. */
  protected ForwardingCollection() {}

  @Override
  protected abstract Collection<E> delegate(); // delegate 返回一个实现了 Collection 接口的对象
	// 其子类，也就是具体装饰类，必须实现 delegate 函数。
  
  @Override
  public Iterator<E> iterator() { // 下面的都是对 Collection 接口方法的重写
    return delegate().iterator(); // 实现方式都是直接调用 delegate 返回对象的方法
  }

  @Override
  public int size() {
    return delegate().size();
  }
  // ... 以下省略
}
```

可以看出来，这里的**抽象构件是 Collection 接口**，具体构件是 java 标准库中实现了 Collection 接口的类，比如说ArrayList。

乍看之下，和书上写的装饰者模式似乎不太一样。书上说，抽象装饰类应该持有抽象构件的引用，但是这个 ForwardingCollection 并没有持有 Collection 的引用。相对的，**guava 的抽象装饰类持有一个叫 delegate 的抽象函数，该函数返回一个抽象构件。** 可以看下实际如何使用：(比如我们想要给 collection 添加一个输出日志功能)

```java
public class AddLoggingCollection<E> extends ForwardingCollection<E> {
  private static final Logger logger = LoggerFactory.getLogger(AddLoggingCollection.class);
  private Collection<E> originalCollection; // 抽象构件的引用在这里持有
  public AddLoggingCollection(Collection<E> originalCollection) { // 构造函数
    this.originalCollection = originalCollection;
  }
  @Override
  protected Collection<E> delegate() { // 重写 delegate 函数，这里相当于把抽象构件交给了抽象装饰类
    return this.originalCollection;
  }
  @Override
  public boolean add(E element) { // 只用选取我们想要的方法添加新功能即可
    logger.info("Add element: " + element);
    return super.add(element);
  }
}
```

如上图，就是一个使用的例子。**可以看到抽象构件的引用，由具体装饰类持有，并通过 delegate 函数，交给抽象装饰类**。**其效果就和抽象装饰类持有抽象构件的引用是一样的**。

我认为这是对用户使用的考量。如果把抽象构件交给 guava 的抽象装饰类，那么用户使用 guava 提供的抽象装饰类过程中，是看不到有这个抽象构件引用的（指物理上看不到，不是对用户隐藏的意思）。如果让用户自己写这部分的代码，整体代码和逻辑会清晰一点。

### Oberserver

> *关于 Guava 的 EventBus 部分主要在：com.google.common.eventbus 中*

#### 简单介绍

#### 观察者模式：

允许定义一种**订阅机制** ，可在对象事件发生时**通知**多个 "观察" 该对象的其他对象。

**两个角色：**

Publisher 和 Listener/Subscribers

**接受事件：**

接受事件的对象应该暴露一个 public 的方法，用 @Subscribe 注解标注，该方法可以接受一个参数，该参数就是 Event 的类型。

当他注册到 EventBus 之后，就可以开始接受事件了。

**发送事件：**

实现 Post(object) 方法，EventBus 会自动将这个时间路由到注册的 Listeners 中。

订阅者将顺序收到这条事件，如果这个需要异步处理，可以使用 AsyncEventBus。

**休眠事件：**

如果一个事件没有订阅者，那么会被认为是 "dead"，这个事件会被封装到一个 DeadEvent 实例中，然后再重新发送回来。

#### 抽象：

下图是应用 EventBus 的大致 UML 图

我们可以看到 eventbus 具有 post 方法，负责发送 eventObject 具体事件

然后这个 EventBus 中具有 register 和 unregister 方法，负责注册和取消注册这个 Event Listener

然后 Guava 根据具体放进来的 eventObject 去调用通知 Listener 的 handler

![图 9](https://s2.loli.net/2021/12/19/IwyRKZqsp3V4JBi.png)  

分发的流程：

已经注册了的事件经过 EventBus 分发到监听了对应对象的订阅者哪里（Super Object）

![图 5](https://s2.loli.net/2021/12/19/gsZT9j5tqP3V7MF.png)  

#### 更具体一点：

> *以* [*huaweicloud/spring-cloud-huawei*](https://github.com/huaweicloud/spring-cloud-huawei/) *中的服务治理配置更新的观察者模式为例*

这里摘抄了一些代码片段，只抽出与观察者模式相关的代码设计实现

#### EventManager

他先是把 EventBus 封装成自己的 EventBus

也就是 `com.google.common.eventbus.EventBus` to `com.huaweicloud.common.event`，前者就是 `Guava` 的 EventBus

订阅者和发送者都只和这个 EventManager 交互，两者之间并不会有直接的耦合关系

```java
package com.huaweicloud.common.event;

import com.google.common.eventbus.EventBus;

public class EventManager {
  private static EventBus eventBus = new EventBus();

  public static EventBus getEventBus() {
    return eventBus;
  }

  // Post 方法，发送事件，这里用的是 Object，所有的事件的超类
  public static void post(Object event) {
    eventBus.post(event);
  }

 // Register 方法，将订阅者注册到 EventBus 中
  public static void register(Object subscriber) {
    eventBus.register(subscriber);
  }
}
```

#### GovernanceProperties(服务治理属性类)

这个类(的实例)就相当于一个订阅者，他将收到所有 `ConfigRefreshEvent` 相关事件的通知

```java
......

public abstract class GovernanceProperties<T extends Configurable> implements InitializingBean {
  ......

  // 我们来看这个服务治理的属性类，它是一个抽象类
  // 这里的 EventManager.register(this); 将这个类(现实中就是他的某一个实例)注册到了 EventBus 中
  protected GovernanceProperties(String key) {
    configKey = key;
    representer.getPropertyUtils().setSkipMissingProperties(true);
    EventManager.register(this);
    entityClass = getEntityClass();
  }

  ......

  // 上文提到过的 Subscribe 注解
  // 这说明了，这个 GovernanceProperties 的具体实现类将会接受到 EventBus 中的事件
  // 这个事件的类型是 ConfigurationChangedEvent
  @Subscribe
  public void onConfigurationChangedEvent(ConfigurationChangedEvent event) {
    // 接收到事件变化后的处理细节
    for (String key : event.getChangedConfigurations()) {
      if (key.startsWith(configKey + ".")) {
        String mapKey = key.substring((configKey + ".").length());
        parsedEntity.remove(mapKey);
        T entityItem = parseEntityItem(mapKey, environment.getProperty(key));
        if (entityItem != null) {
          parsedEntity.put(mapKey, entityItem);
        }
      }
    }
  }
  ......
}
```

```java
......

// 这个就是其中 ConfigRefreshEvent 事件的定义
public class ConfigRefreshEvent extends ApplicationEvent {
  private Set<String> change;

  // 这段注解也说明了这个是在这个发生时将要被调用的
  // this.change = change; 保存了这次事件的内容
  /**
   * Create a new ApplicationEvent.
   * @param source the object on which the event initially occurred (never {@code null})
   */
  public ConfigRefreshEvent(Object source, Set<String> change) {
    super(source);
    this.change = change;
  }

  // 这里的 getChange 就将返回事件中 change 的内容
  public Set<String> getChange() {
    return change;
  }

  public void setChange(Set<String> change) {
    this.change = change;
  }
}

......
```

#### GovernanceConfiguration(服务治理配置)

这里是一个发布者的角色，他只需要将配置变更的通知发布出去，再由订阅者们去进行具体的处理

```java
......

@Bean
  public ApplicationListener<ConfigRefreshEvent> governanceApplicationListener() {
    // 这里调用了 EventManager 的 post 方法，发送了 configRefreshEvent，也就是配置刷新事件
    return configRefreshEvent -> EventManager
        .post(new ConfigurationChangedEvent(new HashSet<>(configRefreshEvent.getChange())));
  }

......
```

#### UML 图

![图 8](https://s2.loli.net/2021/12/19/27jvRACs9bOiNyc.png)  

只要发布者通过同样的接口(EventBus)与所有订阅者进行交互， 那么在程序中新增订阅者时就无需修改已有发布者类的代码。

可以看出订阅者和发布者是解藕了的

#### 为什么要用 EventBus（订阅者模式）？

开闭原则：你无需修改发布者代码就能引入新的订阅者类，反之亦然。

可以将 事先未知的或动态变化的 实际对象通知给其他对象。并且可以解耦合开那些相互协作的类之间的“协作”关系比较复杂的类。

### Immutable(不可变)

#### 定义：

不变类：一个类的内部状态创建后，在整个生命周期都不会发生变化时，这样的类称作不变类。

使用不变类的做法就叫做不变模式，不变模式有两种形式：一种是弱不变模式，一种是强不变模式。

不变模式允许多个对象共享某一个对象，降低了对该对象进行并发访问时的同步化开销。如果需要改变一个不变对象的状态，那么就需要重新创建一个新的同类型对象，并在创建时将这个新的状态存储在新对象中，常见的不变类是`String`类

----

> 我们拿guava中的不可变集合来举例说明，[参考](https://wizardforcel.gitbooks.io/guava-tutorial/content/9.html)

#### 为什么要使用不可变对象？

- 不可变集合不需要考虑变化，可以节省时间和空间。所有的不可变集合都比它们的可变形式有更好的内存利用率（分析和测试细节）。
- 不可变对象被多个线程调用时，不需要考虑读写导致的竞争问题。
- 不可变对象因为有固定不变的特性，所以可以作为常量来安全使用。

#### 使用场景：

如果没有修改某个集合的需求，或者希望某个集合保持不变，就可以把它防御性地拷贝到不可变集中。

在`j2cl`中的`StacktraceAsserter`中，主要作为常量或者生成常量用于进行比较。这种常量会像String类型一样，如果有与现有的值不同的值出现的时候会直接创建一个同类型对象，而不是在原对象上修改。

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-immutable-uml.png) 

### Strategy(策略)

定义了一系列的算法，并将每一个算法封装起来，而且使它们还可以相互替换。策略模式让算法的变化不会影响到使用算法的客户。

策略模式有三个角色：环境类、抽象策略类、具体策略类。

环境类即使用策略的类，它需要持有一个抽象策略类的索引，其构造函数需要传入具体策略类。

具体策略类就是具体定义的算法。

guava 中的 Splitter（字符串切分）就使用了这种模式。下面是uml图：

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-strategy-uml.png) 

Splitter 就是一个环境类，它持有一个抽象策略类的索引。并且它的构造函数 Splitter 还是私有的，即不允许用户自己创建该类的对象。也就是说，Splitter本身还是一个简单工厂。而 on 方法，就是用来创建 Splitter 对象的。

而Strategy就是抽象策略类。

那么具体策略类在哪呢？

我们可以看下 Splitter 类的静态方法 on。on 有很多重载，我们只选其一来看：

```java
public static Splitter on(final CharMatcher separatorMatcher) {
	checkNotNull(separatorMatcher);

  return new Splitter(
  	new Strategy() {
    	@Override
      public SplittingIterator iterator(Splitter splitter, final CharSequence toSplit) {
      	return new SplittingIterator(splitter, toSplit) {
        	@Override
         	int separatorStart(int start) {
          	return separatorMatcher.indexIn(toSplit, start);
          }

          @Override
          int separatorEnd(int separatorPosition) {
          	return separatorPosition + 1;
          }
        };
      }
    });
}
```

这就很容易看出来了，Strategy 在 on 方法中创建，并且是以匿名类的形式。

而 on 方法根据用户传入参数的不同，重载了不同方法，用来创建了不同的 Strategy。也就是说，这里策略模式的作用，就是根据传入参数选择我们要执行的策略。

这个匿名类，就是我们的具体策略类。以上就是策略模式的三要素。

### SimpleFactory(简单工厂模式)

简单工厂模式又称为静态工厂方法模式，属于类创建型模式。可根据参数的不同返回不同类的实例。简单工厂模式专门定义一个类来负责创建其他类的实例，被创建的实例通常具有共同的父类。

简单工厂模式将责任分开，避免用户直接创建对应类的实例，而是通过工厂创建。

guava 中简单工厂模式很常见，比如 collect/Lists

```java
@GwtCompatible(emulated = true)
@ElementTypesAreNonnullByDefault
public final class Lists {
  private Lists() {}

  // ArrayList
  // 第一种创建方法
  @GwtCompatible(serializable = true)
  public static <E extends @Nullable Object> ArrayList<E> newArrayList() {
    return new ArrayList<>();
  }

  // 第二种创建方法
  @SafeVarargs
  @GwtCompatible(serializable = true)
  public static <E extends @Nullable Object> ArrayList<E> newArrayList(E... elements) {
    checkNotNull(elements); // for GWT
    // Avoid integer overflow when a large array is passed in
    int capacity = computeArrayListCapacity(elements.length);
    ArrayList<E> list = new ArrayList<>(capacity);
    Collections.addAll(list, elements);
    return list;
  }
  
  // 第三种创建方法
  @GwtCompatible(serializable = true)
  public static <E extends @Nullable Object> ArrayList<E> newArrayList(
      Iterable<? extends E> elements) {
    checkNotNull(elements); // for GWT
    // Let ArrayList's sizing logic work, if possible
    return (elements instanceof Collection)
        ? new ArrayList<>((Collection<? extends E>) elements)
        : newArrayList(elements.iterator());
  }
  
  // 第四种创建方法
  @GwtCompatible(serializable = true)
  public static <E extends @Nullable Object> ArrayList<E> newArrayList(
      Iterator<? extends E> elements) {
    ArrayList<E> list = newArrayList();
    Iterators.addAll(list, elements);
    return list;
  }
  // ... 以下省略
}
```

可以看到这个类有很多的 newArrayList 的静态方法，是用来创建不同情况的 ArrayList<E> 的。

用户如何使用呢：

```java
// 不使用 guava
List<int> list = new Arraylist<>();

// 使用 guava
List<int> list = Lists.newArrayList();
```

可以看到，Lists 确实避免了用户直接创造对象。这里的 Lists 可以说是一个工厂。

在书上写的简单工厂模式中，需要使用if对传入的参数进行判定，从而执行不同的创建逻辑。

而这里是通过函数重载实现的，根据传入参数类型的不同执行不同操作，根据不同方法创建不同类型

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-simpleFactory-uml.png) 

### 单例模式

guava 中提供了一个非常简单的方法来帮助用户实现单例模式，如下：

```java
public class Singleton {
    private static final Supplier<Singleton> INSTANCE = Suppliers.memoize(Singleton::new);
    
    private Singleton() {}
    
    public static Singleton getSingleton() {
        return INSTANCE.get();
    }
}
```

只需要用 guava 封装好的 Suppliers.memorize 方法，和 get 方法，就可以实现。

其中 get 方法每次返回的都是同一个对象，在底层不变。手写单例模式需要三行，而这里只需要一行。

![](/Users/jacksie/Documents/note/设计模式/guava中的设计模式/pic/guava-singleton-uml.png) 

其实 Suppliers 也是缓存，它提供了 memorize 和 memorizeWithExpiration 两种方法。前者是不超时，即永久存储，后者是超时自动销毁。

我们这里实现单例模式用的是前者。

另外，在 guava 的字符比较器 CharMatcher 中，也用到了单例模式:

```java
public static CharMatcher ascii() {
    return Ascii.INSTANCE;
}

private static final class Ascii extends NamedFastMatcher {

    static final Ascii INSTANCE = new Ascii();

    Ascii() {
      super("CharMatcher.ascii()");
    }
    ...
}
```

