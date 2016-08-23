#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>

CEFWebkitBrowserWnd* CEFWebkitBrowserWnd::pCEFWebkitBrowserWnd = NULL;



CEFWebkitBrowserWnd::CEFWebkitBrowserWnd()
{
	pURLEditCtrl_ = NULL;
	pWebStateCtrl_ = NULL;
	pWKEWebkitCtrl_ = NULL;
	pWebTabContainer_ = NULL;

	pCEFWebkitBrowserWnd = this;

}

CEFWebkitBrowserWnd::~CEFWebkitBrowserWnd()
{

}


CControlUI* CEFWebkitBrowserWnd::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, _T("CEFWebkitBrowser")) == 0)
	{
		return  (pWKEWebkitCtrl_ = new CCEFWebkitUI(GetSafeHwnd()));
	}

	return NULL;
}

void CEFWebkitBrowserWnd::OnFinalMessage(HWND hWnd)
{


	__super::OnFinalMessage(hWnd);
}

LRESULT CEFWebkitBrowserWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	return __super::OnDestroy(uMsg, wParam, lParam, bHandled);
}

//截取WM_CLOSE消息
LRESULT CEFWebkitBrowserWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	if ((pWKEWebkitCtrl_ != NULL) && (!pWKEWebkitCtrl_->IsClosed()))
	{
		//pWKEWebkitCtrl_->CloseAllPage();
		bHandled = TRUE;
		//  Cancel the close.

	}
	else
	{
		bHandled = false;
	}

	return 0;
}

void CEFWebkitBrowserWnd::OnClick(TNotifyUI & msg)
{
	CDuiString strName = msg.pSender->GetName();

	if (strName.Find(_T("webtab")) != -1)
	{
		COptionUI* pOpt = (COptionUI*)msg.pSender;

		COptionTag* pTag = (COptionTag*)pOpt->GetTag();
		if (pTag)
		{
			RECT rc = pOpt->GetPos();
			if (msg.ptMouse.x > rc.right - 20 && msg.ptMouse.y < rc.top + 15)
			{
				pWKEWebkitCtrl_->DelPage(pTag->nID_);

			}
			else
			{
				pWKEWebkitCtrl_->ReFresh(pTag->nID_);
			}

		}
	}
	else if (_T("ui_btn_close")== strName)
	{
		if ((pWKEWebkitCtrl_ != NULL) && (!pWKEWebkitCtrl_->IsClosed()))
		{
			pWKEWebkitCtrl_->CloseAllPage();
		}

	}

}


void CEFWebkitBrowserWnd::InitWindow()
{
	pWebStateCtrl_ = dynamic_cast<CLabelUI*>(FindControl(_T("ui_lbl_status")));
	pURLEditCtrl_ = dynamic_cast<CRichEditUI*>(FindControl(_T("ui_et_urlinput")));
	pWebTabContainer_ = dynamic_cast<CHorizontalLayoutUI*>(FindControl(_T("ui_hl_urltabs")));

}


void CEFWebkitBrowserWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("ui_btn_goback"))
		{

		}
		else if (msg.pSender->GetName() == _T("ui_btn_goforward"))
		{

		}
		else if (msg.pSender->GetName() == _T("ui_btn_ensure"))
		{

		}
		else if (msg.pSender->GetName() == _T("ui_btn_home"))
		{

		}
		else if (msg.pSender->GetName() == _T("ui_btn_newpage"))
		{
			if (pWKEWebkitCtrl_)
			{
				pWKEWebkitCtrl_->NewPage(_T("about:black"));
			}
		}


	}
	//需要关闭richedit的want return属性
	else if (msg.sType == DUI_MSGTYPE_RETURN)
	{
		if (pURLEditCtrl_ == msg.pSender)
		{
			if (pURLEditCtrl_&&pWKEWebkitCtrl_)
			{
				//pWKEWebkitUI->LoadURL(pURLEditUI->GetText().GetData());
			}
		}
	}
	else if (msg.sType == _T("windowinit"))
	{
		OnInitComplate();
	}
	__super::Notify(msg);
}


