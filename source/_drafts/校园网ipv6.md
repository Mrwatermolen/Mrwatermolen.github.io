# ipv6

> 问题描述：路由器支持ipv6，使用PPPoE拨号，ipv6类型采用Native，DHCP-PD打开。如此设置后，路由器的wan口拿到了2001开头的/64的ipv6地址，ssh连接路由器也可以ping通ipv6。但是路由器却无法往下分ipv6地址，路由器下面的设备都只拿到了fe80开头的本地ipv6地址，而且进行test-ipv6全部失败，未侦测到 IPv6 位址。

## 前言

找了很多教程，就把一下讲了一些原理的教程放出来，做个笔记吧。（虽然都没解决，但是大致清楚了问题所在

### 教程一

**本文目的**：完成IPV6设置，让局域内设备获取全球唯一的原生ipv6地址，解决路由LAN口无IPV6地址的问题。

在写老毛子固件IPV6设置之前，我们需要首先了解几个概念：

**1、pppoe-nd-01与pppoe-pd-01模式** 这点非常重要！
根据上海联通技术人员解释，在PPPOE拨号时，会有这两种模式，前者为交换机模式，后者为路由模式。
两者的区别在于，路由器拨号时如果是pppoe-nd-01模式，只能在WAN口分到一个/64的IPV6地址，无法再往LAN口分原生的IPV6地址，局域网内设备均无法获取到原生的公网IPV6地址；而pppoe-pd-01模式会获得一个所谓的PD前缀，即60位掩码的地址 2408:8025:248::/45 delegating-prefix-length 60，这种模式下路由器WAN口会有1个/64的IPV6地址，同时在路由器LAN口会分到1个/60的地址，有了PD前缀，路由器就能往局域网分原生的IPV6地址了，局域网的设备也就能获得原生的全球唯一的IPV6地址了。
**但是具体是哪种模式，与用户无关，只取决于运营商的服务器配置策略，也就是说有没有PD前缀取决于运营商的配置策略**

**2、IPV6地址生成方式stateless（无状态）模式和stateful（有状态）模式的区别**
生成“全球单播地址”（或者“唯一本地地址”），有2种方式
手动配置自动配置
其中“自动配置”根据获取方式，又分为
无状态（Stateless）：根据路由通告报文RA（Router Advertisement）包含的prefix前缀信息自动配置IPv6地址，组成方式是Prefix + (EUI64 or 随机)。Stateless也可以称为SLAAC（Stateless address autoconfiguration）有状态（Stateful）：通过DHCPv6方式获得IPv6地址
其中“有状态”又分为2种
有状态DHCPv6（Stateful DHCPv6）：IPv6地址、其他参数（如DNS）均通过DHCPv6获取无状态DHCPv6（Stateless DHCPv6）：IPv6地址依然通过路由通告RA方式生成，其他参数（如DNS）通过DHCPv6获取
为了避免混淆，在此解释下有状态、无状态到底是什么意思：首先，请明确一点，有状态、无状态仅针对于IPv6地址分配方式，并不包含其他参数
&#9655; 有状态：可控、可管理。在网络中存在一个IP地址管理者，它能够识别客户端，根据不同的客户端，分配对应的IPv6地址，客户端与服务端之间需要维护IP地址的租期及续约。目前实现这种效果的，就是DHCPv6协议，IP地址管理者就是DHCPv6 Server
&#9655; 无状态：不可控、难管理。在网络中只有网关，没有IP地址管理者。因此无人去识别客户端，每个客户端根据网关发送的相同的RA报文内容，自行配置IPv6地址

**想获取原生IPV6地址其实这两种方式都可以，但是我们一般默认选择无状态（Stateless）模式，局域网内有特定管理需要可以选择有状态（Stateful）模式**

