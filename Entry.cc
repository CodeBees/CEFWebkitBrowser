#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include "clientapp.h"
#include <Shlobj.h>
#include <strsafe.h>

//记得拷贝resource中的资源到运行目录



int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//实例句柄与渲染类关联
	CPaintManagerUI::SetInstance(hInstance);

	//Initializes the COM library on the current thread and identifies,初始化COM库, 为加载COM库提供支持
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
	{
		return 0;
	}

	//

	HMODULE hModule = LoadLibrary(_T("HookFlash.dll"));


	cef_enable_highdpi_support();

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif


	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<CCefClientApp> app(new CCefClientApp); //CefApp实现，用于处理进程相关的回调。


	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;
	CefSettingsTraits::init(&settings);
	settings.single_process = true;                //使用多进程模式
	settings.ignore_certificate_errors = true;      //忽略掉ssl证书验证错误
	settings.log_severity = LOGSEVERITY_ERROR;
//	settings.command_line_args_disabled = true;
//	CefString(&settings.locale).FromASCII("zh-CN");
	TCHAR szSpecialPath[MAX_PATH];
	memset(szSpecialPath, '\0', sizeof(szSpecialPath));
	//	GetEnvironmentVariable("",szSpecialPath,sizeof())
	
	if (FALSE!= SHGetSpecialFolderPath(NULL, szSpecialPath, CSIDL_PROFILE, FALSE))
	{
		StringCbCat(szSpecialPath,sizeof(szSpecialPath),_T("\\AppData\\Local\\Temp\\CEF"));
		CefString(&settings.cache_path).FromString(szSpecialPath,sizeof(szSpecialPath)/2,true);
	}
	
	

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	//multi_threaded_message_loop=true 这样设置的目的是使cef的browser ui线程和程序的线程分离，使用duilib的消息循环函数
	settings.multi_threaded_message_loop = true;

	

	// Initialize CEF.
	CefInitialize(main_args, settings, app, sandbox_info);

	//////////////////////////////hook test////////////////////////////////////////////

	/*
	PROCESS_INFORMATION   p;
	STARTUPINFO   startupInfo = { 0 };
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.lpReserved = NULL;
	startupInfo.lpReserved2 = NULL;
	startupInfo.lpDesktop = NULL;
	startupInfo.dwFlags = 0;
	BOOL   res = CreateProcess(L"C:\\WINNT\\system32\\CMD.exe",L"ping 192.168.1.6",    
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,
		NULL,
		NULL,
		&startupInfo,
		&p);
	*/

	//////////////////////////////////////////////////////////////////////////

	
	CEFWebkitBrowserWnd pFrame;
	pFrame.Create(NULL, _T("浏览器"), UI_WNDSTYLE_FRAME | WS_CLIPCHILDREN, WS_EX_ACCEPTFILES);
	pFrame.CenterWindow();

	//	绘制阴影

	CShadowWindow shadowwnd;
	CShadowWindow::Initialize(hInstance);
	shadowwnd.Create(pFrame.GetHWND());
	shadowwnd.SetSize(4);
	shadowwnd.SetPosition(4, 4);
	shadowwnd.SetColor(RGB(22, 22, 22));

	//   pFrame.ShowWindow(true);

	if (!settings.multi_threaded_message_loop)
	{
		//Run the CEF message loop. This function will block until the application
		//recieves a WM_QUIT message.
		CefRunMessageLoop();
		//CefDoMessageLoopWork();

	}
	else
	{
		CPaintManagerUI::MessageLoop();
	}


	CefShutdown();

	//退出程序并释放COM库
	::CoUninitialize();

	return 0;
}