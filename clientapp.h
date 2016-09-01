#pragma once

#include "stdafx.h"
#include "CEFV8HandlerEx.h"

class CCefClientApp : public CefApp, public CefBrowserProcessHandler
{
public:
	CCefClientApp();
	~CCefClientApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}

	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

private:
	CefRefPtr<CCEFV8HandlerEx> v8Handler_;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefClientApp);

private:

};