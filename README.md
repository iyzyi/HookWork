# HookWork

通用HOOK框架及API监测系统的设计与实现。

自认为不入流的毕设，烂中烂。

## 编译

vs2019下编译。

debug模式下生成的程序会带有控制台窗口，用于显示相关debug信息。

另请手动设置Launcher为启动项目。

## 结构

`SGSOL`、`TestCode`、`TestUI`这三个项目为初期测试文件，因其中有些代码具有后续的参考性，所以最终保留了下来。

最终的项目为：

| 名称                   | 功能               |
| :--------------------- | ------------------ |
| Launcher.exe           | 启动器             |
| FrameNetwork.exe       | 网络通信分析框架   |
| FrameFileOperation.exe | 文件操作分析框架   |
| FrameRegOperation.exe  | 注册表操作分析框架 |
| FrameProcessThread.exe | 进程线程分析框架   |
| FrameKeyboardMouse.exe | 键鼠消息分析框架   |
| Inject.dll             | 用于远程注入的DLL  |
| BypassUAC.dll          | 提升权限           |
|MyMessagePack.lib|二进制数据序列化|
|RemoteInjectTool.lib|远程注入工具|
|SimpleHookEngine.lib|通用HOOK框架|

出于代码复用的考虑，几大功能性框架并未整合到一个程序中，便于针对性的二次开发。

## 展示

分析某网游的通信数据：

![image-20220517221718983](http://image.iyzyi.com/img/202205172229466.png)

分析office word创建文件过程：

![image-20220517221846197](http://image.iyzyi.com/img/202205172229468.png)

分析IDA对注册表的操作：

![image-20220517222055150](http://image.iyzyi.com/img/202205172229469.png)

分析edge浏览器对进程线程的操作：

![image-20220517221444981](http://image.iyzyi.com/img/202205172229470.png)

分析键鼠消息：

![image-20220517222945422](http://image.iyzyi.com/img/202205172229471.png)
