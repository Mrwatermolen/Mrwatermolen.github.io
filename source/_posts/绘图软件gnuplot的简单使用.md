---
title: 绘图软件gnuplot的简单使用
author: MH
output: pdf_document
tags:
  - gnuplot
  - math
categories:
  - Linux
date: 2020-11-18 00:00:00
---

## 挖个坑

由于实验数据绘图的需要，本来在树莓派上装了octave好像是缺了什么依赖跑不起来，最后换了gnuplot

<!-- more -->

## 1. 安装

 `sudo apt install gnuplot`

## 2. 简单使用

``` shell
gnuplot #进人gnuplot命令环境
>plot ［-pi:pi］ sin(x) #绘制二维图
＞splot [-1:1] [-1:1] x**2+y**2 #绘制三维图
#　设置范围
>set xrange[2:4]
# 命名
>set title "name"
>set xlabel "xname"
>set ylable "yname"
>replot　#重绘
```

在gnuplot命令环境下也能够运行终端命令不过要在前面加！
 `>!ls`

## 3. 绘制数据图

 `plot '<file_name>' {}`

``` gnuplot
>plot "Documents/1.dat"
>plot "Documents/1.dat" using 2:1 #使用第二列作x　第一列作y
>plot "Documents/1.dat" with line #将点连线
```

## 4. 数据拟合

以线性回归为列

 `>fit a*x+b "data.dat" via a,b`

![获得ａ，ｂ参数](http://mh.hicaiji.com/upload/2020/11/%E6%88%AA%E5%9B%BE_2020-11-19_08-36-04-6a951fbf21b040aa9aad73ae3acaded4-thumbnail.png)

 `>plot 2606.23*x-7.1706, "data.dat"`
