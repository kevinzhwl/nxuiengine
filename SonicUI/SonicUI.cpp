// SonicUI.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "SonicUI.h"
#include "SonicImage.h"
#include "SonicWndEffect.h"
#include "ConstDefine.h"
#include "SonicString.h"
#include "SonicTextScrollBar.h"
#include "InjectTool.h"
#include "SonicPaint.h"
#include "SonicAnimation.h"

// #ifdef _DEBUG
// #pragma comment(lib, "cximageD.lib")
// #pragma comment(lib, "jpegD.lib")
// #pragma comment(lib, "pngD.lib")
// #pragma comment(lib, "zlibD.lib")
// #pragma comment(lib, "InjectToolD.lib")
// #else
#ifdef TRUE
#pragma comment(lib, "cximage.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "png.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "InjectTool.lib")
#endif

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "winmm.lib")

// The following symbol used to force inclusion of this module for _USRDLL
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif  

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_hDll = (HINSTANCE)hModule;
	}
    return TRUE;
}

CSonicUI g_UI;
HINSTANCE g_hDll;

HWND CSonicUI::m_hWnd = NULL;
HWND_TO_DATA CSonicUI::m_mapHwndToData;
LIST_SONIC_BASE CSonicUI::m_CreatedList;
CRect CSonicUI::m_rtUpdate;
BOOL CSonicUI::m_bErase = FALSE;
BOOL CSonicUI::m_bPainting = FALSE;
int CSonicUI::m_nRet = 0;

const char g_ErrorInfo[][256] = {
	"没有错误",
	"未知错误",
	"错误的字符串语法",
	"无效的组件对象Id",
};

ISonicUI * GetSonicUI()
{
	BOOL bRet = FALSE;
	if(CSonicUI::m_hWnd == NULL)
	{
		// Initialization
		__try
		{			
			if(CSonicUI::Init() == FALSE)
			{
				__leave;
			}
			if(CSonicWndEffect::Init() == FALSE)
			{
				__leave;
			}
			if(CSonicImage::Init() == FALSE)
			{
				__leave;
			}
			if(CSonicString::Init() == FALSE)
			{
				__leave;
			}
			bRet = TRUE;
		}
		__finally
		{
			if(bRet)
			{
				CSonicUI::m_hWnd = CreateWindow(MY_WND, NULL, WS_POPUP, 0, 0, 1, 1, NULL, NULL, NULL, NULL);
				g_UI.CreateTip();
				SetTimer(CSonicUI::m_hWnd, TIMER_BASE_DATA, GIF_INTERVAL, NULL);
			}
		}
	}
	else
	{
		bRet = TRUE;
	}
	if(!bRet)
	{
		return NULL;
	}
	return &g_UI;
}

const DWORD * ISonicUI::GetObjectTypePtr(const ISonicBase * pBase)
{
	return &pBase->m_dwType;
}

//////////////////////////////////////////////////////////////////////////
// CSonicUI

LPVOID CSonicUI::m_pOldBeginPaint = NULL;
LPVOID CSonicUI::m_pOldEndPaint = NULL;
LPVOID CSonicUI::m_pOldGetWindowRect = NULL;
LPVOID CSonicUI::m_pOldGetClientRect = NULL;
HDC CSonicUI::m_hPaintDC = NULL;

CSonicUI::CSonicUI()
{
	m_pOwnCapture = NULL;
	m_nRet = 0;
}

CSonicUI::~CSonicUI()
{

}

