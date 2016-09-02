#include "stdafx.h"
#include "BrowserHandlers.h"
#include <wrapper/cef_helpers.h>
#include <base/cef_bind.h>


//TID_UI 线程是浏览器的主线程。如果应用程序在调用调用CefInitialize()时，传递CefSettings.multi_threaded_message_loop = false，这个线程也是应用程序的主线程。
//TID_IO 线程主要负责处理IPC消息以及网络通信。
//TID_FILE 线程负责与文件系统交互。


// 1.  User clicks the window close button which sends an OS close
//     notification (e.g. WM_CLOSE on Windows, performClose: on OS-X and
//     "delete_event" on Linux).
// 2.  Application's top-level window receives the close notification and:
//     A. Calls CefBrowserHost::CloseBrowser(false).
//     B. Cancels the window close.
// 3.  JavaScript 'onbeforeunload' handler executes and shows the close
//     confirmation dialog (which can be overridden via
//     CefJSDialogHandler::OnBeforeUnloadDialog()).
// 4.  User approves the close.
// 5.  JavaScript 'onunload' handler executes.
// 6.  Application's DoClose() handler is called. Application will:
//     A. Set a flag to indicate that the next close attempt will be allowed.
//     B. Return false.
// 7.  CEF sends an OS close notification.
// 8.  Application's top-level window receives the OS close notification and
//     allows the window to close based on the flag from #6B.
// 9.  Browser OS window is destroyed.
// 10. Application's CefLifeSpanHandler::OnBeforeClose() handler is called and
//     the browser object is destroyed.
// 11. Application exits by calling CefQuitMessageLoop() if no other browsers
//     exist.
///


CCefClientHandler::CCefClientHandler() :hWnd_(NULL), is_closing_(false)
{
}

CCefClientHandler::~CCefClientHandler()
{

}


// CefClient methods:
CefRefPtr<CefDisplayHandler> CCefClientHandler::GetDisplayHandler()
{
	return this;
}
CefRefPtr<CefLifeSpanHandler> CCefClientHandler::GetLifeSpanHandler()
{
	return this;
}
CefRefPtr<CefLoadHandler> CCefClientHandler::GetLoadHandler()
{
	return this;
}

CefRefPtr<CefRequestHandler> CCefClientHandler::GetRequestHandler()
{
	return this;
}

// 缓存一个指向browser的引用
void CCefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	// Add to the list of existing browsers.
	browser_list_.push_back(browser);

	int nID = browser->GetIdentifier();

	::PostMessage(hWnd_, UM_CEF_AFTERCREATED, nID, 0);

}


bool CCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	//TID_UI 线程是浏览器的主线程。
	CEF_REQUIRE_UI_THREAD();
	//	AutoLock lock_scope(this);

	lock_.Acquire();


	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();

	for (; bit != browser_list_.end(); bit++)
	{
		if ((*bit)->IsSame(browser))
		{
			browser_list_.erase(bit);
			browser = NULL;
			break;
		}
	}


	lock_.Release();

	if (browser_list_.size() == 0) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;

		::PostMessage(hWnd_, UM_CEF_POSTQUITMESSAGE, 0, 0);
	}


	return false;
}


void CCefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	AutoLock lock_scope(this);
	//	lock_.Acquire();

	BrowserList::iterator bit = browser_list_.begin();

	for (; bit != browser_list_.end(); bit++)
	{
		if ((*bit)->IsSame(browser))
		{
			browser_list_.erase(bit);
			browser = NULL;
			break;
		}
	}


	if (browser_list_.empty())
	{
		is_closing_ = true;
		::PostMessage(hWnd_, UM_CEF_POSTQUITMESSAGE, 0, 0);
		// All browser windows have closed. Quit the application message loop.
		//CefQuitMessageLoop();
		//PostQuitMessage(0l);
	}

	//	lock_.Release();
}

void CCefClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

	CefString* strTmpURL = new CefString(browser->GetMainFrame()->GetURL());
	int nID = browser->GetIdentifier();
	::PostMessage(hWnd_, UM_CEF_WEBLOADSTART, nID, (LPARAM)strTmpURL);

	//return __super::OnLoadStart(browser, frame);
}
void CCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();

	CefString* strTmpURL = new CefString(browser->GetMainFrame()->GetURL());
	int nID = browser->GetIdentifier();
	::PostMessage(hWnd_, UM_CEF_WEBLOADEND, nID, (LPARAM)strTmpURL);

}



void CCefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}





void CCefClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	// globally unique identifier for this browser
	int nID = browser->GetIdentifier();

	CefString* strTitle = new CefString(title);
	::PostMessage(hWnd_, UM_CEF_WEBTITLECHANGE, nID, (LPARAM)strTitle);
}



bool CCefClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{

	//消息处理后记得delete分配的字符串资源
	CefString* strTargetURL = new CefString(target_url);
	::PostMessage(hWnd_, UM_CEF_WEBLOADPOPUP, (WPARAM)0, (LPARAM)strTargetURL);
	return true;

}

