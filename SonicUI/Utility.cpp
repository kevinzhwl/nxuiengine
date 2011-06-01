#include "stdafx.h"
#include "ConstDefine.h"

#include "Tlhelp32.h"


DWORD GetMainThreadId()
{
	DWORD dwCurProcessId = GetCurrentProcessId();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwCurProcessId);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	THREADENTRY32 te;
	te.dwSize=sizeof(THREADENTRY32);
	BOOL fOk=Thread32First(hSnapshot,&te);
	for(;fOk;){
		//the second parameter of CreateToolhelp32Snapshot will not work,so I have to enum
		//all Process
		if (te.th32OwnerProcessID == dwCurProcessId) 
		{
			//found process
			if (te.th32ThreadID!=0) 
			{
				return te.th32ThreadID;
			}
		}
		fOk=Thread32Next(hSnapshot,&te);
	}
	return 0;
}

HWND GetTopParent(HWND hWnd)
{
	HWND hTemp = NULL;
	for (hTemp = hWnd; hWnd; hWnd = GetParent(hWnd))
	{
		hTemp = hWnd;
		if(GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP)
		{
			break;
		}
	}
	return hTemp;
}