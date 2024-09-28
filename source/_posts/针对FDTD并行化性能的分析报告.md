---
title: 针对FDTD并行化性能的分析报告
tags:
  - 并行计算
  - 性能分析
  - benchmark
categories:
  - FDTD
date: 2024-09-28 17:15:39
---

计算算例为：

```cpp
  auto domain{std::make_shared<xfdtd::Object>(
      "domain",
      std::make_unique<xfdtd::Cube>(xfdtd::Vector{-0.175, -0.175, -0.175},
                                    xfdtd::Vector{0.35, 0.35, 0.35}),
      xfdtd::Material::createAir())};
```

<!-- more -->

测试平台

```plain
                          ./+o+-       franzero@franzero-MS-7C94
                  yyyyy- -yyyyyy+      OS: Ubuntu 24.04 noble
               ://+//////-yyyyyyo      Kernel: x86_64 Linux 6.8.0-45-generic
           .++ .:/++++++/-.+sss/`      Uptime: 6h 29m
         .:++o:  /++++++++/:--:/-      Packages: 2282
        o:+o+:++.`..```.-/oo+++++/     Shell: zsh 5.9
       .:+o:+o/.          `+sssoo+/    Disk: 616G / 6.3T (11%)
  .++/+:+oo+o:`             /sssooo.   CPU: AMD Ryzen 5 5600X 6-Core @ 12x 3.7GHz
 /+++//+:`oo+o               /::--:.   RAM: 3630MiB / 15909MiB
 \+/+o+++`o++o               ++////.   
  .++.o+++oo+:`             /dddhhh.  
       .+.o+oo:.          `oddhhhh+   
        \+.++o+o``-````.:ohdhhhhh+    
         `:o+++ `ohhhhhhhhyo++os:     
           .o:`.syhhhhhhh/.oo++o`     
               /osyyyyyyo++ooo+++/    
                   ````` +oo+++o\:    
                          `oo++. 

Architecture:             x86_64
  CPU op-mode(s):         32-bit, 64-bit
  Address sizes:          48 bits physical, 48 bits virtual
  Byte Order:             Little Endian
CPU(s):                   12
  On-line CPU(s) list:    0-11
Vendor ID:                AuthenticAMD
  Model name:             AMD Ryzen 5 5600X 6-Core Processor
    CPU family:           25
    Model:                33
    Thread(s) per core:   2
    Core(s) per socket:   6
    Socket(s):            1
    Stepping:             0
    Frequency boost:      enabled
    CPU(s) scaling MHz:   57%
    CPU max MHz:          4650.2920
    CPU min MHz:          2200.0000
```

Stat报告：`sudo perf stat -e task-clock,cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults,minor-faults,major-faultscpu-migrations,context-switche`

## 1. 无PML边界时

整个计算域大小为70x70x70。时间步长为1200

串行命令：`./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1':

          4,676.22 msec task-clock                       #    0.942 CPUs utilized             
    20,922,169,997      cycles                           #    4.474 GHz                         (83.38%)
    82,578,031,208      instructions                     #    3.95  insn per cycle              (83.36%)
     4,229,791,865      cache-references                 #  904.532 M/sec                       (83.26%)
       102,750,093      cache-misses                     #    2.43% of all cache refs           (83.22%)
     2,996,821,632      branches                         #  640.864 M/sec                       (83.40%)
        14,714,284      branch-misses                    #    0.49% of all branches             (83.38%)
            40,282      page-faults                      #    8.614 K/sec                     
            40,279      minor-faults                     #    8.614 K/sec                     
                 3      major-faults                     #    0.642 /sec                      
                51      cpu-migrations                   #   10.804 /sec                      
             3,125      context-switches                 #  661.987 /sec                      

       4.964122624 seconds time elapsed

       4.520302000 seconds user
       0.099006000 seconds sys
```

### 多线程性能分析

XFDTD已经将线程绑定到CPU核心，0号线程绑定到0号核心，1号线程绑定到1号核心。

