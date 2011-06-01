#pragma once

#include "ISonicUI.h"
using namespace sonic_ui;

#include "DibMgr.h"
#include "ISonicBaseData.h"

#define  DEFAULT_HOVER_COLOR	RGB(0, 0, 0)

#define TIMER_BUTTON_ALPHA		0x1
#define TIMER_GIF				0x2
#define TIMER_SPARKLE			0x4
#define TIMER_FADEOUT			0x8
#define TIMER_ANI_NUM			0x10

class CSonicString : public ISonicString, public ISonicBaseData
{
protected:
	// 私有的数据结构的定义
	// 超链接
	enum enBtnStatus
	{
		BTN_STATUS_NORMAL = 0,
		BTN_STATUS_HOVER,
		BTN_STATUS_CLICK,
	};
	enum enBtnType
	{
		BTN_TYPE_NORMAL = 0,
		BTN_TYPE_CHECK,
		BTN_TYPE_QUICK,
	};
	enum enBlockType
	{
		BLOCK_TYPE_NONE =	0,
		BLOCK_TYPE_CHAR =	0x1,
		BLOCK_TYPE_IMAGE =	0x2,
		BLOCK_TYPE_LINK =	0x4,
		BLOCK_TYPE_GLOBAL = 0x8,
		BLOCK_TYPE_ANI_NUM = 0x10,
	};

	enum enStringEffect
	{
		STRING_EFFECT_NONE = 0,
		STRING_EFFECT_SPARKLE = 0x1,	// 闪烁
		STRING_EFFECT_FADEOUT = 0x2,	// 淡入淡出
		STRING_EFFECT_ANI_NUM = 0x4,	// 动画数字效果
	};

	typedef struct tagLinkInfo
	{
		enum enLinkLine
		{
			LINK_LINE_NONE = 0,		// 没有下划线
			LINK_LINE_HOVER,		// 鼠标移上去有下划线
			LINK_LINE_ALWAYS,		// 始终有下划线
		};
		enLinkLine	nLinkLine;		// 下划线规格
		DWORD		dwHoverColor;	// hover时颜色
		int			nUseHand;		// hover时是否用手型
		int			nBtnType;		// 是否构造为checkbox型的按钮
		BOOL		bCheck;			// checkbox型按钮用
		CString		strLink;		// 超链接URL
		int			nLinkCount;		// 本串的链接个数
		int			nAlphaStep;		// 动态按钮使用，如果不为0，则标志此为动态按钮
		int			nCurrAlpha;		// 动态按钮使用
		int			nPassMsg;		// 传递消息给下层窗体
		void Clear()
		{
			nAlphaStep = 0;
			nCurrAlpha = -1;
			nLinkCount = 0;
			nUseHand = TRUE;
			nLinkLine = LINK_LINE_NONE;
			dwHoverColor = (DWORD)-1;
			nBtnType = 0;	
			bCheck = FALSE;
			strLink.Empty();
			nPassMsg = FALSE;
		}
		tagLinkInfo()
		{
			Clear();
		}
	}LINK_INFO;

	typedef struct tagFontInfo
	{
		BOOL		bValid;			// 标识当前块是否有效
		int			nHeight;		// 大小
		BOOL		bItalic;		// 是否斜体
		BOOL		bBold;			// 是否黑体
		BOOL		bStrikeOut;		// 是否带删除线
		TCHAR		szFaceName[32];	// 字体
		HDC			hdcRestore;		// 切换字体的dc
		HFONT		hOldFont;		// 原字体
		void Init()
		{
			bValid = TRUE;
			nHeight = 12;
			bItalic = FALSE;
			bBold = FALSE;
			bStrikeOut = FALSE;
			lstrcpy(szFaceName, _T("宋体"));
		}
		BOOL SwitchFont(HDC hdc)
		{
			if(bValid == FALSE)
			{
				return FALSE;
			}
			hdcRestore = hdc; 
			LOGFONT lf = {0};
			lf.lfCharSet = GB2312_CHARSET;
			lf.lfHeight = -nHeight;
			lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
			lf.lfQuality = DRAFT_QUALITY;
			lf.lfItalic = bItalic;
			lf.lfStrikeOut = bStrikeOut;
			lstrcpy(lf.lfFaceName, szFaceName);
			hOldFont = (HFONT)SelectObject(hdc, CreateFontIndirect(&lf));
			return TRUE;
		}
		void Restore()
		{
			if(hdcRestore == NULL || hOldFont == NULL || bValid == FALSE)
			{
				return;
			}
			DeleteObject(SelectObject(hdcRestore, hOldFont));
		}
	}FONT_INFO;

