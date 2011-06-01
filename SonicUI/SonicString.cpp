#include "StdAfx.h"
#include ".\sonicstring.h"
#include "SonicUI.h"
#include "ConstDefine.h"
#include "SSE.h"

HFONT CSonicString::m_hFont = NULL;
CSonicString::MAP_STR_TO_FUNC CSonicString::m_mapFunc;

CSonicString::CSonicString(void)
{
	m_dwType = BASE_TYPE_STRING;
	InitValue();
}

CSonicString::~CSonicString(void)
{
	Clear();
}

void CSonicString::InitValue()
{
	m_nCapture = 0;
	memset(&m_Effect, 0, sizeof(m_Effect));
	m_nLineAlign = 0;
	m_nGifCount = 0;
	m_nLastStatus = 0;
	m_nStatus = BTN_STATUS_NORMAL;
	m_nSingleLine = 0;
	m_nBrOffset = 0;
	m_nLineWidth = 0;
	m_nLineSpace = 0;
	m_rtString.SetRect(0, 0, 0, 0);
	m_strNake.Empty();
	m_BlockList.clear();
	m_Link.Clear();
	m_strText.Empty();
	m_strTip.Empty();
	m_GlobalBlock.nType = 0;
	m_bInitNormal = FALSE;
	m_pOrigin = NULL;
	m_pTextCache = NULL;
}

void CSonicString::Clear()
{
	if(IsValid() == FALSE)
	{
		return;
	}
	ShowString(FALSE, FALSE);
	Detach();
	if(m_pOrigin)
	{
		g_UI.DestroyObject(m_pOrigin);
		m_pOrigin = NULL;
	}
	if(m_pTextCache)
	{
		g_UI.DestroyObject(m_pTextCache);
		m_pTextCache = NULL;
	}
	g_UI.ClearTip(this);
	ClearBaseData();
	InitValue();	
}

BOOL CSonicString::Init()
{
	LOGFONT logFont = {0};
	logFont.lfCharSet = GB2312_CHARSET;
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfQuality = DRAFT_QUALITY;
	lstrcpy(logFont.lfFaceName, _T("ËÎÌו"));
	m_hFont = CreateFontIndirect(&logFont);
	if(m_hFont == NULL)
	{
		return FALSE;
	}

	m_mapFunc[_T("a")] = &CSonicString::Ctrl_a;
	m_mapFunc[_T("def")] = &CSonicString::Ctrl_def;
	m_mapFunc[_T("c")] = &CSonicString::Ctrl_c;
	m_mapFunc[_T("linkl")] = &CSonicString::Ctrl_linkl;
	m_mapFunc[_T("linkt")] = &CSonicString::Ctrl_linkt;
	m_mapFunc[_T("linkc")] = &CSonicString::Ctrl_linkc;
	m_mapFunc[_T("linkh")] = &CSonicString::Ctrl_linkh;
	m_mapFunc[_T("p")] = &CSonicString::Ctrl_p;
	m_mapFunc[_T("ph")] = &CSonicString::Ctrl_ph;
	m_mapFunc[_T("pc")] = &CSonicString::Ctrl_pc;
	m_mapFunc[_T("key")] = &CSonicString::Ctrl_key;
	m_mapFunc[_T("btn_type")] = &CSonicString::Ctrl_btn_type;
	m_mapFunc[_T("pClip")] = &CSonicString::Ctrl_pClip;
	m_mapFunc[_T("line_width")] = &CSonicString::Ctrl_line_width;
	m_mapFunc[_T("single_line")] = &CSonicString::Ctrl_single_line;
	m_mapFunc[_T("align")] = &CSonicString::Ctrl_align;
	m_mapFunc[_T("animation")] = &CSonicString::Ctrl_animation;
	m_mapFunc[_T("br_offset")] = &CSonicString::Ctrl_br_offset;
	m_mapFunc[_T("font")] = &CSonicString::Ctrl_font;
	m_mapFunc[_T("font_height")] = &CSonicString::Ctrl_font_height;
	m_mapFunc[_T("font_bold")] = &CSonicString::Ctrl_font_bold;
	m_mapFunc[_T("font_italic")] = &CSonicString::Ctrl_font_italic;
	m_mapFunc[_T("font_strikeout")] = &CSonicString::Ctrl_font_strikeout;
	m_mapFunc[_T("font_face")] = &CSonicString::Ctrl_font_face;
	m_mapFunc[_T("sparkle")] = &CSonicString::Ctrl_sparkle;
	m_mapFunc[_T("sparkle_color")] = &CSonicString::Ctrl_sparkle_color;
	m_mapFunc[_T("sparkle_interval")] = &CSonicString::Ctrl_sparkle_interval;
	m_mapFunc[_T("sparkle_timeout")] = &CSonicString::Ctrl_sparkle_timeout;
	m_mapFunc[_T("fadeout")] = &CSonicString::Ctrl_fadeout;
	m_mapFunc[_T("fadeout_max")] = &CSonicString::Ctrl_fadeout_max;
	m_mapFunc[_T("fadeout_min")] = &CSonicString::Ctrl_fadeout_min;
	m_mapFunc[_T("fadeout_speed")] = &CSonicString::Ctrl_fadeout_speed;
	m_mapFunc[_T("line_space")] = &CSonicString::Ctrl_line_space;
	m_mapFunc[_T("global")] = &CSonicString::Ctrl_global;
	m_mapFunc[_T("pass_msg")] = &CSonicString::Ctrl_pass_msg;
	m_mapFunc[_T("ani_num")] = &CSonicString::Ctrl_ani_num;
	m_mapFunc[_T("ani_num_from")] = &CSonicString::Ctrl_ani_num_from;
	m_mapFunc[_T("ani_num_to")] = &CSonicString::Ctrl_ani_num_to;
	m_mapFunc[_T("ani_num_frame")] = &CSonicString::Ctrl_ani_num_frame;
	m_mapFunc[_T("init_normal")] = &CSonicString::Ctrl_init_normal;
	return TRUE;
}

HFONT CSonicString::GetDefSonicFont()
{
	return m_hFont;
}

BOOL CSonicString::Format(LPCTSTR lpszArg, ...)
{
	TCHAR message[1024];
	va_list argList;
	va_start(argList, lpszArg);
	_vsntprintf(message, sizeof(message), lpszArg, argList);
	va_end(argList);

	Clear();
	m_strNake = message;
	return Translate();
}

