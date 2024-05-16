---
title: 记首次编写CPP库所遇到的问题
tags:
  - CMake
  - CPP
categories:
  - 问题记录
date: 2024-05-16 23:33:48
---


> 毕设是做FDTD仿真的，打算弄一套完整的程序。目前把基础部分搭建起来了，就是FDTD算法核心部分。这部分代码是用C++写的，为了方便以后的使用，打算把这部分代码封装成一个库。方便以后写CLI或者GUI程序的时候直接调用。这是我第一次尝试写一个库，遇到了一些问题，记录一下。
前向声明：水平不高，一知半解，仅做记录，分析全错。问题的来源都是对于作用域的理解不够深刻。

<!-- more -->

## 单例模式的使用：不要在头文件中定义static

单例模式是一个很经典的设计模式。我主要是在使用MPI通信时，需要一个全局的通信器。这个通信器在整个程序中只有一个实例。所以我打算用单例模式来实现这个通信器。但是在实现的过程中遇到了一些问题。

不要在头文件中定义static，主要是针对跨dll的情况。这个情况是我之前写程序时从来未考虑到的。
一个单例模式，在`singleton.h`文件中：

```cpp
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

class Singleton {
public:
  static Singleton &getInstance() {
    static Singleton instance;
    return instance;
  }

public:
  Singleton(Singleton const &) = delete;

  auto operator=(Singleton const &) -> Singleton & = delete;

  Singleton(Singleton &&) noexcept = delete;

  auto operator=(Singleton &&) noexcept -> Singleton & = delete;

private:
  Singleton() = default;
};

#endif // __SINGLETON_H__
```

之前写程序时这样就可以了，自已用也没有问题。但是如果这个库是要给别人用的，那么就会出现问题。库内部使用了这个单例，但是别人在使用这个库的时候，也可能会调用`getInstance`方法去获取这个单例。

static变量的唯一性只是动态库级别的（引用自知乎）。

问题就来了，别人调用`getInstance`方法时，是要引入`singleton.h`头文件的。这样就会导致在别人的代码中也定义了一个`static Singleton instance`。这样就会导致两个不同的实例。

简单的解决办法是，头文件声明`getInstance`方法，把`static Singleton instance`放到`singleton.cpp`文件中。这样就不会出现这个问题了。

## CMake: add_definitions不会传递

在写库的时候，我用了一些宏定义。这些宏定义是在`CMakeLists.txt`文件中通过`add_definitions`添加的。但是在使用这个库的时候，这些宏定义并没有传递过去。

用了`XFDTD_CORE_WITH_MPI`控制了代码中MPI代码部分的控制。Windows和Linux有大问题，一个具体的表现为

```cpp
int main() {
    using A = my_lib::A;
    {
        auto a = std::make_unique<A>(Args);
    }
}
```

类A是管理某个资源的类，析构时释放该资源。
类A的实现包含`XFDTD_CORE_WITH_MPI`控制的代码块，且在类A的头文件中也包含了`XFDTD_CORE_WITH_MPI`控制的代码块。

也就是说类A管理关于MPI的相关资源，上面的代码在Linux和Window下无法运行，报错。Mac下本来也是无法运行的，我在类A得定义下添加了一个空指针`void* _nothing {nullptr}`，这样就可以运行了。

更离谱的是，如果改为

```cpp
int main() {
    using A = my_lib::A;
    {
        auto a = A{Args};
    }
}
```

可以运行。阿哲

这个错误很有意思，有很多种出错的现象。运行时出错，链接出错等。

解决方法（不一定对）。思路是在引用这个库时，判断如果使用了MPI，就添加`XFDTD_CORE_WITH_MPI`宏。

我在cmake安装的中间文件`xfdtd_core-config.cmake.in`添加如下：

```txt
if (XFDTD_CORE_WITH_MPI)
  find_dependency(MPI REQUIRED)
  message(STATUS "MPI include: ${MPI_CXX_INCLUDE_DIRS}")
  add_definitions(-DXFDTD_CORE_WITH_MPI)
endif()
```
