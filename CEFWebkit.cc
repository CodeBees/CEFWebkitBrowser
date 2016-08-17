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

		if (clientHandler_!=NULL)
		{

			for (UINT idx = 0; idx <clientHandler_.get()->browser_list_.size(); idx++)
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

		/*

		else if (event.Type ==UIEVENT_SETFOCUS)
		{
			pWebView_->focus();
		}
		else if (event.Type == UIEVENT_KILLFOCUS)
		{
			pWebView_->unfocus();
		}
		else if(event.Type== UIEVENT_WINDOWSIZE)
		{
			pWebView_->resize(GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam));
		}
		if (!handled)
		{
			//  CControlUI::DoEvent(event);
		}
		*/
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


	//新增页签
	void CCEFWebkitUI::NewPage(std::wstring Url)
	{
		strURLs_.push_back(Url);

		CefWindowInfo info;

		//RECT rectnew = { 0,200,800,600 };//= GetPos();
		RECT rectnew = GetPos();

		info.SetAsChild(hWebKitBrowserWnd_, rectnew);

		CefBrowserSettings browserSettings;

		CefBrowserHost::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(clientHandler_), Url.c_str(), browserSettings, NULL);
		//for (UINT i = 0; i < clientHandlers_.size(); i++)
		//{
		//	if (clientHandlers_.at(i)->GetBrowser() && !clientHandlers_.at(i)->GetBrowser()->GetMainFrame()->GetURL().empty())
		//	{
		//		CefRefPtr<CefBrowserHost> t_host = clientHandlers_.at(i)->GetBrowser()->GetHost();
		//		CefWindowHandle hwnd = t_host->GetWindowHandle();
		//		if (i < clientHandlers_.size() - 1)
		//		{
		//			//MoveWindow(hwnd, 0, 0, 0, 0, false);
		//			ShowWindow(hwnd, SW_HIDE);
		//		}
		//		else //显示最新标签
		//		{
		//			MoveWindow(hwnd, rectnew.left, rectnew.top, rectnew.right-rectnew.left, rectnew.bottom-rectnew.top, true);
		//		}
		//	}
		//}

	//	nHitIndex_ = clientHandlers_.size()-1;

		NeedParentUpdate();
	}


	//删除页签
	void CCEFWebkitUI::DelPage(int idxDel)
	{
		int t_Index = 0;



		vector<CefRefPtr<CefBrowser>>::iterator cWebBrowerit = clientHandler_->browser_list_.begin();

		if (strURLs_.size() > 1)
		{

			for (vector<wstring>::iterator it = strURLs_.begin(); it != strURLs_.end();)   //for循环中不要it++
			{
				if (t_Index == idxDel)
				{

					::MoveWindow(cWebBrowerit->get()->GetHost()->GetWindowHandle(), 0, 0, 0, 0, true);
					clientHandler_->CloseHostBrowser(cWebBrowerit->get(),true);
					//clientHandlers_.erase(cWebBrowerit++);
					strURLs_.erase(it++);
					break;
				}
				else
				{
					it++;
					cWebBrowerit++;
				}

				t_Index++;
			}
		}
		else
		{
			strURLs_.at(0) = _T("about:blank");
			clientHandler_->browser_list_.at(0)->GetMainFrame()->LoadURL(strURLs_.at(0).c_str());
			/*	CEditUI* pEdit = (CEditUI*)m_PaintManager.FindControl(_T("url"));
				if (pEdit)
				{
					pEdit->SetText(m_UEdit.at(0));
				}*/
		}
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

	void CCEFWebkitUI::Refresh()
	{
		if (clientHandler_->browser_list_.empty())
		{
			return;
		}

		clientHandler_->browser_list_.at(nHitIndex_)->Reload();

	}

	CefString CCEFWebkitUI::GetFinalURL(size_t idx)
	{
		if ((idx<0)||(idx>clientHandler_->browser_list_.size()))
		{
			return "";
		}

		return	clientHandler_->browser_list_.at(idx)->GetMainFrame()->GetURL();

	}

	int CCEFWebkitUI::GetHitIndex() const
	{
		return nHitIndex_;
	}

	void CCEFWebkitUI::SetHitIndex(int idx)
	{
		nHitIndex_ = idx;
		//++TODO: show web 
	}

}