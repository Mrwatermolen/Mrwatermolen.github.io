---
title: Xtensor中的xarray和xtensor效率比较
tags:
---

> [Xtensor](https://xtensor.readthedocs.io/en/latest/) Multi-dimensional arrays with broadcasting and lazy computing.

本文并非基准测试。仅仅记录时间变化。

<!-- more -->

## Test Case

浮点数精度为双浮点。

### FDTD球体散射

[例子](https://blog.franzero.net/2024/04/03/FDTD-RCS%E4%B8%80%E4%B8%AA%E6%8F%90%E7%A4%BA/)

整个网格空间大小为63x63x63=250,047。计算时间步为2000。

| Method | Time (ms) |
| --- | --- |
| xarray | 16123 |
| xtensor | 7510 |

提升了2倍之多。

### 天线辐射

网格空间维39x136x136=721,344。计算时间步为7000。

| Method | Time (ms) |
| --- | --- |
| xarray | 235986 |
| xtensor | 72808 |

三倍。

### 所有例子的测试

% table
| Method | User Time (s) | Total Time (min) |
| --- | --- | --- |
| xarray | 615.45 | 10:03.37 |
| xtensor | 214.57 | 3:18.97 |

几乎是3倍的提升。
