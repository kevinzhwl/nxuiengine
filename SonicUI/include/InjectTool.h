#pragma once

// ��Ŀ�����ע��һ��DLL������ָ��һ����ʼִ�к������������������ע��DLL��һ������������Ϊ int _cdecl Func(LPVOID pParam)�ͣ�
// ���ָ����strFuncInit����ô����ֵ����strFuncInit�ķ���ֵ������ֻ����0ʧ��,1�ɹ�
// bFree��ʶ��ǰ��ȥĿ�����ж��һ��DLL����ȷ��ж��ʱû�������߳���ִ��DLL�еĴ��룬�����crash�������ж��DLL��DLLֻ��дDLL�����ƣ���"user32.dll"
extern "C" int WINAPI InjectDLL(DWORD dwProcessId, LPCTSTR strDllName, LPCTSTR strFuncInit = NULL, BOOL bFree = FALSE);

// ��һ��ģ���ָ��DLL�ĵ��������滻�������hModΪNULL��һ��Ϊ��ִ��exe��handle
// ע��szDllNameΪ.dll��׺����ʽ��"User32.dll"��"Gdi32.dll"����Ҫ��full path
extern "C" LPVOID WINAPI ReplaceIATFunc(LPCSTR szModuleName, LPCSTR szDllName, LPCSTR szFuncName, LPVOID lpNewFunc, DWORD * pFuncAddr = NULL);

// ��һ����������ת������ĵ��ã���������ɹ�������һ��0based��index��������RestoreFunc���ָ���ʧ���򷵻�-1
extern "C" int WINAPI ReplaceFunc(LPVOID lpOldFunc, LPVOID lpNewFunc);

// �ָ�һ���Ѹı�ĺ�������
extern "C" BOOL WINAPI RestoreFunc(int nIndex);

// ͨ���޸��ӽ��̵���ڵ�ַ���룬ע��һ��DLL
extern "C" void WINAPI InjectChildProcess(HANDLE hChildProcess, LPCSTR szChildPeFileName, LPCSTR strDllName, LPCSTR szFuncInit = NULL);

// ���һ��PE�ļ�����ڵ�ַ
extern "C" DWORD WINAPI GetEntryPoint(LPCSTR szPEFileName);

// �滻һ��������������һ���ڴ棬��ԭ������������
extern "C" LPVOID ReplaceFuncAndCopy(LPVOID lpOldFunc, LPVOID lpNewFunc);

// ��ô�ָ����ַ��ʼ����С�Ĵ��ڵ���5�ֽڵ�ָ���ֽ���
extern "C" int WINAPI GetMiniInstructionSize(LPVOID lpAddr, int nSize);

//��ȫд�ڴ�
extern "C" BOOL WINAPI WriteMem(HANDLE hProcess, LPVOID lpBaseAddr, const char *pBuff, unsigned int nSize);

// ���ص��ý����е�ָ��ģ��
extern "C" BOOL WINAPI HideDll(LPCSTR lpszDllName);
extern "C" void WINAPI MapDll(HINSTANCE hDll,HINSTANCE hBeMapDll);