LRESULT CALLBACK CSonicUI::InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
		{
			if(wParam == TIMER_BASE_DATA)
			{
				if(ISonicBaseData::m_TimerList.size())
				{
					LIST_BASE_DATA tempList = ISonicBaseData::m_TimerList;
					for(LIST_BASE_DATA::iterator it = tempList.begin(); it != tempList.end(); it++)
					{
						ISonicBaseData * pData = *it;
						pData->OnInternalTimer();
					}
				}
			}
		}
		break;
	case WM_SONIC_INTERNAL_EVENT:
		{
			ISonicBase * pBase = (ISonicBase *)wParam;
			if(!g_UI.IsValidObject(pBase))
			{
				break;
			}
			ISonicBaseData * pData = dynamic_cast<ISonicBaseData *>(pBase);
			if(pData)
			{	
				pData->OnDelegate((UINT)lParam);
			}
		}
		break;
	case WM_SONIC_DETACH:
		{
			g_UI.Detach((ISonicBase *)wParam);
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

ISonicImage * CSonicUI::CreateImage()
{
	m_CreatedList.push_back(new CSonicImage);
	return (ISonicImage *)m_CreatedList.back();
}

ISonicWndEffect * CSonicUI::CreateWndEffect()
{
	m_CreatedList.push_back(new CSonicWndEffect);
	return (ISonicWndEffect *)m_CreatedList.back();
}

ISonicString * CSonicUI::CreateString()
{
	m_CreatedList.push_back(new CSonicString);
	return (ISonicString *)m_CreatedList.back();
}

ISonicTextScrollBar * CSonicUI::CreateTextScrollBar()
{
	m_CreatedList.push_back(new CSonicTextScrollBar);
	return (ISonicTextScrollBar *)m_CreatedList.back();
}

ISonicPaint * CSonicUI::CreatePaint()
{
	m_CreatedList.push_back(new CSonicPaint);
	return (ISonicPaint *)m_CreatedList.back();
}

ISonicAnimation * CSonicUI::CreateAnimation()
{
	m_CreatedList.push_back(new CSonicAnimation);
	return (ISonicAnimation *)m_CreatedList.back();
}

void CSonicUI::DestroyObject(ISonicBase * pBase)
{
	if(IsValidObject(pBase) == FALSE)
	{
		return;
	}
	ClearTip(pBase);
	if(m_CreatedList.size())
	{
		for(LIST_SONIC_BASE::iterator it = m_CreatedList.begin(); it != m_CreatedList.end(); it++)
		{
			if(pBase == *it)
			{
				pBase->Clear();
				m_CreatedList.erase(it);
				*(DWORD *)GetObjectTypePtr(pBase) = 0;
				delete pBase;
				return;
			}
		}
	}
}

BOOL CSonicUI::IsValidObject(const ISonicBase * pBase)
{
	if(pBase == NULL)
	{
		return FALSE;
	}
	if(IsBadReadPtr(pBase, 4))
	{
		return FALSE;
	}
	if(*GetObjectTypePtr(pBase) > BASE_TYPE_NONE && *GetObjectTypePtr(pBase) < BASE_TYPE_END)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicUI::Attach(HWND hWnd, ISonicBase * pBase)
{
	HWND_DATA * pData = &m_mapHwndToData[hWnd];
	if(pData->pOldProc == NULL)
	{
		pData->pOldProc = (WNDPROC)(LONG_PTR)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)SonicWndProc);
		if(pData->pOldProc == NULL)
		{
			return FALSE;
		}
	}
	if(pData->hOldCursor == NULL)
	{
		pData->hOldCursor = (HCURSOR)(LONG_PTR)GetClassLong(m_hWnd, GCL_HCURSOR);
	}
	for(LIST_SONIC_BASE::iterator it = pData->BaseList.begin(); it != pData->BaseList.end(); it++)
	{
		if(*it == pBase)
		{
			return FALSE;
		}
	}
	if(*GetObjectTypePtr(pBase) == BASE_TYPE_WND_EFFECT)
	{
		if(pData->BaseList.size() && *GetObjectTypePtr(pData->BaseList.back()) == BASE_TYPE_WND_EFFECT)
		{
			return FALSE;
		}
		pData->BaseList.push_back(pBase);
	}
	else
	{
		pData->BaseList.push_front(pBase);
	}

	return TRUE;
}

BOOL CSonicUI::Detach(ISonicBase * pBase)
{
	HWND hDest = pBase->GetSafeHwnd();
	HWND_DATA * pData = g_UI.DataFromHwnd(hDest);
	if(pData == NULL || pBase == NULL)
	{
		return FALSE;
	}
	for(LIST_SONIC_BASE::iterator it = pData->BaseList.begin(); it != pData->BaseList.end(); it++)
	{
		if(*it == pBase)
		{
			pData->BaseList.erase(it);
			break;
		}
	}
	return TRUE;
}