BOOL CSonicString::Translate()
{
	if(m_strNake.IsEmpty())
	{
		return FALSE;
	}
	BOOL bRet = TRUE;
	try
	{
		CString strStuff = m_strNake;
		int nCurrentLine = 1;
		while (!strStuff.IsEmpty())
		{
			STRING_BLOCK block;
			block.nLine = nCurrentLine;
			if(strStuff.GetAt(0) != _T('/'))
			{
				strStuff = "/def/" + strStuff;
			}

			// process ctrl text
			int nFind = -1;
			for(int i = 1; i < strStuff.GetLength(); i++)
			{
				if(strStuff.GetAt(i) == _T('/'))
				{
					nFind = i;
					break;
				}
				if(strStuff.GetAt(i) == '\'')
				{
					int nOtherQuote = strStuff.Find('\'', i + 1);
					if(nOtherQuote < 0)
					{
						throw CErrorContext(ERROR_ID_STRING_FORMAT);
					}
					i = nOtherQuote;
				}
			}
			if(nFind <= 1)
			{
				throw CErrorContext(ERROR_ID_STRING_FORMAT);
			}			
			CString strCtrl = strStuff.Mid(1, nFind - 1);
			strStuff = strStuff.Mid(nFind + 1);

			// here has already retrieved ctrl text
 			while(strCtrl.IsEmpty() == FALSE)
			{
				CString strOne;
				int nFind = -1;
				for(int i = 0; i < strCtrl.GetLength(); i++)
				{
					if(strCtrl.GetAt(i) == ',')
					{
						nFind = i;
						break;
					}
					if(strCtrl.GetAt(i) == '\'')
					{
						int nOtherQuote = strCtrl.Find('\'', i + 1);
						if(nOtherQuote < 0)
						{
							throw CErrorContext(ERROR_ID_STRING_FORMAT);
						}
						i = nOtherQuote;
					}
				}
				if(nFind > 0)
				{
					strOne = strCtrl.Left(nFind);
					strCtrl = strCtrl.Mid(nFind + 1);
					strCtrl.TrimLeft();
				}
				else
				{
					strOne = strCtrl;
					strCtrl.Empty();
				}
				
				nFind = strOne.Find('=');
				if(nFind > 0)
				{
					MAP_STR_TO_FUNC::iterator it = m_mapFunc.find((LPCTSTR)strOne.Left(nFind));
					if(it != m_mapFunc.end())
					{
						(this->*(it->second))(strOne.Mid(nFind + 1), block);
					}
					else
					{
						throw CErrorContext(ERROR_ID_STRING_FORMAT);
					}
				}
				else
				{
					MAP_STR_TO_FUNC::iterator it = m_mapFunc.find((LPCTSTR)strOne);
					if(it != m_mapFunc.end())
					{
						(this->*(it->second))(NULL, block);
					}
					else
					{
						throw CErrorContext(ERROR_ID_STRING_FORMAT);
					}
				}
			}

			if(!(block.nType & BLOCK_TYPE_IMAGE) && !(block.nType & BLOCK_TYPE_GLOBAL) && !(block.nType & BLOCK_TYPE_ANI_NUM))
			{
				// process text
				BOOL bDoubleSlash = FALSE;
				int i;
				for(i = 0; i < strStuff.GetLength(); i++)
				{
					if(strStuff.GetAt(i) == _T('\n'))
					{
						break;
					}
					if(strStuff.GetAt(i) == _T('/'))
					{
						if(strStuff.GetAt(i + 1) == _T('/'))
						{
							bDoubleSlash = TRUE;
							i++;
							continue;
						}
						else
						{
							break;
						}
					}
				}
				if(i)
				{
					block.strText = strStuff.Left(i);
					if(bDoubleSlash)
					{
						block.strText.Replace(_T("//"), _T("/"));
					}
					m_strText += block.strText;
				}
				else
				{
					// not text block, just attributes ctrl block, no need to add enter the list
					if(!(block.nType & BLOCK_TYPE_GLOBAL))
					{
						block.nType = BLOCK_TYPE_NONE;
					}
				}
				if(strStuff.GetAt(i) == _T('\n'))
				{
					block.nType |= BLOCK_TYPE_CHAR;
					nCurrentLine++;
					i++;
				}
				strStuff = strStuff.Mid(i);
			}
			
			// before insert
			if(block.nType & BLOCK_TYPE_ANI_NUM)
			{
				int nNum = abs(m_Effect.ani_num.nFrom) > abs(m_Effect.ani_num.nTo) ? m_Effect.ani_num.nFrom : m_Effect.ani_num.nTo;
				block.strText.Format(_T("%d"), nNum);
			}
			
			// insert
			if(block.nType & BLOCK_TYPE_GLOBAL)
			{
				m_GlobalBlock = block;
				m_GlobalBlock.nType &= ~BLOCK_TYPE_GLOBAL;
			}
			else if(block.nType)
			{
				m_BlockList.push_back(block);
			}
		}
	}
	catch (CErrorContext & err)
	{
		STRING_BLOCK block;
		block.strText = g_ErrorInfo[err.nErrorId];
		m_BlockList.clear();
		m_BlockList.push_back(block);
		bRet = FALSE;
	}
	catch (...)
	{
		STRING_BLOCK block;
		block.strText = g_ErrorInfo[ERROR_ID_UNKNOWN];
		m_BlockList.clear();
		m_BlockList.push_back(block);
		bRet = FALSE;
	}

	HDC hdc = GetDC(CSonicUI::m_hWnd);
	HFONT hOldFont = (HFONT)SelectObject(hdc, CSonicString::GetDefSonicFont());
	if(!CalculateBlockRect(hdc))
	{
		Clear();
		bRet = FALSE;
	}
	SelectObject(hdc, hOldFont);
	ReleaseDC(CSonicUI::m_hWnd, hdc);

	m_pOrigin = g_UI.CreatePaint();
	m_pOrigin->Create(FALSE, m_rtString.Width(), m_rtString.Height());
	m_pOrigin->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicString::OnRender);
	return bRet;
}