**3、是否启用napt66插件的问题**
NAPT66是北邮学生开发的在较旧内核的Linux上实现ipv6 nat的内核模块，项目地址：https://github.com/mzweilin/napt66 。
什么是NAT？俗称地址转换，在IPV4的世界里，NAT其实一直在我们身边，当我们路由拨号获取一个公网IP后，会通过NAT技术与局域网内类似192.168.1.*等保留地址进行转换，因为IPV4地址有限！但是IPV6地址不一样了，它是取之不尽用之不竭的，在原生IPV6的世界里面，所有的局域网设备均可以有1个自己的全球唯一的IPV6地址。
那为什么还会有napt66插件呢？这是针对ipv6网络布置不完善、运营商只给分1个IPV6地址或者运营商不下发PD前缀时的一个妥协性的办法！当你没分到PD前缀、只有WAN口有ipv6地址时，就需要这个插件插件做ipv6 nat了，局域网内的设备获取类似fc开头的私有地址，这种模式其实不是原生的ipv6使用方法，有需要可以后面重新写个文章给出教程。
所以，**想获取全球唯一的IPV6地址，napt66插件是不需要启用、不需要启用、不需要启用，说三遍！**

好了，说完这几个基本的概念，让我们来开始老毛子的ipv6的设置：
**硬件**：小娱C5 MT7621A
**固件**：chongshengB XY-C1_3.4.3.9-099 2020.05.07老毛子PADAVAN固件

**1、外网WAN-IPV6协议**
IPv6 连接类型设置为”Native DHCPV6“，WAN IPv6 设置为“Stateless RA”无状态模式，DNSv6 外网设置设置为自动获取，
IPv6 内网设置为自动获取、模式为“Stateless RA”无状态模式（如有管理需要可设置为Stateful有状态模式）

**2、NAPT66不启用、不启用、不启用！**

**3、路由重新拨号**
正常的IPV6网络这是时候，你的路由器的WAN口和LAN口都会分到IPV6地址，也会分到PD前缀地址，如图：

什么？**只有WAN口有IPV6地址,LAN口没IPV6地址？？？**是的，我在刚开始配置ipv6的时候也出现了这样的问题，这个问题的出现主要是运营商服务端IPV6配置策略的问题，没有给用户下发PD前缀，解决方案有2种：
**（1）找运营商！目前IPV6在全国大部分区域已经铺开，相关的技术已经比较成熟，出现这种问题很可能是运营商机房设置策略不对，没有默认给用户下发PD前缀，也就是没有给用户默认启用pppoe-pd-01模式，那就找他们解决，一定要找专家组，普通维修师傅对ipv6接触不多可能解决不了，我这边是连联通自己的专家都解决不了，找的厂家解决的。
（2）如果运营商明确表示他们不给用户发PD前缀，那么就启用NAPT66插件吧，同时要设置路由LAN口为私有地址，同时设置局域网内的DHCP策略，这个教程网上应该还有很多，这里不展开了。**

4、当解决了PD前缀问题后，那么局域网内的设备应该都能获取到联通2408开头的全球唯一IPV6地址了，那这个时候怎么解决从外网直接访问局域网设备的问题呢？我们主观上认为在外网直接输入具备局域网内设备IPV6地址就可以了，但其实这里有个问题，虽然原生IPV6不需要做NAT了，但其实外网到局域网中间还有个路由器，路由器有防火墙；一般默认情况下，路由器收到从外网访问局域网设备IPV6地址时是不做响应的，也不会往内部设备转发流量，所以这里还需要配置一个流量转发策略即防火墙策略；由于IPV6目前相关的路由终端等图形配置界面还不完善，我们不能像ipv4端口转发一样很方面进行图形化配置；当然IPV6防火墙是可以按端口、设备或者IP地址进行按需分配的，但是命令行对于一般用户来说还是过于复杂，所以我们暂时在这里设置IPV6防火墙暂时放行和转发所有的流量：
方法：在参数设置-自定义脚本-在防火墙规则启动后执行,输入：

```
1. ip6tables -F
2. ip6tables -X
3. ip6tables -P INPUT ACCEPT
4. ip6tables -P OUTPUT ACCEPT
5. ip6tables -P FORWARD ACCEPT
```

重启路由！至此，你们大概已经可以正常访问iPV6网站了，也可以从外网直接访问局域网IPV6地址了，当然前提你的终端也已经介入了IPV6网络。

**写在最后：**
虽然全国各地运营商IPV6的硬件已经配置完毕，但是在用户使用方面其实还没有那么大的急迫性，所以针对IPV6的开发、IPV6的教程甚至是运营商的网络维修人员经验还不够完善。当用户碰到问题时，去网上查到更多的是一些各式各样零星的解决方案，不一定能很方便地查到自己针对性解决方案，这跟用户还没有大范围使用IPV6也有很到的关系，相信在不远的将来，IPV6从用户角度会彻底推广开来吧！
本文希望能帮到你！

