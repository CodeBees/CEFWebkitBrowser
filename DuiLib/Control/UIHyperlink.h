#ifndef UI_HYPERLINK_H
#define UI_HYPERLINK_H
#pragma once


namespace DuiLib{


	class UILIB_API CHyperLinkUI : public CButtonUI
	{
	public:
		CHyperLinkUI();
		virtual ~CHyperLinkUI();


		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void SetPos(RECT rc);
		virtual void DoInit();
		virtual void DoEvent(TEventUI& event);
		virtual void DoPaint(HDC hDC, const RECT& rcPaint);






	};




















}


#endif