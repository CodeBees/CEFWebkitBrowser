#include "stdafx.h"
#include "clientapp.h"

CCefClientApp::CCefClientApp()
{

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
	
}