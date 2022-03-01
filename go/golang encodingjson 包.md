## golang encoding/json 包

### 从 Unmarshal 深入

以下为 unmarshal 函数的定义

```go
func Unmarshal(data []byte, v interface{}) error {
	// Check for well-formedness.
	// Avoids filling out half a data structure
	// before discovering a JSON syntax error.
	var d decodeState
	err := checkValid(data, &d.scan)
	if err != nil {
		return err
	}

	d.init(data)
	return d.unmarshal(v)
}
```

首先就是不会这个 decodeState

我们来看看 decodeState 是个什么玩意，还有用到的那几个成员/方法 d.scan、d.init、d.umarshal

根据注释，我们知道 decodeState 就是代表一个状态，又可以形象地描述为 状态机

```go
// decodeState represents the state while decoding a JSON value.
type decodeState struct {
	data                  []byte
	off                   int // next read offset in data
	opcode                int // last read result
	scan                  scanner // d.scan 就是 scanner 类型的一个成员
	errorContext          *errorContext
	savedError            error
	useNumber             bool
	disallowUnknownFields bool
}

// ... 省略
```

似乎看不太懂，没关系，先跳到 checkValid 函数里，根据注释，我们应当知道，这个函数是用来检测格式的完整性的。经过跳转，发现这个函数是写在 scanner.go 文件下的，而且马上就能知道 scan 那个成员，是个什么玩意了。

### Scanner

scanner 的定义：

```go
type scanner struct {
	step func(*scanner, byte) int

	endTop bool

	parseState []int

	err error

	bytes int64
}
```

就是个结构体，其中 step 是一个函数类型的成员

我们也不用被这么长一坨注释吓到，别忘了我们的目的，要找 checkValid 函数

```go
// checkValid verifies that data is valid JSON-encoded data.
// scan is passed in for use by checkValid to avoid an allocation.
func checkValid(data []byte, scan *scanner) error {
	scan.reset()
	for _, c := range data {
		scan.bytes++
		if scan.step(scan, c) == scanError {
			return scan.err
		}
	}
	if scan.eof() == scanError {
		return scan.err
	}
	return nil
}
```

这样看似乎也不是很难。

1. 首先是调用了方法 reset，这个不知道是什么
2. 然后遍历了 data，也就是遍历我们传进来的那个 raw string。也就是 json 格式的字符串
3. scan.bytes++ 按注释来说，bytes 存储总字节数，这里每遍历一个就 + 1 可以理解
4. scan.step 好，这个不懂，但是能看出如果产生了错误，那么返回
5. 调用 eof 方法，猜测可能和到结尾有关，但目前也不知道是什么，只知道产生了错误返回

所以，目前还要继续往下深挖的，有

* scan.reset
* scan.step
* scan.eof

#### scan.reset

```go
// reset prepares the scanner for use.
// It must be called before calling s.step.
func (s *scanner) reset() {
	s.step = stateBeginValue
	s.parseState = s.parseState[0:0]
	s.err = nil
	s.endTop = false
}
```

其实就是初始化操作，值得一提的是 s.step 的赋值

还有就是 byes 那个成员，在这里没有初始化

#### scan.step

从 reset 可以知道，是让 step 指向 stateBeginValue 的一个函数

```go
// stateBeginValue is the state at the beginning of the input.
func stateBeginValue(s *scanner, c byte) int {
	if isSpace(c) {
		return scanSkipSpace
	}
	switch c {
	case '{':
		s.step = stateBeginStringOrEmpty
		return s.pushParseState(c, parseObjectKey, scanBeginObject)
	// ...... 以下省略若干行
  }
	return s.error(c, "looking for beginning of value")
}
```

有点长，但没关系，看函数参数和注释，再联想到调用的地方（遍历data，把每个元素传到step里面）

似乎好像有点懂了。这个每一个 step 函数，就是在判断每一个字符的状态，可能还有点联系上下文的操作。说简单点就是判断是否合法。听不懂没关系，这也是猜测，接下来来看看都写了啥。

第一个函数， isSpace

```go
func isSpace(c byte) bool {
	return c <= ' ' && (c == ' ' || c == '\t' || c == '\r' || c == '\n')
}
```

就是判断第一个字符是不是空或者特殊字符呗，也挺简单的。注意这里的比较是依据 ASCII 的

