## go 自定义类型

go 可以用 type 来自定义类型，如下：

```go
type Skills []string // 自定义了 Skills 类型

type Human struct { // 自定义了结构体类型
  name string
}
```

自定义类型不仅仅是 struct，struct 只是自定义类型里面一种比较特殊的类型而已，还有其他自定义类型申明，可以通过如下这样的申明来实现。

```go
type typeName typeLiteral
```

例：

```go
type ages int

type money float32

type months map[string]int

m := months {
 "January":31,
  "February":28,
 ...
 "December":31,
}
```

有点类似于别名，typedef

## go 函数变量

go 函数也可以作为一种变量，其类型就是所有拥有相同的 参数，相同的返回值的一种类型。

```go
package main

import "fmt"

type testInt func(int) bool // 声明了一个函数类型

func isOdd(integer int) bool {
 if integer%2 == 0 {
 return false
 }
 return true
}

func isEven(integer int) bool {
 if integer%2 == 0 {
 return true
 }
 return false
}

// 声明的函数类型在这个地方当做了一个参数
func filter(slice []int, f testInt) []int {
 var result []int
 for _, value := range slice {
 if f(value) {
 result = append(result, value)
 }
 }
 return result
}

func main(){
 slice := []int {1, 2, 3, 4, 5, 7}
 fmt.Println("slice = ", slice)
 odd := filter(slice, isOdd) // 函数当做值来传递了
 fmt.Println("Odd elements of slice are: ", odd)
 even := filter(slice, isEven) // 函数当做值来传递了
 fmt.Println("Even elements of slice are: ", even)
}
```

> 函数当做值和类型在我们写一些通用接口的时候非常有用，通过上面例子我们看到 testInt 这个类型是一个函数类型，然后两个 filter 函数的参数和返回值与 testInt 类型是一样的，但 是我们可以实现很多种的逻辑，这样使得我们的程序变得非常的灵活。---《go web 编程》

感觉可以用在工作台的 project。

## go 结构体匿名字段

结构体里面匿名字段可以是内置类型、结构体类型、自定义类型等。

匿名的类型可以直接通过访问类型名来访问这个对象。

如果是结构体，可以直接访问这个结构体的成员。

这其实是一个继承的过程。

```go
package main

import "fmt"

type Skills []string

type Human struct {
 name string
 age int
 weight int
}

type Student struct {
 Human // 匿名字段，struct
 Skills // 匿名字段，自定义的类型 string slice
 int // 内置类型作为匿名字段
 speciality string
}

func main() {
 // 初始化学生 Jane
 jane := Student{Human:Human{"Jane", 35, 100}, speciality:"Biology"}
 // 现在我们来访问相应的字段
 fmt.Println("Her name is ", jane.name)
 fmt.Println("Her age is ", jane.age)
 fmt.Println("Her weight is ", jane.weight)
 fmt.Println("Her speciality is ", jane.speciality)
 // 我们来修改他的 skill 技能字段
 jane.Skills = []string{"anatomy"}
 fmt.Println("Her skills are ", jane.Skills)
 fmt.Println("She acquired two new ones ")
 jane.Skills = append(jane.Skills, "physics", "golang")
 fmt.Println("Her skills now are ", jane.Skills)
 // 修改匿名内置类型字段
 jane.int = 3
 fmt.Println("Her preferred number is", jane.int)
}
```

## 方法接收者：值 vs 址

指针作为 Receiver 会对实例对象的内容发生操作,而普通类型作为 Receiver 仅仅是以副本作为操作对象, 并不对原实例对象发生操作。

### 方法

方法可以用在任何**自定义类型**上面。

另外指针调用方法不需要解引用，go 自动帮你解。

一个结构体类型有匿名字段（相当于继承）会继承这个匿名字段所有的方法。

可以通过给这个结构体定义同名的方法实现方法的重写。

## 公有私有

go 里面 大写开头的是公有，小写开头的是私有。方法和函数、变量都是这样。

私有的类型在别的包里不能调用，在同一个包里可以调用。

## interface

interface 是一个方法的集合。

我们通过 interface 来定义对象的一组行为。

interface 类型定义了一组方法，如果某个对象实现了某个接口的所有方法，则此对象就实 现了此接口。

一个对象可以实现任意多个 interface。

* interface 类型存值

  interface类型可以存任何实现了这个接口的值，这个其实和 java、cpp 面向对象很像，就是所谓的上转载。

* 空 interface

  任意的类型都实现了空 interface(我们这样定义：interface{})，也就是包含 0 个 method 的 interface。（所以可以传递任意值）

  空 interface(interface{})不包含任何的 method，正因为如此，所有的类型都实现了空 interface。空 interface 对于描述起不到任何的作用(因为它不包含任何的 method），但是空 interface 在我们需要存储任意类型的数值的时候相当有用，因为它可以存储任意类型的数 值。它有点类似于 C 语言的 void*类型。

  一个函数把 interface{}作为参数，那么他可以接受任意类型的值作为参数，如果一个函数 返回 interface{},那么也就可以返回任意类型的值。

## runtime 处理 goroutine 的函数

runtime 包中有几个处理 goroutine 的函数： 

* Goexit 退出当前执行的 goroutine，但是 defer 函数还会继续调用 
* Gosched 让出当前 goroutine 的执行权限，调度器安排其他等待的任务运行，并在下次某个时候从 该位置恢复执行。 
* NumCPU 返回 CPU 核数量 
* NumGoroutine 返回正在执⾏行和排队的任务总数 
*  GOMAXPROCS 用来设置可以运行的 CPU 核数

