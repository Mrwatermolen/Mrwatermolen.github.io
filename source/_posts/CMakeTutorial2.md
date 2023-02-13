---
title: CMake教程（二）：创建一个简单的静态库
date: 2023-01-28 13:44:59
categories:
  - My Tutorial of CPP
tags:
  - CMake
  - CPP
---

> [CMake Help](https://cmake.org/cmake/help/latest/)
> 系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
> 本文基于[Step 2: Adding a Library](https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20a%20Library.html)
>Rather than placing all of the source files in one directory, we can organize our project with one or more subdirectories. In this case, we will create a subdirectory specifically for our library. Here, we can add a new CMakeLists.txt file and one or more source files. In the top level CMakeLists.txt file, we will use the add_subdirectory() command to add the subdirectory to the build.
>Once the library is created, it is connected to our executable target with target_include_directories() and target_link_libraries().

<!-- more -->

## 创建生成一个静态库

考虑，我们需要一个数学库的开方运算。首先创建一个文件夹就叫做`library`吧，其中包含文件`mathfuctions.h`和`mysqrt.cpp`以及一个`CMakeLists.txt`。

```shell
mkdir library
touch library/mathfunctions.h
touch library/mysqrt.cpp
touch library/CMakeLists.txt
```

在`mathfunctions.h`中写入

```cpp
#ifndef _MATHFUNCTIONS_H_
#define _MATHFUNCTIONS_H_

namespace mymath {

/**
 * @brief 一个简单的根号求解
 *
 * @param x 非负整数
 * @return int x的算数平方根 舍去小数部分
 */
int mySqrt(int x);

} // namespace math

#endif // _MATHFUNCTIONS_H_
```

在`mysqrt.cpp`实现这个方法

```cpp
#include "mathfunctions.h"

int mymath::mySqrt(int x) {
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

编辑`library/CMakeList.txt`文件。

我们要告知`CMake`创建一个库目标，向其添加文件。

```cmake
# 为静态库MathFunctions库添加文件mysqrt.cpp
add_library(MathFunctions STATIC mysqrt.cpp)
```

这里我们定义了一个库`MathFunctions`，向其添加`mysqrt.cpp`文件。

## 在程序中链接静态库

我们返回`MyProject`项目的目录。修改`CMakeList.txt`文件。

为了使用我们自定义的`MathFunctions`库，我们需要告知`CMake`这个库的位置在哪。
库`MathFunctions`定义在`./library/CMakeLists.txt`中。

```cmake
# 添加子文件夹./library
add_subdirectory(library)
```

而后我们在告知`CMake`在生成可执行文件`target1`前要将库`MathFunctions`链接进来。

```cmake
# 为target1链接库MathFunctions
target_link_libraries(target1 PUBLIC MathFunctions)
```

最后我们需要告知库`MathFunctions`的头文件在哪。

```cmake
# 构建生成target1需要的目录文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/library"
                          )
```

最后的`CMakelists.txt`文件为

```cmake
# CMake最低版本要求 VERSION >= 3.25.0
cmake_minimum_required(VERSION 3.25.0)

# 设定项目名 以及版本号 Semantic Versioning
# 语义化版本 参考https://semver.org/
project(MyProject VERSION 1.0.0)

# 生成版本号头文件
configure_file("${PROJECT_SOURCE_DIR}/MyProject.h.in"
               "${PROJECT_SOURCE_DIR}/include/MyProject.h")

# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)
# 生成一个compile_commands.json文件，和基于clang的工具一起使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加子文件夹./library
add_subdirectory(library)

# 添加提供查找头文件的目录
# 当前CMakeList.txt中的所有目标以及所有在其调用点之后添加的子目录中的所有目标将具有此头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 搜索项目目录/source下的所有的源文件并赋值给变量sources
aux_source_directory(${PROJECT_SOURCE_DIR}/source sources)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# 为target1链接库MathFunctions
target_link_libraries(target1 PUBLIC MathFunctions)

# 构建生成target1需要的目录文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/library"
                          )

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

