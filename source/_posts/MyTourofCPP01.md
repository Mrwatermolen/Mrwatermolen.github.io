---
title: MyTourofCPP01
categories:
  - My Tour of CPP
date: 2023-04-05 23:23:43
tags:
---


>Focus on programming techniques, not on language features.

基于c++17标准
快速入门[CS106L](http://web.stanford.edu/class/cs106l/)
阅读书籍笔记《A tour of C++》，《Beginning C++17》

<!-- more -->

## Introduce

C++是编译型语言，是静态类型语言。

Hello World，Type，Variables，Arithmetic略过

### Initialization

Before an object can be used, it must be given a value. C++ offers a variety of notations for expressing initialization.

a universal form based on curly-brace-delimited initializer lists
也称作统一初始化（Uniform Initialization），一些例子

```cpp
std::vector<int> vec{1,2,3,4};
complex<double> z2 {d1,d2};
```

使用统一初始化的好处之一是在有缩窄转换时编译器会提醒报错

```cpp
int i{7.0};
```

在使用`auto`关键字，倾向于使用`=`进行初始化，因为使用`{}`可能会导致一些类型转换，当然如果为了统一，也是可以使用`{}`的。

We use `auto` where we don’t have a specific reason to mention the type explicitly. ‘‘Specific reasons’’ include:

* The definition is in a large scope where we want to make the type clearly visible to readers of our code.
* We want to be explicit about a variable’s range or precision (e.g., double rather than float).

## Scope and Lifetime

Scoped: Local, Class, namespace

## Constants

对于不变的概念在c++中有两种定义：`const`和`constexpr`
`const`：This is used primarily to
specify interfaces so that data can be passed to functions using pointers and references with- out fear of it being modified. 运行期时确定。

`constexpr`:：编译时确定
更多关于`constexpr`[可以参见](https://changkun.de/modern-cpp/zh-cn/02-usability/#constexpr)

指针，引用，赋值略过

## User-Defined Type

Struct,Class,Uion略

### Enumerations

```cpp
enum class Color {Red, Green};
```

以前的用法
Enumerations are used to represent small sets of integer values.

```cpp
enum Color {Red, Green};
```

以前枚举类被当作整数，会在意想不到的地方出问题。

## Modularity

c++中，我们一般在头文件中进行声明**declaration**，而在源文件中进行定义**defination**。当然这不是绝对的。

### Separate Compilation

分离编译的概念，声明和定义是可以分开的，而且使用者可以仅仅使用声明即可。

所以我们将接口声明在一个文件中以供他人使用，而在其他文件中实现这些声明。

**translation unit**: A .cpp file that is compiled by itself (including the h files it #includes)

The compiler processes each translation unit in a program independently to generate an object file.

如果编译单元能够正确生成目标文件，随后会交给链接器去处理，链接目标文件中的使用的外部定义，链接器会把其他目标文件链接起来。

声明可以存在多个，而定义只能存在一个。

In a given translation unit, no variable, function, class type, enumeration type, or template must ever be defined more than once. You can have more than one declaration for a variable or function, for example, but there must never be more than one definition that determines what it is and causes it to be created. If there’s more than one definition within the same translation unit, the code will not compile.

Note: a declaration introduces a name into a scope. a definition not only introduces the name but also defines what it is. in other words, all definitions are declarations, but not all declarations are definitions.

Modules以及Namespace和

### Error Handling

关注运行期错误处理。

Use of the exception-handling mechanisms can make error handling simpler, more systematic, and more readable. To achieve that, don’t overuse try-statements. The main technique for making error handling simple and systematic. RAII

如果一个函数被声明为`noexcept`说明这个函数是不会抛出任何的异常的
If all good intent and planning fails, so that user() still throws, std::terminate() is called to immedi-
ately terminate the program.

```cpp
void foo() noexcept;
```

In well-designed code try-blocks are rare. Avoid overuse by systematically using the RAII technique

这个**Invariant**我不太理解

c++提供一些宏帮助debug。`assert()`假定一个条件，在运行期总是满足

```cpp
void foo() {
  // ...
  int i;
  // ...
  assert(i == 1); // 我们断定此时i一定为1
  // ...
}
```

If the condition of an assert() fails in ‘‘debug mode,’’ the program terminates. If we are not in debug mode, the assert() is not checked.

Static Assertions语法

> static_assert ( bool_constexpr , string )

在编译时期的断言

Function Arguments and Return Values略过

## Classes

Concrete classes，Abstract classes，Classes in class hierarchies

类这块我感觉看设计模式更好一些

`dynamic_cast`

If at run time the object pointed to by the argument of `dynamic_cast` is not of the expected type (here, Smiley) or a class derived from the expected type, dynamic_cast returns nullptr.
We use `dynamic_cast` to a pointer type when a pointer to an object of a different derived class is a valid argument.

When a different type is unacceptable, we can simply `dynamic_cast` to a reference type. If the object is not of the expected type, `dynamic_cast` throws a bad_cast exception:

```cpp
A* a{};
B& b{dynamic_cast<A&>(*a)};
```

Code is cleaner when dynamic_cast is used with restraint.

If we can avoid using type information, we can write simpler and more efficient code, but occasionally type information is lost and must be recovered.

## Essential Operations

## Template

泛型编程我直接过