LRESULT CEFWebkitBrowserWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR szBuf[256];

	switch (uMsg)
	{
	case UM_CEF_COMPLETEELEASE:
		bHandled = TRUE;
		//Sleep(2000);
		//CefQuitMessageLoop();
		//PostQuitMessage(0L);
		break;
	case UM_CEF_WEBLOADPOPUP:
	{

		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			pWKEWebkitCtrl_->NewPage(*pStrComplateURL);

			delete pStrComplateURL;
		}

	}
	break;
	case UM_CEF_WEBLOADEND:
	{

		/*	int index = m_pTabLayoutUI->GetCurSel();
			CHorizontalLayoutUI* pHor = (CHorizontalLayoutUI*)m_pTabLayoutUI->GetItemAt(index);
			if (pHor)
			{
				int iCount = m_pTabSwitchHor->GetCount();
				for (int i = 0; i < iCount; i++)
				{
					COptionUI* pOpt = (COptionUI*)m_pTabSwitchHor->GetItemAt(i);
					CHorizontalLayoutUI* pHaveHor = (CHorizontalLayoutUI*)pOpt->GetTag();
					if (pHaveHor == pHor)
					{
						if (m_cWebClient.at(index)->GetBrowser()->CanGoBack())
						{
							m_BackBtn->SetEnabled(true);
						}
						else
						{
							m_BackBtn->SetEnabled(false);
						}
						if (m_cWebClient.at(index)->GetBrowser()->CanGoForward())
						{
							m_ForwardBtn->SetEnabled(true);
						}
						else
						{
							m_ForwardBtn->SetEnabled(false);
						}
						m_UEdit.at(index) = m_cWebClient.at(index)->m_url.c_str();
						break;
					}
				}
			}*/


		int nBrowserID = wParam;

		if (pURLEditCtrl_)
		{
			pURLEditCtrl_->SetText(pWKEWebkitCtrl_->GetFinalURL(nBrowserID).c_str());
		}

		memset(szBuf, '\0', sizeof(szBuf));
		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			StringCbPrintf(szBuf, sizeof(szBuf), _T("加载完成:%s"), pStrComplateURL->c_str());
			delete pStrComplateURL;
		}

		pWebStateCtrl_->SetText(szBuf);
	}
	break;

	case UM_CEF_WEBLOADSTART:
	{
		memset(szBuf, '\0', sizeof(szBuf));
		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			StringCbPrintf(szBuf, sizeof(szBuf), _T("正在加载:%s"), pStrComplateURL->c_str());
			delete pStrComplateURL;
		}

		pWebStateCtrl_->SetText(szBuf);

		//int index = m_pTabLayoutUI->GetCurSel();
		/*	CHorizontalLayoutUI* pHor = (CHorizontalLayoutUI*)m_pTabLayoutUI->GetItemAt(index);
		if (pHor)
		{
			int iCount = m_pTabSwitchHor->GetCount();
			for (int i = 0; i < iCount; i++)
			{
				COptionUI* pOpt = (COptionUI*)m_pTabSwitchHor->GetItemAt(i);
				CHorizontalLayoutUI* pHaveHor = (CHorizontalLayoutUI*)pOpt->GetTag();
				if (pHaveHor == pHor)
				{
					m_UEdit.at(index) = m_cWebClient.at(index)->m_url.c_str();
					break;
				}
			}
		}*/
	}
	break;
	case UM_CEF_WEBTITLECHANGE:
	{
		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			OnTitleChanged(wParam, *pStrComplateURL);
			delete pStrComplateURL;
		}
	}
	break;
	case UM_CEF_AFTERCREATED:
	{
		OnAfterCreate(wParam);
	}
	break;
	case UM_CEF_BROWSERCLOSE:
	{
		OnBrowserClose(wParam);
	}
		break;
	default:
		break;
	}

	bHandled = FALSE;

	return 0;
}

void CEFWebkitBrowserWnd::OnInitComplate()
{
	if (pWKEWebkitCtrl_)
	{
		pWKEWebkitCtrl_->NewPage(_T("www.baidu.com"));
	}
}


//标题改变
void CEFWebkitBrowserWnd::OnTitleChanged(int nID, const CefString str)
{

	int nCountWebtab = pWebTabContainer_->GetCount();

	for (int idx = 0; idx < nCountWebtab; idx++)
	{

		COptionUI* pOpt = (COptionUI*)pWebTabContainer_->GetItemAt(idx);


		COptionTag* pTag = (COptionTag*)pOpt->GetTag();


		if ((pTag != NULL) && (pTag->nID_ == nID))
		{

			if (str.length() > 8)
			{
				wchar_t strtilte[256] = { '\0' };
				StringCbCopyN(strtilte, sizeof(strtilte), str.c_str(), 6*sizeof(TCHAR));
				StringCbCat(strtilte, sizeof(strtilte) - 6 * sizeof(TCHAR), _T("..."));
				pOpt->SetText(strtilte);
			}
			else
			{
				pOpt->SetText(str.c_str());
			}
			pOpt->SetToolTip(str.c_str());
		}
	}

	return;
}




void CEFWebkitBrowserWnd::OnAfterCreate(int nID)
{
	// 创建标签OPTION按钮
	CDuiString sAttr;
	COptionUI* pOpt = new COptionUI;
	sAttr.Format(_T("name=\"webtab\" width=\"100\" height=\"30\" normalimage=\"tabbar_normal.png\" hotimage=\"tabbar_hover.png\" \
			pushedimage=\"tabbar_pushed.png\" selectedimage=\"tabbar_pushed.png\" group=\"ui_grp_webtab\""));
	pOpt->ApplyAttributeList(sAttr);

	int iCount = pWebTabContainer_->GetCount();
	pWebTabContainer_->AddAt(pOpt, iCount - 1);

	// 把标签按钮和浏览器id关联起来
	COptionTag* pTag = new COptionTag(nID);

	pOpt->SetTag((UINT_PTR)pTag);

}

void CEFWebkitBrowserWnd::OnBrowserClose(int nBrowserID)
{

	int nCountWebtab = pWebTabContainer_->GetCount();

	for (int idx = 0; idx < nCountWebtab; idx++)
	{

		COptionUI* pOpt = (COptionUI*)pWebTabContainer_->GetItemAt(idx);


		COptionTag* pTag = (COptionTag*)pOpt->GetTag();


		if ((pTag != NULL) && (pTag->nID_ == nBrowserID))
		{
			pWebTabContainer_->Remove(pOpt);
			break;
		}
	}


}
