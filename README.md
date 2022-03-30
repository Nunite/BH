# 个人修改BH（汉化）
仅用于单人游戏或个人创建的PD2的私服或其它D2私服<br/>
主要修改内容如下：<br/>
1、添加自动开图功能<br/>
2、汉化所有的BH界面<br/>
3、汉化filter文件（filter文件编码需要改成ANSI）<br/>
4、添加修复部分新功能（主要来源于开源的D2hackmap）<br/>
5、中文输入功能（主要来源于开源的D2hackmap）<br/>
# 效果预览
![图1](https://github.com/weili0677/BH/blob/main/%E7%89%A9%E5%93%81%E6%98%BE%E7%A4%BA.png)<br/>
![图2](https://github.com/weili0677/BH/blob/main/%E5%BF%AB%E6%8D%B7%E8%AF%B4%E6%98%8E.png)<br/>
![图3](https://github.com/weili0677/BH/blob/main/%E5%9C%B0%E5%9B%BE%E6%98%BE%E7%A4%BA.png)<br/>
![图4](https://github.com/weili0677/BH/blob/main/%E5%B1%9E%E6%80%A7%E9%9D%A2%E6%9D%BF.png)<br/>
![图5](https://github.com/weili0677/BH/blob/main/%E5%9C%B0%E9%9D%A2%E6%98%BE%E7%A4%BA%E5%92%8C%E6%8E%89%E8%90%BD%E6%8F%90%E7%A4%BA.png)<br/>


# Project Diablo 2 BH

## Setting up Debug and Build output Folders

Set the following environment variables according to your setup, change the folder to your Diablo II folder. This can either be done in the environment variable tab, or open up powershell as administrator and run the following commands to set it system wide:

> :warning: I suggest using a clean Diablo II 1.13c folder you are not using for anything else for this, since it will override txt files in this folder when either running scripts or debugging with Visual Studio.  
> :warning: Make sure 'game.exe' has compatability mode set to Windows XP (Service Pack 2).

```
[System.Environment]::SetEnvironmentVariable('DIABLO_DEBUG_FOLDER','C:\Program Files (x86)\Diablo II\ProjectD2',[System.EnvironmentVariableTarget]::Machine)
```

You can change the command line arguments as you see fit.

```
[System.Environment]::SetEnvironmentVariable('DIABLO_DEBUG_COMMAND_LINE_ARGUMENTS','-w -ns -direct -txt',[System.EnvironmentVariableTarget]::Machine)
```

Any debug or changed version will only work in *Single Player*. Do not enter multiplayer with a modified BH
