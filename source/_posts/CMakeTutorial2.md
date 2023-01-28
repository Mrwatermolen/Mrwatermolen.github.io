---
title: CMake教程（二）：创建一个简单的静态库
date: 2023-01-28 13:44:59
tags:
---

> [CMake Help](https://cmake.org/cmake/help/latest/)
> 系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
> 本文基于[Step 2: Adding a Library](https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20a%20Library.html)

<!-- more -->

## 组织文件结构

第一节结束后我们的项目结构如下所示：

```plain
.
├── CMakeLists.txt
├── MyProject.h.in
├── build
├── include
└── source
```

构建可执行文件除了`main.cpp`以外还要依赖`myFunctions.h`中声明的`mySqrt()`方法。这里我们可以把这个依赖看成一个静态库。

```Shell
mkdir MyFunctions
mv ./source/mySqrt.cpp ./MyFunctions           
mv ./include/myFunctions.h ./MyFunctions
cd MyFunctions
touch CMakeLists.txt
```

这里注意到我们在这个子文件夹`MyFunctions`中也创建了一个`CMakeLists.txt`文件。

此时开始构建会出错。

## 创建自定义静态库

编辑`MyFunctions/CMakeList.txt`文件。

我们进入到`MyFunctions/`目录中修改里面的`CMakeList.txt`文件。
我们要告知`CMake`创建一个库目标，向其添加文件。

```cmake
# 为静态库MathFunctions库添加文件mySqrt.cpp
add_library(MathFunctions STATIC mySqrt.cpp)
```

这里我们定义了一个库`MathFunctions`，其包含`mySqrt.cpp`文件。
保存退出即可。

## 在程序中链接静态库

完善`CMakeList.txt`

我们返回`MyProject`项目的目录。修改`CMakeList.txt`文件。

为了使用我们自定义的`MathFunctions`库，我们需要告知`CMake`这个库的位置在哪。
库`MathFunctions`定义在`./MyFunctions/CMakeLists.txt`中。

```cmake
# 添加子文件夹./MyFunctions
add_subdirectory(MyFunctions)
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
                          "${PROJECT_SOURCE_DIR}/MyFunctions"
                          )
```

这个`${PROJECT_BINARY_DIR}`我不明白。

最后的`CMakelists.txt`文件为

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

# 添加子文件夹./MyFunctions
add_subdirectory(MyFunctions)

# 添加提供查找头文件的目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 搜索./src下的所有的源文件并赋值给变量sources
aux_source_directory(./source sources)

# 为MyProject添加一个可执行目标target1
add_executable(target1)

# 为target1链接库MathFunctions
target_link_libraries(target1 PUBLIC MathFunctions)

# 构建生成target1需要的目录文件
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/MyFunctions"
                          )

# Specifies sources to use when building a target and/or its dependents.
# 添加变量sources中的源文件给target1
target_sources(target1 PUBLIC ${sources})
```

执行命令，构建`target1`并运行。

```Shell
cmake -B build && cmake --build build && ./build/target1
```

Hello World!
100的算数平方根为10

## Exercise 2 - Making Our Library Optional

略。我没到那个阶段。
