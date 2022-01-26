---
title: 一维FDTD计算编程实例
tags:
  - FDTD
  - 计算物理
  - 吸收边界
  - Fortran
  - 一维FDTD
  - 面电流源的电磁场分布
categories:
  - 电磁场
mathjax: true
---

考虑真空中，一无限大平面电流源：
$$
\vec{J}(z, t) = - \hat{x} I \cdot \cos(\omega t + \phi) \cdot \delta(z) \tag {1-1}
$$
其中$\delta(z)$为冲激函数。求解电磁场分布。

<!-- more -->

## 解析式

使用**Fourier变换**将时域转为频域：
$$
\vec{J}(z, t) = - \Re\left[ I \delta(z) \exp(\jmath \omega t + \phi) \right] \cdot \hat{x}\tag {1-2}
$$
其中$F(\omega)$是$\cos(\omega t + \phi)$的傅里叶变换对。

对无源区，有时谐电磁场波动方程：详见[FDTD前期准备](https://blog.franzero.net/2021/11/04/FDTD%E5%89%8D%E6%9C%9F%E5%87%86%E5%A4%87/)
$$
\begin{cases}
	\nabla^2 \vec{E} + k \frac{\partial^2 \vec{E}}{\partial t^2} & = 0 \\
	\nabla^2 \vec{H} + k \frac{\partial^2 \vec{H}}{\partial t^2} & = 0 \\
	 k = \frac{\omega^2}{\epsilon \mu}
\end{cases} \tag {1-3}
$$
可解得：
$$
\begin{cases}
	\vec{E}(z, \omega) & = \vec{E}^+(z) \exp(-\jmath kz) + \vec{E}^-(z) \exp(\jmath kz) \\
	\vec{H}(z, \omega) & = \vec{H}^+(z) \exp(-\jmath kz) + \vec{H}^-(z) \exp(\jmath kz) \\
\end{cases} \tag {1-3}
$$
$\vec{E}^+(z) \exp(-\jmath kz)$是沿z轴正方向传播，反之反之。

由对称性不难得出：
$$
\begin{cases}
	\vec{H}(z, \omega) & = \hat{y} H^+(z) \exp(-\jmath kz) & z > 0 \\
	\vec{H}(z, \omega) & = \hat{y} H^-(z) \exp(\jmath kz) & z < 0 \\
\end{cases}
$$
由**Maxwell方程**中的：$\nabla \times \vec{H} - \frac{\partial \vec{D}}{\partial t} = \vec{J}$，取一边长为a的正方形`yoz`面。可得：
$$
2 a |H_y| = a I \Rightarrow |H_y| = \frac{I}{2}  \tag {1-1}
$$

以后填坑，实在没明白时域频域互转。

解析解：
$$
\begin{cases}
	\vec{H}(z, t) & = \hat{y} \frac{I}{2} \cos(\omega t - kz + \phi) & z > 0 \\
	\vec{H}(z, t) & = \hat{y} \frac{I}{2} \cos(\omega t + kz + \phi) & z < 0 \\
	\vec{E}(z, t) & = \hat{x} \frac{I}{2Z_0} \cos(\omega t - kz + \phi) & z > 0 \\
	\vec{E}(z, t) & = -\hat{x} \frac{I}{2Z_0} \cos(\omega t + kz + \phi) & z < 0 \\
\end{cases}
$$

## FDTD计算

边界采用一阶Mur近似吸收边界

### 获取差分公式

对于该问题，真空中一维FTDT问题，为求解TEM波。则对于麦克斯韦旋度方程有：
$$
\begin{cases}
	- \frac{\partial H_y}{\partial z} = \epsilon_0 \frac{\partial E_x}{\partial t} \\
	\frac{\partial H_x}{\partial z} = \epsilon_0 \frac{\partial E_y}{\partial t} \\
	0 = E_z \\
	- \frac{\partial E_y}{\partial z} = \epsilon_0 \frac{\partial H_x}{\partial t} \\
	\frac{\partial E_x}{\partial z} = \epsilon \frac{\partial H_y}{\partial t} \\
	0 = H_z \\
\end{cases}
\tag {1-3}
$$
观察到的是Ex与Hy相关，Ey与Hx相关

#### 元胞不含电流源

