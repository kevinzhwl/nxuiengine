// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
//#define _WIN32_WINDOWS 0x0410

/*
#define _CRT_SECURE_NO_DEPRECATE
#include <afx.h>
#include <afxwin.h>
*/
#define _CRT_SECURE_NO_DEPRECATE
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>


// TODO: 在此处引用程序要求的附加头文件
//#include "tool.h"

#include <list>
#include <map>
#include <vector>
using namespace std;