#define MENU_ID_USER_OPENLINK  MENU_ID_USER_FIRST+200
#define MENU_ID_USER_COPYLINK  MENU_ID_USER_FIRST+201

void CCefClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	//在这里，我添加了自己想要的菜单  
	cef_context_menu_type_flags_t flag = params->GetTypeFlags();

	if (flag&CM_TYPEFLAG_LINK)
	{
		model->Clear(); //清除所有菜单项

		model->AddItem(MENU_ID_USER_OPENLINK, L"在新标签页中打开(&T)");//增加菜单项
		model->AddSeparator(); //加分隔线
		model->AddItem(MENU_ID_USER_COPYLINK, L"复制链接地址(&C)");//增加菜单项
		//model->SetEnabled(MENU_ID_USER_OPENLINK, false); //禁用菜单项。
		return;
	}


	if (flag & CM_TYPEFLAG_PAGE)
	{//普通页面的右键消息

		model->SetLabel(MENU_ID_BACK, L"后退");
		model->SetLabel(MENU_ID_FORWARD, L"前进");
		model->SetLabel(MENU_ID_VIEW_SOURCE, L"查看源代码");
		model->SetLabel(MENU_ID_PRINT, L"打印");
		model->AddItem(MENU_ID_RELOAD, L"刷新");
		model->AddItem(MENU_ID_RELOAD_NOCACHE, L"强制刷新");
		model->AddItem(MENU_ID_STOPLOAD, L"停止加载");
		model->SetLabel(MENU_ID_REDO, L"重复");


	}
	if (flag & CM_TYPEFLAG_EDITABLE)
	{//编辑框的右键消息  
		model->SetLabel(MENU_ID_UNDO, L"撤销");
		model->SetLabel(MENU_ID_REDO, L"重做");
		model->SetLabel(MENU_ID_CUT, L"剪切");
		model->SetLabel(MENU_ID_COPY, L"复制");
		model->SetLabel(MENU_ID_PASTE, L"粘贴");
		model->SetLabel(MENU_ID_DELETE, L"删除");
		model->SetLabel(MENU_ID_SELECT_ALL, L"全选");
	}

}

bool CCefClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	//CefString strLinkURL;
	CefString strURLLink;
	CefString* strTargetURL = nullptr;
	HGLOBAL hglbCopy;
	LPTSTR  lptstrCopy;
	int nBuffLength = 0;


	switch (command_id)
	{

	case MENU_ID_USER_OPENLINK:

		strTargetURL = new CefString(params->GetLinkUrl());
		::PostMessage(hWnd_, UM_CEF_WEBLOADPOPUP, (WPARAM)0, (LPARAM)strTargetURL);
		break;
	case MENU_ID_USER_COPYLINK:


		if (!OpenClipboard(frame->GetBrowser().get()->GetHost().get()->GetWindowHandle()))
		{
			return FALSE;
		}

		EmptyClipboard();

		strURLLink = params->GetUnfilteredLinkUrl();

		if (strURLLink.length() != 0)
		{
			nBuffLength = (strURLLink.length() + 1) * sizeof(TCHAR);

			hglbCopy = GlobalAlloc(GMEM_MOVEABLE, nBuffLength);

			if (hglbCopy == NULL)
			{
				CloseClipboard();
				return FALSE;
			}

			// Lock the handle and copy the text to the buffer. 

			lptstrCopy = (LPTSTR)::GlobalLock(hglbCopy);
			memset(lptstrCopy, '\0', nBuffLength);
			memcpy(lptstrCopy, strURLLink.c_str(), nBuffLength - sizeof(TCHAR));

			GlobalUnlock(hglbCopy); // 解除锁定剪贴板
			// Place the handle on the clipboard. 

			//SetClipboardData(CF_TEXT, hglbCopy);
			SetClipboardData(CF_UNICODETEXT, hglbCopy);// CF_UNICODETEXT为Unicode编码  

		}

		CloseClipboard();

		break;
	default:
		break;
	}


	return false;
}



void CCefClientHandler::CloseHostBrowser(CefRefPtr<CefBrowser>browser, bool force_close)
{

	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CCefClientHandler::CloseHostBrowser, this, browser, force_close));
		return;
	}


	int nID = browser->GetIdentifier();

	::PostMessage(hWnd_, UM_CEF_BROWSERCLOSE, nID, 0);
	browser->GetHost()->CloseBrowser(force_close);
}


void CCefClientHandler::CloseAllBrowsers(bool force_close)
{
	//if (!CefCurrentlyOn(TID_UI))
	//{
	//	// Execute on the UI thread.
	//	CefPostTask(TID_UI, base::Bind(&CCefClientHandler::CloseAllBrowsers, this, force_close));
	//	return;
	//}

	lock_.Acquire();
	if (browser_list_.empty()) {
		return;
	}
	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
	{
		(*it)->GetHost()->CloseBrowser(force_close);

	}
	lock_.Release();
}


bool CCefClientHandler::IsClosing() const
{
	return is_closing_;
}


