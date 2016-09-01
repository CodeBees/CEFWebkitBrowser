#include "stdafx.h"
#include "clientapp.h"

CCefClientApp::CCefClientApp()
{
	v8Handler_ = new CCEFV8HandlerEx;

}

CCefClientApp::~CCefClientApp()
{

}


void CCefClientApp::OnBeforeCommandLineProcessing(const CefString & process_type, CefRefPtr<CefCommandLine> command_line)
{
	//加载flash插件
	command_line->AppendSwitchWithValue("--ppapi-flash-path", "ppflash/18_0_0_209/pepflashplayer32_18_0_0_209.dll");
	//manifest.json中的version
	command_line->AppendSwitchWithValue("--ppapi-flash-version", "18.0.0.209");
	command_line->AppendSwitch("--disable-extensions");

}

void CCefClientApp::OnContextInitialized()
{
	std::string app_code =
		"var app;"
		"if (!app)"
		"    app = {};"
		"(function() {"
		"    app.jsInvokeCPlusPlus = function(v1,v2) {"
		"        native function jsInvokeCPlusPlus();"
		"        return jsInvokeCPlusPlus(v1,v2);"
		"    };"
		"})();";

	CefRegisterExtension("v8/app", app_code, v8Handler_);

}

