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


class MyV8Accessor : public CefV8Accessor {
public:
	MyV8Accessor() {}

	virtual bool Get(const CefString& name,
		const CefRefPtr<CefV8Value> object,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE {
		if (name == "myval") {
			// Return the value.
			retval = CefV8Value::CreateString(myval_);
			return true;
		}

		// Value does not exist.
		return false;
	}

	virtual bool Set(const CefString& name,const CefRefPtr<CefV8Value> object,const CefRefPtr<CefV8Value> value,CefString& exception) override {
		if (name == "myval") {
			if (value->IsString()) {
				// Store the value.
				myval_ = value->GetStringValue();
			}
			else {
				// Throw an exception.
				exception = "Invalid value type";
			}
			return true;
		}

		// Value does not exist.
		return false;
	}

	// Variable used for storing the value.
	CefString myval_;

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(MyV8Accessor);
};