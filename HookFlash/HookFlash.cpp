// HookFlash.cpp : 定义 DLL 应用程序的导出函数。
//



#include "stdafx.h"

using namespace std;

HOOK_TRACE_INFO hAHookTrackInfo = { NULL }; // keep track of our hook
HOOK_TRACE_INFO hWHookTrackInfo = { NULL }; // keep track of our hook

//注意调用约定 WINAPI，否则会出现堆栈异常

typedef	BOOL(WINAPI *realCreateProcessWPtr)(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);


typedef BOOL(WINAPI *realCreateProcessAPtr)(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory, \
	LPSTARTUPINFOA lpStartupInfo, \
	LPPROCESS_INFORMATION lpProcessInformation);


realCreateProcessAPtr prealCreateProcessA;
realCreateProcessWPtr prealCreateProcessW;


BOOL WINAPI MYCreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	std::wstring strCommandLine(lpCommandLine);

	//MessageBoxW(GetActiveWindow(), strCommandLine.c_str(), L"createproceW", MB_OK);

	if (string::npos != strCommandLine.find(L"echo NOT SANDBOXED"))
	{
		//MessageBoxW(GetActiveWindow(), strCommandLine.c_str(), L"createproceW", MB_OK);
		return TRUE;
	}
	else
	{
		return (prealCreateProcessW)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}

}

BOOL WINAPI MYCreateProcessA(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	std::string strCommandLine = lpCommandLine;

	if (string::npos != strCommandLine.find("echo NOT SANDBOXED"))
	{
		//MessageBoxA(GetActiveWindow(), strCommandLine.c_str(), "createprocesA", MB_OK);
		return TRUE;
	}
	else
	{
		return (prealCreateProcessA)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}

}




void DoHook()
{

	HMODULE hKernel32 = LoadLibrary(L"kernel32.dll");

	if (!(prealCreateProcessA = (realCreateProcessAPtr)GetProcAddress(hKernel32, "CreateProcessA")))
	{
		MessageBoxW(GetDesktopWindow(), L"GetProcAddress Err", L"CreateProcessA", MB_OK);
		return;
	}

	if (!(prealCreateProcessW = (realCreateProcessWPtr)GetProcAddress(hKernel32, "CreateProcessW")))
	{
		MessageBoxW(GetDesktopWindow(), L"GetProcAddress Err", L"CreateProcessW", MB_OK);
		return;
	}


	NTSTATUS resultA = LhInstallHook(prealCreateProcessA, MYCreateProcessA, NULL, &hAHookTrackInfo);
	NTSTATUS resultW = LhInstallHook(prealCreateProcessW, MYCreateProcessW, NULL, &hWHookTrackInfo);


	if (FAILED(resultA) || (FAILED(resultW)))
	{
		MessageBox(GetForegroundWindow(), _T("Hook Failed"), _T("Error"), MB_OK);
	}

	
	ULONG ACLEntriesA[1] = { 0 };
	ULONG ACLEntriesW[1] = { 0 };

	// If the threadId in the ACL is set to “ 0 ”,
	// then internally EasyHook uses GetCurrentThreadId()
	// Disable the hook for the provided threadIds, enable for all others
	LhSetExclusiveACL(ACLEntriesA, 0, &hAHookTrackInfo);
	LhSetExclusiveACL(ACLEntriesW, 0, &hWHookTrackInfo);

}


void UnHook()
{

	// this will also invalidate "hHook", because it is a traced handle...  
	LhUninstallAllHooks();

	// this will do nothing because the hook is already removed...  
	LhUninstallHook(&hAHookTrackInfo);
	LhUninstallHook(&hWHookTrackInfo);

	// even if the hook is removed, we need to wait for memory release  
	LhWaitForPendingRemovals();

}