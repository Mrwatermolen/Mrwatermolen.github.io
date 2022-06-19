---
title: 色散介质FDTD
tags:
---

## 基本模型

设介质的磁本构关系与频率无关，而介电系数与频率有关
$$
B = \mu H \\
D(\omega) = \epsilon (\omega) E(\omega) = \epsilon_0  [\epsilon_{\infty} + \chi (\omega)] E(\omega)
$$
从而
$$
\left[ \nabla \times H \right] ^{n+1/2} = \frac{D^{n+1} - D^n}{\Delta t} + \sigma \frac{E^{n+1} + E^n}{2} \\
\left[ \nabla \times E \right] ^{n+1} = -\mu \frac{H^{n+1/2} - H^{n-1/2}}{\Delta t} - \sigma_m \frac{H^{n+1/2} + H^{n-1/2}}{2}
$$
可由$E \rightarrow H$，但要得出$E$，先$H \rightarrow D$，再$D \rightarrow E$。（默认$\sigma = 0$吗？）
但我们可见，本构关系为频域上的，要得出时域上的，那么频域上的相乘就为时域上的卷积。

### 色散介质常用的几种频域模型

1. Debye模型
考虑极化过程中的弛豫现象
$$
\epsilon (\omega) = \epsilon_0 \epsilon_{\infty} + \epsilon_0 \frac{\epsilon_s - \epsilon_{\infty}}{1 + \jmath \omega \tau_0}
$$
由傅里叶变换对可知，在时域上为：
$$
\epsilon (t) = \epsilon_0 \epsilon_{\infty} \delta(t) + \frac{\epsilon_0 (\epsilon_s - \epsilon_{\infty})}{\tau_0} \exp{(-\frac{t}{\tau_0})} U(t)
$$
$\delta(t)$为冲激函数，$U(t)$为阶梯函数，对于$\chi$可有：
$$
\chi (t) = v_c \Delta \epsilon \exp{(-v_c t)} U(t) \quad \Delta \epsilon = (\epsilon_s - \epsilon_{\infty}) \quad v_c = 1/\tau_0
$$
2. Drude模型
$$
\epsilon(\omega) = \epsilon_0 \epsilon_{\infty} - \epsilon_0 \frac{\omega_p^2}{\omega(\omega - \jmath v_c)}
$$
时域上，对于$\chi$，可有：
$$
\chi (t) = \frac{\omega_p^2}{v_c} \left[1- exp{(-v_c t)} \right] U(t)
$$
3. Lorentz模型
$$
\epsilon(\omega) = \epsilon_0 \epsilon_{\infty} + \epsilon_0 \frac{(\epsilon_s - \epsilon_{\infty}) \omega^2}{\omega_0^2 - \omega^2 + 2\jmath \omega v_c}
$$
时域上，对于$\chi$，可有：
$$
\chi (t) = \frac{\omega_0^2 \Delta \epsilon}{\sqrt{\omega_0^2 - v_c^2}} exp{(-v_c t)} \sin{\left(t \sqrt{\omega_0^2 - v_c^2} \right)} U(t)
$$

## 色散介质RC-FDTD

$$
E^{n+1} = \frac{\epsilon_{\infty}}{\epsilon_{\infty} + \chi_0}E^n + \frac{1}{\epsilon_{\infty} + \chi_0} \Psi^n + \frac{1}{\epsilon_0 (\epsilon_{\infty} + \chi_0)}(D^{n+1} - D^n)
$$
书上给还给了一个化简式子，应该是默认$\sigma = 0$。
$$
E^{n+1} = \frac{\epsilon_{\infty}}{\epsilon_{\infty} + \chi_0}E^n + \frac{1}{\epsilon_{\infty} + \chi_0} \Psi^n + \frac{1}{\epsilon_0 (\epsilon_{\infty} + \chi_0)}(\nabla \times H)^{n+1/2}
$$
其中
$$
\Psi = \sum_{m=0}^{n-1}E^{n-m} \Delta \chi_m \\
\chi_m = \int_{m\Delta t}^{(m+1)\Delta t} \chi(\tau) d\tau
$$
不同频域模型，对应不同$\Psi$

### Debye模型

$$
\Psi^n = E^n \Delta \chi_0 + \exp(-v_c \Delta)\Psi^{n-1} \quad n \ge 2 \\
\Delta \chi_0 = \Delta \epsilon \left[ 1 - \exp{(-v_c \Delta t)} \right]^2 \\
$$
其中$v_c$为碰撞频率

### Drude模型

$$
\begin{aligned}
    \Psi^n = E^n \Delta \chi_0 + \exp(-v_c \Delta)\Psi^{n-1} \quad n \ge 2 \\
    \Delta \chi_0 = - \frac{\omega_p^2}{v_c^2} \left[ 1 - \exp{(-v_c \Delta t)} \right]^2 \\
