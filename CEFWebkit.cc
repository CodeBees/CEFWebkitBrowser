#include "stdafx.h"
#include "CEFWebkit.h"

namespace DuiLib
{

	CCEFWebkitUI::CCEFWebkitUI(HWND hParent) :nHitIndex_(0)
	{
		hWebKitBrowserWnd_ = hParent;
		clientHandler_ = new CCefClientHandler();
		clientHandler_->hWnd_ = hWebKitBrowserWnd_;
	}

	CCEFWebkitUI::~CCEFWebkitUI()
	{

	}

	LPCTSTR CCEFWebkitUI::GetClass() const
	{
		return _T("WKEWebkitUI");
	}
	LPVOID CCEFWebkitUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_WKEWEBKIT) == 0)
		{
			return static_cast<CCEFWebkitUI*>(this);
		}

		return CControlUI::GetInterface(pstrName);
	}

	void CCEFWebkitUI::DoInit()
	{

		/*	CefRefPtr<CCefClientHandler>client(new CCefClientHandler());
			client->hWnd_ = hWebKitBrowserWnd_;

			clientHandlers_.push_back(client);
			CefWindowInfo info;
			RECT rectnew = { 100,100,800,600 };

			info.SetAsChild(hWebKitBrowserWnd_, rectnew);
			CefBrowserSettings browserSettings;

			std::wstring strURL(_T("http://www.baidu.com"));
			CefBrowserHost::CreateBrowser(info, client, strURL, browserSettings, NULL);

			strURLs_.push_back(strURL);
		*/

		//	NewPage(_T("www.csdn.net"));
		// ++nHitIndex_;
	}

	void CCEFWebkitUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);

		CefRefPtr<CefBrowser> pBrowser = NULL;
		CefRefPtr<CefBrowserHost>  pBrowerHost = NULL;
		CefWindowHandle hwnd = NULL;

		if (clientHandler_ != NULL)
		{

			for (UINT idx = 0; idx < clientHandler_.get()->browser_list_.size(); idx++)
			{

				pBrowser = clientHandler_.get()->browser_list_.at(idx);

				if (pBrowser)
				{
					pBrowerHost = pBrowser->GetHost();

					if (pBrowerHost)
					{

						hwnd = pBrowerHost->GetWindowHandle();
						if ((hwnd != NULL) && (IsWindow(hwnd)))
						{

							if (idx != nHitIndex_)
							{
								//MoveWindow(hwnd, 0, 0, 0, 0, false);
								ShowWindow(hwnd, SW_HIDE);
							}
							else
							{
								ShowWindow(hwnd, SW_SHOW);
								MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
							}

						}

					}

				}



			}


		}

	}

	void CCEFWebkitUI::DoEvent(TEventUI& event)
	{
		__super::DoEvent(event);
	}

	void CCEFWebkitUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("url")) == 0)
		{
			//	strUrl_ = pstrValue;
		}
		else
		{
			CControlUI::SetAttribute(pstrName, pstrValue);
		}
	}


	//ÐÂÔöÒ³Ç©
	void CCEFWebkitUI::NewPage(CefString Url)
	{
		CefWindowInfo info;

		//RECT rectnew = { 0,200,800,600 };//= GetPos();
		RECT rectnew = GetPos();

		info.SetAsChild(hWebKitBrowserWnd_, rectnew);

		CefBrowserSettings browserSettings;

		CefBrowserHost::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(clientHandler_), Url.c_str(), browserSettings, NULL);

		nHitIndex_ = clientHandler_->browser_list_.size();

		NeedParentUpdate();
	}

	//É¾³ýÒ³Ç©
	void CCEFWebkitUI::DelPage(int nWebBrowserID)
	{

		int idx = 0;

		BOOL bNeedRemove = FALSE;

		vector<CefRefPtr<CefBrowser>>::const_iterator  itTmpWebBrower = clientHandler_->browser_list_.begin();

		for (; itTmpWebBrower != clientHandler_->browser_list_.end(); itTmpWebBrower++)
		{
			++idx;

			if (nWebBrowserID == itTmpWebBrower->get()->GetIdentifier())
			{

				if (clientHandler_->browser_list_.size() == 1)
				{
					itTmpWebBrower->get()->GetMainFrame()->LoadURL(_T("about:blank"));
				}
				else
				{
					bNeedRemove = TRUE;
					::MoveWindow(itTmpWebBrower->get()->GetHost()->GetWindowHandle(), 0, 0, 0, 0, true);
					nHitIndex_ = idx - 1;
				}

				break;
			}

		}

		if (bNeedRemove)
		{
			clientHandler_->CloseHostBrowser(itTmpWebBrower->get(), true);
			clientHandler_->browser_list_.erase(itTmpWebBrower);
		}


		ReFresh();

	}

	void CCEFWebkitUI::CloseAllPage()
	{
		clientHandler_->CloseAllBrowsers(true);
	}


	BOOL CCEFWebkitUI::IsClosed() const
	{
		if (clientHandler_->IsClosing())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void CCEFWebkitUI::ReFresh()
	{

		RECT rectnew = GetPos();

		SetPos(rectnew);

		//if (clientHandler_->browser_list_.empty())
		//{
		//	return;
		//}

		//clientHandler_->browser_list_.at(nHitIndex_)->Reload();

	}

	void CCEFWebkitUI::ReFresh(int nWebBrowserID)
	{
		int idx = 0;

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				nHitIndex_ = idx;
			}

			++idx;
		}

		ReFresh();
	}

	void CCEFWebkitUI::ReLoad(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{
			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				it->get()->Reload();
			}
		}

	}

	CefString CCEFWebkitUI::GetFinalURL(int nWebBrowserID)
	{
		CefString strURL = "";

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				strURL = it->get()->GetMainFrame()->GetURL();
			}

		}

		return strURL;

	}

	int CCEFWebkitUI::GetHitIndex() const
	{
		return nHitIndex_;
	}

	void CCEFWebkitUI::SetHitIndex(int idx)
	{
		nHitIndex_ = idx;
		ReFresh();
		//++TODO: show web 
	}

}