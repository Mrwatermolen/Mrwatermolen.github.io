---
title: CMake教程（一）：构建一个简单的可执行文件
date: 2023-01-28 13:34:52
categories:
  - My Tutorial of CPP
tags:
  - CMake
  - CPP
---

> [CMake Help](https://cmake.org/cmake/help/latest/)
系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
本文基于[Step 1: A Basic Starting Point](https://cmake.org/cmake/help/latest/guide/tutorial/A%20Basic%20Starting%20Point.html)
环境Apple clang version 14.0.0 (clang-1400.0.29.202)以及cmake version 3.25.0

利用CMake构建一个简单的可执行文件。

<!-- more -->

## 一个简单的HelloWorld

```Shell
mkdir SimpleProject && cd SimpleProject
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
configure_file("${PROJECT_SOURCE_DIR}/MyProject.h.in"
"${PROJECT_SOURCE_DIR}/MyProject.h")
```

`cmake`在配置阶段会根据`MyProject.h.in`文件和版本号自动生成`MyProject.h`文件，里面是项目版本号的宏定义。

```Shell
cmake -B build
```

项目目录下会生成一个头文件`MyProject.h`，可以包含这个头文件然后打印版本号。

## CMake编译多个C++源文件

### 文件结构

仍旧在`simpleProject`目录下，创建两文件`fun1.h`和`fun1.cpp`。

```Shell
touch myFunctions.h && touch mySqrt.h
```

在`myFunctions.h`中写入

```cpp
#ifndef _MYFUNCTIONS_H_
#define _MYFUNCTIONS_H_

/**
 * @brief 一个简单的根号求解
 * 
 * @param x 非负整数
 * @return int x的算数平方根 舍去小数部分
 */
int mySqrt(int x);

#endif // _MYFUNCTIONS_H_
```

在`mySqrt.cpp`实现这个方法

```cpp
#include "myFunctions.h"

int mySqrt(int x) {
  int l = 0, r = x, ans = -1;
  while (l <= r) {
    int mid = l + (r - l) / 2;
    if ((long long)mid * mid <= x) {
      ans = mid;
      l = mid + 1;
    } else {
      r = mid - 1;
    }
  }
  return ans;
}
```

在`main.cpp`中使用`mySqrt()`方法，修改`main.cpp`

```cpp
#include "myFunctions.h"

#include <iostream>

int main() {
  std::cout << "Hello World!\n";
  std::cout << "100的算数平方根为" << mySqrt(100) << "\n";
}
```

### 修改`CMakeLists.txt`

如果此时直接执行构建`cmake --build build`就会出错，在链接阶段在`main.cpp.o`中存在为定义的符号`mySqrt()`。我们要告诉`cmake`要我们需要编译`main.cpp`和`myFunctions.h`以及`mysqrt.cpp`并且它们链接起来生成可执行程序`target1`

一个比较直观或是笨一点的方法是：
修改CMakelists.txt

```cmake
# 定义变量sources 其包含文件main.cpp myFunctions.h mysqrt.cpp
set(sources main.cpp myFunctions.h mysqrt.cpp)
# 为MyProject添加一个可执行目标target1
add_executable(target1)
```

但是这样的问题在于如果文件很多，这样的手动添加的方式就很麻烦。

我们可以使用`file()`命令。

```cmake
file(GLOB sources CMAKE_CONFIGURE_DEPENDS *.cpp *.h)
```

最后的的`CMakeLists.txt`为

```cmake
# CMake最低版本要求 VERSION >= 3.25.0
cmake_minimum_required(VERSION 3.25.0)

# 设定项目名 以及版本号 Semantic Versioning
# 语义化版本 参考https://semver.org/
project(MyProject VERSION 1.0.0)

configure_file("${PROJECT_SOURCE_DIR}/MyProject.h.in"
               "${PROJECT_SOURCE_DIR}/MyProject.h")

# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)

# 使用GLOB来从源文件树收集*.cpp和*.h文件列表 并存储到变量sources中
# CMAKE_CONFIGURE_DEPENDS告知有目录下有任何的文件修改都会重新配置
file(GLOB sources CMAKE_CONFIGURE_DEPENDS *.cpp *.h)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

最后

```Shell
cmake -B build && cmake --build build && ./build/target1
```

### 源码在子文件夹中

这里我们先组织一下文件结构目录。将源文件和头文件统一管理。

```Shell
mkdir source include
mv *.cpp ./source
mv *.h ./include
```

整个文件结构为

```plain
.
├── CMakeLists.txt
├── MyProject.h.in
├── build
├── include
└── source
```

头文件在`./include`中，源文件在`source`中。
首先修改一下版本号配置文件的生成

```cmake
configure_file("${PROJECT_SOURCE_DIR}/MyProject.h.in"
               "${PROJECT_SOURCE_DIR}/include/MyProject.h")
```

当然我们可以修改`file`命令，让其搜索子目录。然后一个更好的方法是，不使用`file`命令，使用`aux_source_directory`搜寻源文件，使用`include_directories`命令搜寻头文件。

最后的`CMakeLists.txt`

```cmake
# CMake最低版本要求 VERSION >= 3.25.0
cmake_minimum_required(VERSION 3.25.0)

# 设定项目名 以及版本号 Semantic Versioning
# 语义化版本 参考https://semver.org/
project(MyProject VERSION 1.0.0)

configure_file("${PROJECT_SOURCE_DIR}/MyProject.h.in"
               "${PROJECT_SOURCE_DIR}/include/MyProject.h")

# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)

# 添加提供查找头文件的目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 搜索./src下的所有的源文件并赋值给变量sources
aux_source_directory(./src sources)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

重新配置构建并运行查看结果

```Shell
cmake -B build && cmake --build build && ./build/target1
```

Hello World!
100的算数平方根为10
