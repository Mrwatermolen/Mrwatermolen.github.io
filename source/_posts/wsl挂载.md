---
title: WSL 开机启动并挂载 EXT4 硬盘
date: 2025-08-26 23:35:24
tags:
  - WSL
categories:
  - 问题记录
---


## 如何挂载 EXT4 硬盘

> [在 WSL 2 中装载 Linux 磁盘](https://learn.microsoft.com/zh-cn/windows/wsl/wsl2-mount-disk)

确定磁盘，在`powershell`中运行以下命令：

```powershell
 Get-Disk | Format-Table Number, FriendlyName, Size, HealthStatus
```

<!-- more -->

输出为

```plain
Number FriendlyName                         Size HealthStatus
------ ------------                         ---- ------------
     0 Fanxiang S100Pro 1TB        1024209543168 Healthy
     1 HGST HUS728T8TALE6L4        8001563222016 Healthy
     3 SanDisk Ultra 3D NVMe 500GB  500107862016 Healthy
     2 ZHITAI TiPlus7100 2TB       2048408248320 Healthy
```

选择`HGST HUS728T8TALE6L4`确定分区

```powershell
Get-Partition -DiskNumber 1 | Format-Table PartitionNumber, Size, Type
```

输出

```plain
PartitionNumber          Size Type
---------------          ---- ----
              1 6597069766656 Unknown
              2 1404492054528 Basic
```

选择分区`1`

准备进行挂载操作
首先确定`Disk`

```powershell
Get-CimInstance -Query "SELECT * from Win32_DiskDrive"
```

输出

```plain
DeviceID           Caption                     Partitions Size          Model
--------           -------                     ---------- ----          -----
\\.\PHYSICALDRIVE3 SanDisk Ultra 3D NVMe 500GB 2          500105249280  SanDisk Ultra 3D NVMe 500GB
\\.\PHYSICALDRIVE0 Fanxiang S100Pro 1TB        1          1024203640320 Fanxiang S100Pro 1TB
\\.\PHYSICALDRIVE1 HGST HUS728T8TALE6L4        2          8001560609280 HGST HUS728T8TALE6L4
\\.\PHYSICALDRIVE2 ZHITAI TiPlus7100 2TB       3          2048407280640 ZHITAI TiPlus7100 2TB
```

为`\\.\PHYSICALDRIVE1`

挂载命令，注意挂载命令需要以管理员权限运行

```powershell
wsl.exe --mount <Disk> --partition <Index> --type <Type>
wsl.exe --mount \\.\PHYSICALDRIVE1 --partition 1 --type ext4
```

打开 WSL ,不需要管理员权限，在 WSL 中查看是否挂载

```bash
ls /mnt/wsl/
```

输出

```plain
PHYSICALDRIVE1  PHYSICALDRIVE1p1  resolv.conf
```

在 WSL 2 中运行以下命令来列出分区：

```bash
lsblk
```

输出

```plain
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
sda      8:0    0 388.4M  1 disk
sdb      8:16   0   186M  1 disk
sdc      8:32   0     6G  0 disk [SWAP]
sdd      8:48   0   7.3T  0 disk
├─sdd1   8:49   0     6T  0 part /mnt/wsl/PHYSICALDRIVE1p1
└─sdd2   8:50   0   1.3T  0 part
sde      8:64   0     1T  0 disk /var/lib/docker
                                 /mnt/wslg/distro
                                 /
```

可以看到分区已经成功挂载到`/mnt/wsl/PHYSICALDRIVE1p1` 。

### 题外话

已经可以结束了，但是为了方便，创建软连接管理一下

```bash
sudo mkdir -p /mnt/data && sudo ln -s /mnt/wsl/PHYSICALDRIVE1p1 /mnt/data/myLibrary
```

验证

```bash
ls /mnt/data/myLibrary
```

搬输出出来，把`ntfs`分区的数据拿出来，以后吧

```bash
sudo mount -t ntfs-3g -o uid=1000,gid=1000,umask=022 /dev/sdX1 /mnt/ntfs_data
```

## 如何开机启动 WSL

> [实现 WSL 2 开机免登录自动启动](https://sjdhome.com/blog/post/wsl2-auto-start/)

做个备份吧，万一那个 blog 没了呢。

步骤

1. 一定要确保 WSL 当前处于最新版本（即 WSL September 2023 update 之后的版本），系统自带版本不支持这种开机启动。
2. 打开任务计划程序。
3. 点击右边的创建任务。
4. 任务的名称和描述可以随便写，安全选项需要选择“不管用户是否登录都要运行”。
5. 点击上方的“触发器”选项卡，点新建按钮，然后会卡几秒（微软的老 BUG ）。开始任务中选择“启动时”，然后点击确定。
6. “操作”选项卡中，点新建按钮，然后“程序或脚本”下的文本框里输入"C:\Program Files\WSL\wsl.exe"，引号也要带上（非常重要，除了这个目录下的wsl.exe，其他位置的都不行）。添加参数可以根据需要填写，比如-d Debian指定发行版。
7. “条件”选项卡中，所有选项全部取消勾选。
8. ”设置“选项卡中，除了“允许按需执行任务”，其他全部取消勾选。
9. 点击确定关闭窗口。可以先右键运行试试效果。这种方法运行的 WSL 即使当前用户注销也是会继续运行的。

在`powershell`命令中运行命令查看是否成功

```powershell
wsl --list --running
```

输出

```plain
适用于 Linux 的 Windows 子系统分发:
Ubuntu (默认值)
```

## 启动 WSL 后自动挂载 EXT4 硬盘

> 思路来源于：<https://github.com/microsoft/WSL/issues/6073#issuecomment-1266405095>

不过他的对我来说有点问题，执行老是不成功，我自己结合我的情况写了一个，但思路是差不多的。编辑 WSL 中的`/etc/wsl.conf`文件，自动运行命令挂载硬盘。

AI 生成了一个`ps1`脚本自动挂载硬盘，脚本使用`powershell7`运行，所有先决条件是安装`powershell7`。脚本内容
记得变量`$LogPath`路径要改成自己的放置日志路径。

```powershell
# ===========================================
# WSL 自动挂载脚本 (简化版)
# 用途：开机自动挂载多个物理磁盘分区
# ===========================================

# 配置区域 - 可根据需要修改
$isStdout = $true
$LogPath = "C:\Users\Kumi\wsl-mount.log"
$InitialDelay = 1 # 启动后延迟执行时间（秒）

# define logging function
function Write-Log {
    param (
        [string]$Message,
        [bool]$isHeader=$false
    )
    $timestamp = if ($isHeader) { Get-Date -Format "yyyy-MM-dd HH:mm:ss" } else { Get-Date -Format "HH:mm:ss" }
    if ($isStdout) {
        Write-Host "[$timestamp] $Message"
    }
    "[$timestamp] $Message" | Out-File -FilePath $LogPath -Append
}

#region 函数定义
function Test-IsElevated {
    $id = [System.Security.Principal.WindowsIdentity]::GetCurrent()
    $p = New-Object System.Security.Principal.WindowsPrincipal($id)
    if ($p.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)) {
        return $true
    }
    else {
        return $false
    }
}

function Mount-DiskPartition {
    param(
        [int]$DiskNumber,
        [int]$PartitionNumber,
        [string]$FileSystemType = "ext4"
    )

    $mountPoint = "PHYSICALDRIVE${DiskNumber}p${PartitionNumber}"

    # 检查是否已挂载
    $isMounted = wsl.exe ls /mnt/wsl/ 2>$null | findstr /i $mountPoint
    if ($isMounted) {
        Write-Log -Message "注意：分区 $mountPoint 已挂载，跳过操作"
        return $true
    }

    # 确保磁盘离线
    try {
        Set-Disk -Number $DiskNumber -IsOffline $true -ErrorAction Stop
        Write-Log -Message "已将磁盘 $DiskNumber 设置为离线状态"
    }
    catch {
        Write-Log -Message "警告：磁盘 $DiskNumber 可能已离线或不存在 - $($_.Exception.Message)"
    }

    # 执行挂载
    $isAdmin = Test-IsElevated

    if (-not $isAdmin) {
        # throw "挂载操作需要管理员权限"
        Write-Log -Message "警告：当前未以管理员权限运行，挂载操作可能失败"
    } else {
        Write-Log -Message "当前以管理员权限运行"
    }

    Write-Log -Message "开始挂载：磁盘 $DiskNumber 分区 $PartitionNumber"
    wsl.exe --mount "\\.\PHYSICALDRIVE${DiskNumber}" --partition $PartitionNumber --type $FileSystemType

    # 验证挂载结果
    $mountSuccess = wsl.exe ls /mnt/wsl/ | findstr /i $mountPoint
    if ($mountSuccess) {
        write-Log -Message "成功挂载：\\.\PHYSICALDRIVE${DiskNumber} 分区 ${PartitionNumber} -> /mnt/wsl/${mountPoint}"
        return $true
    }
    else {
        throw "挂载失败：无法访问 /mnt/wsl/${mountPoint}"
    }
}
#endregion


Write-Log -Message "===== WSL 挂载脚本开始执行 =====" -isHeader $true

# 添加固定延迟（替代WSL响应检查）
Write-Log -Message "等待 $InitialDelay 秒以确保 WSL 已启动..."
Start-Sleep -Seconds $InitialDelay

try {
    # 定义要挂载的磁盘配置
    $disksToMount = @(
        @{ DiskNumber = 1; PartitionNumber = 1; FileSystemType = "ext4" },
        @{ DiskNumber = 3; PartitionNumber = 1; FileSystemType = "ext4" }
    )

    # 逐个挂载磁盘分区
    foreach ($disk in $disksToMount) {
        try {
            Write-Log -Message "===== 开始处理磁盘 $($disk.DiskNumber) 分区 $($disk.PartitionNumber) ====="

            # 执行挂载操作
            $mountResult = Mount-DiskPartition `
                -DiskNumber $disk.DiskNumber `
                -PartitionNumber $disk.PartitionNumber `
                -FileSystemType $disk.FileSystemType

            Write-Log -Message "===== 磁盘 $($disk.DiskNumber) 分区 $($disk.PartitionNumber) 处理完成 =====`n"
        }
        catch {
            Write-Log -Message "❌ 处理磁盘 $($disk.DiskNumber) 时出错：$($_.Exception.Message)"
        }
    }

    Write-Log -Message "所有挂载操作已完成"
}
catch {
    $errorMsg = $_.Exception.Message
    Write-Log -Message "❌ 脚本级错误：$errorMsg"
    exit 1
}
finally {
    Write-Log -Message "===== WSL 挂载脚本执行结束 =====`n"
}
```

脚本在本机中的路径`C:\Users\Kumi\wsl-mount.ps1`

在 WSL 中，创建文件`/path/to/wsl-auto-mount.sh`，内容为：

```bash
#!/bin/bash
/mnt/c/Program\ Files/PowerShell/7/pwsh.exe -NoProfile -ExecutionPolicy Bypass -File  C:\\Users\\Kumi\\wsl-mount.ps1 > /tmp/wsl-auto-mount.log 2>&1
```

修改 WSL 中的`/etc/wsl.conf`文件，添加：

```conf
[boot]
command="bash /path/to/wsl-auto-mount.sh"
```

可以关闭 WSL 看看效果。

```powershell
wsl --shutdown
```

然后在运行 WSL 看看情况，这里可以运行之前设定好的开机任务测试一下，日志在`/tmp/wsl-auto-mount.log`中。

注意日志编码问题，可以使用以下命令查看：

```bash
cat /tmp/wsl-auto-mount.log | iconv -f GBK -t UTF-8
```

当然也可以直接在 Windows 中查看`$LogPath`日志文件。