修改`./source/main.cpp`文件，调用自定义库中的库函数。

```cpp
#include "hello.h"
#include "mathfunctions.h"

#include <iostream>

int main() {
  printHelloWorld();
  std::cout << "100的算数平方根为" << mymath::mySqrt(100) << "\n";
}
```

执行命令，构建`target1`并运行。

```Shell
cmake -B build && cmake --build build && ./build/target1
```

Hello World!
100的算数平方根为10

## Exercise 2 - Making Our Library Optional

> Now let us make the MathFunctions library optional. While for the tutorial there really isn't any need to do so, for larger projects this is a common occurrence.
>
> CMake can do this using the `option()` command. This gives users a variable which they can change when configuring their cmake build. This setting will be stored in the cache so that the user does not need to set the value each time they run CMake on a build directory.

我们为自定义的库创建一个宏定义`USE_MYMATH`去控制是否导入这个库。

修改`CMakeLists.txt`文件，添加一行

```cmake
# 添加一个变量选项 用户能选择是否开启或关闭 ON为true OFF为false
option(USE_MYMATH "Use tutorial provided math implementation" ON)

if(USE_MYMATH)
  # 添加子文件夹./library
  add_subdirectory(library)
  # 把库MathFunctions添加入EXTRA_LIBS列表中去
  list(APPEND EXTRA_LIBS MathFunctions)
endif()
```

修改

```cmake
# 为target1链接库
target_link_libraries(target1 PUBLIC ${EXTRA_LIBS})

# 构建生成target1需要的目录文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          ${EXTRA_INCLUDES}
                          )
```

最后的`CMakeLists.txt`文件为

```cmake
# CMake最低版本要求 VERSION >= 3.25.0
cmake_minimum_required(VERSION 3.25.0)

# 设定项目名 以及版本号 Semantic Versioning
# 语义化版本 参考https://semver.org/
project(MyProject VERSION 1.0.0)

# 生成版本号头文件
configure_file("MyProject.h.in"
               "MyProject.h")

# 设置c++标准为C++17标准
set(CMAKE_CXX_STANDARD 17)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED on)
# 生成一个compile_commands.json文件，和基于clang的工具一起使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加一个变量选项 用户能选择是否开启或关闭 ON为true OFF为false
option(USE_MYMATH "Use tutorial provided math implementation" ON)

if(USE_MYMATH)
  # 添加子文件夹./library
  add_subdirectory(library)
  # 把库MathFunctions添加入EXTRA_LIBS列表中去
  list(APPEND EXTRA_LIBS MathFunctions)
  list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/library")
endif()

message("${EXTRA_LIBS}")
# 添加提供查找头文件的目录
# 当前CMakeList.txt中的所有目标以及所有在其调用点之后添加的子目录中的所有目标将具有此头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 搜索项目目录/source下的所有的源文件并赋值给变量sources
aux_source_directory(${PROJECT_SOURCE_DIR}/source sources)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# 为target1链接库
target_link_libraries(target1 PUBLIC ${EXTRA_LIBS})

# 构建生成target1需要的目录文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          ${EXTRA_INCLUDES}
                          )

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

将`USE_MYMATH`加入配置文件`MyProject.h.in`中

```cpp
#define MyProject_VERSION_MAJOR @MyProject_VERSION_MAJOR@
#define MyProject_VERSION_MINOR @MyProject_VERSION_MINOR@
#define MyProject_VERSION_PATCH @MyProject_VERSION_PATCH@
#cmakedefine USE_MYMATH
```

在项目中使用`USE_MYMATH`控制，修改`source/main.cpp`

```cpp
#include "MyProject.h"
#include "hello.h"

#ifdef USE_MYMATH
#include "mathfunctions.h"
#endif

#include <cmath>
#include <iostream>

int main() {
  printHelloWorld();
#ifdef USE_MYMATH
  std::cout << "100的算数平方根为" << mymath::mySqrt(100) << "\n";
#else
  std::cout << "使用标准库函数。100的算数平方根为" << sqrt(100) << "\n";
#endif
}
```