BOOL CSonicString::CalculateBlockRect(HDC hdc)
{
	int x = 0;
	int y = 0;
	int nMaxWidth = 0;
	int nMaxHeight = 0;
	int nLinePlus = 0;
	int nCurrentLine = 1;
	int nCurrentBrOffset = 0;

	vector<int>	LineHeightVec;

	STRING_BLOCK_LIST::iterator it;
	for(it = m_BlockList.begin(); it != m_BlockList.end(); it++)
	{
		STRING_BLOCK &block = *it;
		if(block.nType == BLOCK_TYPE_NONE)
		{
			return FALSE;
		}
		block.nLine += nLinePlus;
		if(block.nLine > nCurrentLine)
		{
			LineHeightVec.push_back(nMaxHeight - y);
			// encounter an new line, but single line attribute set, just ignore the rest blocks
			if(m_nSingleLine)
			{
				it = m_BlockList.erase(it);
				it--;
				continue;
			}
			nCurrentLine = block.nLine;
            x = nCurrentBrOffset;
			y = nMaxHeight + m_nLineSpace;
			nCurrentBrOffset = 0;
		}
		if(block.nType & BLOCK_TYPE_CHAR)
		{
			SIZE sz;
			int nBlockWidth = 0;
			block.Font.SwitchFont(hdc);
			if(m_nLineWidth)
			{
				int nMaxBlockChar = 0;
				GetTextExtentExPoint(hdc, block.strText, block.strText.GetLength(), m_nLineWidth - x, &nMaxBlockChar, NULL, &sz);
				nBlockWidth = sz.cx;
				if(nMaxBlockChar < block.strText.GetLength())
				{
					if(x == m_nBrOffset && nMaxBlockChar == 0)
					{
						// not enough width to hold even one character
						return FALSE;
					}
					// need to split into more lines
					nCurrentBrOffset = m_nBrOffset;
					STRING_BLOCK newBlock = block;
					newBlock.nLine = block.nLine - nLinePlus;
					newBlock.strText = block.strText.Mid(nMaxBlockChar);
					block.strText = block.strText.Left(nMaxBlockChar);
					if(m_nSingleLine == 2)
					{
						block.strText += "...";
						m_strTip = m_strText;
					}
					SIZE szAdjust;
					GetTextExtentPoint32(hdc, block.strText, block.strText.GetLength(), &szAdjust);
					nBlockWidth = szAdjust.cx;
					it = m_BlockList.insert(++it, newBlock);
					it--;
					nLinePlus++;
				}
			}
			else
			{
				if(block.strText.IsEmpty())
				{
					GetTextExtentPoint32(hdc, _T(" "), 1, &sz);
					nBlockWidth = 0;
				}
				else
				{
					GetTextExtentPoint32(hdc, block.strText, block.strText.GetLength(), &sz);
					nBlockWidth = sz.cx;
				}
			}
			block.Font.Restore();
			block.rtBlock.SetRect(x, y, x + nBlockWidth, y + sz.cy);
			
		}
		if(block.nType & BLOCK_TYPE_IMAGE)
		{
			if(block.Image.pNormal == block.Image.pHover && block.Image.pHover == block.Image.pClick)
			{					
				block.Image.dp.dwMask |= DP_SRC_CLIP;
				SetRect(&block.Image.dp.rtSrc, 0, 0, block.Image.pNormal->GetWidth() / 3, block.Image.pNormal->GetHeight());
			}
            block.rtBlock = block.Image.pNormal->CalculateRectByDrawParam(x, y, &block.Image.dp);
			if(m_nLineWidth)
			{
				if(block.rtBlock.Width() > m_nLineWidth - m_nBrOffset)
				{
					// even one whole line cannot hold the picture
					return FALSE;
				}
				if(block.rtBlock.right > m_nLineWidth)
				{
					// picture need to move to the next line
					it--;
					nLinePlus++;
					continue;
				}
			}
		}
		// add 2 extra pixel for the underline
		block.rtBlock.bottom += 2;
		x += block.rtBlock.Width();

		// extend the string rect
		if(block.rtBlock.right > nMaxWidth)
		{
			nMaxWidth = block.rtBlock.right;
		}
		if(block.rtBlock.bottom > nMaxHeight)
		{
			nMaxHeight = block.rtBlock.bottom;
		}
	}
	// the last line
	LineHeightVec.push_back(nMaxHeight - y);

	if(m_nLineAlign)
	{
		// adjust the line align
		for(it = m_BlockList.begin(); it != m_BlockList.end(); it++)
		{
			STRING_BLOCK &block = *it;
			if(m_nSingleLine && block.nLine > 1)
			{
				break;
			}
			int nOffsetY = 0;
			if(m_nLineAlign == 1)
			{
				// mid align
				nOffsetY = (LineHeightVec[block.nLine - 1] - block.rtBlock.Height()) / 2;
			}
			else if(m_nLineAlign == 2)
			{
				// bottom align
				nOffsetY = LineHeightVec[block.nLine - 1] - block.rtBlock.Height();
			}
			block.rtBlock.OffsetRect(0, nOffsetY);
		}
	}
	m_rtString.SetRect(0, 0, nMaxWidth, nMaxHeight);
	return TRUE;
}

BOOL CSonicString::TextOut(HDC hdc, int x, int y, HWND hWnd /* = NULL */, RECT * pMemDCRect /* = NULL */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_hWnd == NULL)
	{
		if(hWnd)
		{
			Attach(hWnd);
		}
		else 
		{
			Attach(WindowFromDC(hdc));
		}
	}

	if(m_rtString.left != x || m_rtString.top != y)
	{
		g_UI.ClearTip(this);
	}
	m_pOrigin->Draw(hdc, x, y, hWnd, pMemDCRect);
	m_rtString = m_pOrigin->GetPaintRect();
	return TRUE;
}

BOOL CSonicString::Render(ISonicPaint * pPaint)
{
	CURRENT_PAINT * pCurrPaint = pPaint->GetCurrentPaint();
	if(pCurrPaint == NULL)
	{
		return FALSE;
	}
	RenderImage(pPaint, NULL);
	int nAlpha = -1;
	if(m_pTextCache->GetBits())
	{
		nAlpha = (m_Effect.dwMask & STRING_EFFECT_FADEOUT) ? m_Effect.fadeout.nCurrAlpha : 255;		
	}
	m_pTextCache->Draw(pCurrPaint->hdc, pCurrPaint->x, pCurrPaint->y, pCurrPaint->hWnd, &pCurrPaint->rtMem, nAlpha);
	return TRUE;
}

BOOL CSonicString::IsValid()
{
	return (m_strNake.IsEmpty() == FALSE);
}

BOOL CSonicString::OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(m_Link.nLinkCount == 0)
	{
		// not a button, let the ui message pass through
		return TRUE;
	}

	if(g_UI.GetCapture() && g_UI.GetCapture() != this)
	{
		return TRUE;
	}
	// mouse message below
	switch (msg)
	{
	case WM_MOUSELEAVE:
		return OnMouseLeave();
	case WM_MOUSEMOVE:
		return OnMouseMove(wParam, lParam);
	case WM_LBUTTONDOWN:
		return OnLButtonDown(wParam, lParam);;
	case WM_LBUTTONUP:
		return OnLButtonUp(wParam, lParam);
	case WM_LBUTTONDBLCLK:
		return OnLButtonDblClick(wParam, lParam);
	case WM_RBUTTONDOWN:
		return OnRButtonDown(wParam, lParam);
	case WM_RBUTTONUP:
		return OnRButtonUp(wParam, lParam);
	case WM_RBUTTONDBLCLK:
		return OnRButtonDblClick(wParam, lParam);
	case WM_MOUSEWHEEL:
		return OnMouseWheel(wParam, lParam);
	}
	return TRUE;
}

