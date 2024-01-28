---
title: 相对论性的物理学
tags:
  - 狭义相对论
categories:
  - 电磁场
mathjax: true
date: 2023-11-14 19:27:10
---


假设$\bar{S}$相对于$S$的速度为$v$，且仅在$x$轴上运动，令$t=\bar{t}=0$时刻，两参考系坐标原点重合。洛伦兹变换为：

$$
\begin{equation}
A =
\begin{bmatrix}
\gamma & 0 & 0 &  \mathrm{i}\beta\gamma \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 0 \\
-\mathrm{i}\beta\gamma & 0 & 0 & \gamma \\
\end{bmatrix}
\end{equation}
$$

## 开胃菜

定义一个四维矢量

$$
\begin{equation}
k_{\mu} =
\begin{bmatrix}
k_x \\
k_y \\
k_z \\
\mathrm{i} \frac{\omega}{c}
\end{bmatrix}
\end{equation}
$$

<!-- more -->

点乘上一个事件时空点，有：

$$
\begin{equation}
k_{\mu} \cdot x_{\mu} = \bar{k}_{\mu} \cdot \bar{x}_{\mu}
\end{equation}
$$

有：

$$
\begin{equation}
\vec{k}\cdot\vec{r} - \omega t = \vec{\bar{k}} \cdot \vec{\bar{r}} - \omega \bar{t}
\end{equation}
$$

这就是电磁波的相位不变性。
TODO：为什么如此定义后的$k_{\mu}$一定是一个四维矢量呢

由洛伦兹变换得：

$$
\begin{equation}
\bar{k}_x = \gamma \cdot \left( k_x - \frac{\beta \omega}{c} \right), \quad \bar{\omega} = \gamma \left( \omega - vk_x \right)
\end{equation}
$$

对波矢量有：

$$
\begin{equation}
k_x = \sin{\theta}\cos{\varphi} \frac{\omega}{c}, \quad \bar{k_x} = \sin{\bar{\theta}}\cos{\bar{\varphi}}\frac{\bar{\omega}}{c}
\end{equation}
$$

有：

$$
\begin{equation}
\begin{cases}
    \bar{\omega} = \omega \gamma \left( 1 - \beta \sin{\theta}\cos{\varphi} \right) \\
    \bar{k}_x = \gamma \left( \sin{\theta}\cos{\varphi} - \beta \right)\frac{\omega}{c} \\
    \bar{k}_y = k_y = \sin{\theta}\sin{\varphi} \frac{\omega}{c} \\
    \bar{k}_z = k_z = \cos{\theta} \frac{\omega}{c}\\\
    \tan{\bar{\varphi}} = \frac{\sin{\varphi}}{\gamma \left( \cos{\varphi} -\beta \right)}
\end{cases}
\end{equation}
$$

如果令：

$$
\begin{equation}
\theta = \frac{\pi}{2}
\end{equation}
$$

若设光源相对于坐标系$\bar{S}$静止，则在$S$系中观察者观察到的$S$系的运动光源角频率为

$$
\begin{equation}
\omega = \frac{\omega_0}{\gamma (1 - \beta \cos{\varphi})}
\end{equation}
$$

其中为$\omega_0$光源的静止时辐射的频率，$\varphi$为观察者看到的辐射方向与光源运动方向的夹角。

垂直于光源运动方向观察辐射时：

$$
\begin{equation}
\omega = \omega_0 \sqrt{1 - \beta^2}
\end{equation}
$$

观察到的辐射频率小于静止光源的辐射频率，这现象称为横向多普勒效应。

波源远离观测者时，$\varphi = \pi$则：

$$
\begin{equation}
\omega = \omega_0 \sqrt{\frac{1-\beta}{1+\beta}}
\end{equation}
$$

波源靠近观察者时，$\varphi = 0$则：

$$
\begin{equation}
\omega = \omega_0 \sqrt{\frac{1+\beta}{1-\beta}}
\end{equation}
$$

## 物理规律的协变性

如果一个方程的每一项属于同类协变量，在参考系变换下，每一项都按相同方式变换，结果保持方程
形式不变。例子：对某个方程：

$$
\begin{equation}
F_{\mu} = G_{\mu}
\end{equation}
$$

其中$F_{\mu}$和$G_{\mu}$为四维矢量，则在洛伦兹变换下：

$$
\begin{equation}
\bar{F}_{\mu} = a_{\mu\nu}F_{\nu} = a_{\mu\nu}G_{\nu} = \bar{G}_{\mu}
\end{equation}
$$

保持不变。

在参考系变换下方程形式不变的性质称为协变性。相对性原理要求一切惯性参考系都是等价的。在不同惯性系中，物理规律应该可以表为相同形式。如果表示物理规律的方程是协变的话，它就满足相对性原理的要求。

## 电动力学的相对论不变性

### 四维电流密度矢量

证明略去

$$
\begin{equation}
J_{\mu} = (\textbf{J}, \mathrm{i}c\rho)
\end{equation}
$$

### 四维矢量式

在洛伦茨规范下