2x1x1: `./build/Release-x64/bin/benchmark -t 1200 -t_c 2 1 1`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 2 1 1':

          5,010.83 msec task-clock                       #    1.676 CPUs utilized             
    22,380,340,770      cycles                           #    4.466 GHz                         (83.36%)
    82,709,456,232      instructions                     #    3.70  insn per cycle              (83.19%)
     4,270,380,181      cache-references                 #  852.231 M/sec                       (83.29%)
       108,694,744      cache-misses                     #    2.55% of all cache refs           (83.41%)
     3,032,116,421      branches                         #  605.113 M/sec                       (83.29%)
        19,110,567      branch-misses                    #    0.63% of all branches             (83.46%)
            40,303      page-faults                      #    8.043 K/sec                     
            40,300      minor-faults                     #    8.043 K/sec                     
                 3      major-faults                     #    0.599 /sec                      
                63      cpu-migrations                   #   12.451 /sec                      
             9,973      context-switches                 #    1.971 K/sec                     

       2.989421301 seconds time elapsed

       4.741480000 seconds user
       0.201530000 seconds sys
```

加速比为：4.96/2.99 = 1.66

多线程比单线程快了：（4.96-2.99）/4.96 = 39.72%

多线程的CPU利用率为：167.6%

在Stat报告中，2x1x1的分支预测错误率和缓存未命中率获得了与串行执行差不多的结果。加速比没有达到2，可能是CPU利用率不够高，也许是因为过多的上下文切换。

1x2x1: `./build/Release-x64/bin/benchmark -t 1200 -t_c 1 2 1`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 1 2 1':

          5,252.76 msec task-clock                       #    1.687 CPUs utilized             
    23,275,116,991      cycles                           #    4.431 GHz                         (83.29%)
    82,704,202,147      instructions                     #    3.55  insn per cycle              (83.35%)
     4,554,703,918      cache-references                 #  867.107 M/sec                       (83.47%)
       258,114,565      cache-misses                     #    5.67% of all cache refs           (83.11%)
     3,034,010,364      branches                         #  577.603 M/sec                       (83.33%)
        19,516,081      branch-misses                    #    0.64% of all branches             (83.47%)
            40,297      page-faults                      #    7.672 K/sec                     
            40,294      minor-faults                     #    7.671 K/sec                     
                 3      major-faults                     #    0.571 /sec                      
                64      cpu-migrations                   #   12.297 /sec                      
             8,633      context-switches                 #    1.659 K/sec                     

       3.113548404 seconds time elapsed

       5.030267000 seconds user
       0.153733000 seconds sys
```

加速比为：4.96/3.11 = 1.59

多线程比单线程快了：（4.96-3.11）/4.96 = 37.30%

多线程的CPU利用率为：168.7%

1x2x1相比于2x1x1，在Stat报告中，缓存的未命中率有所增加，几乎是2x1x1或者串行的两倍。导致在CPU利用率相同与2x1x1差不多的情况下，加速效果没有2x1x1好。

1x1x2: `./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 2`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 2':

          7,140.03 msec task-clock                       #    1.774 CPUs utilized             
    32,256,025,684      cycles                           #    4.518 GHz                         (83.31%)
    83,803,187,755      instructions                     #    2.60  insn per cycle              (83.26%)
     8,364,234,889      cache-references                 #    1.171 G/sec                       (83.36%)
       311,977,954      cache-misses                     #    3.73% of all cache refs           (83.37%)
     3,089,285,740      branches                         #  432.671 M/sec                       (83.31%)
        18,422,341      branch-misses                    #    0.60% of all branches             (83.38%)
            40,290      page-faults                      #    5.643 K/sec                     
            40,287      minor-faults                     #    5.642 K/sec                     
                 3      major-faults                     #    0.420 /sec                      
                56      cpu-migrations                   #    7.778 /sec                      
             5,309      context-switches                 #  737.412 /sec                      

       4.024689262 seconds time elapsed

       6.935281000 seconds user
       0.136749000 seconds sys
