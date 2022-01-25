---
title: 使用pandoc将markdown转为pdf遇到的问题
author: MH
output: pdf_document
tags:
  - pandoc
  - Latex
  - markdown
categories:
  - Linux
date: 2020-11-19 00:00:00
---

参考[pandoc转pdf文件用的latex模板介绍（Linux环境）](https://blog.csdn.net/weixin_44375591/article/details/103992360?utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~all~first_rank_v2~rank_v28-3-103992360.nonecase&utm_term=pdflatex%20%E6%A8%A1%E6%9D%BF&spm=1000.2123.3001.4430)

## 错误

> Error: Command failed: pandoc -f markdown+tex_math_single_backslash
> Error producing PDF.
> ! Package inputenc Error: Unicode character 的 (U+7684)
> (inputenc)                not set up for use with LaTeX.
> See the inputenc package documentation for explanation.
> Type  H for immediate help.
> Try running pandoc with --pdf-engine=xelatex.

<!-- more -->

## 解决方法

pdflatex，是不支持中文的，因此需要手动设置编译时所用的引擎为xelatex
安装xelatex
 `sudo apt-get install texlive-xetex`

编译命令改为：
 `pandoc i.md -o o.pdf --pdf-engine=xelatex"`

这时就能够成功了但打开pdf会发现没有中文, 还要设置编译的时候的字体

``` shell
pandoc i.md -o o.pdf --pdf-engine=xelatex -V mainfont="Noto Sans CJK SC" #这里我使用了思源字体
fc-list #可列出所所有字体
```

## 更好的方法

使用Latex模板
模板的话去网上找别人的来用一下就好
我用的是
[pm-template](https://github.com/tzengyuxio/pages/blob/gh-pages/pandoc/pm-template.latex)

把下载下来的pm-template.latex放在$HOME/Templates/下(个人习惯)

 `pandoc i.md -o o.pdf --pdf-engine=xelatex --template=$HOME/Templates/pm-template.latex`

这样就完美了, 但是我还是踩了坑

``` txt
报错kpathsea:make_tex: Invalid filename `LiHei Pro/OT', contains ' '

我看了一下pm-template.latex文件　它规定用的字体我没有为了省事我全部改成了Noto Sans CJK SC
  \setCJKmainfont{Noto Sans CJK SC}  % 設定中文字型
  \setmainfont{Noto Sans CJK SC}  % 設定英文字型
  \setromanfont{Noto Sans CJK SC}  % 字型
  \setmonofont{Noto Sans CJK SC}
```

eisvogel.tex 模板

```bash
! LaTeX Error: File `sourcesanspro.sty' not found.
sudo texhash
! Package fontspec Error: The font "SourceSansPro-Regular" cannot be found.
sudo apt install texlive-fonts-extra
pandoc 308.md -o 308.pdf --template=/home/pi/Templates/eisvogel.tex --pdf-engine=xelatex  -VCJKmainfont="Noto Sans CJK SC"
```

> 上述模板的作者貌似不是亚洲人，他的模板[默认使用的字体](https://link.zhihu.com/?target=https%3A//github.com/Wandmalfarbe/pandoc-latex-template/blob/27fb7e455536012aa7e92151ffad28ff70986f41/eisvogel.tex%23L623)是 Source Sans Pro 字体，这个字体文件里没有 CJK 字符的画法。为了让这个模板默认使用我们安装的 Noto Serif SC 字体，我们需要编辑一下 ~/.local/share/pandoc/templates/eisvogel.latex 。我的版本里，我在 \else % if not pdftex 这一行后面加了三行，使其看上去是这个样子。
>
> https://zhuanlan.zhihu.com/p/341893512

```tex
\else % if not pdftex
  $if(mainfont)$
  $else$
  \usepackage[fallback]{xeCJK}
  \setCJKmainfont{Noto Serif SC}
  \setCJKfallbackfamilyfont{rm}{Noto Serif SC}
  \usepackage[default]{sourcesanspro}
  \usepackage{sourcecodepro}
```

我用的是Noto Sans CJK SC
