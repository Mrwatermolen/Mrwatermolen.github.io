---
title: 矩阵链乘
tags:
  - dynamicProgramming
categories:
  - Algorithm
date: 2021-04-18 13:39:45
---


## 问题描述

矩阵链乘法：给定一个n个矩阵的序列(矩阵链)，：$A_1A_2\dots A_n \Leftrightarrow \lang A_1,A_2,\dots A_n\rang$ ,这里我们不讨论如何求解计算结果，默认采用$\Theta(n^3)$ 的计算方法。我们可以使用括号去表达计算次序，然后利用标准的矩阵相乘去运算。可以发现不同的加括号方式得到的结果是一样的，但复杂程度却是不一样的。

如$A_1A_2A_3$ 为例子：

$$A_1:a*b \quad A_2:b*c \quad A_3:c*d \\ (A_1A_2)A_3:abc+acd=ac(b+d) \\
A_1(A_2A_3):bcd+abd=bd(a+c)$$

那么我们引出定义**满足以下性质的矩阵乘积链称为fully parenthesized(完全括号化的)：单一矩阵，或者是两个完全括号化的矩阵乘积链的积，且已经外加括号。**

如$\lang A_1,A_2,A_3,A_4\rang$有5种完全括号化的矩阵乘积链：1234,1(23)4,12(34),(12)34,(12)(34) 那有没有递推公式???

引出矩阵链乘法问题：给定一个n个矩阵的序列(矩阵链)，矩阵规模$A_i:p_{i-1}*p_i$(满足行是上一个矩阵的列数)求完全括号化方案，使得乘积次数最少。

首先考虑穷举法，设P(n)为n个矩阵的链的完全括号化方案的个数，考虑我们一定会在某个k矩阵上吧矩阵链分割，于是我们有递推公式：

$$P(n)=
\begin{cases}
1,\quad n=1 \\ \sum_{k=1}^{n-1}P(k)P(n-k),\quad n\geq 2
\end{cases}$$

结果为$\Omega(2^n)$，要求用代入法证明，一会证，有点麻烦。

## 动态规划求解

一个指数型的结果是不能让人接受的，考虑动态规划

### 最优解的结构特征

考虑在第k个矩阵分解，得到$A_i\dots A_k \quad A_{k+1}\dots A_j$,为方便讨论简记为$A_{i\dots k},A_{k+1\dots j}$,容易想到如果我们先计算$A_{i\dots k},A_{k+1\dots j}$的最优解，再加上$A_{i\dots k},A_{k+1\dots j}$相乘所需要的次数，就应该是$A_{i\dots j}$一个最优解。这样的解一定是最优解吗？一定是的。看不懂书上的说明，有时间再啃。

### 一个递归的求解方案

记$A_{i\dots j}$的最优解的所要求的乘积次数为$m[i,j]$,于是乎有

$$m[i,j]=
\begin{cases}
0,\quad i=j \\ min_{i\leq k \lt j}\lbrace m[i,k]+m[k+1,j]+p_{i-1}p_kp_j \rbrace, \quad i \lt j
\end{cases}$$

为第四步构造最优解，我们还要使用s[i,j]保存分割位置k

### 计算最优解的值

如果单纯采用m[i,j]的递推公式来进行递归，那也是指数级别的复杂度。我们可以考虑递归时记住已经计算过的结果，为带备忘的自顶向下的方法。

采用带备忘的自顶向下的方法

采用自底向上的方法,首先分析一下：最小问题应该是m[i,i]，而m[i,i]=0，下一个子问题应该是长度为2的m[i,j]，即$j-i=2$，依此类推，一直求到长度为n。于是应该有三个变化的量l、i、k，j可由i，l确定。不考虑i=j。

$$l:leght:2 \rightarrow n \quad i:1\rightarrow n-l+1\quad j = i+l-1 \quad k:i \rightarrow j-1$$

```cpp
void BottomUpMCO(int *p, int **m, int n)
{
    for (int i = 1; i <= n; ++i)
        m[i][i] = 0;
    for (int l = 2; l <= n; ++l)
    {
        for (int i = 1; i <= n - l + 1; ++i)
        {
            int j = l + i - 1;
            int q = INT32_MAX;
            for (int k = i; k < j; ++k)
                q = min(q, m[i][k] + m[k + 1][i] + p[i - 1] * p[k] * p[j]);
            m[i][j] = q;
        }
    }
}
```

### 构造最优解

s[i,j]保存分割位置k

```cpp
void ExtendedBottomUpMCO(int *p, int **m, int **s, int n)
{
    //p.leght=n+1 
    for (int i = 1; i <= n; ++i)
        m[i][i] = 0;
    for (int l = 2; l <= n; ++l)
    {
        for (int i = 1; i <= n - l + 1; ++i)
        {
            int j = l + i - 1;
            m[i][j] = INT32_MAX;
            for (int k = i; k < j; ++k)
            {
                int q = m[i][k] + m[k + 1][i] + p[i - 1] * p[k] * p[j];
                if (q < m[i][j])
                {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }
}

void PrintOptimalParens(int **s, int i, int j)
{
    if (i == j)
        cout << i;
    else
    {
        cout<<"(";
        PrintOptionParens(s,i,s[i][j]);
        PrintOptionParens(s,s[i][j]+1,j);
        cout<<")";
    }
}

int main()
{
    int p[7]={30,35,15,5,10,20,25}, n = 6;
    int **s = new int*[n+1], **m = new int*[n+1];
    for(int i = 0; i <= n; ++i)
    {
        s[i] = new int[n+1];
        m[i] = new int[n+1];
    }
    ExtendedBottomUpMCO(p,m,s,n);
    PrintOptimalParens(s,1,n);
    cout<<'\n'<<m[1][n];
}
```

解释一下PrintOptimalParens，$s[i][j]$意义：矩阵链$A_{i\dots j}$最优括号分割点应该在第$s[i][j]$个矩阵，矩阵链就被分成了两个部分，只有当i=j时，输出矩阵，其他时候，矩阵链还能被分割，就继续递归分割。括号的输出：当矩阵链$A_{i\dots j}$还能再分割时，说明其前面必然有一个$"("$，递归完成后也要把$")"$补上。
