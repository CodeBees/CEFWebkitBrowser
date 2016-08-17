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
		void Refresh();
		CefString GetFinalURL(size_t idx);
		int GetHitIndex() const;
		void SetHitIndex(int idx);
	public:

		//virtual void SetInnerVisible(bool bVisible = true)
		//{
		//	__super::SetInnerVisible(bVisible);
		//	::ShowWindow(hWebKitBrowserWnd_, bVisible);
		//}
	
	public:

	protected:

		HWND hWebKitBrowserWnd_;

		std::vector<std::wstring> strURLs_;

		//vector<CefRefPtr<CCefClientHandler>>   clientHandlers_;
		CefRefPtr<CCefClientHandler>  clientHandler_;
		int nHitIndex_;

	};
}


#endif
