#include "stdafx.h"
#include "UIDateTime.h"
#include "UIIpAddress.h"

#pragma comment( lib, "ws2_32.lib" )

DWORD GetLocalIpAddress()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return 0;
	char local[255] = { 0 };
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return 0;
	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));
	DWORD dwIP = MAKEIPADDRESS(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
	return dwIP;
}

namespace DuiLib
{


	class CIPAddressWnd : public CWindowWnd
	{
	public:
		CIPAddressWnd();

		void Init(CIPAddressUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CIPAddressUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	CIPAddressWnd::CIPAddressWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CIPAddressWnd::Init(CIPAddressUI* pOwner)
	{
		m_pOwner = pOwner;
		m_pOwner->m_nIPUpdateFlag = E_IP_NONE;

		if (m_hWnd == NULL)
		{
			INITCOMMONCONTROLSEX   CommCtrl;
			CommCtrl.dwSize = sizeof(CommCtrl);
			CommCtrl.dwICC = ICC_INTERNET_CLASSES;//指定Class
			if (InitCommonControlsEx(&CommCtrl))
			{
				RECT rcPos = CalPos();
				UINT uStyle = WS_CHILD | WS_TABSTOP | WS_GROUP;
				Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			}
			SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
		}

		if (m_pOwner->GetText().IsEmpty())
			m_pOwner->m_dwIP = GetLocalIpAddress();

		::SendMessage(m_hWnd, IPM_SETADDRESS, 0, m_pOwner->m_dwIP);
		::ShowWindow(m_hWnd, SW_SHOW);
		::SetFocus(m_hWnd);


		m_bInit = true;
	}

	RECT CIPAddressWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		return rcPos;
	}

	LPCTSTR CIPAddressWnd::GetWindowClassName() const
	{
		return _T("IPAddressWnd");
	}

	LPCTSTR CIPAddressWnd::GetSuperClassName() const
	{
		return WC_IPADDRESS;
	}

	void CIPAddressWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if (m_hBkBrush != NULL) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CIPAddressWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if (uMsg == WM_KILLFOCUS)
		{
			bHandled = TRUE;
			return 0;
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		// 		else if (uMsg == WM_SETFOCUS)
		// 		{
		// 			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		// 		}
		else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK))
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
			m_pOwner->m_nIPUpdateFlag = E_IP_DELETE;
			m_pOwner->UpdateText(m_pOwner->m_nIPUpdateFlag);
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
			m_pOwner->m_nIPUpdateFlag = E_IP_KEEP;
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if (uMsg == OCM_COMMAND) {
			if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS)
			{
				lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
			}
		}
		// 		else if( uMsg == OCM_COMMAND ) {
		// 			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		// 			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
		// 				RECT rcClient;
		// 				::GetClientRect(m_hWnd, &rcClient);
		// 				::InvalidateRect(m_hWnd, &rcClient, FALSE);
		// 			}
		// 		}
				//	else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) {
				// 		m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
				//	}
				// 		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
				// 			if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
				// 			::SetBkMode((HDC)wParam, TRANSPARENT);
				// 			DWORD dwTextColor = m_pOwner->GetTextColor();
				// 			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
				// 			if( m_hBkBrush == NULL ) {
				// 				DWORD clrColor = m_pOwner->GetNativeEditBkColor();
				// 				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
				// 			}
				// 			return (LRESULT)m_hBkBrush;
				// 		}
		else bHandled = FALSE;
		if (!bHandled) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CIPAddressWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HWND hWndFocus = GetFocus();
		while (hWndFocus)
		{
			if (GetFocus() == m_hWnd)
			{
				bHandled = TRUE;
				return 0;
			}
			hWndFocus = GetParent(hWndFocus);
		}

		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		if (m_pOwner->m_nIPUpdateFlag == E_IP_NONE)
		{
			::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&m_pOwner->m_dwIP);
			m_pOwner->m_nIPUpdateFlag = E_IP_UPDATE;
			m_pOwner->UpdateText(m_pOwner->m_nIPUpdateFlag);
		}
		::ShowWindow(m_hWnd, SW_HIDE);
		//PostMessage(WM_CLOSE);
		return lRes;
	}

	// LRESULT CDateTimeWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	// {
	// 	if( !m_bInit ) return 0;
	// 	if( m_pOwner == NULL ) return 0;
	// 	// Copy text back
	// 	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	// 	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	// 	ASSERT(pstr);
	// 	if( pstr == NULL ) return 0;
	// 	::GetWindowText(m_hWnd, pstr, cchLen);
	// 	m_pOwner->m_sText = pstr;
	// 	m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
	// 	return 0;
	// }

	//////////////////////////////////////////////////////////////////////////
	//
	CIPAddressUI::CIPAddressUI()
	{
		m_dwIP = GetLocalIpAddress();
		m_bReadOnly = false;
		m_pWindow = NULL;
		m_nIPUpdateFlag = E_IP_UPDATE;
		UpdateText(m_nIPUpdateFlag);		// add by:daviyang35 初始化显示IP地址
		m_nIPUpdateFlag = E_IP_NONE;
	}

	LPCTSTR CIPAddressUI::GetClass() const
	{
		return _T("IPAddressUI");
	}

	LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_IPADDRESS) == 0) return static_cast<CIPAddressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	DWORD CIPAddressUI::GetIP()
	{
		return m_dwIP;
	}

	void CIPAddressUI::SetIP(DWORD dwIP)
	{
		m_dwIP = dwIP;
		UpdateText(E_IP_UPDATE);
	}

	void CIPAddressUI::SetReadOnly(bool bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		Invalidate();
	}

	bool CIPAddressUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CIPAddressUI::UpdateText(EIPAddressUpdateFlag updateFlag)
	{
		if (updateFlag == E_IP_DELETE)
		{
			SetText(_T(""));
		}
		else if (updateFlag == E_IP_UPDATE)
		{
			TCHAR szIP[MAX_PATH] = { 0 };
			in_addr addr;
			addr.S_un.S_addr = m_dwIP;
			_stprintf(szIP, _T("%d.%d.%d.%d"), addr.S_un.S_un_b.s_b4, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b1);
			SetText(szIP);
		}
	}

	void CIPAddressUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if (event.Type == UIEVENT_WINDOWSIZE)
		{
			if (m_pWindow != NULL) m_pManager->SetFocusNeeded(this);
		}
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			if (m_pWindow != NULL) return;
		}
		if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
		{
			if (m_pWindow)
			{
				return;
			}
			m_pWindow = new CIPAddressWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			m_pWindow->ShowWindow();
		}
		if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
		{
			if (IsEnabled()) {
				GetManager()->ReleaseCapture();
				if (IsFocused() && m_pWindow == NULL)
				{
					m_pWindow = new CIPAddressWnd();
					ASSERT(m_pWindow);
				}
				if (m_pWindow != NULL)
				{
					m_pWindow->Init(this);
					m_pWindow->ShowWindow();
				}
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			return;
		}
		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			return;
		}

		CLabelUI::DoEvent(event);
	}

	void CIPAddressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CLabelUI::SetAttribute(pstrName, pstrValue);
	}

}
