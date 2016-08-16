#include "StdAfx.h"
#include "UIHyperlink.h"


namespace DuiLib{

	CHyperLinkUI::CHyperLinkUI()
	{
	}

	CHyperLinkUI::~CHyperLinkUI()
	{

	}
	LPCTSTR CHyperLinkUI::GetClass() const
	{
		return _T("HyperLinkUI");
	}
	LPVOID CHyperLinkUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_HYPERLINK) == 0) return static_cast<CHyperLinkUI*>(this);
		return CButtonUI::GetInterface(pstrName);

	}
	void CHyperLinkUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
	}

	void CHyperLinkUI::SetPos(RECT rc)
	{

	}
	void CHyperLinkUI::DoInit()
	{

	}
	void CHyperLinkUI::DoEvent(TEventUI& event)
	{

	}
	void CHyperLinkUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{

	}


}