\end{aligned}
$$

### Lorentz模型

以后补

### 改进RC-FDTD之分段线性循环卷积

## 色散介质Z变换

### Debye介质Z变换

由$D(\omega) = \epsilon (\omega) E(\omega)$，对Debye介质而言
\[\epsilon (\omega) = \epsilon_0 \epsilon_{\infty} + \epsilon_0 \frac{\epsilon_s - \epsilon_{\infty}}{1 + \jmath \omega \tau_0} + \epsilon_0 \frac{\sigma}{\jmath \omega \epsilon_0}\]
这里加入了损耗项$\frac{\sigma}{\jmath \omega \epsilon_0}$。可记$\Delta \epsilon = \epsilon_s - \epsilon_{\infty}$
\[
    \begin{aligned}
        I(\omega) = \frac{\sigma}{\jmath \omega \epsilon_0} E(\omega) \\
        S(\omega) = \frac{\Delta \epsilon}{1 + \jmath \omega \tau_0} E(\omega) \\
    \end{aligned}
\]
有：
\[D(\omega) = \epsilon_0 [\epsilon_{\infty}E(\omega) + S(\omega) + I(\omega)]\]
过渡至z域：
\[
    \begin{aligned}
        I(z) &= \frac{\sigma \cdot \frac{\Delta t}{\epsilon_0}}{1-z^{-1}} E(z) \\
        S(z) &= \frac{\Delta \epsilon \cdot \frac{\Delta t}{\tau_0}}{1 - z^{-1} \exp{(-\frac{\Delta t}{\tau_0})}} E(z) \\
    \end{aligned}
\]
代入，解之得：
\[
    \begin{aligned}
        I(z) = z^{-1} I(z) +  \frac{\sigma \Delta t}{\epsilon_0} E(z) \\
        S(z) =  \exp{(-\frac{\Delta t}{\tau_0})}z^{-1} S(z) + \frac{\Delta \epsilon \Delta t}{\tau_0} E(z) \\
        E(z) = \frac{\frac{D(z)}{\epsilon_0} - z^{-1}I(z) - \exp{(-\frac{\Delta t}{\tau_0})}z^{-1} S(z)}{\epsilon_{\infty} + \frac{\sigma \Delta t}{\epsilon_0} + \frac{\Delta \epsilon \Delta t}{\tau_0}}
    \end{aligned}
\]
对应离散时域：
\[
    \begin{aligned}
        I^n = I^{n-1} +  \frac{\sigma \Delta t}{\epsilon_0} E^n \\
        S^n =  \exp{(-\frac{\Delta t}{\tau_0})}S^{n-1} + \frac{\Delta \epsilon \Delta t}{\tau_0} E^n\\
        E^n = \frac{\frac{D^n}{\epsilon_0} - I^{n-1} - \exp{(-\frac{\Delta t}{\tau_0})}S^{n-1}}{\epsilon_{\infty} + \frac{\sigma \Delta t}{\epsilon_0} + \frac{\Delta \epsilon \Delta t}{\tau_0}}
    \end{aligned}
\]
若对二维TM波问题，考虑无耗介质（这里不对）：
\[
    \begin{aligned}
        D_z^{n+1}(i, j) = D^{n}(i, j) + \frac{\Delta t}{\Delta x} \left[ H_y^{n+1/2}(i+1/2,j) - H_y^{n+1/2}(i-1/2,j) \right] - \frac{\Delta t}{\Delta y} [H_x^{n+1/2}] \\
        H_x^{n+1/2}(i, j+1/2) =  H_x^{n-1/2}(i, j+1/2) - \frac{\Delta t \mu}{\Delta y} [E_z^{n}] \\
        H_y^{n+1/2}(i+1/2, j) =  H_y^{n-1/2}(i+1/2, j) + \frac{\Delta t \mu}{\Delta x} [E_z^{n}] \\
    \end{aligned}
\]
由D、I、S推出E，由E推出I、S、H，由H推出D。
算例
\[
\begin{cases}
\Psi^n = E^n \Delta \chi_0 + \exp(-v_c \Delta)\Psi^{n-1} \quad n \ge 2 \\
\Delta \chi_0 = \Delta \epsilon \left[ 1 - \exp{(-v_c \Delta t)} \right]^2 \\
\end{cases}
\]
设定$\epsilon_{\infty} = 7$，$\Delta \epsilon = 3$， $\tau_0 = 7.0 \times 10^{-10}s$
激励源位于中心，使用高斯脉冲源。

### Drude介质Z变换

