#pragma once

#include "resource.h"
#include "CEFWebkit.h"

class CEFWebkitBrowserWnd : public WindowImplBase
{

public:
    CEFWebkitBrowserWnd();
    ~CEFWebkitBrowserWnd();


    virtual LPCTSTR GetWindowClassName() const      { return _T("CEFWebkitBrowserWnd"); }
    virtual CDuiString GetSkinFile()                { return _T("skin.xml"); }
    virtual CDuiString GetSkinFolder()              { return _T("Skin"); }
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	void OnInitComplate();
public:
    static CEFWebkitBrowserWnd* pCEFWebkitBrowserWnd;
    
    wstring strURL_;
    wstring strTitle_;
    CCEFWebkitUI* pWKEWebkitCtrl_; 
private:
    CRichEditUI*        pURLEditCtrl_;
	CLabelUI*			pWebStateCtrl_;

};