```

加速比为：4.96/4.02 = 1.23

多线程比单线程快了：（4.96-4.02）/4.96 = 18.95%

多线程的CPU利用率为：177.4%

1x1x2的性能几乎是最差的，最高的CPU利用率，却获得了最差的加速比。从报告中可以看到，缓存未命中率是虽然为3.73%，但是缓存引用次数是最高的，达到了80亿次，是其他的两倍。

原因： 3维数组的存储是行优先，索引按x,y,z顺序获取，即在z方向上是连续的。1x1x2的划分，切断了z方向上的连续性，缓存的引用次数增加。

在FDTD程序中占用大量时间的是Update函数，里面有一个三重for循环，按x,y,z的顺序遍历

```cpp
for (int x = is; x < ie; ++x) {
  for (int y = js; y < je; ++y) {
    for (int z = ks; z < ke; ++z) {
      // ...
    }
  }
}
```

某次循环中，加载到缓存的数据为`[x_i, y_i, z_i], [x_i, y_i, z_i+1], [x_i, y_i, z_i+2]`，在1x1x2的划分下，访问到`[x_i, y_i, z_i+1]`时，z方向上的循化结束，下一次访问`[x_i, y_i+1, z_i]`时，z方向上的数据不在缓存中，需要重新加载。1x1x2划分会多次出现这样的情形，大约会是`nx*ny`次。

> PS:在FDTD中多重for循环的分支预测错误却不是主要的性能瓶颈。

### 多进程性能分析

2x1x1: `mpiexec --allow-run-as-root --bind-to core  -n 2  ./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1`

报告如下

```plain
 Performance counter stats for 'mpiexec --allow-run-as-root --bind-to core -n 2 ./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1':

          5,260.68 msec task-clock                       #    1.797 CPUs utilized             
    23,443,996,940      cycles                           #    4.456 GHz                         (83.07%)
    86,076,787,999      instructions                     #    3.67  insn per cycle              (83.40%)
     4,404,421,901      cache-references                 #  837.235 M/sec                       (83.42%)
       112,032,269      cache-misses                     #    2.54% of all cache refs           (83.25%)
     3,371,057,675      branches                         #  640.803 M/sec                       (83.43%)
        21,297,282      branch-misses                    #    0.63% of all branches             (83.44%)
            44,539      page-faults                      #    8.466 K/sec                     
            44,535      minor-faults                     #    8.466 K/sec                     
                 4      major-faults                     #    0.760 /sec                      
               136      cpu-migrations                   #   25.852 /sec                      
             6,136      context-switches                 #    1.166 K/sec                     

       2.926716656 seconds time elapsed

       5.015942000 seconds user
       0.233263000 seconds sys
```

1x1x2: `mpiexec --allow-run-as-root --bind-to core  -n 2  ./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1`

报告如下
```plain
 Performance counter stats for 'mpiexec --allow-run-as-root --bind-to core -n 2 ./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 1':

          5,401.87 msec task-clock                       #    1.802 CPUs utilized             
    24,066,472,199      cycles                           #    4.455 GHz                         (83.29%)
    88,250,212,639      instructions                     #    3.67  insn per cycle              (83.34%)
     4,487,750,246      cache-references                 #  830.778 M/sec                       (83.39%)
       145,999,530      cache-misses                     #    3.25% of all cache refs           (83.28%)
     3,748,377,075      branches                         #  693.904 M/sec                       (83.40%)
        20,100,212      branch-misses                    #    0.54% of all branches             (83.30%)
            44,628      page-faults                      #    8.262 K/sec                     
            44,624      minor-faults                     #    8.261 K/sec                     
                 4      major-faults                     #    0.740 /sec                      
               122      cpu-migrations                   #   22.585 /sec                      
             6,166      context-switches                 #    1.141 K/sec                     

       2.997049471 seconds time elapsed

       5.182734000 seconds user
       0.208839000 seconds sys
```

不受空间划分影响，多进程的性能表现与多线程的性能表现差不多。

### 总结

多线程在最佳情况下和多进程的性能表现差不多。

## 2. 有PML边界时

整个计算域大小为86x86x86。时间步长为1200。PML边界层数为8。

串行命令：`./build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 1`

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 1':

         12,992.83 msec task-clock                       #    0.977 CPUs utilized             
    58,734,301,508      cycles                           #    4.521 GHz                         (83.36%)
   203,700,658,783      instructions                     #    3.47  insn per cycle              (83.29%)
     9,971,769,749      cache-references                 #  767.483 M/sec                       (83.34%)
       525,622,296      cache-misses                     #    5.27% of all cache refs           (83.35%)
     7,340,039,091      branches                         #  564.930 M/sec                       (83.35%)
        31,616,197      branch-misses                    #    0.43% of all branches             (83.30%)
            76,039      page-faults                      #    5.852 K/sec                     
            76,036      minor-faults                     #    5.852 K/sec                     
                 3      major-faults                     #    0.231 /sec                      
                58      cpu-migrations                   #    4.464 /sec                      
             6,443      context-switches                 #  495.889 /sec                      

      13.294737831 seconds time elapsed

      12.758390000 seconds user
       0.164669000 seconds sys
```

### 多线程性能分析

XFDTD已经将线程绑定到CPU核心，0号线程绑定到0号核心，1号线程绑定到1号核心。