对于Drude模型和非磁化等离子体：
\[\epsilon (\omega) = \epsilon_0 \epsilon_{\infty} - \epsilon_0 \frac{\omega_p^2}{\omega(\omega - \jmath v_c)} + \epsilon_0 \frac{\sigma}{\jmath \omega \epsilon_0}\]
化简：
\[\epsilon (\omega) = \epsilon_0 \left[\epsilon_{\infty} + \frac{1}{\jmath \omega \epsilon_0}\left(\sigma + \frac{\epsilon_0 \omega_p^2}{v_c}\right) - \frac{\omega_p^2 / v_c}{v_c + \jmath \omega}\right]\]
我们记：
\[
    \begin{aligned}
        I(\omega) = \frac{\left(\sigma + \frac{\epsilon_0 \omega_p^2}{v_c}\right)}{\jmath \omega \epsilon_0} E(\omega) \\
        S(\omega) = -\frac{\omega_p^2 / v_c}{v_c + \jmath \omega} E(\omega) \\
    \end{aligned}
\]
有：
\[D(\omega) = \epsilon_0 [\epsilon_{\infty}E(\omega) + S(\omega) + I(\omega)]\]
过渡至z域：
\[
    \begin{aligned}
        I(z) &= \frac{\left(\sigma + \frac{\epsilon_0 \omega_p^2}{v_c}\right) \frac{\Delta t}{\epsilon_0}}{1-z^{-1}} E(z) \\
        S(z) &= -\frac{\omega_p^2 \Delta t / v_c}{1 - z^{-1} \exp{(-v_c \Delta t)}} E(z) \\
    \end{aligned}
\]
代入，解之得：
\[
    \begin{aligned}
        I(z) = z^{-1} I(z) +  \left(\sigma + \frac{\epsilon_0 \omega_p^2}{v_c}\right) \frac{\Delta t}{\epsilon_0} E(z) \\
        S(z) =  \exp{(-v_c \Delta t)}z^{-1} S(z) - \frac{\omega_p^2 \Delta t}{v_c} E(z) \\
        E(z) = \frac{D(z) - \epsilon_0 z^{-1}I(z) - \epsilon_0 \exp{(-v_c \Delta t)}z^{-1} S(z)}{\epsilon_0 \epsilon_{\infty} + \sigma \Delta t}
    \end{aligned}
\]
对应离散时域：
\[
    \begin{aligned}
        I^n = I^{n-1} +  \left(\sigma + \frac{\epsilon_0 \omega_p^2}{v_c}\right) \frac{\Delta t}{\epsilon_0} E^n \\
        S^n =  \exp{(-v_c \Delta t)}S^{n-1} - \frac{\omega_p^2 \Delta t}{v_c} E^n\\
        E^n = \frac{D^n -  \epsilon_0 I^{n-1} - \epsilon_0 \exp{(-v_c \Delta t)}S^{n-1}}{\epsilon_0 \epsilon_{\infty} + \sigma \Delta t}
    \end{aligned}
\]
若对二维TM波问题，考虑无耗介质：K
\[
    \begin{aligned}
        D_z^{n+1}(i, j) = D^{n}(i, j) + \frac{\Delta t}{\Delta x} \left[ H_y^{n+1/2}(i+1/2,j) - H_y^{n+1/2}(i-1/2,j) \right] - \frac{\Delta t}{\Delta y} [H_x^{n+1/2}] \\
        H_x^{n+1/2}(i, j+1/2) =  H_x^{n-1/2}(i, j+1/2) - \frac{\Delta t \mu}{\Delta y} [E_z^{n}] \\
        H_y^{n+1/2}(i+1/2, j) =  H_y^{n-1/2}(i+1/2, j) + \frac{\Delta t \mu}{\Delta x} [E_z^{n}] \\
    \end{aligned}
\]
算例
设定$\epsilon_{\infty} = 1$，$f_p = 2.87 GHz$，$\omega_p = 2\pi f_p$，$v_c = 200 GHz$
激励源位于中心，使用高斯脉冲源。

## SARC-FDTD

冲激函数$h(t) = \sum_{q=1}^{Q}{H_q\exp{(-\alpha_q t)}U(t)}$，其中$Q$为极点个数，$\alpha_q$为极点

## 色散介质的吸收边界

### Mur吸收边界

一阶形式 左边界为例子：
$$
f^{n+1}(i) =2f^n(i+1) + (g_1 + g_2) \left[ f^{n+1}(i + 1) - f^(i) - f^n(i+2) + f^{n-1}(i + 1)\right] - \\
g_1 g_2 \left[ f^{n+1}(i + 2) - f^(i + 1) + f^{n-1}(i)\right] - f^{n - 1}(i + 2)
$$
$$
g_1 = \frac{v_1 \Delta t - \delta}{v_1 \Delta t + \delta} \\ g_2 = \frac{v_2 \Delta t - \delta}{v_2 \Delta t + \delta}
$$
