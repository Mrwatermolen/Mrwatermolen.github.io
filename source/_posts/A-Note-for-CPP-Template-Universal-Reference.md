---
title: A Note for CPP Template Universal Reference
tags: Universal_Reference template
categories: cpp
date: 2024-01-13 13:16:09
---


如下的模版函数

<!-- more -->

```cpp
template <typename T> inline auto myFunc(T &&x) {
  return std::exp(std::forward<T>(x));
}
```

这段代码没问题

```cpp
double x = 0;
auto y = myFunc(x);
```

推导为`double myFunc<double &>(double &x)`

下面这段也OK

```cpp
auto y = myFunc(0.1);
```

显示实例化模版参数呢？

```cpp
{ auto y = myFunc<double>(0.1); } // OK

{
  double x = 0;
  auto y = myFunc<double>(x);
} // Error
```

`myFunc`被实例化为了`double myFunc<double>(double &&x)`接收右值引用，而`x`是一个左值，右值引用不能绑定一个左值，因此报错。

以下三种方式都可行

```cpp
{
  double x = 0;
  auto y = myFunc<double &>(x);
}

{
  double x = 0;
  auto y = myFunc<double>(std::move(x));
}

{
  double x = 0;
  auto y = myFunc<double>(std::forward<double>(x));
}
```

注意完美转发`std::forward<double>(x)`，其实就相当于

```cpp
double foo(double x) {return x;}
```

## Reason

显示实例化模版函数后`T&& x`中的`T&&`就不在是万能引用了，模版参数`T`被实例化为了`double`那么函数接收的参数为`double&& x`。

## 复杂一点

```cpp
#include <chrono>
#include <cmath>
#include <tuple>
#include <utility>

template <typename Func, typename... Args>
inline auto measureTime(Func&& func, Args&&... args) {
  using FuncReturnType = decltype(func(std::forward<Args>(args)...));
  using TimeDuration = decltype(std::chrono::high_resolution_clock::now() -
                                std::chrono::high_resolution_clock::now());

  const auto start = std::chrono::high_resolution_clock::now();
  if constexpr (std::is_same_v<void, FuncReturnType>) {
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return (end - start);
  } else {
    FuncReturnType ret = func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::tuple<TimeDuration, FuncReturnType>{(end - start), ret};
  }
}

template <typename T>
inline auto myFunc(T&& x) {
  return std::exp(std::forward<T>(x));
}

```

`measureTime`，其参数`func`是一个函数对象，其参数类型为`Args&&...`，返回值类型为`FuncReturnType`。`measureTime`函数的返回值类型为`TimeDuration`或者`std::tuple<TimeDuration, FuncReturnType>`。用于测量`func`的运行时间。

下面这段错误

```cpp
auto f = [](double x) {
  return measureTime(myFunc<double>, x);
};
```

原因就是显示实例化了模版函数`myFunc`，只能接受右值引用，而`x`是一个左值，因此报错。

好那我们尝试不显示实例化模版函数。如下：

```cpp
{
  auto f = [](double x) { return measureTime(myFunc, x); };
  f(0.1);
}
```

报错是`Candidate template ignored: couldn't infer template argument 'Func'`，唔，似乎只能显示实例化模版函数了。

```cpp
{
  auto f = [](double x) { return measureTime(myFunc<double>, std::forward<double>(x)); };
  f(0.1);
  double x = 0.1;
  f(x);
}
```

等一下，下面这段也是可行的

```cpp
{
  auto f = [](auto &&x) {
    using ArgType = decltype(x);
    return measureTime(myFunc<ArgType>, std::forward<ArgType>(x));
  };
  f(0.1);
  double x = 0.1;
  f(x);
}
```

## Achieved

`measureTime`，其参数`func`是一个函数对象，其参数类型为`Args&&...`，返回值类型为`FuncReturnType`。`measureTime`函数的返回值类型为`TimeDuration`或者`std::tuple<TimeDuration, FuncReturnType>`。用于测量`func`的运行时间。

`computeDerivative`，其参数`x`是一个引用类型，其参数类型为`T&&`，返回值类型为`auto`。用于计算`f`在`x`处的导数。

`myFunc`，其参数`x`是一个引用类型，其参数类型为`T&&`，返回值类型为`auto`。用于计算`exp(x)`。

