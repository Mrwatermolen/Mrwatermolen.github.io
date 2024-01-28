---
title: 001-The One Dimensional Scalar Wave Equation
tags:
  - FDTD
  - 计算物理
categories:
  - FDTD NOTE Allen Taflove
date: 2023-11-13 14:35:28
mathjax: true
---


考虑一个在自由空间中一维传播波方程

$$
\begin{equation}
\frac{\partial^2 u}{\partial t^2} = c^2\frac{\partial^2 u}{\partial x^2}
\end{equation}
$$

该方程的通解为：

<!-- more -->

$$
\begin{equation}
u(x,t) = f(ct + x) + g(ct - x)
\end{equation}
$$

$f(ct +x )$为向-x方向传播的波，即经过$\Delta t$时间后，空间上必须要减少$c\Delta t$的距离获得波前的值。相反的，$g(ct-x)$为向+x方向传播的波。

## 色散

色散定义为波的波长随频率的变化而变化（物理量随频率的变化而变化）。更常见的，色散关系由波数$k$随角频率$\omega$的变化而变化呈现。

对于一维传播波方程，假设有一正弦形式传播的单色波

$$
\begin{equation}
u(x,t) = e^{j (\omega t - kx)}
\end{equation}
$$

有如下关系

$$
\begin{equation}
\begin{aligned}
-\omega^2 u = -k^2c^2  u \\
\omega^2 = k^2c^2 \\
k = \pm \frac{\omega}{c}
\end{aligned}
\end{equation}
$$

这就是一维单色波传播的色散关系。

## 有限差分

Pass

## 数值色散关系

我们使用有限差分可以得到

$$
\begin{equation}
u_{i}^{n+1} = \frac{c^2\Delta t^2}{\Delta x^2} \cdot
\left[ u_{i+1}^n - 2u_{i}^n + u_{i-1}^n \right] +
2 u_{i}^n - u_i^{n-1}
\end{equation}
$$

考虑数值计算得到的数值波数为$\tilde{k}$，考虑单色波解$u =  e^{j (\omega t - \tilde{k}x)}$，代入后得到

$$
\begin{equation}
\tilde{k} = \frac{1}{\Delta x} \cdot
\cos^{-1}
\left\{
1 + \frac{\Delta x^2}{c^2\Delta t^2} \left[ \cos{(\omega\Delta t) - 1} \right]
\right\}
\end{equation}
$$

可见色散关系与离散的时间和空间尺度相关。

* 考虑离散的时间和空间尺度都非常小，使得$\omega\Delta t \to 0$和$k\Delta x \to 0$

$$
\begin{equation}
\tilde{k} = k
\end{equation}
$$

* 如果取$\frac{\Delta x}{c\Delta t} = 1$

$$
\begin{equation}
\tilde{k} = k
\end{equation}
$$

* 如果取不够小，存在数值相速度误差。

记

$$
\begin{equation}
S = \frac{c\Delta t}{\Delta x}
\end{equation}
$$

为稳定限制因素（Courant limited factor），再记

$$
\begin{equation}
N_{\lambda} = \frac{\lambda_0}{\Delta x}
\end{equation}
$$

为网格分辨率（grid sampling resolution）

$$
\begin{equation}
\tilde{k} = \frac{1}{\Delta x} \cdot
\cos^{-1}
\left\{
1 + \frac{1}{S^2} \left[ \cos{\left(\frac{2\pi S}{N_{\lambda}}\right) - 1} \right]
\right\}
\end{equation}
$$

这两个因素决定了数值波数的取值。

如果，$S$和$N_{\lambda}$的取值使得数值波数为一虚部不为0的复数，可以预计数值计算得到的结果为hushibo，且相速度超过了光速。使得数值波数为实数的条件为

$$
\begin{equation}
N_{\lambda} \ge 2\pi S / \cos^{-1}{(1-2S^2)}
\end{equation}
$$

即使数值波数为实数，但如果$S$和$N_{\lambda}$的取值不恰当，仍然会造成相速度误差，表现为小于光速。（一维下可以选择特殊的值进行消除，S=1。但是对于二维和三维情况下，无法消除这种数值色散误差）

## 数值稳定性分析

之前写过了

一维情况下为：

$$
S \le 1 \to \Delta t \le \frac{\Delta x}{c}
$$
