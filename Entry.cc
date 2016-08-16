#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include "clientapp.h"

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

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;
	CefSettingsTraits::init(&settings);
	settings.single_process = false;                //使用多进程模式
	settings.ignore_certificate_errors = true;      //忽略掉ssl证书验证错误
	settings.command_line_args_disabled = true;
	//CefString(&settings.locale).FromASCII("zh-CN");


#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	//multi_threaded_message_loop=true 这样设置的目的是使cef的browser ui线程和程序的线程分离，使用duilib的消息循环函数
	settings.multi_threaded_message_loop = true;

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<CCefClientApp> app(NULL);

	// Initialize CEF.
	CefInitialize(main_args, settings, app.get(), sandbox_info);


	CEFWebkitBrowserWnd pFrame;
	pFrame.Create(NULL, _T("浏览器"), UI_WNDSTYLE_FRAME | WS_CLIPCHILDREN, 0);
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

	}
	else
	{
		DuiLib::CPaintManagerUI::MessageLoop();
	}

	CefShutdown();

	//退出程序并释放COM库
//	::CoUninitialize();

	return 0;
}