BOOL CSonicString::Attach(HWND hWnd)
{
	if(m_hWnd || IsValid() == FALSE)
	{
		return FALSE;
	}
	if(hWnd == NULL || IsWindow(hWnd) == FALSE)
	{
		return FALSE;
	}
	m_hWnd = hWnd;
	if(m_bInitNormal == FALSE)
	{
		if(m_Effect.dwMask & STRING_EFFECT_SPARKLE)
		{
			AnimateSparkle(TRUE);
		}
		if(m_Effect.dwMask & STRING_EFFECT_FADEOUT)
		{
			AnimateFadeout(TRUE);
		}
		if(m_Effect.dwMask & STRING_EFFECT_ANI_NUM)
		{
			AnimateAniNum(TRUE);
		}
	}
	if(m_nGifCount)
	{
		AnimateGif(TRUE);
	}
	PrepareTextCache();
	return g_UI.Attach(hWnd, this);
}

int CSonicString::GetWidth()
{
	return m_rtString.Width();
}

int CSonicString::GetHeight()
{
	return m_rtString.Height();
}

BOOL CSonicString::OnMouseLeave()
{
	if(m_nStatus == BTN_STATUS_HOVER || (m_Link.nBtnType == BTN_TYPE_QUICK && m_nStatus != BTN_STATUS_NORMAL))
	{
		FireDelegate(DELEGATE_EVENT_MOUSELEAVE);
		ChangeStatus(BTN_STATUS_NORMAL);
	}
	return TRUE;
}

BOOL CSonicString::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInLink(pt))
	{
		if(m_nStatus == BTN_STATUS_NORMAL)
		{
			FireDelegate(DELEGATE_EVENT_MOUSEENTER);
			ChangeStatus(BTN_STATUS_HOVER);
		}
		else
		{
			FireDelegate(DELEGATE_EVENT_MOUSEHOVER);
		}
	}
	else
	{
		if(m_nStatus == BTN_STATUS_HOVER || (m_Link.nBtnType == BTN_TYPE_QUICK && m_nStatus != BTN_STATUS_NORMAL))
		{
			FireDelegate(DELEGATE_EVENT_MOUSELEAVE);
			ChangeStatus(BTN_STATUS_NORMAL);
		}
	}
	return TRUE;
}

BOOL CSonicString::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(m_nStatus == BTN_STATUS_CLICK)
	{
		if(m_Link.nBtnType == BTN_TYPE_QUICK)
		{
			ChangeStatus(BTN_STATUS_HOVER);
			return m_Link.nPassMsg;
		}
		g_UI.ReleaseCapture();
		if(PtInLink(pt))
		{
			FireDelegate(DELEGATE_EVENT_CLICK);

			int nStatus = BTN_STATUS_HOVER;
			if(m_Link.nBtnType == BTN_TYPE_CHECK)
			{
				m_Link.bCheck = !m_Link.bCheck;
				nStatus = m_Link.bCheck ? BTN_STATUS_CLICK : BTN_STATUS_HOVER;
			}
			ChangeStatus(nStatus);
			return m_Link.nPassMsg;
		}
		else
		{
			int nStatus = BTN_STATUS_NORMAL;
			if(m_Link.nBtnType == BTN_TYPE_CHECK)
			{
				nStatus = m_Link.bCheck ? BTN_STATUS_CLICK : BTN_STATUS_NORMAL;
			}
			ChangeStatus(nStatus);
		}
	}
	return TRUE;
}

BOOL CSonicString::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(m_nStatus != BTN_STATUS_NORMAL)
	{
		if(PtInLink(pt))
		{
			ChangeStatus(BTN_STATUS_CLICK);
			if(m_Link.nBtnType == BTN_TYPE_QUICK)
			{
				FireDelegate(DELEGATE_EVENT_CLICK);
			}
			else
			{
				g_UI.SetCapture(this);
			}
			return m_Link.nPassMsg;
		}
	}
	return TRUE;
}

BOOL CSonicString::OnLButtonDblClick(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInLink(pt))
	{
		FireDelegate(DELEGATE_EVENT_DBLCLICK);
		return m_Link.nPassMsg;
	}
	return TRUE;
}

BOOL CSonicString::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(m_nStatus != BTN_STATUS_NORMAL)
	{
		if(PtInLink(pt))
		{
			FireDelegate(DELEGATE_EVENT_RCLICK);
			return m_Link.nPassMsg;
		}
	}
	return TRUE;
}

BOOL CSonicString::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CSonicString::OnRButtonDblClick(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInLink(pt))
	{
		FireDelegate(DELEGATE_EVENT_DBLRCLICK);
		return m_Link.nPassMsg;
	}
	return TRUE;
}

BOOL CSonicString::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	ScreenToClient(m_hWnd, &pt);
	if(PtInLink(pt))
	{
		FireDelegate(DELEGATE_EVENT_MOUSEWHEEL);
		return m_Link.nPassMsg;
	}
	return TRUE;
}

BOOL CSonicString::ForceRedraw(BOOL bErase /* = FALSE */)
{
	Redraw();
	return TRUE;
}

BOOL CSonicString::MessageFilter(CSonicString * pStr, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(pStr->IsVisible() == FALSE)
	{
		return TRUE;
	}
	if(message == WM_MOUSEMOVE)
	{
		POINT pt = {LOWORD(lParam), HIWORD(lParam)};
		if(pStr->PtInString(pt) && pStr->m_strTip.IsEmpty() == FALSE)
		{
			g_UI.SetTipText(pStr->m_strTip, pStr->m_hWnd, pStr->m_rtString, pStr);
		}
	}
	return pStr->OnWndMsg(message, wParam, lParam);
}

void CSonicString::ChangeStatus(int nStatus)
{
	if(m_nStatus != nStatus)
	{
		m_nLastStatus = m_nStatus;
		m_nStatus = nStatus;
		if(m_Link.nUseHand)
		{
			if(m_nStatus == BTN_STATUS_HOVER && m_nLastStatus == BTN_STATUS_NORMAL)
			{
				g_UI.SetCursorHand(m_hWnd, TRUE);
			}
			if(m_nStatus == BTN_STATUS_NORMAL)
			{
				g_UI.SetCursorHand(m_hWnd, FALSE);
			}
		}
		if(m_Link.nAlphaStep == 0 || m_nStatus == BTN_STATUS_CLICK || (m_nStatus == BTN_STATUS_HOVER && m_nLastStatus == BTN_STATUS_CLICK))
		{
			m_Link.nCurrAlpha = -1;
			KillInternalTimer(TIMER_BUTTON_ALPHA);
			ForceRedraw();
		}
		else
		{
			m_Link.nCurrAlpha = 0;
			SetInternalTimer(TIMER_BUTTON_ALPHA, ANIMATION_INTERVAL);
		}
	}
}

