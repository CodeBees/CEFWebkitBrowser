#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>

CEFWebkitBrowserWnd* CEFWebkitBrowserWnd::pCEFWebkitBrowserWnd=NULL;



CEFWebkitBrowserWnd::CEFWebkitBrowserWnd()
{
   // pWKEWebkitUI=NULL;
    pURLEditUI=NULL;
    pCEFWebkitBrowserWnd=this;
	pWKEWebkitUI_ = NULL;
}

CEFWebkitBrowserWnd::~CEFWebkitBrowserWnd()
{
	int a = 30;
}


CControlUI* CEFWebkitBrowserWnd::CreateControl( LPCTSTR pstrClassName )
{
    if (_tcsicmp(pstrClassName, _T("CEFWebkitBrowser")) == 0)
    {
        return  (pWKEWebkitUI_=new CCEFWebkitUI(GetSafeHwnd()));
    }

    return NULL;
}

void CEFWebkitBrowserWnd::OnFinalMessage(HWND hWnd)
{

	
	__super::OnFinalMessage(hWnd);
}

LRESULT CEFWebkitBrowserWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//::PostQuitMessage(0L);

	return __super::OnDestroy(uMsg,wParam,lParam,bHandled);
}

//截取WM_CLOSE消息
LRESULT CEFWebkitBrowserWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	if (!pWKEWebkitUI_->IsClosed())
	{
		pWKEWebkitUI_->CloseAllPage();
		bHandled = TRUE;
	}

	bHandled = FALSE;

	return 0;
}


void CEFWebkitBrowserWnd::InitWindow()
{

    //pURLEditUI = dynamic_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("ui_et_urlinput")));

    //pWKEWebkitUI = dynamic_cast<CWKEWebkitUI*>(m_PaintManager.FindControl(_T("ui_wke_wkebrowser")));
    //if (pWKEWebkitUI)
    //{

    //    wkeClientHanler_.onTitleChanged = onTitleChangedLocal;
    //    wkeClientHanler_.onURLChanged = onURLChangedLocal;

    //    pWKEWebkitUI->SetClientHandler(&wkeClientHanler_);

    //    jsBindFunction("jsInvokeCPlusPlus", jsInvokeCPlusPlus, 2);//这里绑定js函数，让js主动调用c++函数


    //    pWKEWebkitUI->LoadFile(_T("htmlexample/index.html"));
    //}



}


void CEFWebkitBrowserWnd::Notify( TNotifyUI& msg )
{
    if( msg.sType == _T("click") ) 
    {	
        if( msg.pSender->GetName() == _T("ui_btn_goback") ) 
        {
           
        }
        if( msg.pSender->GetName() == _T("ui_btn_goforward") ) 
        {
           
        }
        if( msg.pSender->GetName() == _T("ui_btn_ensure") ) 
        {
           
        }
        if( msg.pSender->GetName() == _T("ui_btn_home") ) 
        {
           
        }
    }
    //需要关闭richedit的want return属性
    else if (msg.sType==DUI_MSGTYPE_RETURN)
    {
        if (pURLEditUI==msg.pSender)
        {
            if (pURLEditUI&&pWKEWebkitUI_)
            {
                //pWKEWebkitUI->LoadURL(pURLEditUI->GetText().GetData());
            }
        }
    }
    __super::Notify(msg);
}


LRESULT CEFWebkitBrowserWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int ss = 6;

	switch (uMsg)
	{
	case UM_CEFCOMPLETEELEASE:
		bHandled = TRUE;
		PostQuitMessage(0L);
		break;
	case UM_WEBLOADPOPUP:
		ss = 6;
		break;
	default:
		break;
	}

    bHandled=FALSE;

    return 0;
}