---
title: c++协程初识
tags:
  - Note
  - Coroutine
categories: cpp
date: 2024-09-30 23:28:46
---

>A coroutine is a function that maintains its state between calls. In that, it’s a bit like a function object, but the saving and restoring of its state between calls are implicit and complete.

<!-- more -->

占个位置

```cpp
#include <coroutine>
#include <exception>
#include <format>
#include <iostream>
#include <random>
#include <thread>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(50, 200);

class CoroProducer {
public:
  auto await_ready() {
    std::format_to(std::ostreambuf_iterator<char>(std::cout),
                   "CoroProducer::await_ready\n");
    return false;
  }

  auto await_suspend(std::coroutine_handle<> h) {
    // do something before suspend
    std::format_to(std::ostreambuf_iterator<char>(std::cout),
                   "CoroProducer::await_suspend: Try to read\n");
    auto t = std::thread([h, this]() {
      // simulate the async operation to get data
      // std::this_thread::sleep_for(std::chrono::seconds(1));
      auto sleep_ms = dis(gen) * 5;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
      this->_data = dis(gen);
      std::format_to(std::ostreambuf_iterator<char>(std::cout),
                     "CoroProducer::await_suspend: Awake. data: {}\n", _data);
      h.resume();
    });

    t.detach();
  }

  auto await_resume() {
    // do something after resume
    return _data;
  }

private:
  int _data{};
};

template <typename T> class CoroConsumer {
public:
  CoroConsumer(T v) : _value(v) {}

  auto await_ready() { return false; }

  auto await_suspend(std::coroutine_handle<> h) {
    std::format_to(std::ostreambuf_iterator<char>(std::cout),
                   "CoroConsumer: value: {}\n", _value);
    h.resume();
  }

  auto await_resume() {
    // do something after resume
  }

private:
  T _value;
};

template <typename T> struct CoroRet {
  struct promise_type;

  struct promise_type {
    T _value;

    auto initial_suspend() {
      return std::suspend_never{};
      // return std::suspend_always{};
    }

    auto return_value(T v) { _value = v; }

    auto yield_value(T v) {
      _value = v;
      return CoroConsumer{v};
    }

    auto final_suspend() noexcept {
      // return std::suspend_never{}; // auto destroy the handle
      return std::suspend_always{}; // let the caller to destroy the handle
    }

    auto get_return_object() {
      return CoroRet<T>{HandleType::from_promise(*this)};
    }

    auto unhandled_exception() { std::terminate(); }
  };

  using HandleType = std::coroutine_handle<promise_type>;

  CoroRet(HandleType h) : _handle(h) {}

  CoroRet(const CoroRet &) = delete;

  CoroRet(CoroRet &&s) noexcept : _handle(s._handle) { s._handle = nullptr; }

  ~CoroRet() {
    if (_handle) {
      _handle.destroy();
    }
  }

  CoroRet &operator=(const CoroRet &) = delete;

  CoroRet &operator=(CoroRet &&s) noexcept {
    _handle = s._handle;
    s._handle = nullptr;
    return *this;
  }

  auto get() { return _handle.promise()._value; }

  auto done() const { return _handle.done(); }

private:
  HandleType _handle;
};

CoroRet<int> add(int times) {
  std::format_to(std::ostreambuf_iterator<char>(std::cout),
                 "Accumulate Start\n");
  int total = 0;
  for (int i = 0; i < times; i++) {
    auto val = co_await CoroProducer{};
    co_yield val;
    total += val;
    std::format_to(std::ostreambuf_iterator<char>(std::cout),
                   "times: {}, total: {}\n", i, total);
  }

  std::format_to(std::ostreambuf_iterator<char>(std::cout), "Accumulate End\n");

  co_return total;
}

int main() {
  auto res = add(5);
  while (!res.done()) {
    // busy waiting
  }

  std::format_to(std::ostreambuf_iterator<char>(std::cout), "Result: {}\n",
                 res.get());
  return 0;
}

```
