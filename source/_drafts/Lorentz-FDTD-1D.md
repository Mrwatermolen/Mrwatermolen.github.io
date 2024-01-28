---
title: Lorentz-FDTD-1D
tags:
---

Ref: <https://doi.org/10.1109/LAWP.2015.2481933>

电磁波为TEM波。有一介质板相对于$S$系以速度$v$向$+z$轴运动。设$\bar{S}$系相对介质板静止。设$t=0$时刻$S$系和$\bar{S}$系的原点重合。

洛伦兹变换矩阵为

$$
\begin{equation}
A =
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & \gamma & \mathrm{i}\beta\gamma  \\
0 & 0 & -\mathrm{i}\beta\gamma & \gamma \\
\end{bmatrix}
\end{equation}
$$

由于是一维情况

$$
\begin{equation}
A =
\begin{bmatrix}
\gamma & \mathrm{i}\beta\gamma  \\
-\mathrm{i}\beta\gamma & \gamma \\
\end{bmatrix}
\end{equation}
$$

## 思路

$S$系中一静止光源发射平面波$E_i$，将$S$系的平面波通过洛伦兹变换转到$\bar{S}$系中，此时在$\bar{S}$系中应用FDTD算法，获得的反射波和透射波，再通过洛伦兹变换回到$S$系。

## 入射波的转换

假设入射波为时谐
