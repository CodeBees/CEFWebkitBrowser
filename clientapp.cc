#include "stdafx.h"
#include "clientapp.h"

CCefClientApp::CCefClientApp()
{
	v8Handler_ = new CCEFV8HandlerEx;

}

CCefClientApp::~CCefClientApp()
{

}


CefRefPtr<CefBrowserProcessHandler> CCefClientApp::GetBrowserProcessHandler()
{
	return this;
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


//CefRefPtr<CefRenderProcessHandler> CCefClientApp::GetRenderProcessHandler()
//{
//	return this;	
//}

void CCefClientApp::OnWebKitInitialized()
{
	std::string app_code =
		//-----------------------------------
		//声明JavaScript里要调用的Cpp方法
		"var app;"
		"if (!app)"
		"  app = {};"
		"(function() {"

		//  jsInvokeCPlusPlus 
		"  app.jsInvokeCPlusPlus = function(v1, v2) {"
		"    native function jsInvokeCPlusPlus();"
		"    return jsInvokeCPlusPlus(v1, v2);"
		"  };"

		"})();";


	// Register app extension module

	// JavaScript里调用app.jsInvokeCPlusPlus时，就会去通过CefRegisterExtension注册的CefV8Handler列表里查找
	// 找到"v8/app"对应的CCEFV8HandlerEx，就调用它的Execute方法
	// 假设v8Handler_是CCefClientApp的一个成员变量
	//v8Handler_ = new CCEFV8HandlerEx();

	CefRegisterExtension("v8/app", app_code, v8Handler_);

}

void CCefClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> object = context->GetGlobal();// 获取到window  
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("C++ created Value!");
	object->SetValue("jsValue", str, V8_PROPERTY_ATTRIBUTE_NONE);
}


void CCefClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{

}