$$
\begin{equation}
\begin{cases}
    \nabla^2\textbf{A} - \frac{1}{c^2}\frac{\partial^2 \textbf{A}}{\partial t^2} = -\mu_0 \textbf{J} &\text{}\\
    \nabla^2\varphi - \frac{1}{c^2}\frac{\partial^2 \varphi}{\partial t^2} = -\frac{\rho}{\varepsilon_0} &\text{}\\
    \nabla \cdot \textbf{A} + \frac{1}{c} \frac{\partial \varphi}{\partial t} = 0 &\text{Lorenz gauge}
\end{cases}
\end{equation}
$$

有达朗贝尔算符

$$
\begin{equation}
\Box \equiv \nabla^2-\frac{1}{c^2}\frac{\partial^2}{\partial t^2} = \frac{\partial}{\partial x_{\mu}}\frac{\partial}{\partial x_{\mu}}
\end{equation}
$$

是洛伦兹标量算符。于是有：

$$
\begin{equation}
\begin{aligned}
    \Box \textbf{A} = -\mu_0 \textbf{J}\\
    \Box \varphi = -c^2\mu_0 \rho\\
\end{aligned}
\end{equation}
$$

由协变性，$\textbf{A}$和$\varphi$应该也可统一为一四维矢量式：

$$
\begin{equation}
A_{\mu} = \left( \textbf{A}, \frac{\mathrm{i}}{c}\varphi \right)
\end{equation}
$$

于是有：

$$
\begin{equation}
\Box A_{\mu} = -u_0 J_{\mu}
\end{equation}
$$

这个应该不是证明，这里只是定义了四维矢量式。

选自《电动力学简明教程》
> 若用实验能证实$J_{\mu}$和$A_{\mu}$有四维矢量的变换性质，那么电磁规律就必服从相对性原理。

### 电磁场张量

假定现在肯定了电动力学规律的协变性。

由：

$$
\begin{equation}
\textbf{B} = \nabla \times \textbf{A}
\end{equation}
$$

可有：

$$
\begin{equation}
B_1 = \frac{\partial A_3}{\partial x_2} - \frac{\partial A_2}{\partial x_3} \dots
\end{equation}
$$

同理可得$\textbf{E}$。

记：

$$
\begin{equation}
F_{\mu\nu} = \frac{\partial A_{\nu}}{\partial x_{\mu}} - \frac{\partial A_{\mu}}{\partial x_{\nu}}
\end{equation}
$$

可知$F_{\mu\nu}$为一反对称张量。称为电磁场张量。

根据2阶四维张量的协变性。

$$
\begin{equation}
\bar{F}_{\mu\nu} = a_{\mu\lambda}a_{\mu\tau}F_{\lambda\tau}
\end{equation}
$$

写为矩阵关系为：

$$
\begin{equation}
\bar{F}_{\mu\nu} = AF_{\lambda\tau}A^{-1}
\end{equation}
$$

得

$$
\begin{equation}
\begin{cases}
    \bar{E}_{\parallel} = E_{\parallel} &\text{与速度$v$平行}\\
    \bar{E}_{\perp} = \gamma\left( \textbf{E} + \textbf{v} \times \textbf{B} \right)_{\perp} &\text{与速度$v$垂直}\\
    \bar{B}_{\parallel} = B_{\parallel} &\\
    \bar{B}_{\perp} = \gamma\left( \textbf{B} - \textbf{v} \times \textbf{E} / c^2\right)_{\perp} &\text{}\\
\end{cases}
\end{equation}
$$

当$v\ll c$时，忽略二阶无穷小量。

$$
\begin{equation}
\bar{\textbf{E}} = \textbf{E} + \textbf{v} \times \textbf{B}, \quad\bar{\textbf{B}} = \textbf{B} - \frac{\textbf{v}}{c^2} \times \textbf{E}
\end{equation}
$$

矢势和标势统一为四维矢量以及电场和磁场统一为四维张量，反映出电磁场的统一性和相对性。电动力学基本方程式对任意惯性参考系成立。在坐标变换下，势按四维矢量变换，电磁场按四维张量变换。

选自《电动力学简明教程》

>电磁场是 一个整体，而电场或磁场都各是其部分表现。在相对论中，这整体才得到了统一的描写。顺便指出，电场和磁场用不同单位来量度是历史造成的。在国际际单位制中，电场强度和磁感应强度甚至有不同的量纲两者之比是c的量纲，这是单位制的人为性被过分地利用了的后果，因它对电磁学的理论讨论很不利，所以国际单位制在理论研究中很少被采用。
>
>若采用洛伦兹变换代替御里略变換，那么电磁规律是与相对性原理相治的。这正是爱因斯坦建立狭义相对论的基础。出此引申出两个要点：（1）“相洽”并不意味着“必然服从”，因此这里是需要有进一步的实践检验的；（2）当事实已确证电动力学服从相对论的要求，那么各种电磁学量在惯性系转换下的变换关系就都确定了。

### 电磁场的不变量

$$
\begin{equation}
\frac{1}{2}F_{\mu\nu}F_{\mu\nu} = B^2 - \frac{E^2}{c^2}
\end{equation}
$$

后面还有，这个是真不明白了。

$$
\begin{equation}
\textbf{B} = \frac{1}{c} \textbf{n}\times \textbf{E}
\end{equation}
$$