**原文地址**：http://blog.qint.top/archives/26/ 《老毛子PADAVAN固件IPV6设置&上海联通获取PD前缀让局域网设备获取全球唯一IPV6地址》

### 教程二

1、NATIVE
这个是需要运营商（ISP）支持的，目前天津联通还不支持，这种方式允许路由器下级的客户端与拨号服务器直接通信并获取原生IPv6地址，如果在支持的ISP下，选择路由器上的NATIVE就可以了，NATIVE下可以配置无状态配置IPv6或者静态IPv6或者DHCP分配IPv6。

2、黑科技PASSTHROUGH/RELAY
运营商不支持NATIVE，但是我又想得到全球可达的外网IPv6地址怎么办，这就需要黑科技Passthrough了。在这个模式下，路由器成为你的下一跳，与NAT的区别是，路由器只会转发数据包，并不会修改源地址，而且因为IPv6的relay协议（多了很多设备间通信协商的功能）和在这种分配模式下，前四部份确定的整个IPv6网段就是你的了，所以你的IPv6地址就是全球可达的。

在这种模式下，你可以使用客户端无状态自动生成的IPv6地址，也可以停用本机接受RA广播并使用自己配置的地址，比如拨号获取了2001:db8:3c4d:1:的前缀，你可以把设备配置成2001:db8:3c4d:1::2并且可以通信。但是一定要关闭RA广播防止设备通过DHCP来获取地址，与IPv4不同，在IPv6协议中，网卡可以同时有多个IPv6地址，并且DHCP获取地址的优先级高于静态地址。路由器配置也着重讲这个方法。

这种方法在原生IPv6（Native IPv6）+自动分配IPv6地址的网络环境下（很多高校的校园网都具备），可以使局域网中的每个节点都申请到一个IPv6地址。也就是说，路由器只对IPv4地址起NAT作用，对IPv6地址来说只相当于交换机。

3、NAT66
这种方式就是像在IPv4模式下一样，路由器替换源地址，并且分配给你的是内网的IPv6地址，也就是前缀第一部分在2000-3FFF之外的地址，比如极路由的PPPOEv6模式，就是4006开头的IPv6地址。很多人仍然认为NAT66获取的全球不可达地址比PASSTHROUGH获取的全球可达地址安全，个人认为这是不全面的。
（1）、现在骇客技术的发达，内网防御有用，但是用处不大。
（2）、IPv4外网可达地址不安全是由于IPv4地址少，能够在可接受的时限内实现全网暴力扫描来攻击，但是对于IPv6巨大的地址池来说，目前来看，全网爬虫的精细扫描已经是扯淡了。
（3）、PASSTHROUGH仍然可以通过ip6tables的filter表的FORWARD规则来过滤。
（4）、长远来看全球可达地址对于游戏、PT下载等需要外网可达地址才能发挥高性能的程序有巨大意义。

### 教程三

来自：https://www.cnblogs.com/zafu/p/9186966.html

华硕已经自带 6relayd

ssh连入路由器。

首先查看wan口的网卡

```shell
ifconfig
```

应该输出一堆。可以参考这个文章https://blog.csdn.net/u013485792/article/details/50943069

我摘录一下重点

>eth0
>eth0是一块物理网卡。eth0.1 eth0.2都是从此设备上虚拟出来的。
>
>eth0.1 是vlan1分出的lan口。
>
>eth0.2 是vlan分出的wan口。
>
>
>rao raio
>这两个是成对出现，一看就知道是无线设备，它们各自对应一个SSID，分别是2.4G和5G。
>
>
>br-lan
>br-lan 虚拟设备，用于LAN口设备桥接，可以用brctl show查看使用情况。
>
>br-lan = eth0.1 + rai0 + ra0，即将有线LAN口和无线网统一划分为 LAN，便于管理！
>
>
>lo
>lo 虚拟设备，自身的回环网设备。
>
>pppoe-wan
>虚拟设备，他就是常见的拨号宽带上网，需要有ISP提供的用户名密码，连接后方可激此接口！

eth0.1应该就是指eth1

```shell
6relayd -d -A eth0 br0
```

路由器下所有的设备都能拿到ipv6了

### 教程四

https://post.smzdm.com/p/alpz5z98/

