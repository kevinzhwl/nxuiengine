// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�

#if defined(_DEBUG)
#define DEBUG_WAS_DEFINED
#undef _DEBUG
#endif

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>

#if defined(DEBUG_WAS_DEFINED)
#define _DEBUG
#endif

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "ISonicUI.h"
using namespace sonic_ui;
