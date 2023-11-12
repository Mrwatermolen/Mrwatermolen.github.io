---
title: 'CPP的std::call_once与模版编程的一个小坑'
tags: Note
date: 2023-04-07 16:45:40
---


C++支持可变模版参数

```cpp
template <typename... Args> void foo(Args... args);
```

而`std::call_once`准确执行一次可调用 (Callable) 对象 f ，即使同时从多个线程调用。

有时候为了方便会把`std::once_flag`声明在函数内中，比如说

```cpp
void callOne() {
  static std::once_flag flag;
  std::call_once(flag, []() { // do something});
  });
}
```

但是和可变模版参数结合在一起就一些看起来神奇的特性

```cpp
static int counter{0};

template <typename... Args> void test(Args... args) {
  static std::once_flag flag;
  static int calls{0};
  std::call_once(flag, [&]() {
    std::cout << "counter:" << ++counter << " call_once_times:" << ++calls
              << std::endl;
  });
}

int main() {
  test();
  test(1);
}
```

可以发现有两行输出

```text
counter:1 call_once_times:1
counter:2 call_once_times:1
```

这应该是模版展开了，其实是两个不同的函数了。解决方法也很简单，把`flag`变量往上提即可。

## 问题的发现

写单例模式时，类似于这样

```cpp
#include <mutex>
#include <utility>

template <typename T> class Singleton {
public:
  template <typename... Args> static T *getInstance(Args... args);

private:
  static inline T *instance{nullptr};
};

template <typename T>
template <typename... Args>
T *Singleton<T>::getInstance(Args... args) {
  static std::once_flag flag;
  std::call_once(flag,
                 [&]() { instance = new T(std::forward<Args>(args)...); });

  return instance;
};

int main() {
  Singleton<int>::getInstance(1);
  Singleton<int>::getInstance();
}
```

第二次会得到一个新的实例。问题就出在`static std::once_flag flag`不应该在函数体中定义，应该在类中声明定义。