void CSonicString::OnClickLink(ISonicString * pStr, DWORD dwReserve)
{
	ShellExecute(NULL, _T("open"), _T("iexplore.exe"), m_Link.strLink, NULL, SW_SHOW);
}

void CSonicString::OnInternalTimer(DWORD dwTimerId)
{
	// timer alpha
	if(dwTimerId & TIMER_BUTTON_ALPHA)
	{
		OnTimerButtonAlpha();
	}

	// timer gif
	if(dwTimerId & TIMER_GIF)
	{
		OnTimerGif();
	}

	// TIMER_SPARKLE
	if(dwTimerId & TIMER_SPARKLE)
	{
		OnTimerSparkle();
	}

	// TIMER_FADEOUT
	if(dwTimerId & TIMER_FADEOUT)
	{
		OnTimerFadeout();
	}

	if(dwTimerId & TIMER_ANI_NUM)
	{
		OnTimerAniNum();
	}
}

void CSonicString::DrawImageBlock(HDC hdc, int x, int y, STRING_BLOCK &block, int nStatus, int nAlpha /* = -1 */)
{
	if((block.nType & BLOCK_TYPE_IMAGE) == FALSE)
	{
		return;
	}
	ISonicImage * pImg = block.Image.pNormal;
	switch (nStatus)
	{
	case BTN_STATUS_NORMAL:
		{
			pImg = block.Image.pNormal;
			if(pImg == block.Image.pHover)
			{
				SetRect(&block.Image.dp.rtSrc, 0, 0, pImg->GetWidth() / 3, pImg->GetHeight());
			}
		}
		break;
	case BTN_STATUS_HOVER:
		{
			if(block.Image.pHover == NULL)
			{
				pImg = block.Image.pNormal;
			}
			else
			{
				pImg = block.Image.pHover;
				if(pImg == block.Image.pNormal)
				{
					SetRect(&block.Image.dp.rtSrc, pImg->GetWidth() / 3, 0, pImg->GetWidth() * 2 / 3, pImg->GetHeight());
				}
			}
		}
		break;
	case BTN_STATUS_CLICK:
		{
			if(block.Image.pClick == NULL)
			{
				pImg = block.Image.pNormal;
			}
			else
			{
				pImg = block.Image.pClick;
				if(pImg == block.Image.pNormal)
				{
					SetRect(&block.Image.dp.rtSrc, pImg->GetWidth() * 2 / 3, 0, pImg->GetWidth(), pImg->GetHeight());
				}
			}
		}
		break;
	}
	if(nAlpha != -1)
	{
		block.Image.dp.dwMask |= DP_ALPHA;
		block.Image.dp.cAlpha = (unsigned char)nAlpha;
	}
	else
	{
		block.Image.dp.dwMask &= ~DP_ALPHA;
	}
	pImg->Draw(hdc, x + block.rtBlock.left, y + block.rtBlock.top, &block.Image.dp);
}

BOOL CSonicString::AnimateGif(BOOL bBegin /* = TRUE */)
{
	if(m_nGifCount == 0)
	{
		return FALSE;
	}
	if(bBegin)
	{
		SetInternalTimer(TIMER_GIF, GIF_INTERVAL);
	}
	else
	{
		KillInternalTimer(TIMER_GIF);
	}
	return TRUE;
}

BOOL CSonicString::PtInLink(POINT pt)
{
	if(PtInString(pt) == FALSE)
	{
		return FALSE;
	}
	pt.x -= m_rtString.left;
	pt.y -= m_rtString.top;
	for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
	{
		STRING_BLOCK &block = *it;
		if(block.rtBlock.PtInRect(pt) && (block.nType & BLOCK_TYPE_LINK))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSonicString::PtInString(POINT pt)
{
	if(m_pOrigin == NULL)
	{
		return FALSE;
	}
	return PtInRect(m_pOrigin->GetMsgRect(), pt);
}

void CSonicString::DrawTextBlock(HDC hdc, int x, int y, STRING_BLOCK &block)
{
	SetTextColor(hdc, GetBlockTextColor(block));
	if((m_Effect.dwMask & STRING_EFFECT_SPARKLE) && m_Effect.sparkle.nSparkleSwitch)
	{
		SetTextColor(hdc, m_Effect.sparkle.nSparkleColor[m_Effect.sparkle.nCurrSparkle]);
	}
	block.Font.SwitchFont(hdc);
	::TextOut(hdc, x + block.rtBlock.left, y + block.rtBlock.top, block.strText, block.strText.GetLength());
	block.Font.Restore();
}

void CSonicString::DrawUnderLine(HDC hdc, int x, int y, STRING_BLOCK &block)
{
	HPEN pen = CreatePen(PS_SOLID, 1, GetBlockTextColor(block));
	HPEN hOldPen = (HPEN)SelectObject(hdc, pen);
	int x1 = block.rtBlock.left + x;
	int x2 = block.rtBlock.right + x;
	y += block.rtBlock.bottom - 1;
	MoveToEx(hdc, x1, y, NULL);
	LineTo(hdc, x2, y);
	SelectObject(hdc, hOldPen);
	DeleteObject(pen);
}

LPCTSTR CSonicString::GetStr()
{
	return m_strNake;
}

void CSonicString::ShowString(BOOL bShow /* = TRUE */, BOOL bRedraw /* = TRUE */)
{
	if(bShow == IsVisible())
	{
		return;
	}
	if(m_pOrigin)
	{
		m_pOrigin->ShowPaint(bShow, bRedraw);
	}
	if(bShow == FALSE)
	{
		if(m_nStatus != BTN_STATUS_NORMAL)
		{
			ChangeStatus(BTN_STATUS_NORMAL);
		}
		g_UI.ClearTip(this);
		if(g_UI.GetCapture() == this)
		{
			g_UI.ReleaseCapture();
		}
	}
}

BOOL CSonicString::IsVisible()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_pOrigin == NULL)
	{
		return FALSE;
	}
	return m_pOrigin->IsVisible();
}