	typedef struct tagImageInfo
	{
		ISonicImage *	pNormal;	// 正常时的图片，不为0时IMAGE_INFO有效
		ISonicImage *	pHover;		// 悬浮时的图片
		ISonicImage *	pClick;		// 点击时的图片
		DRAW_PARAM	dp;				// 绘制参数
		int	nGifInterval;			// 控制换帧用
	}IMAGE_INFO;

	typedef struct tagStringBlock
	{
		int			nType;		// 块类型
		int			nLine;		// 本块在整个串中处于第几行
		CRect		rtBlock;	// 块所占区域大小
		CString		strText;	// 文本内容
		DWORD		dwColor;	// 字体颜色
		union
		{
			FONT_INFO	Font;	// 字体信息
			IMAGE_INFO	Image;	// 图片信息
		};
		tagStringBlock()
		{
			nType = BLOCK_TYPE_CHAR;
			nLine = 1;
			dwColor = 0;
			memset(&Image, 0, sizeof(Image));
		}
	}STRING_BLOCK;

	typedef list<STRING_BLOCK> STRING_BLOCK_LIST;

	typedef struct tagSparkleStruct
	{
		int nSparkleColor[3];
		int nSparkleTimeout;
		int nSparkleInterval;
		int nCurrSparkle;
		int nCurrTime;
		int nSparkleSwitch;
	}SPARKLE_STRUCT;

	typedef struct tagFadeoutStruct
	{
		int nMaxAlpha;
		int nMinAlpha;
		int nCurrAlpha;
		int nAlphaSpeed;
		int nAlphaSwitch;
	}FADEOUT_STRUCT;

