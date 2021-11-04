---
title: 钢条切割
tags:
  - dynamicProgramming
categories:
  - Algorithm
date: 2021-04-18 13:40:10
mathjax: true
---


## 钢条切割问题

### 问题描述

一段长度为i的钢条的价格$p_i$，那么给定一段长为n的钢条和价格表p，求最佳的切割方案使得出售价格最高。

### 递归求解

长为n的钢条，每段长度为1的一端总有两种方案，切与不切，则我们有$2^{n-1}$种切割方案。

切割方案，以及售价$r_n$可表示为

$$n=i_1+i_2+\dots +i_k \\ r_n = \sum_{j=1}^{k}p_{i_j} \\ \rightarrow \; r_n = max(p_n,r_1+r_{n-1},r_2+r_{n-2},\dots,r_{n-1}+r_{1})$$

售价$r_n$就表示为长度为n的钢铁的最高售价，其等价于$r_n = max_{1\le i \le n}(p_i+r_{n-i})$

由此我们得到了一个递归式，注意到$r_0 = 0$。

```cpp
int CutRod(int *p, int n)
{
    //递归求解
    if (n == 0)
        return 0;
    int q = INT32_MIN;
    for (int i = 1; i <= n; ++i)
    {
        q = max(q, p[i] + CutRod(p, n - i));
    }
    return q;
}
```

#### 分析

效率很差，因为，CutRod会反复求解已经求解过的值，如i=2时，会求解：i=1,i=0，然后比较后求得i=2的解，但并未保存，而后i=3,又要重新求解i=2,i=1,i=0,再比较后求得i=3。其时间复杂度递归式：

$$T(n)=1+\sum_{i=0}^{n-1}T(i) \\ T(0)=1 \\ \because T(n-1)=1+\sum_{i=0}^{n-2}T(i) \\ \therefore T(n)-T(n-1)=T(n-1) \Rightarrow T(n)=2^n$$

### 动态规划求解

#### 带备忘的自顶向下

递归求解之所以效率很慢，因为没有保存已经求解过的子问题的解，为改进，我们就在求解前先检查是否已经求解过解，那么我们就需要额外的空间去储存这些解。

```cpp
int MemoizedCutRod(int *p, int n)
{
    int *r = new int[n+1];//从0-n
    for (int i = 0; i <= n; ++i)
        r[i] = INT32_MIN;
    return MemoizedCutRodAux(p, r, n);
}

int MemoizedCutRodAux(int *p, int *r, int n)
{
    if (r[n] >= 0)
        return r[n]; //考虑到收益总是非负值,r[n]ge0说明r[n]已经被记录了直接返回
    int q = 0;
    if (n > 0)
    {
        for (int i = 1; i <= n; ++i)
        {
            q = max(q, p[i] + MemoizedCutRodAux(p, r, n - i));
        }
    }
    r[n] = q;
    return q;
}
```

#### 自底向上法

我们先求解最小的问题，再一步步往上求解更大的问题。$r_n = max_{1\le i \le n}(p_i+r_{n-i})$

```cpp

int BottomUpCutRod(int *p, int n)
{
    if (n <= 0)
        return 0;
    int *r = new int[n+1];
    r[0] = 0;//n=0,售价为0
    for (int i = 1; i <= n; ++i)
    {
        int q = INT32_MIN;
        for (int j = 1; j <= i; ++j)
            q = max(q, p[j] + r[i - j]);
        r[i] = q;
    }
    return r[n];
} 
```

这两种方法的时间复杂度都为$\Theta (n^2)$

### 重规划

上面的动态规格只给出了最高售价，未给出切割方案。为了得到切割方案，我们应该去存储得到最高售价时的切割方案

先考虑自底向上的方法

s的确定应该实在q改变时，只要修改一下 `q = max(q, p[j] + r[i - j]);` 即可

```cpp
int ExtendedBottomUpCutRod(int *p, int n)
{
    if (n <= 0)
        return 0;
    int *r = new int[n + 1];
    int *s = new int[n + 1];
    r[0] = 0, s[0] = 0;
    for (int i = 1; i <= n; ++i)
    {
        int q = INT32_MIN;
        for (int j = 1; j <= i; ++j)
        {
            if (q < p[j] + r[i - j])
            {
                q = p[j] + r[i - j];
                s[i] = j;
            }
        }
        r[i] = q;
    }
    cout << "i:     ";
    for (int i = 1; i <= 10; ++i)
    {
        cout << i << '\t';
    }
    cout << '\n'
         << "r[i]:  ";
    for (int i = 1; i <= 10; ++i)
    {
        cout << r[i] << '\t';
    }
    cout << '\n'
         << "s[i]:  ";
    for (int i = 1; i <= 10; ++i)
    {
        cout << s[i] << '\t';
    }
    cout << '\n';
    return r[n];
}
```

考虑自顶向下的方法，也是找到q改变的时候记录一下

```cpp
int MemoizedCutRodAux(int *p, int *r, int *s, int n)
{
    if (r[n] >= 0)
        return r[n]; //考虑到收益总是非负值,r[n]ge0说明r[n]已经被记录了直接返回
    int q = 0;
    if (n > 0)
    {
        for (int i = 1; i <= n; ++i)
        {
            int temp = MemoizedCutRodAux(p, r, s, n - i);
            if (q < p[i] + temp)
            {
                q = p[i] + temp;
                s[n] = i;
            }
        }
    }
    r[n] = q;
    return q;
}
```
