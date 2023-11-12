---
title: CPP的单例模式
tags:
---
单例模式：保证一个类仅有一个实例，并提供一个该实例的全局访问点。

单例模式禁止外部拷贝。类自身负责保存它的唯一实例。

<!-- more -->

## 线程安全的单例模式

```cpp
#ifndef _SINGLETON_UNIQUE_PTR_H_
#define _SINGLETON_UNIQUE_PTR_H_

#include <memory>
#include <mutex>

// 为了能够接受含参构造的单例模式

/**
 * @brief 单例模式 懒加载 使用std::unique_ptr管理所有权 支持含参构造 线程安全的
 *
 */
template <typename T> class SingletonUniquePtr {
public:
  // 禁止拷贝构造以及拷贝赋值
  SingletonUniquePtr<T>(const T &) = delete;
  auto operator=(const T &) -> SingletonUniquePtr<T> & = delete;

  /**
   * @brief 获得全局唯一实例化对象 TODO(franzero):
   * 返回一个std::unique_ptr的引用真的合适吗
   *
   * @tparam Args
   * @param args
   * @return std::unique_ptr<T>& 全局唯一实例化对象的std::unique_ptr引用
   */
  template <typename... Args>
  static auto getInstance(Args &&...args) -> std::unique_ptr<T> &;

private:
  SingletonUniquePtr<T>() = default;
  ~SingletonUniquePtr<T>() = default;

  static inline std::unique_ptr<T> instance{nullptr}; // 全局唯一实例化对象
  static inline std::once_flag instanceFlag;          // 是否实例化标志
};

template <typename T>
template <typename... Args>
auto SingletonUniquePtr<T>::getInstance(Args &&...args)
    -> std::unique_ptr<T> & {

  // 保证只会执行一次
  // TODO(franzero): 异常处理
  std::call_once(instanceFlag, [&]() {
    instance = std::make_unique<T>(std::forward<Args>(args)...);
  });
  return instance;
}

#endif // _SINGLETON_UNIQUE_PTR_H_
```

## 单例模式的应用场景（TODO）

当你只需要存在一个唯一的实例化对象时

* 防止全局变量的命名污染
* 读取配置文件，读取的配置项是公有的，一个地方读取了所有地方都能用，没有必要所有的地方都能读取一遍配置。
* 数据库连接池，多线程的线程池。

引用[C++ 单例模式智能指针实现](https://blog.csdn.net/zhiqiang0819/article/details/124996422)不一定对，只做备份
>注意：
因为是单例模式，相当于全局变量的存在，在程序加载过程中可能会存在instance多线程同时加载的情况， 为了避免该懒加载模式出现问题，最好加一个资源锁。
另外，单例模式设计过程需要明确区分，句柄初始化和数据加载初始化。
1、句柄初始化
句柄初始化为类或者结构体初始化，
2、数据加载初始化
而数据加载初始化是为了服务正常工作，需要加载配置数据或者连接初始化（如服务配置、tcp连接、服务初始化、db数据库连接初始化、MQ连接初始化、redis连接初始化）
个人理解，单例模式适合做句柄初始化，从程序架构上来看，最好将逻辑分开，一则污染了单例模式的设计本意，二则代码逻辑上对单例模式也是承重的负担。数据加载初始化建议采用工厂模式解耦处理。
