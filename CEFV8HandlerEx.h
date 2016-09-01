#pragma once
#include "stdafx.h"
#include <cef_v8.h>
#include <cef_base.h>

class CCEFV8HandlerEx : public CefV8Handler {
public:
	CCEFV8HandlerEx();
	~CCEFV8HandlerEx();
public:
	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override;
private:
	// Map of message callbacks.
	typedef std::map<std::pair<std::string, int>, std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >CallbackMap;
	CallbackMap callback_map_;
public:
	IMPLEMENT_REFCOUNTING(CCEFV8HandlerEx);
};

