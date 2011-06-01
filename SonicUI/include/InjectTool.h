#pragma once

// 向目标进程注入一个DLL，可以指定一个初始执行函数，这个函数必须是注入DLL的一个导出函数且为 int _cdecl Func(LPVOID pParam)型，
// 如果指定了strFuncInit，那么返回值就是strFuncInit的返回值，否则只返回0失败,1成功
// bFree标识当前是去目标进程卸载一个DLL，请确保卸载时没有其它线程在执行DLL中的代码，否则会crash，如果是卸载DLL，DLL只能写DLL的名称，如"user32.dll"
extern "C" int WINAPI InjectDLL(DWORD dwProcessId, LPCTSTR strDllName, LPCTSTR strFuncInit = NULL, BOOL bFree = FALSE);

// 将一个模块的指定DLL的导出函数替换掉，如果hMod为NULL，一般为可执行exe的handle
// 注意szDllName为.dll后缀，形式如"User32.dll"，"Gdi32.dll"，不要用full path
extern "C" LPVOID WINAPI ReplaceIATFunc(LPCSTR szModuleName, LPCSTR szDllName, LPCSTR szFuncName, LPVOID lpNewFunc, DWORD * pFuncAddr = NULL);

// 将一个函数调用转换成你的调用，如果操作成功，返回一个0based的index，可以用RestoreFunc来恢复，失败则返回-1
extern "C" int WINAPI ReplaceFunc(LPVOID lpOldFunc, LPVOID lpNewFunc);

// 恢复一个已改变的函数调用
extern "C" BOOL WINAPI RestoreFunc(int nIndex);

// 通过修改子进程的入口地址代码，注入一个DLL
extern "C" void WINAPI InjectChildProcess(HANDLE hChildProcess, LPCSTR szChildPeFileName, LPCSTR strDllName, LPCSTR szFuncInit = NULL);

// 获得一个PE文件的入口地址
extern "C" DWORD WINAPI GetEntryPoint(LPCSTR szPEFileName);

// 替换一个函数，并分配一块内存，把原函数保存下来
extern "C" LPVOID ReplaceFuncAndCopy(LPVOID lpOldFunc, LPVOID lpNewFunc);

// 获得从指定地址开始，最小的大于等于5字节的指令字节数
extern "C" int WINAPI GetMiniInstructionSize(LPVOID lpAddr, int nSize);

//安全写内存
extern "C" BOOL WINAPI WriteMem(HANDLE hProcess, LPVOID lpBaseAddr, const char *pBuff, unsigned int nSize);

// 隐藏调用进程中的指定模块
extern "C" BOOL WINAPI HideDll(LPCSTR lpszDllName);
extern "C" void WINAPI MapDll(HINSTANCE hDll,HINSTANCE hBeMapDll);