BOOL CSonicString::AnimateFadeout(BOOL b /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(!(m_Effect.dwMask & STRING_EFFECT_FADEOUT))
	{
		return FALSE;
	}
	if(QueryInternalTimer(TIMER_FADEOUT) == FALSE)
	{
		if(b)
		{
			m_Effect.fadeout.nCurrAlpha = 0;
			m_Effect.fadeout.nAlphaSpeed = abs(m_Effect.fadeout.nAlphaSpeed);
			if(IsVisible() == FALSE)
			{
				ShowString(TRUE);
			}
		}
		else if(m_Effect.fadeout.nCurrAlpha)
		{
			m_Effect.fadeout.nAlphaSpeed = -abs(m_Effect.fadeout.nAlphaSpeed);
		}
		SetInternalTimer(TIMER_FADEOUT, FADEOUT_INTERVAL);
	}
	else
	{
		if(b == FALSE)
		{
			m_Effect.fadeout.nAlphaSpeed = -abs(m_Effect.fadeout.nAlphaSpeed);
		}
		else
		{
			m_Effect.fadeout.nAlphaSpeed = abs(m_Effect.fadeout.nAlphaSpeed);
		}
	}
    m_Effect.fadeout.nAlphaSwitch = b;	
	return TRUE;
}

BOOL CSonicString::AnimateSparkle(BOOL b /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if((m_Effect.dwMask & STRING_EFFECT_SPARKLE) == FALSE)
	{
		return FALSE;
	}
	m_Effect.sparkle.nSparkleSwitch = b;
	if(QueryInternalTimer(TIMER_SPARKLE) == FALSE)
	{
		if(b)
		{
			m_Effect.sparkle.nCurrTime = 0;
			m_Effect.sparkle.nCurrSparkle = 0;
			SetInternalTimer(TIMER_SPARKLE, m_Effect.sparkle.nSparkleInterval);
		}
	}
	else
	{
		if(b)
		{
			m_Effect.sparkle.nCurrTime = 0;
			m_Effect.sparkle.nCurrSparkle = 0;
		}
		else
		{
			KillInternalTimer(TIMER_SPARKLE);
			ForceRedraw();
		}
	}	
	return TRUE;
}

BOOL CSonicString::Detach()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_hWnd == NULL)
	{
		return FALSE;
	}
	g_UI.Detach(this);
	m_hWnd = NULL;
    return TRUE;
}

void CSonicString::Redraw()
{
	if(IsValid() == FALSE)
	{
		return;
	}
	m_pOrigin->Redraw();
}

DWORD CSonicString::GetBlockTextColor(STRING_BLOCK & block)
{
	if((block.nType & BLOCK_TYPE_LINK) && (m_nStatus != BTN_STATUS_NORMAL) && m_Link.dwHoverColor != (DWORD)-1)
	{
		return m_Link.dwHoverColor;
	}
	else
	{
		return block.dwColor;
	}
}

void CSonicString::PrepareTextCache()
{
	m_pTextCache = g_UI.CreatePaint();
	if(g_UI.IsUpdateLayeredWindow(m_hWnd) || (m_Effect.dwMask & STRING_EFFECT_FADEOUT))
	{
		m_pTextCache->Create(TRUE, m_rtString.Width(), m_rtString.Height());
		SetBkMode(m_pTextCache->GetDC(), TRANSPARENT);
		DeleteObject(SelectObject(m_pTextCache->GetDC(), m_hFont));
	}
	else
	{
		m_pTextCache->Create(FALSE, m_rtString.Width(), m_rtString.Height());
	}
	m_pTextCache->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicString::RenderText);
}

void CSonicString::Ctrl_a(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.nType |= BLOCK_TYPE_LINK;
	m_Link.nLinkCount++;
	if(szValue)
	{
		m_Link.strLink = szValue;
		m_Link.strLink.Trim('\'');
		Delegate(DELEGATE_EVENT_CLICK, NULL, this, &CSonicString::OnClickLink);
	}
}

void CSonicString::Ctrl_def(LPCTSTR szValue, STRING_BLOCK & block)
{
	if(m_GlobalBlock.nType)
	{
		int nLine = block.nLine;
		block = m_GlobalBlock;
		block.nLine = nLine;
	}
}

void CSonicString::Ctrl_c(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%x"), &block.dwColor);
}

void CSonicString::Ctrl_btn_type(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Link.nBtnType);
}

void CSonicString::Ctrl_line_width(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_nLineWidth);
}

void CSonicString::Ctrl_single_line(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_nSingleLine);
}

void CSonicString::Ctrl_br_offset(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_nBrOffset);
}

void CSonicString::Ctrl_p(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.nType &= ~BLOCK_TYPE_CHAR;
	block.nType |= BLOCK_TYPE_IMAGE;
	_stscanf(szValue, _T("%d"), &block.Image.pNormal);
	if(!g_UI.IsValidObject(block.Image.pNormal))
	{
		throw CErrorContext(ERROR_ID_INVALID_OBJECT_ID);
	}
	if(block.Image.pNormal->GetFrameCount())
	{
		m_nGifCount++;
	}
}

void CSonicString::Ctrl_ph(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Image.pHover);
	if(!g_UI.IsValidObject(block.Image.pHover))
	{
		throw CErrorContext(ERROR_ID_INVALID_OBJECT_ID);
	}
}

void CSonicString::Ctrl_pc(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Image.pClick);
	if(!g_UI.IsValidObject(block.Image.pClick))
	{
		throw CErrorContext(ERROR_ID_INVALID_OBJECT_ID);
	}
}

void CSonicString::Ctrl_linkl(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Link.nLinkLine);
}

void CSonicString::Ctrl_linkt(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_strTip = szValue;
	m_strTip.Trim('\'');
}

void CSonicString::Ctrl_linkh(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%x"), &m_Link.dwHoverColor);
}

void CSonicString::Ctrl_linkc(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Link.nUseHand);
}

void CSonicString::Ctrl_key(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.Image.dp.dwMask |= DP_COLOR_KEY;
	_stscanf(szValue, _T("%x"), &block.Image.dp.dwColorKey);
}

void CSonicString::Ctrl_pClip(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.Image.dp.dwMask |= DP_SRC_CLIP;
	_stscanf(szValue, _T("%d|%d|%d|%d"), &block.Image.dp.rtSrc.left, &block.Image.dp.rtSrc.top, &block.Image.dp.rtSrc.right,
		&block.Image.dp.rtSrc.bottom);
}

void CSonicString::Ctrl_animation(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Link.nAlphaStep);
	if(m_Link.nAlphaStep > 255 || m_Link.nAlphaStep < 0)
	{
		throw CErrorContext(ERROR_ID_STRING_FORMAT);
	}
}

void CSonicString::Ctrl_align(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_nLineAlign);
}

void CSonicString::Ctrl_font(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.Font.Init();
}

void CSonicString::Ctrl_font_bold(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Font.bBold);
}

void CSonicString::Ctrl_font_height(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Font.nHeight);
}

void CSonicString::Ctrl_font_italic(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Font.bItalic);
}

