---
title: FDTD计算RCS的一个问题
tags: FDTD
categories:
  - 电磁场
date: 2024-04-03 10:39:47
---


> 记录一下FDTD仿真遇到的一个问题。碎碎念

考虑如下一个球体散射的例子

<img src="https://s2.loli.net/2024/04/02/aMP2m8dYLi9vEWy.png" alt="球体散射" width="50%">

球体半径为0.1。入射场是沿着z轴传播的平面波，极化方向为x，波形为高斯波形。
<!-- more -->

## 问题

一开始是做完了，但是精度惨不忍睹，尤其是后向散射这一块。针对双站的RCS。取网格尺寸为0.75cm。

![Figure_1.png](https://s2.loli.net/2024/04/03/b8IZEOMxit4LTBA.png)

C++的数据是我做的，后向散射那和Mie理论的误差太大了。

## 寻找问题来源

### TFSF泄漏

先说结论：没有泄漏

最开始我认为是TFSF产生了泄漏。虽然TFSF场我在2维时已经验证过没有泄漏，但3维的情况并未验证。
画了一下仿真时的XZ平面的切面图，肉眼没有发现泄漏。（XY、YZ平面都画了，这里就只放一个就好了）

![ex_xz.gif](https://s2.loli.net/2024/04/03/MQYt2CODErwseRj.gif)

然后和纯散射场的结果进行了对比，在距离CPML边界3个网格的地方取一点（其实也就是NF2FF的惠更斯面）观察散射场。

![scatter_ex_time.png](https://s2.loli.net/2024/04/03/idKskVxPXD8ywFY.png)

橙色是TFSF的散射场，绿色是纯散射场。可以看出两者只有时延。做了一下FFT，验证了两个波的一致性。

![scatter_ex_freq.png](https://s2.loli.net/2024/04/03/RXayvQtju2BhnrD.png)

### NF2FF

先说结论：没有问题

会不会是近远场变换出了问题呢。把六个惠更斯面都拆分了，一个个分析。直接上有问题比较大的那几个

法向量是XN的平面的结果，先放势函数。

![a_theta.png](https://s2.loli.net/2024/04/03/sjmy9auDIpFGVfx.png)

![f_phi.png](https://s2.loli.net/2024/04/03/k51BKj679enhOlM.png)

实部虚部和相位这些不用看，因为我的场和Matlab计算产生的场有一个时延，所以肯定是不一样的，看模值即可，基本是吻合的。然后计算一下E面的RCS。

![rcs_db.png](https://s2.loli.net/2024/04/03/tziYHypE6d42MsX.png)

在后向散射这块，RCS出现了2个dB的误差！试着把dB去掉。

![rcs.png](https://s2.loli.net/2024/04/03/JnB2csrPp5N7RVf.png)

也许是因为数值太小了，0.0001的波动都会使得结果有较大浮动。（我真的以为这个就是结论了）

## 解决

### 时间步问题

FDTD的更新框架是：H->E。
于是我的TFSF也是这么更新入射场的。

整体的更新框架：
TFSF更新入射场
FDTD更新磁场
修正磁场
FDTD更新电场
修正电场

但是问题在于：

t=0.5时，FDTD修正TFSF边界上的磁场需要用到的是t=0时刻时的电场值。如果TFSF是H->E，那么TFSF里的入射场就永远超前了。

所以，把TFSF改为E->H。

结果呢：

![Figure_2.png](https://s2.loli.net/2024/04/03/GaY2FUs57HSC3Ne.png)