---
title: ffmpeg使用
date: 2021-07-04 12:42:00
tags:
  - cmd
  - bat
  - win
categories:
  - ffmpeg
---

## 使用FFmpeg合并MP4视频

### 使用FFmpeg拼接

速度慢，文件大

将 mp4 先转码为 mpeg文件，mpeg是支持简单拼接的，然后再转回 mp4。

```bash
ffmpeg -i 1.mp4 -qscale 4 1.mpg
ffmpeg -i 2.mp4 -qscale 4 2.mpg
cat 1.mpg 2.mpg | ffmpeg -f mpeg -i - -qscale 6 -vcodec mpeg4 output.mp4
#-qscale <数值> 以<数值>质量为基础的VBR，取值0.01-255，约小质量越好
```

### 使用ts拼接

速度快，文件小

先将 mp4 转化为同样编码形式的 ts 流，因为 ts流是可以 concate 的，先把 mp4 封装成 ts ，然后 concate ts 流， 最后再把 ts 流转化为 mp4。

```bash
ffmpeg -i 1.mp4 -vcodec copy -acodec copy -vbsf h264_mp4toannexb 1.ts
ffmpeg -i 2.mp4 -vcodec copy -acodec copy -vbsf h264_mp4toannexb 2.ts
ffmpeg -i "concat:1.ts|2.ts" -acodec copy -vcodec copy -absf aac_adtstoasc output.mp4
```

————————————————
版权声明：本文为CSDN博主「熊观鱼」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/east196/article/details/79821361

### 使用mpeg拼接（另一种方法）

```bash
ffmpeg -i 1.mp4 –f mpeg 1.mpg
ffmpeg -i 2.mp4 –f mpeg 2.mpg
```

win10，打开cmd（copy命令在powershell和gitbash里都不好使）

```
copy /b 1.mpg + 2.mpg 3.mpg
ffmpeg -i 3.mpg -f mp4 3.mp4
```

### FFmpeg合并MP4视频总结

ts是真的又快又好

## FFmpeg批量处理mp4 win

```bash
setlocal enabledelayedexpansion
set n=0
for %%i in (*.mp4) do (
    set /a n+=1
    echo %%i
    echo !n!
    ffmpeg -i "%%i" -c copy "%%~ni.ts"
)
if !n! geq 10 (
    set /a n=0
    for %%i in (*.ts) do (
        echo %%i
        set /a n+=1
        if !n! geq 10 (
            rename  "%%i"  "file !n!.ts"
        ) else (
        	rename  "%%i"  "file 0!n!.ts"
        )
    )
) else (
    set /a n=0
    for %%i in (*.ts) do (
        set /a n+=1
        rename  "%%i"  "file !n!.ts"
    )
)
dir *.ts /B >filelist.txt
if !n! geq 10 (
    set /a n=0
    for %%i in (*.ts) do (
        echo %%i
        set /a n+=1
        if !n! geq 10 (
            rename  "%%i"  "!n!.ts"
        ) else (
        	rename  "%%i"  "0!n!.ts"
        )
    )
) else (
    set /a n=0
    for %%i in (*.ts) do (
        set /a n+=1
        rename  "%%i"  "!n!.ts"
    )
)
ffmpeg -f concat -i filelist.txt -acodec copy -vcodec copy -absf aac_adtstoasc output.mp4
del *.ts
```

## FFmpeg提取字幕流

```bash
ffmpeg -i input.mkv -an -vn -scodec copy sub.ass
```

