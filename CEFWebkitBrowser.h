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
	virtual void OnClick(TNotifyUI& msg) override;
    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	void OnInitComplate();
	//标题栏发生改变
	void OnTitleChanged(int nWebBrowserID,const CefString str);
	//浏览器页创建成功
	void OnAfterCreate(int nWebBrowserID);
	//浏览器页关闭
	void OnBrowserClose(int nBrowserID);

	void OnWebLoadEnd(WPARAM wparam,LPARAM lparam);
	void OnWebLoadStart(WPARAM wParam, LPARAM lParam);
	COptionUI* GetActiveOption() const;

	void SwitchUIState();

public:
    static CEFWebkitBrowserWnd* pCEFWebkitBrowserWnd;
    
    wstring strURL_;
    wstring strTitle_;
    CCEFWebkitUI* pWKEWebkitCtrl_; 
private:
	CRichEditUI*			pURLEditCtrl_;
	CEditUI*				pSearchEditCtrl_;
	CLabelUI*				pWebStateCtrl_;
	CHorizontalLayoutUI*	pWebTabContainer_;
	CButtonUI*				pGoBackCtrl_;			
	CButtonUI*				pGoForwardCtrl_;		

	class COptionTag
	{
	public:
		int nID_;
		COptionTag::COptionTag(int nID) :nID_(nID)
		{

		};
	};

	


};
