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

#if defined(_DEBUG)
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#pragma message ("Compiling MFC header files in release mode.")
#endif

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>

#if defined(_DEBUG_WAS_DEFINED)
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
//#include "tool.h"

#include <list>
#include <map>
#include <vector>
using namespace std;
