---
title: PML边界
tags:
---

## Berenger完全匹配层 BPML

### 二维形式Berenger场

TE波
![fig2.png](https://s2.loli.net/2022/03/13/EraO76P4TWQIvx5.png)
$$
E^{n+1}_{zx}(i,j) = ca(i,j) \cdot E^{n}_z(i,j)  + cb(i,j)  \cdot \left[ \frac{H_y^{n+1/2}(i+1/2, j) - H_y^{n+1/2}(i-1/2, j)}{\Delta x} \right]
$$
PML边界中
$$
E^{n+1}_{zx}(i,j) = \exp(-\sigma_x(i) \frac{\Delta t}{\mu_0}) \cdot E^{n}_z(i,j)  + cb(i,j)  \cdot \left[ \frac{H_y^{n+1/2}(i+1/2, j) - H_y^{n+1/2}(i-1/2, j)}{\Delta x} \right]
$$
对于和理想导体的交界面上$\sigma_x(i) = 0$
$$
E^{n+1}_{zx}(i,j) = \exp{(-\sigma_x(i) \frac{\Delta t}{\mu_0})} \cdot E^{n}_z(i,j)  + \frac{1 - \exp{(-\sigma_x(i) \frac{\Delta t}{\mu_0})}}{\Delta x \sigma_x(i)}  \cdot \left[ \frac{H_y^{n+1/2}(i+1/2, j) - H_y^{n+1/2}(i-1/2, j)}{\Delta x} \right]
$$
该式退为：
$$
E^{n+1}_{zx}(i,j) = E^{n}_z(i,j)  + \frac{\Delta t}{\mu_0}  \cdot \left[ \frac{H_y^{n+1/2}(i+1/2, j) - H_y^{n+1/2}(i-1/2, j)}{\Delta x} \right]
$$
对于

## CPML

考虑时谐场的Maxwell方程组：
\[
    \begin{cases}
        \nabla_s \times \vec{E} = -\jmath \mu \omega \vec{H} \\
        \nabla_s \times \vec{H} = \jmath \epsilon \omega \vec{E} \\
        \nabla_s \cdot (\epsilon \vec{E}) = 0 \\
        \nabla_s \cdot (\mu \vec{H}) = 0 \\
    \end{cases}
\]
对算子$\nabla_s$有定义：
\[
    \nabla_s = \hat{x} \frac{1}{s_x} \cdot \frac{\partial}{\partial x} + \hat{y} \frac{1}{s_y} \cdot \frac{\partial}{\partial y} + \hat{z} \frac{1}{s_z} \cdot \frac{\partial}{\partial z}
\]
$s_x,s_y,s_z \in Z$称为伸缩因子，它们是关于对应坐标的函数$s_x = s_x(x)$，满足以上关系的介质称为坐标伸缩介质——CPML。
其平面波（时间因子体现在$\vec{E_0}$中）：
\[\vec{E} = \vec{E_0} \exp{(-\jmath \vec{k} \cdot \vec{r})}\]
波矢量$\vec{k} = \hat{x} k_x + \hat{y} k_y + \hat{z} k_z$
则：
\[
    \begin{aligned}
        \nabla_s \cdot \vec{E} &= -\jmath (\hat{x} \frac{k_x}{s_x} + \hat{y} \frac{k_y}{s_y}+ \hat{z}\frac{k_z}{s_z}) \cdot \vec{E} \\
        &= -j \vec{k_s} \cdot \vec{E} \\
    \end{aligned}
\]
同理，我们有：
\[
    \nabla_s \times \vec{E} = -\jmath \vec{k_s} \times \vec{E}
\]
对于CPML，我们有算子关系：
\[
    \nabla_s \leftrightarrow -\jmath \vec{k_s}
\]
因此对Maxwe方程组：
\[
    \begin{cases}
        \vec{k_s} \times \vec{E} = \mu \omega \vec{H} \\
        \vec{k_s} \times \vec{H} = -\epsilon \omega \vec{E} \\
        \vec{k_s} \cdot (\epsilon \vec{E}) = 0 \\
        \vec{k_s} \cdot (\mu \vec{H}) = 0 \\
    \end{cases}
\]
则：
\[
    \begin{aligned}
        \nabla_s \times \left(\nabla_s \times \vec{E} \right) &= \nabla_s\left(\nabla_s \cdot \vec{E} \right) - \nabla_s^2 \vec{E} \\
        \vec{k_s} \times \mu \omega \vec{H} &= -\left( -\jmath \vec{k_s} \cdot -\jmath \vec{k_s} \right) \vec{E} \\
        \mu \epsilon \omega^2 \vec{E} &= \left(\vec{k_s} \cdot \vec{k_s} \right) \vec{E}
    \end{aligned}
\]
由此：
\[\left(\vec{k_s} \cdot \vec{k_s} \right) = \left(\frac{k_x^2}{s_x^2} + \frac{k_y^2}{s_y^2}+ \frac{k_z^2}{s_z^2}\right) = \mu \epsilon \omega^2\]
记：
\[\kappa = \mu \epsilon \omega^2\]
则有：
\[\frac{k_x}{s_x} = \kappa \sin\theta \cos\phi\]
即：
\[k_x = s_x\kappa \sin\theta \cos\phi\]
则当$s_x$存在不为0的虚部时。平面波将发生衰减。对于其他方向也同理。
d
