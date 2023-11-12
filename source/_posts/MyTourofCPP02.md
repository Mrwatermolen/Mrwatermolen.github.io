---
title: MyTourofCPP02
date: 2023-04-20 03:32:03
tags:
---


## Standary Library Overivew

略过

<!-- more -->

## Strings

If you need a C-style string (a zero-terminated array of char), string offers read-only access to its contained characters.

```cpp
void print(const string& s) {
    printf("For people who like printf: %s\n",s.c_str());
}
```

A string literal is by definition a `const char*`. To get a literal of type `std::string` use a `s` suffix.

### String Implementation

现在的字符串的实现使用了`short string optimitized`，对短的字符串，把字符串存储在对象本身中，如果是长的字符串则用把字符串存在`free store`，然后指向它。长短的定义有实现决定。

In particular, in multi-threaded implementations, memory allocation can be relatively costly. Also, when lots of strings of differing lengths are used, memory fragmentation can result. These are the main reasons that the short-string optimization has become ubiquitous.

To handle multiple character sets，`string`其实是`basic_string<char>`的别名

```cpp
template<typename Char> class basic_string {
// ... string of Char ... 
};

using string = basic_string<char>;
```

### String View

对于传递字符串而言，由于字符串的类型多样比如`std::string`，C风格的字符串等，而这可能使得情况变得复杂（？）为解决可以使用`std::string_view`。注意能使用`std::string_view`的场景必须是只读的。`std::string_view`的实现类似于一个指针指向头，然后内部记录一个长度。

## Regular Expressions

标准库提供了正则表达式的支持
To learn

## Input and Output

标准库没有网络流的IO。

IO流。Output流提供操作符`<<`把目标加入到流中。`<ostream>`头文件中全局变量`std::cout`是标准输出流，`std::cerr`是标准错误输出流。对于`char`类型（或是八位的整型）`std::cout`会将它们转换为ASCII码对应的字符。
Input流提供`>>`从流中读取数据。`<istream>`头文件中的全局变量`std::cin`是标准输入流。

### File System

标准库提供文件系统库，在头文件`<filesystem>`中。
Using`<filesystem>`, we can portably

* 浏览文件系统以及传递文件系统路径
* 知晓文件类型已经权限

## Containers

### Algorithms

## Utilties

### Resource Management

管理资源是个大问题。一些资源必须要请求，之后又要必须释放，否则系统就会认为这个资源并未闲置，正在被人占有。对于一个长期运行的程序来说，无法正确及时地释放资源被称作泄漏，会有大问题。即使是一些short program，也是有大问题的。

RAII思想。Resource acquistion is initialization。
智能指针`unique_ptr`以及`shared_ptr`

Where do we use ‘‘smart pointers’’ (such as unique_ptr) rather than resource handles with oper- ations designed specifically for the resource (such as vector or thread)? Unsurprisingly, the answer is ‘‘when we need pointer semantics.’’
比如分享目标需要传递指针时，或使用类的多态时。

锁的管理`scoped_lock`和`unique_lock`等

### 移动语义

本章略过

## Numerics

略过

## Concurrency

参考《C++ Concurrency in Action》
