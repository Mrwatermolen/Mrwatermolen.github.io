---
title: 最长公共子序列
tags:
  - dynamicProgramming
categories:
  - Algorithm
date: 2021-04-18 13:40:39
mathjax: true
---


## 问题描述

1. 子序列：序列$X_n=<x_1,\dots ,x_n>,Z_k=<z_1,\dots ,z_k>$，当存在一个严格递增的X的下标序列$<i_1,\dots ,i_k>$，对所有$j=1,2,\dots,k$，满足$x_{i_j}=z_j$，称$Z_k$为$X_n$的一个子序列。
2. 公共子序列对序列X、Y，序列Z是他们的子序列，Z为X、Y的公共子序列。

那么给定两个序列X，Y，求其最长的公共子序列。

## 刻画最优解的结构特征

子问题对应序列前缀的最长公共子序列。

### 给出最长公共子序列的定理

对$X_m,Y_n$，最长公共子序列为$Z_k$

$$1.x_m=y_n\Rightarrow z_k=x_m=y_n,Z_{k-1}是X_{m-1},Y_{n-1}的最长公共子序列\\ 2.x_m\neq y_n,z_k\neq x_m \Rightarrow Z_k是X_{m-1},Y_n的最长公共子序列 \\ 3.x_m\neq y_n,z_k\neq y_n \Rightarrow Z_k是X_m,Y_{n-1}的最长公共子序列$$

可以用反证法证明结论的正确性，如1.若$Z_{k-1}$不是满足要求最长的公共子序列，那有另一个W序列为最长公共子序列，W中再加入一项就为$X_m,Y_n$的最长公共子序列，但不为$Z_{k-1}$，故矛盾。

## 递归定义最优解的值

最优解的值为最长公共子序列的长度：关键。

按照定理找出递推式子，定义$c[m,n]=Z_k.Length$ 有：

$$c[i,j]=
\begin{cases}
0,\quad i=j=0 \\
c[i-1,j-1]+1,\quad x_i=y_j,i\gt 0,j\gt 0 \\
max\lbrace c[i-1,j],c[i,j-1]\rbrace,\quad x_i\neq y_j,i\gt 0,j\gt 0
\end{cases}$$

反映出子问题的重叠性

## 计算最优解的值

```cpp
int LCSLength(string X, string Y)
{
    int **c = new int *[X.length() + 1];
    for (int i = 0; i <= X.length(); ++i)
        c[i] = new int[Y.length() + 1];
    for (int i = 0; i <= X.length(); ++i)
        c[i][0] = 0;
    for (int i = 0; i <= Y.length(); ++i)
        c[0][i] = 0;
    
    //关键
    for (int i = 1; i <= X.length(); ++i)
        for (int j = 1; j <= Y.length(); ++j)
            if (X[i - 1] == Y[j - 1])
                c[i][j] = c[i - 1][j - 1] + 1;
            else
                c[i][j] = max(c[i - 1][j], c[i][j - 1]);
    return c[X.length()][Y.length()];
}

int main()
{
    string X = "ABCBDAB", Y = "BDCABA";
    cout << LCSLength(X, Y) << endl;
    return 0;
}
```

空间复杂度：$O(nm)$，时间复杂度：$O(nm)$

考虑一个空间复杂度为：$min(n,m)$的算法，考虑到其实每次只使用了这一行和上一行的结果，考虑用两个数组存储这一行的结果和上一行的结果即可，数组长度应该为列的长度，即Y的长度。考虑到最小，可以考虑比较序列长度后，使得行的长度总是为最大。

![LongestCommonSubsequenceProblem(LCS)%204f2996c538244eb889418f9f5a8ef2f8/Screenshot_2021-04-14_at_12.57.53_PM.png](LongestCommonSubsequenceProblem(LCS)%204f2996c538244eb889418f9f5a8ef2f8/Screenshot_2021-04-14_at_12.57.53_PM.png)

![LongestCommonSubsequenceProblem(LCS)%204f2996c538244eb889418f9f5a8ef2f8.png](LongestCommonSubsequenceProblem(LCS)%204f2996c538244eb889418f9f5a8ef2f8.png)

