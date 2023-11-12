---
title: 'Github SSH kex_exchange_identification: Connection closed by remote host'
tags:
  - openssl
  - ssh
  - github
categories:
  - Git
date: 2023-02-15 23:29:58
---


没有解决，思路全错。可能就是网络问题。

## 错误描述

使用`git`通过`SSH`克隆远程仓库时，出现错误

> ssh_dispatch_run_fatal: Connection to 20.205.243.166 port 22: Operation timed out
fatal: Could not read from remote repository.
>
> Please make sure you have the correct access rights
and the repository exists.

我记得我是有私钥的，做个测试。

```shell
ssh ssh -T git@github.com
```

报错
> kex_exchange_identification: Connection closed by remote host
Connection closed by 20.205.243.166 port 22

<!-- more -->

## 可能原因

搜了一下，出现**kex_exchange_identification: Connection closed by remote host**这个问题好像没啥通用的原因。

有个人提到

>In OpenSSH source code, kex_exchange_identification is a function to exchange server and client identification (duh), and the specified error happened if the socket connection between OpenSSH server and client is interrupted (see EPIPE), i.e. client already closed its connection
来源<https://serverfault.com/questions/1015547/what-causes-ssh-error-kex-exchange-identification-connection-closed-by-remote>

我突然记起来前段时间使用`brew install openssl`安装了一个`openssl`。于是系统里有了两个`openssl`。

```shell
which -a openssl
/opt/homebrew/bin/openssl
/usr/bin/openssl
```

所以可能是这个新的`openssl`不认原来的。（猜测

先Debug一下。

```shell
ssh -vT git@github.com
```

输出如下：

```plain
OpenSSH_9.0p1, LibreSSL 3.3.6
debug1: Reading configuration data /User/you/.ssh/config
debug1: Reading configuration data /etc/ssh/ssh_config
debug1: /etc/ssh/ssh_config line 21: include /etc/ssh/ssh_config.d/* matched no files
debug1: /etc/ssh/ssh_config line 54: Applying options for *
debug1: Authenticator provider $SSH_SK_PROVIDER did not resolve; disabling
debug1: Connecting to github.com port 22.
debug1: Connection established.
...
Hi xx! You've successfully authenticated, but GitHub does not provide shell access.
debug1: channel 0: free: client-session, nchannels 1
Transferred: sent 3156, received 2704 bytes, in 0.9 seconds
Bytes per second: sent 3620.5, received 3101.9
debug1: Exit status 1
```

居然又可以了。。。 好了，可以否决这个想法了。

> Operation timed out

可能就是网络问题吧。

## 解决方法：没有。好像是网络问题

网上有人说可以重新生成key解决，没试。
