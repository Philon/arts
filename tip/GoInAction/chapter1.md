# 《GO语言实战》学习笔记1
- 时间：2007年9月的某个下午
- 地点：Google公司
- 人物：
Rob Pike(罗勃特·派克，unix成员，参与开发UTF-8)  
Ken Thompson(肯尼斯·汤普逊，unix和c语言作者)   
Robert Griesemer(罗伯特·格瑞史莫，参与v8引擎、甲骨文JVM开发)

刚刚被C++标准委员会的人叫去讨论的他们，看到了下一代C++的新颖功能，由衷地发出感慨：`你们还觉得C++的特性不够多，不够复杂么！`回到办公室，三人接着各种喷，山河日下啊，是时候展现表演的技术了！

所以他们利用Google给他们的20%自由支配时间，开始倒腾一种新语言，要简洁、高效、便于大型复杂软件开发。由于三人都是各种领域中的翘楚，吸纳了各种语言的优点，用实力告诉全世界什么叫做——less than more。

他们把这门语言命名为GO，并配了一只土拨鼠作为吉祥物，为什么呢？估计是某个下午，他们当中的谁正在撸着C++代码，然后忍无可忍蹦出一句👇：

![](https://i.loli.net/2019/01/18/5c418a238e7a3.jpeg)

*以上是我结合历史背景瞎编的*
***
## Why GO？
1. **较高的开发速度**

大型软件往往需要极漫长的编译时间，我曾尝试过手动编译gcc、chrome等经典软件，少了1个小时出不来，而且还是在不出错的情况下。而GO程序可以在1秒内完成编译，较大型的软件也仅需几十秒。

此外，GO继承了静态语言和动态语言的优势，你可以像运行脚本一样运行go语言快速看结果，也可以直接编译成二进制提高性能。同时，GO不存在像javascript的变量类型不明确的问题，类似C/C++/Java一样，如果某个变量类型错误，在编译阶段你就能定位到。

2. **为并发而生**

现如今，100多核的高性能服务器早已司空见惯，而传统的编程语言如C++/Java依旧是单核思想，尽管有线程机制，但开发者不得不谨慎思考`全局变量、共享内存、IO`这些资源访问方式，从而产生大量与业务无关的代码。

GO语言提供goroutine和channel两种机制，也是该语言的核心思想之一：
- goroutine负责启动某个业务函数独立运行
- 业务所生产/消费的数据直接放到channel中
两个goroutine间共享一个channel，其中的数据是同步的，开发者大可不必操心所谓的`互斥/同步`等访问机制，只需专注写业务代码。

3. **强大的类型系统**

和C语言一样，GO语言仅内置了几种如int、string等类型，同时支持开发者自定义类型。

如果从面向对象的视角来看，GO和Java/C#不一样，它不支持类型继承。换而言之，类似`Student -> Peopler -> Object`这种类型继承的思想在GO中是不存在的。GO提供一种全新的理念——行为建模。

`如果一只动物叫起来像鸭子，那它可能就是鸭子！`

记住上面这句话，这是GO对“继承”的重新定义，提供一种叫做“接口”的概念来对各种类型进行打包组合。初识GO的时候我很不理解这个概念，毕竟接口的概念面向对象也有(简单的理解就是留给继承者去实现的方法)，我仅在此试图做个简单总结：

假设我们要为某个软件实现将数据/文件拷贝到USB设备的业务，同时要便于今后千奇百怪的存储设备的扩展。

如果用Java，大概会这么实现：
```java
public interface UsbDisk {
   void read();
   void write();
}

public class UsbHDD implements UsbDisk ...
public class UsbSSD implements UsbDisk ...

file.copyTo(UsbDevice device);
```
说白了，为了满足`file.copyTo()`这个方法能够支持各种各样的类型，我们需要先抽象一个`UsbDisk `(不管是接口还是基类)，而实现它的`UsbHDD UsbSSD`总存在着千丝万缕的关系，否则就无法被copyTo调用。

有意思的地方来了，USB口不仅可以插存储设备，还可以插鼠标、键盘、手机等，这些设备也存在读写操作，比如`iPhone`这个类没有继承或实现，或者干脆就不属于`UsbDisk`，怎么办？

可能有一波自诩大师的架构者们会说：`哪个二货会这么干？从一开始就应该抽象一个UsbDevice`。是的，正如我们今天看到的，很多面向对象的框架下，数以百计的类型都有一个共同的基类——Object。

但如果用GO，则完全不同：
```go
// 为当前业务定义好某种接口
type UsbDisk interface {
    Read()
    Write()
}

// 定义一个UsbHDD类型，实现read/write方法，但没有“继承”
type UsbHDD struct {}
func (d UsbHDD) Read() ...
func (d UsbHDD) Wirte() ...

// UsbSSD和IPhone的实现同上👆
type UsbSSD struct {} ...
// 就算iPhone不属于存储设备，只要实现读写操作，一样可用
type IPhone struct {} ...

// file.copyTo，接受任何实现UsbDisk接口方法的类型
func copyTo(device UsbDisk) {
	device.Read()
	device.Write()
}
```
可以看到，GO语言中`UsbHDD UsbSSD IPhone`这三个类之间根本没有任何关系，也不继承`UsbDisk`，但却可以通过以下方法来调用：
```go
func main() {
	var hdd UsbHDD
	var ssd UsbSSD

	copyTo(hdd)
	copyTo(ssd)
}
```
也就是说，`实现了相同方法的类型(叫起来像鸭子)，可以被当作同一类型(它就是鸭子)`。如果再遇到业务扩展时，不需要推翻之前的架构，或者陷入“抽象”的哲学思考当中。(啊～人和咸鱼，到底有什么共性呢)

PS：相比于面向对象，GO语言的接口概念比较颠覆，一口气写多了😅

4. **内存垃圾回收**

还是原来的配方，还是原来的味道，本书也只是提了这么一句。我觉得垃圾回收机制根本算不上GO的优势，毕竟很多经典语言都有的功能好伐。(C/C++笑而不语，谁敢和我比经典)

总之，作者的意思应该是说，憋管内存分配的问题，放心大胆地用。另外GO虽然有指针，但不是你想的那样。

## Hello GO！
好了，说了那么多，无非就是helloworld：
```go
package main	// 每个go源码都所属一个包，参考java
import "fmt"	// 导入依赖包，参考java

// 入口函数，main函数必须在main包当中，必须！
func main() {
    fmt.Println("Hello world!")
}
```

## 小结一下
- GO是一门现代计算机技术驱动下的语言
- GO通过goroutine和channel机制优雅地解决并发问题
- GO同时具备静态和动态类型语言的有点，可以二进制或脚本形式运行
- GO的接口思想是一种“鸭子类型”的继承概念
- GO同样提供内存垃圾回收管理机制
- GO提供了类似Java/NodeJS/dotnet一样功能丰富的包
- [The Go Playground](http://play.golang.org)可以在web上执行代码，前提是科学上网