---
title: CMake教程（四）：添加生成器表达式
tags:
  - CMake
  - CPP
categories:
  - My Tutorial of CPP
date: 2023-02-13 18:09:31
---



> [CMake Help](https://cmake.org/cmake/help/latest/)
> 系列基于[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
> 本文基于[Step 4: Adding Generator Expressions](https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20Generator%20Expressions.html)

生成器表达式的值在构建（build）阶段才能得到。生成器表达式可用于根据某些条件设置某些变量和编译选项。
Generator expressions may be used to enable conditional linking, conditional definitions used when compiling, conditional include directories and more. The conditions may be based on the build configuration, target properties, platform information or any other queryable information.

<!-- more -->

## Exercise 1 - Setting the C++ Standard with Interface Libraries

Before we use generator expressions let's refactor our existing code to use an INTERFACE library. We will use that library in the next step to demonstrate a common use for generator expressions.

Add an INTERFACE library target to specify the required C++ standard.

为我们的目标已经库明确`C++`标准。

修改`CMakelists.txt，创建库并添加编译器特性

```cmake
# 创建一个interface库:tutorial_compiler_flags
add_library(tutorial_compiler_flags INTERFACE)
# 添加编译器特性C++17
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_17)
```

为目标添加编译器特性

```cmake
target_link_libraries(target1 PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
```

为库`MathFunctions`添加编译器特性

```cmake
# link our MathFunctions library to our new tutorial_compiler_flags library
target_link_libraries(MathFunctions tutorial_compiler_flags)
```

With this, all of our code still requires C++ 17 to build. Notice though that with this method, it gives us the ability to be specific about which targets get specific requirements. In addition, we create a single source of truth in our interface library.

## Exercise 2 - Adding Compiler Warning Flags with Generator Expressions

Add compiler warning flags when building but not for installed versions.

修改`CMakeLists.txt`

we determine which compiler our system is currently using to build since warning flags vary based on the compiler we use.

```cmake
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
```

we can use another generator expression to apply the respective flags only when the variables are true. We use `target_compile_options()` to apply these flags to our interface library.

```cmake
target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
  "$<${msvc_cxx}:-W3>"
)
```

Lastly, we only want these warning flags to be used during builds. Consumers of our installed project should not inherit our warning flags. To specify this, we wrap our flags in a generator expression using the BUILD_INTERFACE condition. The resulting full code looks like the following:

```cmake
target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
```