我所在学校宿舍的校园网用了一种叫做DHCPv6的姿势来给大家分配校园网，而且还只给每一个客户端分配一个IP，天啊，IPv6那么多地址，为什么只给一个！于是就悲剧了，路由器自带的IPv6选项在我的网络情况下都不可用。那么第一步就是确定你的网络环境是什么。最简单的方法就是打开你的网络设置，如果你发现你的IPv6前缀是128位，又或者如果你发现路由器自带的IPv6选项在你的网络环境中都用不了，那么就很有可能是跟我一样的网络环境了。

在这种网络环境下，除了NAT这样比较不符合IPv6精神的做法之外，唯一可用的就是桥接模式。也就是让路由器在IPv6网络上像一台[交换机](https://www.smzdm.com/fenlei/jiaohuanji/)一样工作，这样就能无缝接入校园网IPv6啦。网上有很多配置桥接模式的方法，AC86U也十分简单，通过ssh（关于如何ssh站内已经有很多文章介绍了，我就不重复说明啦）登陆到路由器上，在/jffs/scripts/wan-start脚本中添加下面两行命令就可以轻松搞定～

> ebtables -t broute -A BROUTING -i eth0 -p ! ipv6 -j DROP
>
> brctl addif br0 eth0

重启路由之后就可以开心的使用IPv6啦！但是！这个时候路由器就没办法获取IPv6了，你的NAS只有IPv6地址，又想在路由器上用，就很尴尬了，所以需要有一个特殊姿势让路由器在桥接模式下也获取到IPv6。由于之前在学校只学过理论知识，只知道理论可行，但是对实现不了解。恰好因为[华硕](https://pinpai.smzdm.com/1681/)路由器使用了openwrt，由于开源协议的限制，华硕自己的固件也要开源（所以才有了梅林），这样一来就可以通过围观固件源码的方法搞定这件事情了！下面就以AC86U为对象，介绍怎么在开启IPv6桥接模式的情况下为路由器获取IPv6地址。

首先要做一些准备工作：

将路由器的 IPv6 设置为 passthrough。然后通过 ssh 登陆路由器，运行下面这行命令

> ps | grep odhcp6c

你应该会看到有类似这样一行输出

> odhcp6c -df -R -s /tmp/dhcp6c -N try -c 000300010123d231c3a3 -r23 -r24 -r82 -r83 br0

其中有一串字符 000300010123d231c3a3，这串字符是你的**设备ID**，记下来这串字符，稍后要用到。

然后**关闭路由器的IPv6功能**。复制下面这一大长串脚本到你的/jffs/scripts/wan-start脚本中，一定要记得把最后一行的设备ID**替换**成你在准备工作里面获取到的ID，保存！重启！你会发现路由器自己也获取到IPv6地址了，LAN里的设备也正常获取到了IPv6地址！

```sh
#!/bin/sh

# IPv6 bridge
ebtables -t broute -A BROUTING -i eth0 -p ! ipv6 -j DROP
brctl addif br0 eth0

# enable IPv6 on eth0
echo 0 > /proc/sys/net/ipv6/conf/eth0/disable_ipv6
echo 2 > /proc/sys/net/ipv6/conf/eth0/accept_dad
echo 2 > /proc/sys/net/ipv6/conf/eth0/dad_transmits
echo 1 > /proc/sys/net/ipv6/conf/eth0/accept_ra
echo 0 > /proc/sys/net/ipv6/conf/eth0/forwarding
# see lan.c config_ipv6
echo 0 > /proc/sys/net/ipv6/conf/br0/disable_ipv6
echo 0 > /proc/sys/net/ipv6/conf/all/disable_ipv6
echo 0 > /proc/sys/net/ipv6/conf/default/disable_ipv6
echo 2 > /proc/sys/net/ipv6/conf/br0/accept_dad
echo 2 > /proc/sys/net/ipv6/conf/br0/dad_transmits
# set_default_accept_ra
echo 1 > /proc/sys/net/ipv6/conf/all/accept_ra
echo 1 > /proc/sys/net/ipv6/conf/default/accept_ra
echo 0 > /proc/sys/net/ipv6/conf/all/forwarding
# Allow router get IPv6 Address
# When user disable IPv6, system will set ip6tables ALL policy to DROP
# wait after that, then set our firewall
sleep 10
# set up firewall
ip6tables -P INPUT ACCEPT
ip6tables -P OUTPUT ACCEPT

ip6tables -A OUTPUT -p tcp -j ACCEPT
ip6tables -A OUTPUT -p udp -j ACCEPT
# input rules
ip6tables -A INPUT -p ipv6-crypt -j ACCEPT
ip6tables -A INPUT -p ipv6-auth -j ACCEPT
ip6tables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
ip6tables -A INPUT -m state --state NEW -j ACCEPT
ip6tables -A INPUT -m state --state INVALID -j DROP
# allow DHCPv6
ip6tables -A INPUT -p udp --sport 547 --dport 546 -j ACCEPT
ip6tables -A INPUT -p udp --sport 500 --dport 500 -j ACCEPT
ip6tables -A INPUT -p udp --sport 4500 --dport 4500 -j ACCEPT
# allow ipv6-icmp related packet
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type router-solicitation -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type packet-too-big -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type time-exceeded -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type parameter-problem -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type echo-request -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type echo-reply -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 130 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 131 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 132 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type router-solicitation -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type router-advertisement -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type neighbour-solicitation -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type neighbour-advertisement -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 141 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 142 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 143 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 148 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 149 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 151 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 152 -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 153 -j ACCEPT
ip6tables -A INPUT -j DROP

# Start DHCPv6 for LAN on br0
# In my enviroment, IPv6 address spwan from a DHCPv6 server
odhcp6c -df -R -s /tmp/dhcp6c -N try -c 你的设备ID -r23 -r24 -r82 -r83 br0
```

张大妈没有贴代码的选项，大家凑合看吧。

#### 原理解释

这一段较为硬核，需要一定的专业知识，可以跳过不影响你使用。方便好奇的同学了解背后的原理，并根据自己的设备因地制宜的修改设置。

接下来我会解释上面这个脚本在让路由获取IPv6地址的部分过程中都做什么，为什么这样做。在我的网络环境里，只有把路由器设置为 IPv6 passthrough 的时候路由器能正确获取 IPv6 地址。所以需要弄清楚这个模式做了什么操作。好在华硕固件是开源的，可以直接从官网下载。下文的讨论基于写这篇文章时能拿到的最新固件 3.0.0.4.384.32797 的源码。可以到[官网支持页面](https://go.smzdm.com/b959f880655d0b78/ca_bb_yc_163_70043652_10289_1681_185_0)选择操作系统为“其他”就可以看到源码下载链接。由于梅林是基于华硕固件源码的小改版本，也可以直接看[梅林仓库](https://github.com/RMerl/asuswrt-merlin.ng)的代码。

拿到了源码，直接搜索 PASSTHROUGH 关键字，发现启动 IPv6 Passthrough 的代码在 asuswrt/release/src/router/rc/wan.c下面，快速过一遍 IPv6 相关的代码，发现 IPv6 的设置主要有以下几个方面。

首先是开启IPv6，设置了accept_ra, forwarding一些字段。这些字段的意义都可以 Google 到。比如下面代码中设置的字段是互斥的，一个为1另一个就得为0。这两个字段决定了系统是工作在路由状态还是工作在主机状态。ra 是 router advertisement 的缩写，是 IPv6 网络用来自动配置路由的协议。我们想让路由器在LAN的状态像主机一样获取到 IP，就需要将路由器设置为主机状态，请求路由配置。

> case IPV6_PASSTHROUGH:
> \#endif
> ipv6_sysconf(wan_ifname, "accept_ra", 1);
> ipv6_sysconf(wan_ifname, "forwarding", 0);

代码中还有很多设置IPv6 相关配置的地方， 分散在各处，就不一一列出来了。总结下来就是分别在 WAN 口 和 LAN 口设置下面的相关参数。相关的代码可以在lan.c 和 wan.c 中找到。下面这段脚本就是用来设置对应参数的。

> \# enable IPv6 on eth0
> echo 0 > /proc/sys/net/ipv6/conf/eth0/disable_ipv6
> echo 2 > /proc/sys/net/ipv6/conf/eth0/accept_dad
> echo 2 > /proc/sys/net/ipv6/conf/eth0/dad_transmits
> echo 1 > /proc/sys/net/ipv6/conf/eth0/accept_ra
> echo 0 > /proc/sys/net/ipv6/conf/eth0/forwarding
> \# see lan.c config_ipv6
> echo 0 > /proc/sys/net/ipv6/conf/br0/disable_ipv6
> echo 0 > /proc/sys/net/ipv6/conf/all/disable_ipv6
> echo 0 > /proc/sys/net/ipv6/conf/default/disable_ipv6
> echo 2 > /proc/sys/net/ipv6/conf/br0/accept_dad
> echo 2 > /proc/sys/net/ipv6/conf/br0/dad_transmits
> \# set_default_accept_ra
> echo 1 > /proc/sys/net/ipv6/conf/all/accept_ra
> echo 1 > /proc/sys/net/ipv6/conf/default/accept_ra
> echo 0 > /proc/sys/net/ipv6/conf/all/forwarding

其次是开启relay功能，顺着 wan.c 再往下看，到了配置 IPv6 passthrough 的代码

> case IPV6_PASSTHROUGH:
> start_6relayd();
> /* fall through */
> \#endif
> case IPV6_NATIVE_DHCP:
> start_dhcp6c();

这段代码做了两步操作，第一步是启动了 6relayd ，可以确认华硕是用 6relayd 做的中继，据论坛用户表示该项目不是很稳定，并且在我的网络环境无法实现中继；第二步是启动了 DHCPv6 客户端，为特定的端口分配IP。具体代码实现太长，这里贴出一小部分

> int
> start_dhcp6c(void)
> {
> char *wan_ifname = (char *) get_wan6face();
> char *dhcp6c_argv[] = { "odhcp6c",
> "-df",
> "-R",
> "-s", "/tmp/dhcp6c",
> "-N", "try",
> NULL, NULL, /* -c duid */
> NULL, NULL, /* -FP len:iaidhex */
> NULL, NULL, /* -rdns -rdomain */
> NULL, NULL, /* -rsolmaxrt -r infmaxrt */
> NULL, /* -v */
> NULL, /* interface */
> NULL };
> int index = 7;
> struct duid duid;
> char duid_arg[sizeof(duid)*2+1];
> char prefix_arg[sizeof("128:xxxxxxxx")];
> int service;
> ...

可以看到路由器使用 odhcp6c 为自己在 WAN 口上获取IP。这个时候可以脑补这段代码获取参数值，这样比较麻烦且容易出错，所以才出现了配置部分一开头的使用ps | grep odhcp6c 查看参数的trick。经过多次验证参数是不变的，所以可以通过这个方法获取启动 odhcp6c 客户端的参数表。有一点主要注意的是默认情况下我们是为 WAN 口绑定 IP，而本文中的网络情况是 WAN 口 与 LAN 口已经被桥接在一起了，在 IPv6 的层面已经没有了内外网的区分。所以应为网桥，也就是 br0 绑定IP。

最后还需要调整防火墙设置，因为当路由器关闭了 IPv6 功能后，会默认过滤丢弃掉所有的 IPv6 数据包，所以还需要设置对应的防火墙规则。设置防火墙规则的代码散落在各处，找起来十分麻烦，且很难找全，所以祭出查看 odhcp6c 参数的方法，通过运行ip6tables -L 查看 filter 表中所有的规则，然后手写一份一样的出来就可以啦，于是就有了上面那一大长串 IPv6 的过滤规则。

最终，通过看源码可以梳理出来路由器在设置 IPv6 passthrough 后的行为如下

1. 修改系统设置，启用所有端口的 IPv6 功能
2. 添加防火墙规则，允许 IPv6 数据包通过路由器
3. 开启 DHCPv6 客户端，为路由器申请IP地址

所以按照这些步骤，就可以自己手动设置桥接后的路由器通过HDCPv6服务申请IPv6 地址，还顺便复习了一波计算机网络的知识2333。

## 问题所在

问题很简单，我们学校的校园网是pppoe-nd-01模式，就算使用桥接，如：passthrough模式，能拿到ipv6地址，但是也是没有Internet访问权限的。也就是说这个只有拨号拿到的地址才能用。之后我又找到了这个[NJUPT_IPv6](https://github.com/kaijianyi/NJUPT_IPv6)方案。悲剧的是，我的路由器是K2P B1，刷不了Padavan，所以我的一个方案是可以使用**NAT6**。

当然还有另一个方案：PPPoE Relay。PPPOE中继就是多拨，可是我没有这么多的账号，学校一个账号只能拨一个设备。。。

## NAT6





