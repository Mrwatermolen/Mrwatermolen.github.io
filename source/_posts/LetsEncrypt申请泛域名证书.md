---
title: Let's Encrypt申请泛域名证书
tags:
  - pt
  - HTTPS
  - Cloudflare
categories:
  - nas
date: 2022-01-25 08:58:29
---

`Let‘s Encrypt`可以免费申请到能得到浏览器认证的TLS证书，当然申请的证书只有90天有效期，不过可以免费续期。这里采用官方推荐使用`certbot` 脚本申请证书。由于家用宽带禁封了80端口，会导致在域名认证这一步上`Let‘s Encrypt`无法判断你对域名拥有管理权，导致申请失败。所以在申请时，要么找一台80端口没被封的服务器，或者使用手工添加配置 DNS 记录。不过手动去DNS解析服务商配置 DNS 记录通过验证，`certbot` 也提供了自动脚本，详情见[User Guide — Certbot 1.22.0 documentation (eff-certbot.readthedocs.io)](https://eff-certbot.readthedocs.io/en/stable/using.html#manual)，我使用的是Cloudflare。

<!-- more -->

## 安装

```
sudo apt install certbot
sudo apt install python3-certbot-dns-cloudflare
```

## 获取API key

查阅[Welcome to certbot-dns-cloudflare’s documentation! — certbot-dns-cloudflare 0 documentation](https://certbot-dns-cloudflare.readthedocs.io/en/stable/)

```
nano ~/cloudflare.ini
```

将信息填入后修改文件权限。

```
chmod 600 ~/cloudflare.ini
```

## 申请泛域名证书

假定域名为`example.com`

```
sudo certbot certonly --dns-cloudflare --dns-cloudflare-credentials ~/cloudflare.ini --dns-cloudflare-propagation-seconds 60 --preferred-challenges dns -d *.example.com
```

## 续期

```
cerbot renew
```