void CSonicString::Ctrl_font_strikeout(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &block.Font.bStrikeOut);
}

void CSonicString::Ctrl_font_face(LPCTSTR szValue, STRING_BLOCK & block)
{
	CString str = szValue;
	str.Trim(_T('\''));
	_stscanf(str, _T("%s"), &block.Font.szFaceName);
}

void CSonicString::Ctrl_sparkle(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_Effect.dwMask |= STRING_EFFECT_SPARKLE;
	m_Effect.sparkle.nSparkleInterval = 300;
	m_Effect.sparkle.nSparkleColor[0] = RGB(255, 0, 0);
	m_Effect.sparkle.nSparkleColor[1] = RGB(0, 255, 0);
	m_Effect.sparkle.nSparkleColor[2] = RGB(0, 0, 255);
}

void CSonicString::Ctrl_sparkle_color(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%x|%x|%x"), &m_Effect.sparkle.nSparkleColor[0], &m_Effect.sparkle.nSparkleColor[1], &m_Effect.sparkle.nSparkleColor[2]);
}

void CSonicString::Ctrl_sparkle_interval(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.sparkle.nSparkleInterval);
}

void CSonicString::Ctrl_sparkle_timeout(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.sparkle.nSparkleTimeout);
}

void CSonicString::Ctrl_fadeout(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_Effect.dwMask |= STRING_EFFECT_FADEOUT;
	m_Effect.fadeout.nMaxAlpha = 255;
	m_Effect.fadeout.nMinAlpha = 255;
	m_Effect.fadeout.nAlphaSpeed = 15;
	m_Effect.fadeout.nCurrAlpha = 255;
}

void CSonicString::Ctrl_fadeout_max(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.fadeout.nMaxAlpha );
	if(m_Effect.fadeout.nMaxAlpha)
	{
		m_Effect.fadeout.nMaxAlpha  = m_Effect.fadeout.nMaxAlpha  % 256;
	}
}

void CSonicString::Ctrl_fadeout_min(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.fadeout.nMinAlpha);
	if(m_Effect.fadeout.nMinAlpha)
	{
		m_Effect.fadeout.nMinAlpha = m_Effect.fadeout.nMinAlpha % 256;
	}
}

void CSonicString::Ctrl_fadeout_speed(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.fadeout.nAlphaSpeed);
	if(m_Effect.fadeout.nAlphaSpeed)
	{
		m_Effect.fadeout.nAlphaSpeed = m_Effect.fadeout.nAlphaSpeed % 256;
	}
}

void CSonicString::SetCapture()
{
	if(m_nCapture == 0)
	{
		::SetCapture(m_hWnd);
	}
	m_nCapture++;
}

void CSonicString::ReleaseCapture()
{
	m_nCapture--;
	if(m_nCapture == 0)
	{
		::ReleaseCapture();
	}
}

void CSonicString::Ctrl_line_space(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_nLineSpace);
}

void CSonicString::Ctrl_global(LPCTSTR szValue, STRING_BLOCK & block)
{
	block.nType |= BLOCK_TYPE_GLOBAL;
}

void CSonicString::Ctrl_pass_msg(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_Link.nPassMsg = TRUE;
}

void CSonicString::Ctrl_ani_num(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_Effect.dwMask |= STRING_EFFECT_ANI_NUM;
	m_Effect.ani_num.nFrom = 0;
	m_Effect.ani_num.nTo = 100;
	m_Effect.ani_num.nFrame = 30;
	block.nType |= BLOCK_TYPE_ANI_NUM;
}

void CSonicString::Ctrl_ani_num_from(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.ani_num.nFrom);
}

void CSonicString::Ctrl_ani_num_to(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.ani_num.nTo);
}

void CSonicString::Ctrl_ani_num_frame(LPCTSTR szValue, STRING_BLOCK & block)
{
	_stscanf(szValue, _T("%d"), &m_Effect.ani_num.nFrame);
}

BOOL CSonicString::AnimateAniNum(BOOL b /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(!(m_Effect.dwMask & STRING_EFFECT_ANI_NUM))
	{
		return FALSE;
	}
	if(QueryInternalTimer(TIMER_ANI_NUM))
	{
		if(!b)
		{
			KillInternalTimer(TIMER_ANI_NUM);
			for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
			{
				STRING_BLOCK & block = *it;
				if(block.nType & BLOCK_TYPE_ANI_NUM)
				{
					block.strText.Format(_T("%d"), m_Effect.ani_num.nTo);
				}
			}
		}
	}
	else
	{
		if(b)
		{
			SetInternalTimer(TIMER_ANI_NUM, 30);
			for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
			{
				STRING_BLOCK & block = *it;
				if(block.nType & BLOCK_TYPE_ANI_NUM)
				{
					block.strText.Empty();
				}
			}
			m_Effect.ani_num.nCurrFrame = 0;
		}
	}
	return TRUE;
}

void CSonicString::OnTimerAniNum()
{
	m_Effect.ani_num.nCurrFrame++;
	int nCurr = m_Effect.ani_num.nFrom + (m_Effect.ani_num.nTo - m_Effect.ani_num.nFrom) * m_Effect.ani_num.nCurrFrame / m_Effect.ani_num.nFrame;
	for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
	{
		STRING_BLOCK & block = *it;
		if(block.nType & BLOCK_TYPE_ANI_NUM)
		{
			block.strText.Format(_T("%d"), nCurr);
		}
	}
	if(m_Effect.ani_num.nCurrFrame >= m_Effect.ani_num.nFrame)
	{
		KillInternalTimer(TIMER_ANI_NUM);
		FireDelegate(DELEGATE_EVENT_ANI_NUM_OVER);
	}
	ForceRedraw();
}

void CSonicString::OnTimerButtonAlpha()
{
	if(m_Link.nAlphaStep)
	{
		m_Link.nCurrAlpha += m_Link.nAlphaStep;
		if(m_Link.nCurrAlpha >= 255 || m_Link.nCurrAlpha <= 0)
		{
			m_Link.nCurrAlpha = -1;
			KillInternalTimer(TIMER_BUTTON_ALPHA);
		}
		ForceRedraw();
	}
}

void CSonicString::OnTimerGif()
{
	if(m_nGifCount)
	{
		BOOL bRedraw = FALSE;
		for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
		{
			STRING_BLOCK &block = *it;
			if((block.nType & BLOCK_TYPE_IMAGE) && block.Image.pNormal->GetFrameCount())
			{
				block.Image.nGifInterval += GIF_INTERVAL;
				// divide 2 for loose of windows timer
				if(((int)block.Image.pNormal->GetFrameDelay() >> 1) <= block.Image.nGifInterval)
				{
					// switch the frame
					block.Image.pNormal->SetCurrentFrame();
					block.Image.nGifInterval = 0;
					bRedraw = TRUE;
					if(block.Image.pNormal->GetCurrentFrame() == 0)
					{
						// one round
						FireDelegate(DELEGATE_EVENT_GIF_OVER);
					}
				}
			}
		}
		if(bRedraw)
		{
			ForceRedraw();
		}
	}
}