$$cur[j]=
\begin{cases}
pre[j-1]+1,\quad x_i=y_j\\
max(pre[j]+cur[j-1]),\quad x_i\neq y_j
\end{cases}$$

```cpp
int LCSLength(string X, string Y)
{
    int minLen = min(X.length(), Y.length());
    if (minLen == X.length())
        swap(X, Y);
    int *cur = new int[minLen + 1], *pre = new int[minLen + 1];
    for (int i = 0; i <= minLen; ++i)
        cur[i] = 0, pre[i] = 0;
    for (int i = 1; i <= X.length(); ++i)
    {
        for (int j = 1; j <= Y.length(); ++j)
        {
            if (X[i - 1] == Y[j - 1])
                cur[j] = pre[j - 1] + 1;
            else
                cur[j] = max(pre[j], cur[j - 1]);
        }
        for (int k = 0; k <= minLen; ++k)
            pre[k] = cur[k];
    }
    return cur[minLen];
}
```

## 重构最优解

如果使用第一种方法，那么二维数组c中已经拥有能够重构最优解的信息了。如果使用了第二种方法，cur以及pre的信息还不足够重构最优解，需要再引入一个二维数组b，记录路线。

### 引入数组b

先看简单的。重构解可以考虑递归算法，从终点跑回原点出求出最大公共子序列，$b[i][j]=1$代表$X_{i-1}=Y_{j-1}$，即X，Y都包含的元素，则输出，但是注意的是应该先递归到底部在考虑输出，所以先继续递归。$b[i][j]=2$代表LCS在$X_{i-1},Y_{j}$即还要继续寻找。3同理。

```cpp
void PrintLCS(string A, int **b, int i, int j)
{
    if (!(i * j))
        return;

    if (b[i][j] == 1)
    {
        PrintLCS(A, b, i - 1, j - 1);
        cout << A[i - 1];//传入Y则输出A[j-1]
    }
    else if (b[i][j] == 2)
        PrintLCS(A, b, i - 1, j);
    else
        PrintLCS(A, b, i, j - 1);
}

int LCSLength(string X, string Y)
{
    int minLen = min(X.length(), Y.length());
    if (minLen == X.length())
        swap(X, Y);
    int *cur = new int[minLen + 1], *pre = new int[minLen + 1];
    int **b = new int *[X.length() + 1];
    for (int i = 0; i <= X.length(); ++i)
        b[i] = new int[Y.length() + 1];
    for (int i = 0; i <= minLen; ++i)
        cur[i] = 0, pre[i] = 0;

    for (int i = 1; i <= X.length(); ++i)
    {
        for (int j = 1; j <= Y.length(); ++j)
        {
            if (X[i - 1] == Y[j - 1])
            {
                cur[j] = pre[j - 1] + 1;
                b[i][j] = 1;
            }
            else if (pre[j] >= cur[j - 1])
            {
                cur[j] = pre[j];
                b[i][j] = 2;
            }
            else
            {
                cur[j] = cur[j - 1];
                b[i][j] = 3;
            }
        }
        for (int k = 0; k <= minLen; ++k)
            pre[k] = cur[k];
    }

    PrintLCS(X, b, X.length(), Y.length());//也可以传Y
    return cur[minLen];
}
```

### c[i][j]重构解

二维数组c中已经拥有能够重构最优解的信息了，有了上面的例子，其实就是用c去代替b的作用。改一下PrintLCS即可

```cpp
void PrintLCS(string A, int **b, int i, int j)
{
    if (!(i * j))
        return;

    if (b[i][j] == (b[i-1][j-1] + 1))
    {
        PrintLCS(A, b, i - 1, j - 1);
        cout << A[i - 1];
    }
    else if (b[i][j] == (b[i-1][j]))
        PrintLCS(A, b, i - 1, j);
    else
        PrintLCS(A, b, i, j - 1);
}
```

## 思考题

求一个n个数的序列的最长单调递增序列。
