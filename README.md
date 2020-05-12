# 自制操作系统
learn to make OS
自制操作系统一方面是兴趣，另一方面是为了更好得理解Linux内核

> 本项目1.0基于于渊——《Orange's一个操作系统的实现》、川合秀实——《30天自制操作系统》，2.0基于田宇——《一个64位操作系统的设计与实现》，3.0在两个版本（都是半成品）的基础上自己修改简化，目前正在完善中，也是未完成品。

## 使用|Usage

`cd OS/OSv3.0/`

`./build`

## 环境|prepare

本项目基于 bochs 调试开发

```
sudo apt-get install bochs
sudo apt-get install bochs-x
```

## 一些辣鸡功能

1. ls，列出当前软盘的文件
2. help help me，显示帮助信息
3. cls，清空屏幕
4. set bg 文件名（可忽略空格），设置背景，如下

![](./截图录屏_选择区域_20200512131904.jpg)