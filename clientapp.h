#pragma once

#include "stdafx.h"
#include "CEFV8HandlerEx.h"

class CCefClientApp : public CefApp, public CefBrowserProcessHandler, CefRenderProcessHandler
{
public:
	CCefClientApp();
	~CCefClientApp();


	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() {
		return this;
	}

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override; 
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
	
	
	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;
	//CefRenderProcessHandler methods

//	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()  override;

	virtual void OnWebKitInitialized() override;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)  override;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;


private:
	CefRefPtr<CCEFV8HandlerEx> v8Handler_;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefClientApp);

private:

};