---
title: 树莓派vscode无法记住登录账户的问题
tags:
  - vsocode
categories:
  - Linux
date: 2020-12-16 22:24:12
---


## 问题解决

* 查看输出
`The name org.freedesktop.secrets was not provided by any .service files`

* 解决方案
按[这个网址](https://github.com/microsoft/vscode/issues/92972#issuecomment-625751232)所说
`sudo apt install gnome-keyring`
重启vscode 再次登录账户后会要求创建一个密码环 可以直接留空
