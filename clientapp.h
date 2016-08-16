#pragma once

#include "stdafx.h"

class CCefClientApp : public CefApp, public CefBrowserProcessHandler
{
public:
	CCefClientApp();
	~CCefClientApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefClientApp);

private:

};