2x1x1: `./build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 2 1 1`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 2 1 1':

         14,354.55 msec task-clock                       #    1.845 CPUs utilized             
    64,710,649,907      cycles                           #    4.508 GHz                         (83.34%)
   204,088,834,355      instructions                     #    3.15  insn per cycle              (83.40%)
     9,999,684,154      cache-references                 #  696.621 M/sec                       (83.28%)
       539,018,362      cache-misses                     #    5.39% of all cache refs           (83.38%)
     7,374,690,193      branches                         #  513.753 M/sec                       (83.31%)
        36,375,992      branch-misses                    #    0.49% of all branches             (83.28%)
            76,027      page-faults                      #    5.296 K/sec                     
            76,024      minor-faults                     #    5.296 K/sec                     
                 3      major-faults                     #    0.209 /sec                      
                57      cpu-migrations                   #    3.971 /sec                      
            13,435      context-switches                 #  935.940 /sec                      

       7.778543962 seconds time elapsed

      14.035339000 seconds user
       0.240954000 seconds sys
```

1x1x2: `./build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 2`

报告如下

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 2':

         25,672.21 msec task-clock                       #    1.917 CPUs utilized             
   116,805,723,909      cycles                           #    4.550 GHz                         (83.33%)
   205,888,912,957      instructions                     #    1.76  insn per cycle              (83.34%)
    19,328,422,246      cache-references                 #  752.893 M/sec                       (83.34%)
     1,145,169,180      cache-misses                     #    5.92% of all cache refs           (83.32%)
     7,512,461,061      branches                         #  292.630 M/sec                       (83.32%)
        61,675,797      branch-misses                    #    0.82% of all branches             (83.35%)
            76,035      page-faults                      #    2.962 K/sec                     
            76,032      minor-faults                     #    2.962 K/sec                     
                 3      major-faults                     #    0.117 /sec                      
                63      cpu-migrations                   #    2.454 /sec                      
            12,918      context-switches                 #  503.190 /sec                      

      13.393647514 seconds time elapsed

      25.365368000 seconds user
       0.215383000 seconds sys
```

1x1x2的加速效果极差，在最高的CPU利用率下，速度和串行执行差不多。缓存引用次数和分支次数都是最高的。

### 多进程性能分析

`mpiexec --allow-run-as-root --bind-to core  -n 2  ./build/Release-x64/bin/benchmark --with_pml  -t 1200 -t_c 1 1 1`

1x1x2报告如下

```plain
 Performance counter stats for 'mpiexec --allow-run-as-root --bind-to core -n 2 ./build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 1':

         15,365.46 msec task-clock                       #    1.925 CPUs utilized             
    69,081,113,395      cycles                           #    4.496 GHz                         (83.32%)
   213,563,002,414      instructions                     #    3.09  insn per cycle              (83.33%)
    10,682,103,857      cache-references                 #  695.202 M/sec                       (83.34%)
       645,525,024      cache-misses                     #    6.04% of all cache refs           (83.36%)
     8,511,543,167      branches                         #  553.940 M/sec                       (83.39%)
        45,406,924      branch-misses                    #    0.53% of all branches             (83.26%)
            81,154      page-faults                      #    5.282 K/sec                     
            81,151      minor-faults                     #    5.281 K/sec                     
                 3      major-faults                     #    0.195 /sec                      
               308      cpu-migrations                   #   20.045 /sec                      
             9,800      context-switches                 #  637.794 /sec                      

       7.980754877 seconds time elapsed

      15.049898000 seconds user
       0.298889000 seconds sys
```

2x1x1

```plain
 Performance counter stats for 'mpiexec --allow-run-as-root --bind-to core -n 2 ./build/Release-x64/bin/benchmark --with_pml -t 1200 -t_c 1 1 1':

         14,956.17 msec task-clock                       #    1.924 CPUs utilized             
    67,179,684,611      cycles                           #    4.492 GHz                         (83.32%)
   208,882,939,244      instructions                     #    3.11  insn per cycle              (83.42%)
    10,231,637,604      cache-references                 #  684.108 M/sec                       (83.35%)
       549,804,692      cache-misses                     #    5.37% of all cache refs           (83.31%)
     7,747,500,546      branches                         #  518.014 M/sec                       (83.31%)
        39,120,745      branch-misses                    #    0.50% of all branches             (83.29%)
            81,026      page-faults                      #    5.418 K/sec                     
            81,023      minor-faults                     #    5.417 K/sec                     
                 3      major-faults                     #    0.201 /sec                      
               232      cpu-migrations                   #   15.512 /sec                      
             9,647      context-switches                 #  645.018 /sec                      

       7.771637133 seconds time elapsed

      14.583200000 seconds user
       0.356060000 seconds sys
```

