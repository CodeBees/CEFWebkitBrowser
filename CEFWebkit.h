#pragma  once 

#ifndef UIWKEWEBKIT_H
#define UIWKEWEBKIT_H
#include "stdafx.h"
#include <vector>
#include "BrowserHandlers.h"

namespace DuiLib
{
	class CCEFWebkitUI : public CControlUI
	{
	public:
		CCEFWebkitUI(HWND hParent);
		~CCEFWebkitUI();

		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void DoInit();
		virtual void DoEvent(TEventUI& event);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetPos(RECT rc);

	public:
		void NewPage(std::wstring Url);
		void DelPage(int idxDel);
		void CloseAllPage();
		BOOL IsClosed() const ;
	public:

		//virtual void SetInnerVisible(bool bVisible = true)
		//{
		//	__super::SetInnerVisible(bVisible);
		//	::ShowWindow(hWebKitBrowserWnd_, bVisible);
		//}

	

		//BOOL Attach(HWND hWndNew)
		//{
		//	if (!::IsWindow(hWndNew))
		//	{
		//		return FALSE;
		//	}

		//	hWebKitBrowserWnd_ = hWndNew;
		//	return TRUE;
		//}

		//HWND Detach()
		//{
		//	HWND hWnd = hWebKitBrowserWnd_;
		//	hWebKitBrowserWnd_ = NULL;
		//	return hWnd;
		//}

	protected:

		HWND hWebKitBrowserWnd_;

		std::vector<std::wstring> strURLs_;
		vector<CefRefPtr<CCefClientHandler>>   clientHandlers_;
		int nHitIndex_;

	};
}


#endif
