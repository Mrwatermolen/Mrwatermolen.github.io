---
title: CMake教程（一）：构建一个简单的可执行文件
date: 2023-01-28 13:34:52
categories:
  - Tutorial of CMake
tags:
  - CMake
  - CPP
---

> [CMake Help](https://cmake.org/cmake/help/latest/)
系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
本文基于[Step 1: A Basic Starting Point](https://cmake.org/cmake/help/latest/guide/tutorial/A%20Basic%20Starting%20Point.html)
环境Apple clang version 14.0.0 (clang-1400.0.29.202)以及cmake version 3.25.0
一些可以参考的资料[learnCmake](https://github.com/Akagi201/learning-cmake)

利用CMake构建一个简单的可执行文件。

<!-- more -->

## 一个简单的HelloWorld

```Shell
mkdir simpleProject && cd simpleProject
touch main.cpp 
touch CMakeLists.txt
```

在`main.cpp`中写入

```cpp
#include <iostream>

int main() { std::cout << "Hello World!\n"; }
```

### 简单的`CMakeLists.txt`的编写

编辑`CMakeLists.txt`文件写入

```cmake
cmake_minimum_required(VERSION 3.25.0) # CMake最低版本要求 VERSION >= 3.25.0

project(MyProjetc) # 设定项目名

add_executable(target1 main.cpp)
```

简单的解释，从`main.cpp`中编译链接生成`target1`可执行程序。

终端下

```Shell
cmake -B build
```

称为配置阶段(configure)。检测环境并生成构建规则，并在`build`目录下生成本地构建系统能识别的项目文件。
使用终端命令`ls`可以看到项目目录下生成了一个`build`文件夹

下一步，为构建阶段(build)。

```Shell
cmake --build build
```

`CMake`程序会进入`./build`构建生成目标可执行文件`target1`。可以在终端中

```Shell
./build/target1
```

Hello World!

## 指定C++标准以及配置项目版本号

指定C++标准，在`CMakeLists.txt`中添加一行

```cmake
# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)
```

配置项目版本号
修改`CMakeLists.txt`的中的`project(MyProjetc)`为`project(MyProjetc  VERSION 1.0.0)`

> Sometimes it may be useful to have a variable that is defined in your CMakelists.txt file also be available in your source code. In this case, we would like to print the project version.
One way to accomplish this is by using a configured header file. We create an input file with one or more variables to replace. These variables have special syntax which looks like @VAR@. Then, we use the configure_file() command to copy the input file to a given output file and replace these variables with the current value of VAR in the CMakelists.txt file.

生成配置头文件

创建文件`MyProject.h.in`并写入以下内容

```cpp
#define MyProject_VERSION_MAJOR @MyProject_VERSION_MAJOR@
#define MyProject_VERSION_MINOR @MyProject_VERSION_MINOR@
#define MyProject_VERSION_PATCH @MyProject_VERSION_PATCH@
```

向`CMakeLists.txt`添加

```cmake
configure_file("MyProject.h.in"
               "MyProject.h")
```

`Cmake`在配置阶段会根据`MyProject.h.in`文件和版本号自动生成`MyProject.h`文件，里面是项目版本号的宏定义。

```Shell
cmake -B build
```

会在项目二进制目录目录下会生成一个头文件`MyProject.h`，在`CMake`中有保留变量`PROJECT_BINARY_DIR`指示目录位置。在这个例子中为`build`目录。

在项目中使用，修改`CMakelists.txt`

```cmake
# 指定目标target1包含的头文件路径
# PUBLIC表示当前目标target1可以使用这些头文件 如果外部目标依赖target1，也可以使用这些头文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )
```

## CMake分离编译

### 目录结构

仍旧在`simpleProject`目录下，创建两文件`hello.h`和`hello.cpp`。

除去`build`文件夹，目录结构为

```tree
.
├── CMakeLists.txt
├── MyProject.h.in
├── include
│   └── hello.h
└── src
    ├── hello.cpp
    └── main.cpp
```

向`hello.h`文件写入

```cpp
#ifndef _HELLO_H_
#define _HELLO_H_

#include <iostream>

void printHelloWorld();

#endif // _HELLO_H_
```

声明了一个函数，在`hello.cpp`中实现

```cpp
#include "hello.h"

void printHelloWorld() { std::cout << "Hello World!\n"; }
```

在`main.cpp`中调用

```cpp
#include "hello.h"

int main() {
  printHelloWorld();
}
```

`CMakelists.txt`文件为

```cmake
# CMake最低版本要求 VERSION >= 3.25.0
cmake_minimum_required(VERSION 3.25.0)

# 设定项目名 以及版本号 Semantic Versioning
# 语义化版本 参考https://semver.org/
project(MyProject VERSION 1.0.0)

# 生成项目配置头文件
configure_file("MyProject.h.in"
               "MyProject.h")

# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)
# 生成一个compile_commands.json文件，和基于clang的工具一起使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# file 命令使用示例
# file(GLOB sources CMAKE_CONFIGURE_DEPENDS *.cpp *.h)

# 搜索项目目录/src下的所有的源文件并赋值给变量sources
aux_source_directory(${PROJECT_SOURCE_DIR}/src sources)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# 指定目标target1包含的头文件路径
# PUBLIC表示当前目标target1可以使用这些头文件 如果外部目标依赖target1，也可以使用这些头文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "include"
                          )

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

最后构建

```shell
cmake -B build && cmake --build build && ./build/target1
```