LRESULT CALLBACK CSonicUI::SonicWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND_DATA * pData = g_UI.DataFromHwnd(hWnd);
	if(pData == NULL)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	if(pData->BaseList.size() == 0)
	{
		WNDPROC pOldProc = pData->pOldProc;
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)pOldProc);
		HWND_TO_DATA::iterator it = m_mapHwndToData.find(hWnd);
		if(it != m_mapHwndToData.end())
		{
			m_mapHwndToData.erase(it);
		}
		return CallWindowProc(pOldProc, hWnd, message, wParam, lParam);
	}
	m_nRet = 0;
	switch (message)
	{
	case WM_PAINT:
		{
			m_hPaintDC = (HDC)wParam;
		}
		break;
	case WM_ERASEBKGND:
		{
			m_bErase = TRUE;
		}
		break;
	case WM_NCDESTROY:
		{
			WNDPROC pOldProc = pData->pOldProc;
			g_UI.DestroyAttachedObject(hWnd);
			return CallWindowProc(pOldProc, hWnd, message, wParam, lParam);
		}
		break;
	}

	BOOL bRet = TRUE;
	LIST_SONIC_BASE TempList = pData->BaseList;
	for(LIST_SONIC_BASE ::iterator it = TempList.begin(); it != TempList.end(); it++)
	{
		ISonicBase * pBase = *it;
		switch (*g_UI.GetObjectTypePtr(pBase))
		{
		case BASE_TYPE_STRING:
			{
				bRet &= CSonicString::MessageFilter((CSonicString *)pBase, message, wParam, lParam);
			}
			break;
		case BASE_TYPE_WND_EFFECT:
			{
				// last block
				CSonicWndEffect * pEffect = (CSonicWndEffect *)pBase;
				if(bRet)
				{
					bRet &= pEffect->OnWndMsg(message, wParam, lParam);
				}
			}
			break;
		case BASE_TYPE_ANIMATION:
			{
				CSonicAnimation * pAni = (CSonicAnimation *)pBase;
				if(bRet)
				{
					bRet &= pAni->OnWndMsg(message, wParam, lParam);
				}
			}
			break;
		}
	}

	switch (message)
	{
	case WM_NCMOUSEMOVE:
		{
			if(pData->nMouseStatus != msNcHover)
			{
				PostMessage(hWnd, WM_SONIC_MOUSE_ENTER, 0, 0);
				pData->nMouseStatus = msNcHover;
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(pData->nMouseStatus != msHover)
			{
				PostMessage(hWnd, WM_SONIC_MOUSE_ENTER, 0, 0);
				pData->nMouseStatus = msHover;
			}
		}
		break;
	case WM_SONIC_MOUSE_ENTER:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE;
			if(pData->nMouseStatus == msNcHover)
			{
				tme.dwFlags |= TME_NONCLIENT;
			}
			tme.dwHoverTime = 0;
			TrackMouseEvent(&tme);
		}
		break;
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		{
			if(message == WM_NCMOUSELEAVE && pData->nMouseStatus == msHover)
			{
				break;
			}
			if(message == WM_MOUSELEAVE && pData->nMouseStatus == msNcHover)
			{
				break;
			}
			pData->nMouseStatus = msNormal;
			PostMessage(hWnd, WM_SONIC_MOUSE_LEAVE, 0, 0);
		}
		break;
	}
	if(bRet == FALSE)
	{
		return m_nRet;
	}
	return CallWindowProc(pData->pOldProc, hWnd, message, wParam, lParam);
}

HWND_DATA * CSonicUI::DataFromHwnd(HWND hWnd)
{
	if(hWnd == NULL || IsWindow(hWnd) == FALSE)
	{
		return NULL;
	}
	HWND_TO_DATA::iterator it = m_mapHwndToData.find(hWnd);
	if(it == m_mapHwndToData.end())
	{
		return NULL;
	}
	return &it->second;
}

