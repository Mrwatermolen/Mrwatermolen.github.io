---
title: vim的安装配置
author: MH
output: pdf_document
tags:
  - Vim
categories:
  - Linux
date: 2020-11-17 00:00:00
---

## VIM的源码安装

*本文主要引用[YouCompleteMe：Building Vim from source](https://github.com/ycm-core/YouCompleteMe/wiki/Building-Vim-from-source)*

> OS：Raspbian 10（类debian） Kernel: armv7l

### 下载VIM　卸载系统自带VIM

```bash
sudo apt install libncurses5-dev libgtk2.0-dev libatk1.0-dev \
libcairo2-dev libx11-dev libxpm-dev libxt-dev python2-dev \
python3-dev ruby-dev lua5.2 liblua5.2-dev libperl-dev git
# 安装依赖

git clone https://github.com/vim/vim.git
sudo apt remove vim vim-runtime gvim

# 一般来说
cd vim
sudo make install
```

### 特殊需求

但如果有特殊要求的，在 `cd vim` 后还需要手动配置一些参数
比如之后要安装补全插件**YouCompleteMe**

``` shell
cd vim
./configure --with-features=huge \
            --enable-multibyte \
            --enable-rubyinterp=yes \
            --enable-python3interp=yes \
            --with-python3-config-dir=$(python3-config --configdir) \
            --enable-perlinterp=yes \
            --enable-luainterp=yes \
            --enable-gui=gtk2 \
            --enable-cscope \
            --prefix=/usr/local
sudo make && make install
```

注意 `$(python3-config --configdir)` 要自己去找到路径
我的路径为 `/usr/lib/python3.7/config-3.7m-aarch64-linux-gnu` 可供参考
对Ubuntu系统需要注意的是

> Note for Ubuntu users: You can only use Python 2 or Python 3. If you try to compile vim with both python-config-dir and python3-config-dir, YouCompleteMe will give you an error YouCompleteMe unavailable: requires Vim compiled with Python (2.6+ or 3.3+) support, when you start VIM.

大意就是只能使用２或者３中的一个。所以上面的配置中我只开启了python3

最后
 `vim --version`

检查是否安装成功
