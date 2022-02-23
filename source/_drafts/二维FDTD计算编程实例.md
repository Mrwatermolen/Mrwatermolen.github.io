---
title: 二维FDTD计算编程实例
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

线电流源辐射的定量计算。设二维FDTD区域为(-160:160, -160:160); 四周吸收边界采用UPML或者二阶Mur吸收边界，线电流源位于区域中心处。时谐场频率$f = 0.2GHz$

<!-- more -->

## 编程求解

解析式有点复杂，先编程求解。

此为二维TM波，物理量与z坐标无关，即$\frac{\partial}{\partial z} =  0$以及$E_z \neq 0 \quad H_z = 0$

由麦克斯韦旋度方程有：
$$
\begin{cases}
	\frac{\partial H_y}{\partial x}- \frac{\partial H_x}{\partial y} = \epsilon_0 \frac{\partial E_z}{\partial t} + \sigma E_z \\
	\frac{\partial E_z}{\partial y} = - \mu_0 \frac{\partial H_x}{\partial t} \\
	\frac{\partial E_z}{\partial x} = \mu_0 \frac{\partial H_y}{\partial t} \\
\end{cases}
$$

### 元胞不包含电流源

由[FDTD基本公式](https://blog.franzero.net/2021/12/16/FDTD%E5%9F%BA%E6%9C%AC%E5%85%AC%E5%BC%8F/)所给出的汇总式：
$$
\begin{cases}
	\vec{E}^{n+1} = CA \cdot \vec{E}^{n}  + CB \left[ \nabla \times \vec{H} \right]^{n+1/2} \\
	\vec{H}^{n+1/2} = CP \cdot \vec{H}^{n-1/2}  - CQ \left[ \nabla \times \vec{E} \right]^{n} \\
\end{cases}
$$
对于该问题，真空中有：
$$
CA = CP = 1, CB = \frac{\Delta t}{\epsilon_0},CQ = \frac{\Delta t}{\mu_0}
$$


可有：
$$
E^{n+1}_z(i,j) = E^{n}_z(i,j)  + \frac{\Delta t}{\epsilon_0} \left[ \frac{H_y^{n+1/2}(i+1/2, j) - H_y^{n+1/2}(i-1/2, j)}{\Delta x} - \frac{H_y^{n+1/2}(i, j+1/2) - H_y^{n+1/2}(i, j-1/2)}{\Delta y} \right]
$$
对应`Fortran`代码：

```
Ez(i,j,t) = Ez(i,j,t-1) + cb * (( Hy(i,j,t-1) - Hy(i-1,j,t-1) ) / delta_x - (( Hx(i,j,t-1) - Hx(i,j - 1,t-1) ) / delta_y)
```

同理：

```
Hy(i, j, t) = Hy(i, j, t - 1) + cq * ( Ez(i + 1, j, t) - Ez(i, j, t) ) / delta_x
Hx(i, j, t) = Hx(i, j, t - 1) - cq * ( Ez(i, j + 1, t) - Ez(i, j, t) ) / delta_y
```

### 元胞包含电流源

仅仅只有$E_z$节点位于电流源上，则：

参考