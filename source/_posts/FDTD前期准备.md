---
title: FDTD前期准备
date: 2021-11-04 23:00:07
tags:
  - FDTD
  - Fourier变换
  - 计算物理
categories:
  - 电磁波
mathjax: true
---


> 暂时先叫这个标题吧
>
> 从头开始推

## 电磁场知识

旋度公式：
$$
\nabla \times (\vec{A} \times \vec{B}) = (\vec{B} \cdot \nabla)\vec{A} - (\vec{A} \cdot \nabla)\vec{B} - \vec{B}(\nabla \cdot \vec{A}) + \vec{A}(\nabla \cdot \vec{B}) \tag {1-1}
$$

### Maxwell Equation

$$
\left\{
    \begin{array}{a}
        \nabla \times \vec{E} + \frac{\partial \vec{B}}{\partial t} = \vec{0} \\
        \nabla \times \vec{H} - \frac{\partial \vec{D}}{\partial t} = \vec{J} \\
        \nabla \cdot \vec{D} = \rho \\
        \nabla \cdot \vec{B} = 0
    \end{array}
\right. \tag {1-2}
$$

其中：
$$
\nabla \cdot \vec{J} + \frac{\partial \rho}{\partial t} = 0 \tag {1-3}
$$
在线性各项同性介质中，本构关系为：
$$
\left\{
    \begin{array}{}
        \vec{D} = \epsilon \vec{E} \\
        \vec{H} = \mu \vec{B} \\
        \vec{J} = \sigma \vec{E} \\
    \end{array}
\right. \tag {1-4}
$$

### 时谐电磁场

在时谐场中，所有的电磁场量：
$$
\vec{E},\vec{D},\vec{H},\vec{B}
$$
均为余弦或正弦函数形式。$\vec{E} = E_0 \cos(\omega t + \phi)$

采用复数表示法。
$$
\vec{E} = \Re\left[ E_0 \exp(\jmath \omega t + \phi) \right] \tag {1-5}
$$
时谐因子为$\exp(\jmath \omega t )$，此时有时间导数算子与频域算子的对应关系：
$$
\frac{\partial}{\partial t} \rightarrow \jmath \omega \tag {1-6}
$$

### 波动方程的推导

取磁场的旋度方程$\nabla \times \vec{H} = \vec{J} + \frac{\partial \vec{D}}{\partial t}$
$$
\begin{align*}
	\nabla \times (\nabla \times \vec{H}) & = \nabla \times (\vec{J} + \frac{\partial \vec{D}}{\partial t}) \\
	\nabla (\nabla \cdot \vec{H}) - \nabla^2 \vec{H} & = \nabla \times \vec{J} - \frac{1}{\epsilon} \frac{\partial \vec{B}}{\partial t} \\
	\nabla^2 \vec{H} - \frac{1}{\epsilon \mu} \frac{\partial^2 \vec{H}}{\partial t^2} & = -\nabla \times \vec{J} \\
\end{align*} \tag {1-7}
$$
同理有：
$$
\begin{cases}
	\nabla^2 \vec{E} - \frac{1}{\epsilon \mu} \frac{\partial^2 \vec{E}}{\partial t^2} & = \frac{\nabla \rho}{\epsilon} + \mu \frac{\partial \vec{J}}{\partial t} \\
	\nabla^2 \vec{H} - \frac{1}{\epsilon \mu} \frac{\partial^2 \vec{H}}{\partial t^2} & = -\nabla \times \vec{J} \\
\end{cases} \tag {1-8}
$$


## 傅里叶变换Fourier Transform（FT）

引入FT实现时域与频域之间的转换

### 连续函数的傅里叶变换

H(f)的Fourier变换的定义为：
$$
h(t) = \int_{-\infty}^{\infty} {H(f)\exp(\jmath2\pi ft)} \,{\rm d}f \tag{2-1}
$$
其逆变换（Inverse Fourier Transform，IFT）：
$$
H(f) = \int_{-\infty}^{\infty} {h(t)\exp(-\jmath2\pi ft)} \,{\rm d}t \tag {2-2}
$$
一个性质
$$
H^*(-f) = H^*(f) \tag {2-3}
$$
实际应用中频率为正值，则式（2-1）可变为：
$$
h(t) = \left[ \int_{-\infty}^0 {H(f)\exp(\jmath2\pi ft)} \,{\rm d}f \right]^* + \int_0^{\infty} {H(f)\exp(\jmath2\pi ft)} \,{\rm d}f = 2\Re{\int_0^{\infty} {H(f)\exp(\jmath2\pi ft)} \,{\rm d}f} \tag {2-4}
$$


附录：常用函数的Fourier变换对

待补充

### 离散函数的傅里叶变换

Discrete Fourier Transform（DFT）定义了两个数组之间的映射变换

q(n)的离散Fourier变换为：
$$
Q(m) = \sum_{n=0}^{N-1} q(n) \exp\left[-\jmath(\frac{2\pi}{N})m \cdot n \right] \quad m=0,1,\cdots, N-1 \tag {2-5}
$$
IDFT：
$$
q(n) = \sum_{m=0}^{N-1} Q(m) \exp\left[\jmath(\frac{2\pi}{N})m \cdot n \right] \quad n=0,1,\cdots, N-1 \tag {2-6}
$$

### 将连续函数离散化

实际信号通常为因果信号，即有
$$
h(t)=
	\begin{cases}
		0, & t \lt 0 \\
		h(t), & t \ge 0 \\
	\end{cases} \tag {2-7}
$$
则等式（2-1）变为
$$
H(f) = \int_0^{\infty} h(t) \exp(-\jmath2\pi ft) \,{\rm d}t \tag {2-8}
$$
若将时间离散化：
$$
H(f) \approx \Delta t \sum_{n=0}^{N-1}h(n\Delta t) \exp(-\jmath2\pi f n\Delta t) \tag {2-9}
$$
考虑实际**时域波形拖尾的有限性**（？？？？why），上式的求和为有限项。再将频率也离散化：
$$
f = m\Delta f \tag {2-10}
$$
令
$$
\Delta f = \frac{1}{N\Delta t} \tag {2-11}
$$
带入（2-9）即可得：
$$
\begin{align}
	H(m\Delta f) & = \Delta t \sum_{n=0}^{N-1}h(n\Delta t) \exp(-\jmath2\pi m \frac{1}{N\Delta t} n\Delta t) \\
				 & = \Delta t \sum_{n=0}^{N-1}h(n\Delta t) \exp(-\jmath \frac{2\pi}{N} mn) \\
\end{align} \tag {2-12}
$$
好了，终于离散化成功了！

## 数值计算：有限差分

### 基本公式

一阶中心差商：
$$
\frac{ {\rm d} u_i}{ {\rm d} x} = \frac{u_{i + 1} - u_{i - 1} }{2h} + O(h^2) \tag {3-1}
$$
二阶中心差商：
$$
\frac{ {\rm d^2} u_i}{ {\rm d} x^2} = \frac{u_{i + 1} - 2u_i + u_{i - 1} }{h^2} + O(h^2) \tag {3-2}
$$

#### 处理边界时

一阶前向差商：
$$
\frac{ {\rm d} u_i}{ {\rm d} x} = \frac{-u_{i + 2} + 4u_{i + 1} - 3u_i}{2h} + O(h^2) \tag {3-3}
$$
一阶后向差商：
$$
\frac{ {\rm d} u_i}{ {\rm d} x} = \frac{u_{i - 2} - 4u_{i - 1} + 3u_i}{2h} + O(h^2) \tag {3-4}
$$

### 普适的方程推导（在一维情况下推导，可推广到多维）

有如下方程：
$$
y''(x) = u(x) + v(x)y(x) + w(x)y'(x) \tag {3-5}
$$

#### 推导

进行离散化处理：
$$
\begin{align}
	\text{步长}h & = \frac{b - a}{n} \\
	x_i & = ih \\
	y_i & = y(x_i) \\
\end{align}
\tag {3-6}
$$

对式（3-5）：
$$
\begin{align}
	& \frac{y_{i + 1} - 2y_i + y_{i - 1}}{h^2} = u_i + v_i y_i + w_i \frac{y_{i + 1} - y_{i - 1}}{2h} \\
	\Rightarrow & -(1 - \frac{1}{2} hw_i) y_{i+1} + (2 + h^2 v_i) y_i - (1 + \frac{1}{2} hw_i) y_{i-1} = -h^2u_i \\
\end{align}
\tag {3-7}
$$
记：
$$
a_i = -(1 + \frac{1}{2} hw_i) ,\quad b_i = (2 + h^2 v_i) ,\quad c_i = -(1 - \frac{1}{2} hw_i) ,\quad d_i = h^2u_i \tag {3-8}
$$
可有：
$$
a_iy_{i-1} + b_iy_i + c_iy_{i+1} = d_i \tag {3-9}
$$

#### 边界条件确定解

第一类边界条件：
$$
y(a) = \alpha , \quad y(b) = \beta \quad a \lt b \tag {3-10}
$$
带入边界条件得：
$$
\begin{align}
	y_0 & = \alpha \\
	a_1y_0 + b_1y_1 + c_1y_2 & = d_1 \\
	& \vdots \\
	a_{n - 2}y_{n - 3} + b_{n - 2}y_{n-2} + c_{n-2}y_{n-1} & = d_{n-2} \\
	y_{n - 1} & = \beta \\
\end{align}
\tag {3-11}
$$
有矩阵：
$$
\begin{bmatrix}
	b_1 & c_1 & 0 & 0 & \cdots & 0 \\
    a_2 & b_2 & c_2 & 0 & \cdots & 0 \\
    0 & a_3 & b_3 & c_3 & \cdots & 0 \\
    \vdots & & \ddots & & \cdots & \vdots \\
    0 & \cdots & & a_{n-3} & b_{n-3} & c_{n-3} \\
    0 & \cdots & & 0 & a_{n-2} & b_{n-2} \\
\end{bmatrix}
\begin{bmatrix}
    y_1 \\
    y_2 \\
    \vdots \\
    y_{n-3} \\
    y_{n-2} \\
\end{bmatrix}
=
\begin{bmatrix}
    d_1 - a_1 \alpha \\
    d_2 \\
    d_3 \\
    \vdots \\
    d_{n-3} \\
    d_{n-2} - c_{n-2} \\
\end{bmatrix}
\tag {3-12}
$$
出现了三对角方程组，可以采用追赶法编程求解。Fortran程序我已经丢失了以后补上。
第二类边界条件
$$
y'(a) = \alpha ,\quad y'(b) = \beta \quad a \lt b \tag {3-13}
$$
有矩阵：
$$
\begin{bmatrix}
	-3 & 4 & - 1& 0 & \cdots & 0 \\
    0 & a_1 & b_1 & c_1 & \cdots & 0 \\
    \vdots & & \ddots & & \cdots & \vdots \\
    0 & \cdots & a_{n-2} & b_{n-2} & c_{n-2} & 0 \\
    0 & \cdots & & 1 & -4 & 3 \\
\end{bmatrix}
\begin{bmatrix}
    y_0 \\
    y_1 \\
    \vdots \\
    y_{n-2} \\
    y_{n-1} \\
\end{bmatrix}
=
\begin{bmatrix}
	2\alpha h \\
    d_1 - a_1 \alpha \\
    d_2 \\
    d_3 \\
    \vdots \\
    d_{n-2} \\
    2\beta h \\
\end{bmatrix}
\tag {3-14}
$$
第三类边界条件：
$$
\alpha(a) y(a) + \beta(a) y(a) = \gamma(a) ,\quad \alpha(b) y(b) + \beta(b) y(b) = \gamma(b) \quad a \lt b \tag {3-15}
$$
同理一样可得一nxn的三对角矩阵
