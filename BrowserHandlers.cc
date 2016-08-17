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
	int a = 30;
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

}


bool CCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	//TID_UI 线程是浏览器的主线程。
	CEF_REQUIRE_UI_THREAD();
	AutoLock lock_scope(this);

	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	return false;
}


void CCefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	AutoLock lock_scope(this);

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit)
	{
		if ((*bit)->IsSame(browser)) 
		{
			browser_list_.erase(bit);
			break;
		}
	}

	if (browser_list_.empty()) {
		// All browser windows have closed. Quit the application message loop.
	
		CefQuitMessageLoop();
		//PostQuitMessage(0l);
	}

}

void CCefClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

	CefString* strTmpURL = new CefString(browser->GetMainFrame()->GetURL());
	::PostMessage(hWnd_, UM_CEF_WEBLOADSTART, NULL, (LPARAM)strTmpURL);
	
	//return __super::OnLoadStart(browser, frame);
}
void CCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();

	CefString* strTmpURL = new CefString(browser->GetMainFrame()->GetURL());
	::PostMessage(hWnd_, UM_CEF_WEBLOADEND, NULL, (LPARAM)strTmpURL);

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

	CefString* strTitle = new CefString(title); ;
	::PostMessage(hWnd_, UM_CEF_WEBTITLECHANGE, NULL, (LPARAM)strTitle);

	//CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	//SetWindowText(hwnd, std::wstring(title).c_str());
}



bool CCefClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{

	CefString strCurURL = target_url;
	::PostMessage(hWnd_, UM_CEF_WEBLOADPOPUP, NULL, (LPARAM)&strCurURL);
	return true;

}


void CCefClientHandler::CloseHostBrowser(CefRefPtr<CefBrowser>browser, bool force_close)
{
	browser->GetHost()->CloseBrowser(force_close);
}


void CCefClientHandler::CloseAllBrowsers(bool force_close)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CCefClientHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	if (browser_list_.empty())
		return;

	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
	{
		(*it)->GetHost()->CloseBrowser(force_close);
	}
}