再回去看调用的地方，如果是这种特殊字符，就返回常量 scanSkipSpace 。这是常量 int(9)

注意go里面声明常量如果首行赋值 iota ，那么默认从上往下从 0 开始递增赋值，自增长，如图:

```go
const (
	// Continue.
	scanContinue     = iota // uninteresting byte 值等于0
	scanBeginLiteral        // end implied by next result != scanContinue 值等于1
	scanBeginObject         // begin object 值等于2
	// ... 以下省略若干行
)
```

我们再回去看一下 调用 step 的地方，step 的返回值只有是 scanError ，也就是 int(11) 的时候，才返回

所以这里处理的步骤就和 常量 scanSkipSpace 字面意思一样了，就是跳过 space（空白/无效 字符）

... 经过我一波的看，好像理解了，这个 step 函数，通过每一步的重新赋值切换当前状态。

还挺牛的，这个写法，我把原来 scanner 600行代码缩写成了100来行的伪代码，瞬间就看懂了，如下:

```go
// 解析过程中返回的一些状态
const (
	// Continue.
	scanContinue     = iota // uninteresting byte
	scanBeginLiteral        // end implied by next result != scanContinue
	scanBeginObject         // begin object
	scanObjectKey           // just finished object key (string)
	scanObjectValue         // just finished non-last object value
	scanEndObject           // end object (implies scanObjectValue if possible)
	scanBeginArray          // begin array
	scanArrayValue          // just finished array value
	scanEndArray            // end array (implies scanArrayValue if possible)
	scanSkipSpace           // space byte; can skip; known to be last "continue" result

	// Stop.
	scanEnd   // top-level value ended *before* this byte; known to be first "stop" result
	scanError // hit an error, scanner.err.
)

// 这里的常量是放在栈里面的
const (
	parseObjectKey   = iota // parsing object key (before colon)
	parseObjectValue        // parsing object value (after colon)
	parseArrayValue         // parsing array value
)

Unmarshal(data, v){// v 是 interface 类型的结构体，用来存储反序列化后的 json 对象
    var d decodeState
	checkValid(){
		for <each char> in range data:
            // step 只处理错误，也就是只处理返回 scanError 但情况，如果不是，则 step 函数保持不变，进入下一次循环
			step(<each char>): // step 等于下列函数中的一个
                1. stateBeginValue { // 初始化为此函数
				    				if isSpace => return scanSkipState // 空白的直接跳过
                    switch(<each char>):
                    case '{':  => step = stateBeginStringOrEmpty => goto 2. 
                                  push parseObjectKey; // 将第一个解析对象的键这个状态塞进栈里
                                  return scanBeginObject;
                                  // 返回状态,这里源码写的很有意思，效率不高，但足够优雅
                    case '[': => step = stateBeginValueOrEmpty => goto 5.
                                 push parseArrayValue;
                                 return scanBeginArray;
                    case '"': => step = stateInString => goto 6.
                                 return scanBeginLiteral;
                    case '-': => step = stateNeg: // TODO
                                 return stateBeginLiteral;
                    case '0': => step = state0; // TODO
                                 return stateBeginLiteral;
                    case 't': => step = stateT; // TODO
                                 return stateBeginLiteral;
                    case 'f': => step = stateF; // TODO
                                 return stateBeginLiteral;
                    case 'n': => step = stateN; // TODO
                                 return stateBeginLiteral;
                    case from 1 to 9: => step = state1; // TODO
                                         return stateBeginLiteral;
                    if no case: => return error
                }   
                2. stateBeginStringOrEmpty { // 匹配到 '{' 的情况
                    if isSpace => return scanSkipSpace // 空白直接跳过
                    if '}': => stack[len-1] = parseObjectValue  // 栈的最后一位原来是 parseObjectKey
                        // 现在设置成 parseObjectValue，就是从解析 key 状态直接变到解析 value
                        return stateEndValue(s,<each char>) => goto 3.
                    else: => return stateBeginString(s,<each char>) => goto 7.
                    // 也就是说这里排除掉了 empty 的情况。注意这里的函数名，是 stateBeginStringOrEmpty
                }
                3. stateEndValue { // 匹配完的情况，几个函数都会跳到这里
                    if len(stack) == 0:
                        step = stateEndTop => goto 4.
                        s.EndTop = true; // s 是我们扫描类型 Scanner 的一个实体
                        // EndTop 标志表示到达了末尾
                        return stateEndTop(s,<each char>) => goto 4.
                    if isSpace => step = stateEndValue = > goto 3.; return scanSkipState // 跳过空白
                    switch (stack[len-1]): // 取出栈顶元素，判断是三种状态中的哪一种
                    case parseObjectKey:
                        if <each char> == ':':
                            stack[len-1] = parseObjectValue // 从 key 改成 value
                            step = stateBeginValue => goto 1.
                            return scanObjectKey // 表示已完成 key 的扫描
                    case parseObjectValue:
                        if <each char> == ',':
                            stack[len-1] = parseObjectKey // 从 value 改成 key
                            step = stateBeginString => goto 7.
                            return scanObjectValue // 表示已完成 value 的扫描
                        if <each char> == '}':
                            popStackFunction() => goto 8.
                            return scanEndObject // 返回一个对象扫描完成
                    case parseArrayValue:
                        if <each char> == ',':
                            step = stateBeginValue => goto 1.
                            return scanArrayValue
                        if <each char> == ']':
                            popStackFunction() => goto 8.
                            return scanEndArray
                    // 其实说到底，就是 scan key 和 scan value 两种状态。
                    // 其中 value 又分为 array 
                    // key 扫描完后，一定会遇到 ':'
                    // value 扫描完后，可能会遇到 ',' 或者 '}'
                    // array 同理，会遇到 ',' 和 ']'
                    // 其中 stack 存储的是当前扫描的状态，是扫描 key 还是 扫描 value 和 array
                    // 我在这里省略了 return error，源码中只要有一个地方有问题就直接 return error
                }
                4. stateEndTop { // 只有栈为空的时候才跳到这个状态
                    if !isSpace => return scanError
                    return scanError // 是的，这个函数只有 return error，因为按理说已经读完最顶层了
                    // 就算还有字符也只剩下 space 定义中的字符，不如就上面 for 循环直接退出。
                    // 但是这里没有退出，说明还有控股，如果不是空格那就有错
                }
                5. stateBeginValueOrEmpty { // 读了 '[' 之后的状态
                    if isSpace => return scanSkipSpace
                    if <each char> == ']' => return stateEndValue(s,<each char>) => goto 3.
                    return stateBeginValue(s,<each char>) => 1.
                }
                6. stateInString { // 读了 '"' 之后的状态
                    if <each char> == '"':
                        step = stateEndValue => goto 3.
                        return scanContinue
                    if <each char> == '\\':
                        step = stateInStringEsc // TODO
                        return scanContinue
                    if <each char> < 0x20:
                        return error
                    return scanContinue
                }
                7. stateBeginString { // 读 '"' 之前，可能会读 '"' 读状态
                    if isSpace => return scanSkipSpace
                    if <each char> == '"':
                        step = stateInString => goto 6.
                        return scanBeginLiteral
                    return error
                }  
                8. popStackFunction { // 弹出栈操作
                    pop stack[len-1]
                    if len == 0:
                        step = stateEndTop => goto 4.
                        s.EndTop = true
                    else:
                        step = stateEndValue => goto 3.
                }
        // 还有一些状态没有列出来，但那些都是匹配 string 时候的子状态，不是主要的
        if s.eof() == scanError = > return scanError
        return nil
    }
    // handle checkvalid error
    // ...

    d.init(data)
	return d.unmarshal(v)
}
```

说白了就是几个状态的转换，我们可以画个图来理解这个流程：

...

我觉得这个对我们自己开发还是有一定启发和帮助的，但是因为源码中用了大量的函数调用，所以可能会比较慢。

### umarshal 方法

我们用相同的方法，写伪代码来理解它









* sync.Pool

  对于很多需要重复分配、回收内存的地方，`sync.Pool` 是一个很好的选择。频繁地分配、回收内存会给 GC 带来一定的负担，严重的时候会引起 CPU 的毛刺，而 `sync.Pool` 可以将暂时不用的对象缓存起来，待下次需要的时候直接使用，不用再次经过内存分配，复用对象的内存，减轻 GC 的压力，提升系统的性能。

  https://www.cnblogs.com/qcrao-2018/p/12736031.html

* 函数类型

  感觉函数类型在标准库里面经常用到。c 语言是函数指针，go 语言里是函数类型，这是一种多态？





* valyala/fastjson 简单快速的 json 库