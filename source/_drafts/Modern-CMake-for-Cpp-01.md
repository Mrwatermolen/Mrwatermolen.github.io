---
title: Modern CMake for Cpp 01
tags:
---

> Technical requirements

```bash
cmake -B <build tree> -S <source tree>
cmake --build <build tree>
```

build tree is the path to target/output directory, source tree is the path at which your source code is located.
For convenience

```bash
cmake -B build
cmake --build build
```

## Section 1 Introducing CMake

Navigation

* Understanding the basics
* Install CMake (Pass)
* Mastering the command line
* Navigating the project file
* Discovering scripts and modules

### Understand Basic

How does CMake work?

This process has three stages: Configuration Generation Building

**Configuration**: This stage is about reading project details stored in a directory, called the source tree, and preparing an output directory or build tree for the generation stage.

**Generation**: After reading the project configuration, CMake will generate a ***buildsystem*** for the exact environment it is working in.  During this stage, CMake can still apply some final touches to the build configuration by evaluating generator expressions. (The generation stage is executed automatically after the configuration stage.)

**Building**: In turn, these build tools will execute steps to produce targets with compilers, linkers, static and dynamic analysis tools, test frameworks, reporting tools, and anything else you can think of.

### The Command Line

```bash
cmake -S ./project -B ./build
cmake -G <generator-name> <path-to-source>
cmake -D <var>[:<type>]=<value> <path-to-source>
cmake --build <dir> [<options>] [-- <build-tool-options>]
```

Pass

### Navigating the Project File

Project source tree

Build tree

Listfiles:

Files that contain the CMake language are called listfiles and can be included one in another, by calling include() and find_package(), or indirectly with add_ subdirectory()

CMakeLists.txt:

CMakeCache.txt:

The Config-files for packages:

A big part of the CMake ecosystem includes the external packages that projects can depend on. They allow developers to use libraries and tools in a seamless, cross-platform way. Packages that support CMake should provide a configuration file so that CMake understands how to use them.

Pass

### Discovering scripts and modules

#### Scripts

To configure project building, CMake offers a platform-agnostic programming language. This comes with many useful commands. You can use this tool to write scripts that come with your project or are completely independent.