```cpp
#include <chrono>
#include <cmath>
#include <tuple>
#include <utility>

template <typename Func, typename... Args>
inline auto measureTime(Func&& func, Args&&... args) {
  using FuncReturnType = decltype(func(std::forward<Args>(args)...));
  using TimeDuration = decltype(std::chrono::high_resolution_clock::now() -
                                std::chrono::high_resolution_clock::now());

  const auto start = std::chrono::high_resolution_clock::now();
  if constexpr (std::is_same_v<void, FuncReturnType>) {
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return (end - start);
  } else {
    FuncReturnType ret = func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::tuple<TimeDuration, FuncReturnType>{(end - start), ret};
  }
}


template <typename T, typename Func>
inline auto computeDerivative(T&& x, Func&& f) {
  return (f(x + 1e-6) - f(x)) / 1e-6;
}

template <typename T>
inline auto myFunc(T&& x) {
  return std::exp(std::forward<T>(x));
}

```

以下代码

```cpp
#include "test.h"

int main() {
  auto f = [](auto&& x) { return measureTime(myFunc, x); };

  auto der_f = [](auto&& x) {
    return measureTime(computeDerivative, x, myFunc);
  };

  // use explicit type
  using DataType = double;
  auto f_d = [](DataType x) { return measureTime(myFunc<DataType>, x); };
}
```

会在12行，也就是声明lambda对象`f_d`时报错。错误信息为：
> rvalue reference to type 'double' cannot bind to lvalue of type 'double'

因为`myFunc`的参数类型为`T&&`，而`f_d`的参数类型为`DataType`，`DataType`是`double`的别名，因此`f_d`的参数类型为`double`，而`double`是一个左值类型，因此`f_d`的参数类型为左值类型，而`myFunc`的参数类型为右值引用类型，因此`f_d`的参数类型与`myFunc`的参数类型不匹配，因此报错。

解决方法：就这个问题而言`MyFunc`可以将参数类型`T&&`修改为`const T&`，然后删去完美转发函数。

```cpp
template <typename T>
inline auto myFunc(const T& x) {
  return std::exp((x));
}
```

可是，如果，意外的是。没有修改`MyFunc`。我们修改了lambda的声明

```cpp
auto f_d = [](DataType x) { return measureTime(myFunc<DataType>, std::forward<DataType>(x)); };
// or
auto f_d = [](DataType x) { return measureTime(myFunc<double&>, x); };
```

也是行的。唔，然后继续到导数,如果没有修改声明，那么下面的代码

```cpp
auto der_f_d = [](DataType x){
    return measureTime(computeDerivative<DataType, DataType (*)(DataType)>,
                       std::forward<DataType>(x), myFunc<DataType>);
  };
```

修改，要么用lambda在包一层。或者修改函数的声明。

解决办法：修改函数的声明。

```cpp
template <typename T, typename Func>
inline auto computeDerivative(const T& x, Func&& f) {
  return (f(x + 1e-6) - f(x)) / 1e-6;
}

template <typename T>
inline auto myFunc(const T& x) {
  return std::exp((x));
}
```

lambda的声明改为

```cpp
auto der_f_d = [](DataType x){
    return measureTime(computeDerivative<DataType, DataType (*)(const DataType&)>,
                       x, myFunc<DataType>);
  };
```

或者

修改函数声明

```cpp
template <typename T, typename Func>
inline auto computeDerivative(T&& x, Func&& f) {
  auto&& y = x + 1e-6;
  return (f(std::forward<T>(y)) - f(std::forward<T>(x))) / 1e-6;
}

template <typename T>
inline auto myFunc(T&& x) {
  return std::exp(std::forward<T>(x));
}
```

然后lambda声明为

```cpp
auto der_f_d = [](DataType x) {
    return measureTime(computeDerivative<DataType, DataType (*)(DataType&&)>,
                       std::forward<DataType>(x), myFunc<DataType>);
  };
```

一个不用修改函数声明的方法是，再包一层lambda

函数声明为

```cpp
template <typename T, typename Func>
inline auto computeDerivative(T&& x, Func&& f) {
  return (f(x+1e-6) - f(x)) / 1e-6;
}

template <typename T>
inline auto myFunc(T&& x) {
  return std::exp(std::forward<T>(x));
}
```

然后这么定义：

```cpp
  auto ff = [](DataType x) { return myFunc(x); };

  auto der_f_d = [&ff](DataType x) { return computeDerivative(x, ff); };
```

### 结论

多用`const T&`。
