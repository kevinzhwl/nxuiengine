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
	// ˽�е����ݽṹ�Ķ���
	// ������
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
		STRING_EFFECT_SPARKLE = 0x1,	// ��˸
		STRING_EFFECT_FADEOUT = 0x2,	// ���뵭��
		STRING_EFFECT_ANI_NUM = 0x4,	// ��������Ч��
	};

	typedef struct tagLinkInfo
	{
		enum enLinkLine
		{
			LINK_LINE_NONE = 0,		// û���»���
			LINK_LINE_HOVER,		// �������ȥ���»���
			LINK_LINE_ALWAYS,		// ʼ�����»���
		};
		enLinkLine	nLinkLine;		// �»��߹��
		DWORD		dwHoverColor;	// hoverʱ��ɫ
		int			nUseHand;		// hoverʱ�Ƿ�������
		int			nBtnType;		// �Ƿ���Ϊcheckbox�͵İ�ť
		BOOL		bCheck;			// checkbox�Ͱ�ť��
		CString		strLink;		// ������URL
		int			nLinkCount;		// ���������Ӹ���
		int			nAlphaStep;		// ��̬��ťʹ�ã������Ϊ0�����־��Ϊ��̬��ť
		int			nCurrAlpha;		// ��̬��ťʹ��
		int			nPassMsg;		// ������Ϣ���²㴰��
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
		BOOL		bValid;			// ��ʶ��ǰ���Ƿ���Ч
		int			nHeight;		// ��С
		BOOL		bItalic;		// �Ƿ�б��
		BOOL		bBold;			// �Ƿ����
		BOOL		bStrikeOut;		// �Ƿ��ɾ����
		char		szFaceName[32];	// ����
		HDC			hdcRestore;		// �л������dc
		HFONT		hOldFont;		// ԭ����
		void Init()
		{
			bValid = TRUE;
			nHeight = 12;
			bItalic = FALSE;
			bBold = FALSE;
			bStrikeOut = FALSE;
			lstrcpy(szFaceName, "����");
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
		ISonicImage *	pNormal;	// ����ʱ��ͼƬ����Ϊ0ʱIMAGE_INFO��Ч
		ISonicImage *	pHover;		// ����ʱ��ͼƬ
		ISonicImage *	pClick;		// ���ʱ��ͼƬ
		DRAW_PARAM	dp;				// ���Ʋ���
		int	nGifInterval;			// ���ƻ�֡��
	}IMAGE_INFO;

	typedef struct tagStringBlock
	{
		int			nType;		// ������
		int			nLine;		// �������������д��ڵڼ���
		CRect		rtBlock;	// ����ռ�����С
		CString		strText;	// �ı�����
		DWORD		dwColor;	// ������ɫ
		union
		{
			FONT_INFO	Font;	// ������Ϣ
			IMAGE_INFO	Image;	// ͼƬ��Ϣ
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
	virtual BOOL Format(LPCSTR lpszArg, ...);
	virtual BOOL TextOut(HDC hdc, int x, int y, HWND hWnd = NULL, RECT * pMemDCRect = NULL);
	virtual BOOL IsValid();
	virtual BOOL OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual int GetHeight();
	virtual int GetWidth();
	virtual BOOL AnimateGif(BOOL bBegin = TRUE);
	virtual LPCSTR GetStr();
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
	void Ctrl_a(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_def(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_c(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkl(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkc(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkt(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_linkh(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_ph(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_p(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_pc(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_key(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_btn_type(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_pClip(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_line_width(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_single_line(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_align(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_animation(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_br_offset(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_height(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_bold(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_italic(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_strikeout(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_font_face(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_color(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_interval(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_sparkle_timeout(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_max(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_min(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_fadeout_speed(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_line_space(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_global(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_pass_msg(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_from(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_to(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_ani_num_frame(LPCSTR szValue, STRING_BLOCK & block);
	void Ctrl_init_normal(LPCSTR szValue, STRING_BLOCK & block);

	typedef void (CSonicString::* FUNC_CTRL)(LPCSTR, STRING_BLOCK &);
	typedef map<string, FUNC_CTRL> MAP_STR_TO_FUNC;
	static MAP_STR_TO_FUNC m_mapFunc;
protected:
	CString	m_strTip;		// ���hoverʱ��tip
	CString m_strNake;
	CString m_strText;		// ����������Ϣ�Ĵ��ı�
	STRING_BLOCK_LIST m_BlockList;
	LINK_INFO	m_Link;		// ������Ϣ
	STRING_EFFECT	m_Effect;	// �ִ���Ч
	STRING_BLOCK	m_GlobalBlock;

	BOOL m_bInitNormal;
	int m_nCapture;
	int m_nLineAlign;		// 0���϶��룬1���ж��룬2���¶���
	int m_nGifCount;		// �ִ���gifͼƬ�ĸ���
	int m_nStatus;			// ���״̬
	int m_nLastStatus;		// ǰһ�����״̬����̬��ťʱ����
	int	m_nLineWidth;
	int m_nLineSpace;
	int m_nSingleLine;		// 0��������ǿ�Ƶ�����ʾ	1�����õ�����ʾ		2�����������ʾ���£���������ֽ���...��β
	int m_nBrOffset;		// ���к󣬵ڶ��п�ʼÿ�е�����ƫ��
	CRect m_rtString;

	ISonicPaint * m_pOrigin;
	ISonicPaint * m_pTextCache;
	static HFONT	m_hFont;
};
