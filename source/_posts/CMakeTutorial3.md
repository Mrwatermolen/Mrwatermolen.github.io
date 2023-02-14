---
title: CMake教程（三）：添加库使用需求
tags:
  - CMake
  - CPP
categories:
  - Tutorial of CMake
date: 2023-02-13 14:55:13
---


> [CMake Help](https://cmake.org/cmake/help/latest/)
> 系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
> 本文基于[Step 3: Adding Usage Requirements for a Library](https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20Usage%20Requirements%20for%20a%20Library.html)

<!-- more -->

## Adding Usage Requirements for a Library

### 使用INTERFACE

修改`library/CMakeLists.txt`

```cmake
# 为静态库MathFunctions库添加文件mySqrt.cpp
add_library(MathFunctions STATIC mysqrt.cpp)

# We want to state that anybody linking to MathFunctions needs to include the current source directory, 
# while MathFunctions itself doesn't.
# This can be expressed with an INTERFACE usage requirement. 
# Remember INTERFACE means things that consumers require but the producer doesn't.
# 注意这里用到的方式是INTERFACE，它的意思是，用到这个项目（库MathFunctions）的其他项目才需要执行这个命令，而这个项目本身不需要执行
# 我们将为MathFunctions库添加一条限制，任何调用了这个库的项目都必须把这个库给引用上。
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )
```

修改`CMakeLists.txt`，Now that we've specified usage requirements for MathFunctions we can safely remove our uses of the `EXTRA_INCLUDES` variable from the top-level CMakeLists.txt。

```cmake
if(USE_MYMATH)
  # 添加子文件夹./library
  add_subdirectory(libs/mathFunctions)
  # 把库MathFunctions添加入EXTRA_LIBS列表中去
  list(APPEND EXTRA_LIBS MathFunctions)
endif()
```

以及

```cmake
target_include_directories(target1 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )
```

这里我们无需额外告知库的头文件的位置了，我们把关于`EXTRA_INCLUDES`都删除了。

Notice that with this technique, the only thing our executable target does to use our library is call target_link_libraries() with the name of the library target. In larger projects, the classic method of specifying library dependencies manually becomes very complicated very quickly.
