// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
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
#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
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


// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
//#include "tool.h"

#include <list>
#include <map>
#include <vector>
using namespace std;
