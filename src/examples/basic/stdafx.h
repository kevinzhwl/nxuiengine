// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

// TODO: 在此处引用程序要求的附加头文件

#if defined(_DEBUG)
#define DEBUG_WAS_DEFINED
#undef _DEBUG
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>

#if defined(DEBUG_WAS_DEFINED)
#define _DEBUG
#endif

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "ISonicUI.h"
using namespace sonic_ui;
