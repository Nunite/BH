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

// Item模块文本
#ifndef UI_ITEM_TAB
    #define UI_ITEM_TAB "物品显示"
#endif

#ifndef UI_ITEM_SETTINGS
    #define UI_ITEM_SETTINGS "设置"
#endif

#ifndef UI_ITEM_ALWAYS_SHOW
    #define UI_ITEM_ALWAYS_SHOW "永久显示物品"
#endif

#ifndef UI_ITEM_SHOW_RANGES
    #define UI_ITEM_SHOW_RANGES "显示物品变量"
#endif

#ifndef UI_ITEM_SHOW_ILVL
    #define UI_ITEM_SHOW_ILVL "显示物品等级"
#endif

#ifndef UI_ITEM_DISPLAY_STYLE
    #define UI_ITEM_DISPLAY_STYLE "显示样式 (仅不启用loot过滤时生效)"
#endif

#ifndef UI_ITEM_SHOW_ETH
    #define UI_ITEM_SHOW_ETH "显示ETH(无形)"
#endif

#ifndef UI_ITEM_SHOW_SOCKETS
    #define UI_ITEM_SHOW_SOCKETS "显示孔数"
#endif

#ifndef UI_ITEM_SHOW_RUNE_NUM
    #define UI_ITEM_SHOW_RUNE_NUM "显示符文编号#"
#endif

#ifndef UI_ITEM_ALT_STYLE
    #define UI_ITEM_ALT_STYLE "Alt物品样式"
#endif

#ifndef UI_ITEM_COLOR_MOD
    #define UI_ITEM_COLOR_MOD "多彩模式"
#endif

#ifndef UI_ITEM_SHORT_NAME
    #define UI_ITEM_SHORT_NAME "物品短名称"
#endif

#ifndef UI_ITEM_LOOT_FILTER
    #define UI_ITEM_LOOT_FILTER "Loot过滤器"
#endif

#ifndef UI_ITEM_ENABLE_FILTER
    #define UI_ITEM_ENABLE_FILTER "开启Loot过滤"
#endif

#ifndef UI_ITEM_DROP_NOTIFY
    #define UI_ITEM_DROP_NOTIFY "物品掉落提示"
#endif

#ifndef UI_ITEM_CLOSE_NOTIFY
    #define UI_ITEM_CLOSE_NOTIFY "物品接近提示"
#endif

#ifndef UI_ITEM_DETAIL_NOTIFY
    #define UI_ITEM_DETAIL_NOTIFY "物品明细提示"
#endif

#ifndef UI_ITEM_VERBOSE_NOTIFY
    #define UI_ITEM_VERBOSE_NOTIFY "提示更加丰富"
#endif

#ifndef UI_ITEM_FILTER_LEVEL
    #define UI_ITEM_FILTER_LEVEL "过滤等级:"
#endif

#ifndef UI_ITEM_SHOW_ALL
    #define UI_ITEM_SHOW_ALL "0 - 显示所有物品"
#endif

#ifndef UI_ITEM_STANDARD
    #define UI_ITEM_STANDARD "1 - 标准"
#endif

#ifndef UI_ITEM_AFFIX_LEVEL
    #define UI_ITEM_AFFIX_LEVEL "词缀等级: %d\n"
#endif

#ifndef UI_ITEM_ITEM_LEVEL
    #define UI_ITEM_ITEM_LEVEL "物品等级: %d\n"
#endif 