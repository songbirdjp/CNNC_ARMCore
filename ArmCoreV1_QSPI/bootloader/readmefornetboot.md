## 在Windows上使用Git Bash和TFTP

### 1. 安装TFTP客户端

Windows 10及以上版本通常自带TFTP客户端，但默认是未启用的。你可以通过以下步骤启用它：

1. 打开“控制面板”。
2. 选择“程序”。
3. 点击“启用或关闭Windows功能”。
4. 在列表中找到“TFTP客户端”，勾选并确认。

### 2. 使用Git Bash

打开Git Bash，使用TFTP命令进行文件传输。例如：
    tftp -i 192.168.1.100 put firmware.bin