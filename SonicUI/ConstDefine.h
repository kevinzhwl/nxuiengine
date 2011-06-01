#pragma once


#define IMAGE_NAME		"IMAGE"
#define MY_WND			"SonicWnd"
#define CHILD_PROC		"ChildProc"
#define EFFECT_HELP_WND	"EffectHelpWnd"
#define EHT_PROP		"EHTProp"
#define WND_EFFECT_PROP_NAME	_T("SonicWndEffect")
#define DEFAULT_JPEG_QULITY		75

#define PAI		3.14159265
#define ROUND(x) (int)((x) > 0 ? (x) + 0.5 : (x) - 0.5)
#define CONVERT_RGB(rgb) ((rgb) >> 16 & 0xff | (rgb) << 16 & 0xff0000 | (rgb) & 0xff00)

// internal timer defined must be times of base interval
#define BASE_INTERVAL			10
#define ANIMATION_INTERVAL		20
#define GIF_INTERVAL			10
#define FADEOUT_INTERVAL		50
//////////////////////////////////////////////////////////////////////////
// error info
class CErrorContext
{
public:
	int nErrorId;
	CErrorContext(int _nErrorId) : nErrorId(_nErrorId){};	
};

enum enErrorId
{
	ERROR_ID_NONE = 0,
	ERROR_ID_UNKNOWN,
	ERROR_ID_STRING_FORMAT,
	ERROR_ID_INVALID_OBJECT_ID,
};

extern const char g_ErrorInfo[][256];

extern DWORD GetMainThreadId();
extern HWND GetTopParent(HWND hWnd);

