// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#define WINVER 0x0501
//#define _WIN32_WINDOWS 0x0410
//#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_DEPRECATE

#include <afx.h>

// Windows 头文件:
#include <windows.h>
#include <afxwin.h>

// TODO: 在此处引用程序要求的附加头文件
//#include "..\include\tool.h"

#include <list>
#include <map>
#include <vector>
#include <string>
using namespace std;