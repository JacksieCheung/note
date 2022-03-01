## Wrapper 模式

> 开放-关闭原则：类应该对扩展开放，对修改关闭。

wrapper 模式又叫包装模式，分为以下两种：

* adapter pattern 适配器模式
* decorater pattern 装饰模式

### adapter pattern

### decorater pattern

动态地给一个对象增加一些额外的职责（功能/方法），就增加对象功能来说，比生成子类实现更加灵活。

直接上uml图：

![](/Users/jacksie/Documents/note/设计模式/pic/decorater.png) 

最上面的 Component 是一个抽象构件，就是一个接口。ConcreteComponent 是实现该接口的具体构件。

Decorator是抽象装饰类，持有一个Component 的引用，它实现接口的方法里面直接调用该引用实现的方法。

再往后面就是具体装饰类A和B，他们可以扩展方法/改写方法等。

看不懂没关系，直接上代码：

```java
interface Component { // 抽象构件
  public void operation();
}

class ConcreteComponent implements Component { // 具体构件
  public ConcreteComponent(){}
  
  public void operation() {
    // do something ...
  }
}

abstract class Decorater implements Component { // 抽象装饰类
  Component selfComponent; // 持有抽象构件的引用
  
  public Decorater(Component component) {
    selfComponent = component;
  }
  
  public void operation() { // 所有方法的实现，都是调用抽象构件引用到实现方法
    selfComponent.operation();
  }
}

class ConcreteDecoraterA extends Decorater { // 具体装饰类作为抽象装饰类的子类
  public ConcreateDecoraterA(Component component) {
    super(component);
  }
  
  public void operation() { // 可以自己针对方法修改
    // add something ...
    super.operation();
    // add something ...
  }
  
  public void operation2() { // 也可以自己加新的方法
    // add someting
  }
  // add other functions ...
}

// other ConcreateDecoraterB ... 
```

如上。

代码写出来应该就很清楚了，如果还是看不懂，应该是 java 语法的问题。

现在知道了装饰模式怎么写，但是它的特点：**动态地给一个对象增加一些额外的职责** 怎么体现呢？

看一下代码如何调用就知道了：

```java
public class Client {
  public static void main(String args[]){
    Component c = new ConcreteComponent();
    c.operation();
    
    // use c to do something ...
    
    // d 也可以用 Component 类型存，但是不能调用子类新的方法
    ConcreteDecorater d = new ConcreteDecoraterA(c);
    d.operation();
    d.operation2();
  }
}
```

如上，这样就很清晰了，具体构件 c 可以随时在代码中动态装饰，而不是通过子类继承写死在代码里。

必要时取出d的引用，还可以变回去。

所以说装饰者模式完全做到了和父类解耦合，只要具体构件实现了 Component ，不管声明了多少个不同的具体构件，都能直接被具体装饰者装饰。而具体装饰者也不受具体构件影响，不管有多少个不同的具体装饰者，都能装饰具体构件。

而且具体构件还可以进行多次不同的装饰。

另外装饰者模式有分为透明和半透明两种：透明的装饰模式，要求具体构件角色、装饰角色的接口与抽象构件角色的接口完全一致。

简单的来说，就是透明要求装饰类不能有新的方法。







