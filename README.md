#个人修改BH（汉化）
仅用于个人创建的PD2的私服或其它D2私服
主要修改内容如下：
1、添加自动开图功能
2、汉化所有的BH界面
3、汉化filter文件（filter文件编码需要改成ANSI）
4、添加修复部分新功能（主要来源于开源的D2hackmap）
#图片预览



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