	typedef struct tagAniNumStruct
	{
		int nFrame;
		int nFrom;
		int nTo;
		int nCurrFrame;
	}ANI_NUM_STRUCT;
	typedef struct tagStringEffect
	{
		DWORD dwMask;
		SPARKLE_STRUCT	sparkle;
		FADEOUT_STRUCT	fadeout;
		ANI_NUM_STRUCT	ani_num;
	}STRING_EFFECT;

public:
	CSonicString(void);
	~CSonicString(void);
	virtual void Clear();
	virtual BOOL Format(LPCTSTR lpszArg, ...);
	virtual BOOL TextOut(HDC hdc, int x, int y, HWND hWnd = NULL, RECT * pMemDCRect = NULL);
	virtual BOOL IsValid();
	virtual BOOL OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual int GetHeight();
	virtual int GetWidth();
	virtual BOOL AnimateGif(BOOL bBegin = TRUE);
	virtual LPCTSTR GetStr();
	virtual void ShowString(BOOL bShow = TRUE, BOOL bRedraw = TRUE);
	virtual BOOL IsVisible();
	virtual BOOL AnimateFadeout(BOOL b = TRUE);
	virtual BOOL AnimateSparkle(BOOL b = TRUE);
	virtual BOOL AnimateAniNum(BOOL b = TRUE);
	virtual BOOL Detach();
	virtual void Redraw();
	virtual const RECT * GetRect();

public:
	static BOOL Init();
	static HFONT GetDefSonicFont();
	static BOOL MessageFilter(CSonicString * pStr, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	DWORD GetBlockTextColor(STRING_BLOCK & block);
	BOOL Translate();
	BOOL CalculateBlockRect(HDC hdc);
	BOOL Render(ISonicPaint * pPaint);
	BOOL Attach(HWND hWnd);
	BOOL OnMouseMove(WPARAM wParam, LPARAM lParam);
	BOOL OnMouseLeave();
	BOOL OnLButtonDown(WPARAM wParam, LPARAM lParam);
	BOOL OnLButtonUp(WPARAM wParam, LPARAM lParam);
	BOOL OnLButtonDblClick(WPARAM wParam, LPARAM lParam);
	BOOL OnRButtonDown(WPARAM wParam, LPARAM lParam);
	BOOL OnRButtonUp(WPARAM wParam, LPARAM lParam);
	BOOL OnRButtonDblClick(WPARAM wParam, LPARAM lParam);
	BOOL OnMouseWheel(WPARAM wParam, LPARAM lParam);
	BOOL ForceRedraw(BOOL bErase = FALSE);
	void ChangeStatus(int nStatus);
	void OnClickLink(ISonicString * pStr, DWORD dwReserve);
	void OnInternalTimer(DWORD dwTimerId);
	void DrawImageBlock(HDC hdc, int x, int y, STRING_BLOCK &block, int nStatus, int nAlpha = -1);
	void DrawTextBlock(HDC hdc, int x, int y, STRING_BLOCK &block);
	void DrawUnderLine(HDC hdc, int x, int y, STRING_BLOCK &block);
	BOOL PtInLink(POINT pt);
	BOOL PtInString(POINT pt);
	void PrepareTextCache();
	void SetCapture();
	void ReleaseCapture();
	void OnTimerAniNum();
	void OnTimerButtonAlpha();
	void OnTimerGif();
	void OnTimerSparkle();
	void OnTimerFadeout();
	void OnRender(ISonicPaint * pPaint, LPVOID);
	void RenderImage(ISonicPaint * pPaint, LPVOID);
	void RenderText(ISonicPaint * pPaint, LPVOID);
	void InitValue();
private:
	void Ctrl_a(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_def(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_c(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkl(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkc(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkt(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkh(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_ph(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_p(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_pc(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_key(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_btn_type(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_pClip(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_line_width(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_single_line(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_align(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_animation(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_br_offset(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_height(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_bold(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_italic(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_strikeout(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_face(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_color(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_interval(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_timeout(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_max(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_min(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_speed(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_line_space(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_global(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_pass_msg(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_from(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_to(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_frame(LPCTSTR szValue, STRING_BLOCK & block);
	void Ctrl_init_normal(LPCTSTR szValue, STRING_BLOCK & block);

	typedef void (CSonicString::* FUNC_CTRL)(LPCTSTR, STRING_BLOCK &);
#if(defined UNICODE || defined _UINCODE)
	typedef map<wstring, FUNC_CTRL> MAP_STR_TO_FUNC;
#else
	typedef map<string, FUNC_CTRL> MAP_STR_TO_FUNC;
#endif
	static MAP_STR_TO_FUNC m_mapFunc;
protected:
	CString	m_strTip;		// 鼠标hover时的tip
	CString m_strNake;
	CString m_strText;		// 不带控制信息的纯文本
	STRING_BLOCK_LIST m_BlockList;
	LINK_INFO	m_Link;		// 连接信息
	STRING_EFFECT	m_Effect;	// 字串特效
	STRING_BLOCK	m_GlobalBlock;

	BOOL m_bInitNormal;
	int m_nCapture;
	int m_nLineAlign;		// 0行上对齐，1行中对齐，2行下对齐
	int m_nGifCount;		// 字串中gif图片的个数
	int m_nStatus;			// 鼠标状态
	int m_nLastStatus;		// 前一次鼠标状态，动态按钮时有用
	int	m_nLineWidth;
	int m_nLineSpace;
	int m_nSingleLine;		// 0，不启用强制单行显示	1，启用单行显示		2，如果单行显示不下，最后三个字节以...结尾
	int m_nBrOffset;		// 换行后，第二行开始每行的行首偏移
	CRect m_rtString;

	ISonicPaint * m_pOrigin;
	ISonicPaint * m_pTextCache;
	static HFONT	m_hFont;
};