ISonicWndEffect * CSonicUI::EffectFromHwnd(HWND hWnd)
{
	HWND_DATA * pData = DataFromHwnd(hWnd);
	if(pData == NULL || pData->BaseList.empty())
	{
		return FALSE;
	}
	if(*g_UI.GetObjectTypePtr(pData->BaseList.back()) == BASE_TYPE_WND_EFFECT)
	{
		return (ISonicWndEffect *)pData->BaseList.back();
	}
	return NULL;
}

void CSonicUI::DestroyAllObject()
{
	if(m_CreatedList.size())
	{
		for(LIST_SONIC_BASE::iterator it = m_CreatedList.begin(); it != m_CreatedList.end(); it++)
		{
			ISonicBase * pBase = *it;
			pBase->Clear();
			delete pBase;
		}
		m_CreatedList.clear();
	}
	if(m_mapHwndToData.size())
	{
		m_mapHwndToData.clear();
	}	
}

BOOL CSonicUI::IsUpdateLayeredWindow(HWND hWnd)
{
	if((GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == FALSE)
	{
		return FALSE;
	}
	if(GetLayeredWindowAttributes(hWnd, NULL, NULL, NULL))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CSonicUI::SetTipText(LPCTSTR lpszText, HWND hWnd, RECT &rt, ISonicBase * pBase)
{
	if(IsValidObject(pBase) == FALSE)
	{
		return FALSE;
	}
	if(m_Tooltip.GetCurrentHolder() != pBase->GetObjectId())
	{
		m_Tooltip.SetTipText(lpszText, hWnd, rt, pBase->GetObjectId());
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicUI::ClearTip(ISonicBase * pBase)
{
	if(IsValidObject(pBase) == FALSE)
	{
		return FALSE;
	}
	if(m_Tooltip.GetCurrentHolder() == pBase->GetObjectId())
	{
		m_Tooltip.ClearTip();
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicUI::CreateTip()
{
	return m_Tooltip.Create(m_hWnd);
}

LPCTSTR CSonicUI::HandleRawString(LPCTSTR lpszStr, int nType, LPCTSTR lpszUrlAttr /* = NULL */)
{
	static CString strRet;
	strRet = lpszStr;
	CString strUrl;
	int nEnd = 0;
	for(int i = 0; i < strRet.GetLength(); i++)
	{
		if(strRet.GetAt(i) == 0)
		{
			strRet.SetAt(i, _T('/'));
			strRet.Insert(i + 1, _T("def/"));
			i += 4;
			nEnd = 0;
			continue;
		}
		if(strRet.GetAt(i) == _T('/'))
		{
			if(nType & RSCT_DOUBLE)
			{
				strRet.Insert(i, _T('/'));
				i++;
			}
			if(nType & RSCT_REVERSE)
			{
				strRet.SetAt(i, _T('\\'));
			}
		}
		else if((nType & RSCT_URL) && !nEnd && (strRet.GetAt(i) == _T('h') || strRet.GetAt(i) == _T('w')))
		{
			int nStart = -1;
			int nUrlHead = 0;
			if(strRet.GetAt(i) == _T('h'))
			{
				nUrlHead = 5;
				strUrl = strRet.Mid(i, nUrlHead);
				if(strUrl.CompareNoCase(_T("http:")) == 0)
				{
					nStart = i;
				}
			}
			if(strRet.GetAt(i) == _T('w'))
			{
				nUrlHead = 4;
				strUrl = strRet.Mid(i, nUrlHead);
				if(strUrl.CompareNoCase(_T("www.")) == 0)
				{
					nStart = i;
				}
			}
			if(nStart >= 0)
			{
				nEnd = strRet.GetLength();
				for(int j = nStart; j < strRet.GetLength(); j++)
				{
					TCHAR c = strRet.GetAt(j);
					if(c <= 13 || c == _T(',') || c == _T(' ') || c == _T(';') || c == _T('@'))
					{
						nEnd = j;
						break;
					}
				}
				strUrl = strRet.Mid(nStart, nEnd - nStart);
				strRet.Insert(nEnd, _T('\0'));
				CString strInsert;
				strInsert.Format(_T("/a='%s'"), strUrl);
				if(lpszUrlAttr)
				{
					strInsert += _T(", ");
					strInsert += lpszUrlAttr;
				}
				strInsert += _T("/");
				strRet.Insert(nStart, strInsert);
				i += strInsert.GetLength() - 1 + nUrlHead;
				continue;
			}
		}
	}
	return strRet;
}

LRESULT CSonicUI::CallOldProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND_DATA * pData = DataFromHwnd(hWnd);
	if(pData == NULL || pData->pOldProc == NULL)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return CallWindowProc(pData->pOldProc, hWnd, message, wParam, lParam);
}

BOOL CSonicUI::Init()

{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CSonicUI::InternalWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= MY_WND;
	wcex.hIconSm		= NULL;
	if(!RegisterClassEx(&wcex))
	{
		return FALSE;
	}
	HMODULE hMod = GetModuleHandle(_T("User32.dll"));
	if(hMod == NULL)
	{
		return FALSE;
	}
	m_pOldBeginPaint = ReplaceFuncAndCopy(GetProcAddress(hMod, "BeginPaint"), MyBeginPaint);
	m_pOldEndPaint = ReplaceFuncAndCopy(GetProcAddress(hMod, "EndPaint"), MyEndPaint);
	if(m_pOldEndPaint == NULL || m_pOldEndPaint == NULL)
	{
		return FALSE;
	}
	m_pOldGetWindowRect = ReplaceFuncAndCopy(GetProcAddress(hMod, "GetWindowRect"), MyGetWindowRect);
	m_pOldGetClientRect = ReplaceFuncAndCopy(GetProcAddress(hMod, "GetClientRect"), MyGetClientRect);
	if(m_pOldGetClientRect == NULL || m_pOldGetWindowRect == NULL)
	{
		return FALSE;
	}
	return TRUE;
}

HDC CSonicUI::MyBeginPaint( HWND hwnd, LPPAINTSTRUCT lpPaint )
{
	HDC hdc;
	if(m_hPaintDC)
	{
		memset(lpPaint, 0, sizeof(PAINTSTRUCT));
		lpPaint->hdc = m_hPaintDC;
		GetClientRect(hwnd, &lpPaint->rcPaint);
		hdc = m_hPaintDC;
		g_UI.m_rtUpdate = lpPaint->rcPaint;
	}
	else
	{
		GetUpdateRect(hwnd, g_UI.m_rtUpdate, FALSE);
		__asm
		{
			push [ebp + 0ch]
			push [ebp + 8h]
			call [m_pOldBeginPaint]
			mov [hdc], eax
		}
	}
	g_UI.m_bPainting = TRUE;
	return hdc;
}

BOOL CSonicUI::MyEndPaint( HWND hWnd, CONST PAINTSTRUCT *lpPaint )
{
	BOOL bRet = TRUE;
	HWND_DATA * pData = DataFromHwnd(hWnd);
	if(pData)
	{
		for(LIST_SONIC_BASE::iterator it = pData->BaseList.begin(); it != pData->BaseList.end(); it++)
		{
			ISonicBase * pBase = *it;
			switch (pBase->GetType())
			{
			case BASE_TYPE_WND_EFFECT:
				{
					CSonicWndEffect * pEffect = (CSonicWndEffect *)pBase;
					pEffect->FinalEffect(lpPaint->hdc);
				}
				break;
			case BASE_TYPE_TEXT_SCROLL_BAR:
			case BASE_TYPE_ANIMATION:
				{
					ISonicBaseCtrl * pCtrl = dynamic_cast<ISonicBaseCtrl *>(pBase);
					if(pCtrl)
					{
						pCtrl->Render(lpPaint->hdc);
						pCtrl->SwitchRender(TRUE);
					}
				}
				break;
			}
			
		}
		pData->RedrawList.clear();
	}
	if(m_hPaintDC)
	{
		m_hPaintDC = NULL;
		return TRUE;
	}
	else
	{
		__asm
		{
			push [ebp + 0ch]
			push [ebp + 8h]
			call [m_pOldEndPaint]
			mov [bRet], eax
		}
	}
	GetClientRect(hWnd, g_UI.m_rtUpdate);
	g_UI.m_bPainting = FALSE;
	return bRet;
}

BOOL CSonicUI::MyGetWindowRect(HWND hWnd, LPRECT lpRect)
{
	BOOL bRet = TRUE;
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(hWnd);
	if(pEffect && pEffect->IsAlphaPerPixel())
	{
		SetRect(lpRect, pEffect->m_rtCache.left, pEffect->m_rtCache.top, pEffect->m_rtCache.right, pEffect->m_rtCache.bottom);
	}
	else
	{
		__asm
		{
			push [ebp + 0ch]
			push [ebp + 8h]
			call [m_pOldGetWindowRect]
			mov [bRet], eax
		}
	}
	return bRet;
}

BOOL CSonicUI::MyGetClientRect(HWND hWnd, LPRECT lpRect)
{
	BOOL bRet = TRUE;
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(hWnd);
	if(pEffect && pEffect->IsAlphaPerPixel())
	{
		SetRect(lpRect, 0, 0, pEffect->m_rtCache.Width(), pEffect->m_rtCache.Height());
	}
	else
	{
		__asm
		{
			push [ebp + 0ch]
			push [ebp + 8h]
			call [m_pOldGetClientRect]
			mov [bRet], eax
		}
	}
	return bRet;
}

BOOL CSonicUI::IsCursorInWnd(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	return DefWindowProc(hWnd, WM_NCHITTEST, NULL, MAKELPARAM(pt.x, pt.y)) != 0;
}

BOOL CSonicUI::RectInRect(LPRECT pRect1, LPRECT pRect2)
{
	if(pRect2->left >= pRect1->left && pRect2->top >= pRect1->top && pRect2->right <= pRect1->right && pRect2->bottom <= pRect1->bottom)
	{
		if(EqualRect(pRect1, pRect2) == FALSE)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CSonicUI::SetCursorHand(HWND hWnd, BOOL b)
{
	HWND_DATA * pData = DataFromHwnd(hWnd);
	if(pData == NULL)
	{
		return;
	}
	if(b)
	{
		pData->nHandCount++;
	}
	else
	{
		pData->nHandCount--;
		pData->nHandCount = pData->nHandCount < 0 ? 0 : pData->nHandCount;
	}
	if(pData->nHandCount)
	{
		SetClassLong(hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)LoadCursor(NULL , IDC_HAND));
		SendMessage(hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}
	else
	{
		SetClassLong(hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)pData->hOldCursor);
		SendMessage(hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}
}

void CSonicUI::SetCapture(ISonicBase * pBase)
{
	m_pOwnCapture = pBase;
	::SetCapture(pBase->GetSafeHwnd());
}

void CSonicUI::ReleaseCapture()
{
	::ReleaseCapture();
	m_pOwnCapture = NULL;
}

ISonicBase * CSonicUI::GetCapture()
{
	return m_pOwnCapture;
}

void CSonicUI::DestroyAttachedObject(HWND hWnd)
{
	HWND_DATA * pData = DataFromHwnd(hWnd);
	if(pData == NULL)
	{
		return;
	}
	LIST_SONIC_BASE TempList = pData->BaseList;
	for(LIST_SONIC_BASE::iterator it = TempList.begin(); it != TempList.end(); it++)
	{
		DestroyObject(*it);
	}
}

void CSonicUI::SetCallWndProcRet(int nRet)
{
	m_nRet = nRet;
}

BOOL CSonicUI::Redraw(ISonicPaint * pPaint, RECT * pRt /* = NULL */, BOOL bErase /* = TRUE */)
{
	if(pPaint == NULL || pPaint->IsValid() == FALSE)
	{
		return FALSE;
	}
	if(pPaint->GetSafeHwnd() == NULL)
	{
		return FALSE;
	}
	HWND_DATA * pData = DataFromHwnd(pPaint->GetSafeHwnd());
	if(pData == NULL)
	{
		return FALSE;
	}
	REDRAW_ITEM item;
	item.pPaint = pPaint;
	if(pRt)
	{
		item.rt = *pRt;
	}
	else
	{
		item.rt = *pPaint->GetPaintRect();
	}
	pData->RedrawList.push_back(item);
	InvalidateRect(pPaint->GetSafeHwnd(), &item.rt, bErase);
	return TRUE;

}