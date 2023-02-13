---
title: Qt6绑定QMYSQL驱动
categories:
  - Qt
date: 2023-02-09 13:45:01
tags:
---


Qt官方文档
[How to Build the QMYSQL Plugin on Unix and macOS](https://doc.qt.io/qt-6/sql-driver.html#how-to-build-the-qmysql-plugin-on-unix-and-macos)

stackoverflow上的一个参考
[Qt C++ QMYSQL driver not loaded on MacOS - Tutorial for building QMYSQL Plugin](https://stackoverflow.com/questions/60834609/qt-c-qmysql-driver-not-loaded-on-macos-tutorial-for-building-qmysql-plugin)

Github上的一个脚本
[Build and Install MySQL Plugin for QT on MacOS](https://gist.github.com/wingsofovnia/40db428ad1984dda13580cb7a88e877e)

但是有个问题，我是通过`brew`安装Qt的。解决办法也很简单。无需源码编译。

```shell
brew install qt-mysql
```

[How to install MySQL on macOS](https://flaviocopes.com/mysql-how-to-install/)

<!-- more -->

## 一个note

> In #9117, we switched to a new prefix of /opt/homebrew for installations on Apple Silicon. This was written and shipped with heroic speed to help prevent strange issues with bleeding edge users on the first consumer Apple Silicon Macs.

使用命令

```shell
qmake -query QT_INSTALL_PREFIX
```

查询到的Qt安装路径为`/opt/homebrew`

针对`<qt_installation_path>/Src/qtbase/src/plugins/sqldrivers`

```shell
ls $(qmake -query QT_INSTALL_PREFIX)
```

可以观察到没有`Src`这个文件夹。

实际的驱动库的目录在这`/opt/homebrew/Cellar/qt/6.4.2_1/share/qt/plugins/sqldrivers`
