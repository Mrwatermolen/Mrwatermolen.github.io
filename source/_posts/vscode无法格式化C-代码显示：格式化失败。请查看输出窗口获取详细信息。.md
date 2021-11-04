---
title: vscode无法格式化C++代码显示：格式化失败
tags:
  - vscode
  - raspberry
  - c/c++
categories:
  - Linux
date: 2020-11-26 21:23:04
---

在输出中，终端窗口的Task下拉列表切换成C/C++
>/home/pi/.vscode/extensions/ms-vscode.cpptools-1.1.2/bin/../LLVM/bin/clang-format: error while loading shared libraries: libtinfo.so.5: cannot open shared object file: No such file or directory

## 解决方法

把libtinfo.so.5这包装上就好老
`sudo apt-get install libncurses5`
~~百度来的方法`sudo ln -s /usr/lib/libtinfo.so.6 /usr/lib/libtinfo.so.5`~~
