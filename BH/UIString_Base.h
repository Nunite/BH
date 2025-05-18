#pragma once

/*
 * UIString_Base.h
 * 基础字符串定义文件
 * 
 * 此文件定义了所有UI相关字符串的通用结构和命名约定
 * 简体中文和繁体中文版本的字符串常量将分别在UIString_SC.h和UIString_TC.h中定义
 */

// 构建目标的语言定义
#if defined(LANG_SC)
    #include "UIString_SC.h"
#elif defined(LANG_TC)
    #include "UIString_TC.h"
#else
    // 默认使用简体中文版本
    #include "UIString_SC.h"
#endif

// 通用字符串ID结构，所有界面文本都应使用这些宏定义
// 统一的命名规则：UI_[模块名称]_[用途]

// 通用界面相关
#ifndef UI_OK
    #define UI_OK "确定"
#endif

#ifndef UI_CANCEL
    #define UI_CANCEL "取消"
#endif

#ifndef UI_APPLY
    #define UI_APPLY "应用"
#endif

// 设置界面标签
#ifndef UI_SETTINGS_TITLE
    #define UI_SETTINGS_TITLE "设置"
#endif

#ifndef UI_GENERAL_TAB
    #define UI_GENERAL_TAB "通用"
#endif

#ifndef UI_QUICKHELP_TAB
    #define UI_QUICKHELP_TAB "快捷说明"
#endif

// GameSettings模块文本
#ifndef UI_GS_ADV_STATS
    #define UI_GS_ADV_STATS "显示玩家/雇佣兵高级属性"
#endif

#ifndef UI_GS_EXP_METER
    #define UI_GS_EXP_METER "显示经验条"
#endif

#ifndef UI_GS_REMOVE_SHAKE
    #define UI_GS_REMOVE_SHAKE "移除地震效果"
#endif

#ifndef UI_GS_CPU_PATCH
    #define UI_GS_CPU_PATCH "CPU 补丁"
#endif

// 快捷说明界面
#ifndef UI_QH_MOUSE_HOVER
    #define UI_QH_MOUSE_HOVER "鼠标移物品上时"
#endif

#ifndef UI_QH_SHIFT_LEFT
    #define UI_QH_SHIFT_LEFT "Shift+左键"
#endif

#ifndef UI_QH_SHIFT_LEFT_DESC
    #define UI_QH_SHIFT_LEFT_DESC "如果鉴定书在背包，就可以快速鉴定物品"
#endif

#ifndef UI_QH_SHIFT_RIGHT
    #define UI_QH_SHIFT_RIGHT "Shift+右键"
#endif

#ifndef UI_QH_SHIFT_RIGHT_DESC
    #define UI_QH_SHIFT_RIGHT_DESC "在打开的背包、箱子、盒子之间移动"
#endif

#ifndef UI_QH_CTRL_RIGHT
    #define UI_QH_CTRL_RIGHT "Ctrl+右键"
#endif

#ifndef UI_QH_CTRL_RIGHT_DESC
    #define UI_QH_CTRL_RIGHT_DESC "把物品扔地上"
#endif

#ifndef UI_QH_CTRL_SHIFT_RIGHT
    #define UI_QH_CTRL_SHIFT_RIGHT "Ctrl+Shift+右键"
#endif

#ifndef UI_QH_CTRL_SHIFT_RIGHT_DESC
    #define UI_QH_CTRL_SHIFT_RIGHT_DESC "移动物品到关闭着的盒子"
#endif 