## 更大并行度

多线程4x1x1: `./build/Release-x64/bin/benchmark -t 1200 -t_c 4 1 1`

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 4 1 1':

          6,700.63 msec task-clock                       #    2.954 CPUs utilized             
    29,755,851,505      cycles                           #    4.441 GHz                         (83.51%)
    83,205,528,265      instructions                     #    2.80  insn per cycle              (83.10%)
     4,199,090,239      cache-references                 #  626.671 M/sec                       (83.31%)
        93,311,424      cache-misses                     #    2.22% of all cache refs           (83.27%)
     3,088,270,961      branches                         #  460.893 M/sec                       (83.47%)
        26,095,146      branch-misses                    #    0.84% of all branches             (83.34%)
            40,295      page-faults                      #    6.014 K/sec                     
            40,292      minor-faults                     #    6.013 K/sec                     
                 3      major-faults                     #    0.448 /sec                      
                64      cpu-migrations                   #    9.551 /sec                      
            22,750      context-switches                 #    3.395 K/sec                     

       2.268141292 seconds time elapsed

       6.323666000 seconds user
       0.291030000 seconds sys
```

多线程1x1x4: `./build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 4`

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 1 1 4':

         14,000.02 msec task-clock                       #    3.473 CPUs utilized             
    63,236,927,938      cycles                           #    4.517 GHz                         (83.28%)
    86,178,188,140      instructions                     #    1.36  insn per cycle              (83.34%)
    14,446,451,610      cache-references                 #    1.032 G/sec                       (83.39%)
       683,928,113      cache-misses                     #    4.73% of all cache refs           (83.37%)
     3,274,725,625      branches                         #  233.909 M/sec                       (83.33%)
        27,821,046      branch-misses                    #    0.85% of all branches             (83.29%)
            40,298      page-faults                      #    2.878 K/sec                     
            40,295      minor-faults                     #    2.878 K/sec                     
                 3      major-faults                     #    0.214 /sec                      
                65      cpu-migrations                   #    4.643 /sec                      
            11,451      context-switches                 #  817.928 /sec                      

       4.030696660 seconds time elapsed

      13.727729000 seconds user
       0.188187000 seconds sys
```

多线程2x2x1: `'./build/Release-x64/bin/benchmark -t 1200 -t_c 2 2 1`

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 2 2 1':

          7,203.20 msec task-clock                       #    3.054 CPUs utilized             
    32,083,910,773      cycles                           #    4.454 GHz                         (83.21%)
    82,976,776,310      instructions                     #    2.59  insn per cycle              (83.38%)
     4,303,902,243      cache-references                 #  597.499 M/sec                       (83.26%)
       157,530,568      cache-misses                     #    3.66% of all cache refs           (83.28%)
     3,099,766,338      branches                         #  430.332 M/sec                       (83.38%)
        26,860,431      branch-misses                    #    0.87% of all branches             (83.49%)
            40,318      page-faults                      #    5.597 K/sec                     
            40,315      minor-faults                     #    5.597 K/sec                     
                 3      major-faults                     #    0.416 /sec                      
                63      cpu-migrations                   #    8.746 /sec                      
            22,403      context-switches                 #    3.110 K/sec                     

       2.358286286 seconds time elapsed

       6.834764000 seconds user
       0.283820000 seconds sys
```

多线程1x2x2: `./build/Release-x64/bin/benchmark -t 1200 -t_c 1 2 2`

```plain
 Performance counter stats for './build/Release-x64/bin/benchmark -t 1200 -t_c 1 2 2':

         14,890.42 msec task-clock                       #    3.480 CPUs utilized             
    67,470,192,018      cycles                           #    4.531 GHz                         (83.39%)
    84,160,170,749      instructions                     #    1.25  insn per cycle              (83.28%)
     7,447,985,604      cache-references                 #  500.186 M/sec                       (83.34%)
     1,035,847,953      cache-misses                     #   13.91% of all cache refs           (83.29%)
     3,165,261,031      branches                         #  212.570 M/sec                       (83.37%)
        29,836,493      branch-misses                    #    0.94% of all branches             (83.34%)
            40,295      page-faults                      #    2.706 K/sec                     
            40,292      minor-faults                     #    2.706 K/sec                     
                 3      major-faults                     #    0.201 /sec                      
                57      cpu-migrations                   #    3.828 /sec                      
            20,126      context-switches                 #    1.352 K/sec                     

       4.278950130 seconds time elapsed

      14.502632000 seconds user
       0.288415000 seconds sys
```