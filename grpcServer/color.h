/*************************************************************************
	> File Name: color.h
	> Author: Ttw
	> Mail: 1658339000@qq.com
	> Created Time: Thu Aug  7 19:57:39 2025
 ************************************************************************/


/*
|  语 义   | 颜色 | ANSI 码     | 记忆口诀       |
|  -----  |  -- | ---------- | ---------- |
| 错误    | 红   | `\033[31m` | 红停         |
| 警告    | 黄   | `\033[33m` | 黄灯慢        |
| 接收消息 | 绿   | `\033[32m` | 绿灯行—数据进来   |
| 发送消息 | 蓝   | `\033[34m` | 蓝向外—数据出去   |
| 进入函数 | 青   | `\033[36m` | 青 trace，最淡 |
| 重置    | 默认   | `\033[0m`  | 用完必回       |
*/


#ifndef _COLOR_H
#define _COLOR_H

#define CLR_NONE         "\033[m"

#define CLR_RED          "\033[0;32;31m" 	// 错误

/// @brief 警告
#define CLR_YELLOW       "\033[1;33m" 		// 警告

/// @brief 接收消息
#define CLR_GREEN        "\033[0;32;32m" 	// 接收消息

#define CLR_BLUE         "\033[0;32;34m" 	// 发送消息

#define CLR_CYAN         "\033[0;36m" 		// 进入函数

#define LIGHT_RED    "\033[1;31m"
#define LIGHT_GREEN  "\033[1;32m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"


#endif

