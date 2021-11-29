---
title: OMV6在树莓派上的安装
tags:
---

# OMV6在树莓派上的安装

> OMV全称是openmediavault，是一个基于debian开源的nas管理系统,干净好用
>
> 设备：PI 4 8GB
>
> 系统：Debian 11 bullseye

安装前一定要谨慎，判断是否有这个需求，而且OMV会接管大量底层，不要拿主力机去试验。

## 安装OMV6

参考官网https://forum.openmediavault.org/index.php?thread/39490-install-omv6-on-debian-11-bullseye/

先添加源

```shell
nano add.sh #创建脚本
# 写入这些
cat <<EOF >> /etc/apt/sources.list.d/openmediavault.list
deb http://packages.openmediavault.org/public shaitan main
# deb http://downloads.sourceforge.net/project/openmediavault/packages shaitan main
## Uncomment the following line to add software from the proposed repository.
# deb http://packages.openmediavault.org/public shaitan-proposed main
# deb http://downloads.sourceforge.net/project/openmediavault/packages shaitan-proposed main
## This software is not part of OpenMediaVault, but is offered by third-party
## developers as a service to OpenMediaVault users.
# deb http://packages.openmediavault.org/public shaitan partner
# deb http://downloads.sourceforge.net/project/openmediavault/packages shaitan partner
EOF

# 添加开始
chmod +x add.sh 
sudo ./add.sh
```

再执行安装

```shell
nano install-omv.sh #创建脚本
# 写入这些
export LANG=C.UTF-8
export DEBIAN_FRONTEND=noninteractive
export APT_LISTCHANGES_FRONTEND=none
apt-get install --yes gnupg
wget -O "/etc/apt/trusted.gpg.d/openmediavault-archive-keyring.asc" https://packages.openmediavault.org/public/archive.key
apt-key add "/etc/apt/trusted.gpg.d/openmediavault-archive-keyring.asc"
apt-get update
apt-get --yes --auto-remove --show-upgraded \
    --allow-downgrades --allow-change-held-packages \
    --no-install-recommends \
    --option DPkg::Options::="--force-confdef" \
    --option DPkg::Options::="--force-confold" \
    install openmediavault-keyring openmediavault

# Populate the database.
omv-confdbadm populate

# Display the login information.
cat /etc/issue

# 安装开始
chmod +x install-omv.sh
sudo ./install-omv.sh
```

安装结束

```
To manage the system visit the openmediavault web control panel:

eth0: 192.168.1.6
eth0: fe80::9d86:6e7:5e02:45b9

By default the web control panel administrator account has the
username 'admin' and password 'openmediavault'.
It is recommended that you change the password for this account
within the web control panel or using the 'omv-firstaid' CLI
command.

For more information regarding this appliance, please visit the
web site: https://www.openmediavault.org
```

注意：由于omv会接管大量debian底层的设置，比如smb直接被接管了，ssh也禁止出来root用户以外的用登陆

直接访问http://192.168.1.6/，注意：omv和nginx在没有做特处理前是冲突的

默认用户：admin 默认密码：openmediavault

安装OMV-Extra：

```shell
sudo wget -O - https://github.com/OpenMediaVault-Plugin-Developers/installScript/raw/master/install | sudo bash
```

新版本的OMV-Extra，装了点击进入时，会出现**显示软件错误点鼠标左键**的错误，先切换成英文界面进入，再换回中文界面即可。

## OMV设置ssh登录

OpenMediaVault 修改了 SSH 的权限机制，只有在SSH 用户组中的用户才允许通过 ssh 登陆服务器。

用户管理->用户->用户组，勾上ssh



## OMV设置smb共享

先把transmission停了。

存储器->文件系统->挂载，挂载磁盘

存储器->文件系统->共享文件夹->创建，注意相对路径那如果你是想访问根目录，就填入./

服务->SMB->共享->创建，在选择分享文件夹拿选择你刚刚创建的共享文件夹

服务->SMB->设置，勾选启动



## OMV设置transmission

悲剧的事情来了，我原来tr挂的文件夹时/home/pi/DiskShare，现在OMV接管后磁盘都挂载在/srv/目录下了。命名格式：文件系统的默认挂载点名称被统一设置成了 `dev-disk-by-uuid-XXXX` 这种格式.

解决链接：https://it.ismy.fun/2021/04/02/omv-5-change-default-mountpoint-name/

修改挂载点名称可以编辑 `/etc/openmediavault/config.xml` 配置文件，找到里面的 `<fstab></fstab>`这对标签中的文件挂载点信息`<mntent></mntent>`：

```xml
<fstab>
      <mntent>
        <uuid>ed23096c-3982-4a61-8b3e-28517653adb0</uuid>
        <fsname>/dev/disk/by-uuid/95a27f78-b514-4aec-abf2-77e75be86e0e</fsname>
        <dir>/srv/dev-disk-by-uuid-4dc04a66-7390-4587-811f-d561bbe74990</dir>
        <type>ext4</type>
        <opts>defaults,nofail,user_xattr,usrjquota=aquota.user,grpjquota=aquota.group,jqfmt=vfsv0,acl</opts>
        <freq>0</freq>
        <passno>2</passno>
        <hidden>0</hidden>
      </mntent>
      <mntent>
    </fstab>
```

把其中的 `<dir>/srv/dev-disk-by-uuid-4dc04a66-7390-4587-811f-d561bbe74990</dir>` 改成希望使用的挂载点名称，比如 `<dir>/srv/disk1</dir>`

修改完成以后，执行命令：

```shell
$ sudo omv-salt deploy run fstab
```

这个命令会根据修改后的配置文件，生成新的 `/etc/fstab` 配置信息。

重启系统，检查是否修改成功。

先备份一份

一般会成功的

注意此时transmission没有权限去访问数据，要把transmission也添加到pi组里面即可：

用户管理->用户组->编辑->成员：勾选debian-transmission即可（也不一定）