void CSonicString::OnTimerFadeout()
{
	m_Effect.fadeout.nCurrAlpha += m_Effect.fadeout.nAlphaSpeed;
	if(m_Effect.fadeout.nAlphaSwitch == FALSE)
	{
		if(m_Effect.fadeout.nCurrAlpha <= 0)
		{
			m_Effect.fadeout.nCurrAlpha = 0;
			ShowString(FALSE);
			KillInternalTimer(TIMER_FADEOUT);
			FireDelegate(DELEGATE_EVENT_SHOWGENTLY_OVER);
		}
	}
	else
	{
		if(m_Effect.fadeout.nAlphaSpeed > 0)
		{
			if(m_Effect.fadeout.nCurrAlpha >= m_Effect.fadeout.nMaxAlpha)
			{
				m_Effect.fadeout.nCurrAlpha = m_Effect.fadeout.nMaxAlpha;
				m_Effect.fadeout.nAlphaSpeed = -abs(m_Effect.fadeout.nAlphaSpeed);
				if(m_Effect.fadeout.nMaxAlpha == m_Effect.fadeout.nMinAlpha)
				{
					KillInternalTimer(TIMER_FADEOUT);
					m_Effect.fadeout.nAlphaSwitch = FALSE;
					FireDelegate(DELEGATE_EVENT_SHOWGENTLY_OVER);
				}
			}
		}
		else
		{
			if(m_Effect.fadeout.nCurrAlpha <= m_Effect.fadeout.nMinAlpha)
			{
				m_Effect.fadeout.nCurrAlpha = m_Effect.fadeout.nMinAlpha;
				m_Effect.fadeout.nAlphaSpeed = abs(m_Effect.fadeout.nAlphaSpeed);
			}
		}
	}
	ForceRedraw();
}

void CSonicString::OnTimerSparkle()
{
	m_Effect.sparkle.nCurrSparkle = (m_Effect.sparkle.nCurrSparkle + 1) % (sizeof(m_Effect.sparkle.nSparkleColor) / 4);
	if(m_Effect.sparkle.nSparkleTimeout)
	{
		m_Effect.sparkle.nCurrTime += m_Effect.sparkle.nSparkleInterval;
		if(m_Effect.sparkle.nCurrTime >= m_Effect.sparkle.nSparkleTimeout)
		{
			m_Effect.sparkle.nSparkleSwitch = FALSE;
			KillInternalTimer(TIMER_SPARKLE);
			FireDelegate(DELEGATE_EVENT_SPARKLE_OVER);
		}
	}
	ForceRedraw();
}

void CSonicString::OnRender(ISonicPaint * pPaint, LPVOID)
{
	const CURRENT_PAINT * pCurrPaint = pPaint->GetCurrentPaint();
	if(pCurrPaint == NULL)
	{
		return;
	}
	Render(pPaint);
}

void CSonicString::RenderImage(ISonicPaint * pPaint, LPVOID)
{
	if(pPaint->GetCurrentPaint() == NULL)
	{
		return;
	}
	HDC hdc = pPaint->GetCurrentPaint()->hdc;
	int x = pPaint->GetCurrentPaint()->x;
	int y = pPaint->GetCurrentPaint()->y;
	for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
	{
		STRING_BLOCK &block = *it;
		if(block.nType & BLOCK_TYPE_IMAGE)
		{
			if(m_Effect.dwMask & STRING_EFFECT_FADEOUT)
			{
				DrawImageBlock(hdc, x, y, block, m_nStatus, m_Effect.fadeout.nCurrAlpha);
			}
			else
			{
				if(m_Link.nCurrAlpha == -1)
				{
					DrawImageBlock(hdc, x, y, block, m_nStatus);
				}
				else
				{
					if(m_nStatus == BTN_STATUS_HOVER && m_nLastStatus == BTN_STATUS_NORMAL)
					{
						DrawImageBlock(hdc, x, y, block, m_nLastStatus);
						DrawImageBlock(hdc, x, y, block, m_nStatus, m_Link.nCurrAlpha);
					}
					if(m_nStatus == BTN_STATUS_NORMAL && m_nLastStatus != BTN_STATUS_NORMAL)
					{
						DrawImageBlock(hdc, x, y, block, m_nStatus);
						DrawImageBlock(hdc, x, y, block, BTN_STATUS_HOVER, 255 - m_Link.nCurrAlpha);
					}
				}
			}
		}
	}
}

void CSonicString::RenderText(ISonicPaint * pPaint, LPVOID)
{
	if(pPaint->GetCurrentPaint() == NULL)
	{
		return;
	}
	if(pPaint->GetBits())
	{
		CSSE::DoOr(0x01000000, pPaint->GetBits(), pPaint->GetSize());
	}
	HDC hdc = pPaint->GetCurrentPaint()->hdc;
	int x = pPaint->GetCurrentPaint()->x;
	int y = pPaint->GetCurrentPaint()->y;
	HFONT hOld = (HFONT)SelectObject(hdc, m_hFont);
	int nOldBkMode = GetBkMode(hdc);
	SetBkMode(hdc, TRANSPARENT);
	for(STRING_BLOCK_LIST::iterator it = m_BlockList.begin(); it != m_BlockList.end(); it++)
	{
		STRING_BLOCK &block = *it;
		if(block.nType & BLOCK_TYPE_CHAR)
		{
			DrawTextBlock(hdc, x, y, block);
		}
		if(block.nType & BLOCK_TYPE_LINK)
		{
			if((m_Link.nLinkLine == 1 && m_nStatus != BTN_STATUS_NORMAL)
				|| m_Link.nLinkLine == 2)
			{
				// draw under line
				DrawUnderLine(hdc, x, y, block);
			}
		}
	}
	SetBkMode(hdc, nOldBkMode);
	SelectObject(hdc, hOld);
	if(pPaint->GetBits())
	{
		CSSE::OpenAlpha(pPaint->GetBits(), pPaint->GetSize());
	}
}

void CSonicString::Ctrl_init_normal(LPCTSTR szValue, STRING_BLOCK & block)
{
	m_bInitNormal = TRUE;
}

const RECT * CSonicString::GetRect()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	if(m_pOrigin == FALSE)
	{
		return NULL;
	}
	return m_pOrigin->GetPaintRect();
}