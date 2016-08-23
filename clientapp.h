#pragma once

#include "stdafx.h"

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
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefClientApp);

private:

};