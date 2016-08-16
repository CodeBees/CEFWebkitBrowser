#include "stdafx.h"
#include "BrowserHandlers.h"
#include <wrapper/cef_helpers.h>
#include <base/cef_bind.h>




int CCefClientHandler::nBrowerReferenceCount_ = -1;

CCefClientHandler::CCefClientHandler() :hWnd_(NULL), browser_(NULL)
{

}

CCefClientHandler::~CCefClientHandler() {

}

CefRefPtr<CefBrowser> CCefClientHandler::GetBrowser()
{
	return browser_;
};


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




//// 缓存一个指向browser的引用
void CCefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	++nBrowerReferenceCount_;
	browser_ = browser;
}

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

//bool CCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
//{
//
//	// Allow the close. For windowed browsers this will result in the OS close
//	// event being sent.
//
//
//
//	return false;
//}


bool CCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	//CEF_REQUIRE_UI_THREAD();

	browser_ = NULL;

	return false;
}


void CCefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	//CEF_REQUIRE_UI_THREAD();

	--nBrowerReferenceCount_;

	if (nBrowerReferenceCount_ < 0)
	{
		CefQuitMessageLoop();
		//::PostQuitMessage(0L);
		//SendMessage(hWnd_, UM_CEFCOMPLETEELEASE, 0, 0);
	}

}

void CCefClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{

	strCurURL_ = browser->GetMainFrame()->GetURL();
	::PostMessageA(hWnd_, UM_WEBLOADSTART, NULL, (LPARAM)&strCurURL_);
	return __super::OnLoadStart(browser, frame);
}
void CCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	strCurURL_ = browser->GetMainFrame()->GetURL();
	::PostMessageA(hWnd_, UM_WEBLOADEND, NULL, (LPARAM)&strCurURL_);
}



void CCefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	//CEF_REQUIRE_UI_THREAD();

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


void CCefClientHandler::CloseHostBrowser(bool force_close)
{
	browser_->GetHost()->CloseBrowser(force_close);
}



void CCefClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	//CEF_REQUIRE_UI_THREAD();
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	SetWindowText(hwnd, std::wstring(title).c_str());
}



bool CCefClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{
	strCurURL_ = target_url;
	::PostMessage(hWnd_, UM_WEBLOADPOPUP, NULL, (LPARAM)&strCurURL_);
	return true;


}


void CloseBrowser(bool force_close)
{
}
