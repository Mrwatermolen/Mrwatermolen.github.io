---
title: CMake教程（五）：安装和测试
tags:
  - CMake
  - CPP
categories:
  - Tutorial of CMake
date: 2023-02-13 18:09:33
---


> [CMake Help](https://cmake.org/cmake/help/latest/)
> 系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
> 本文基于[Step 5: Installing and Testing](https://cmake.org/cmake/help/latest/guide/tutorial/Installing%20and%20Testing.html)

<!-- more -->

## Exercise 1 - Install Rules

修改`MathFunctions/CMakeLists.txt`，添加

```cmake
set(installable_libs MathFunctions tutorial_compiler_flags)
# For MathFunctions, we want to install the libraries and header file to the lib and include directories respectively.
install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES mathfunctions.h DESTINATION include)
```

修改`CMakeLists.txt`，添加

```cmake
install(FILES "${PROJECT_BINARY_DIR}/MyProject.h"
              "include/hello.h"
              DESTINATION include
)
```

运行命令

```shell
cmake -B build
cmake --build build
--config Release
```

执行安装

```shell
cmake --install build
```

默认情况下，安装路径为`/usr/local`，window系统为`C:/Program Files/${PROJECT_NAME}`。可以设定安装路径：

```shell
cmake --install build --prefix "/home/myuser/installdir" 
```

安装成功后，执行命令

```shell
cd /home/myuser/installdir
tree .
```

可以得到如下文件目录结构

```tree
.
├── bin
│   └── target1
├── include
│   ├── MyProject.h
│   ├── hello.h
│   └── mathfunctions.h
└── lib
    └── libMathFunctions.a
```

可执行文件在`bin`目录中。

## Exercise 2 - Testing Support
