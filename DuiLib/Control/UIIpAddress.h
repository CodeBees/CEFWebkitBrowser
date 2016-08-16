#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#pragma once

//给该控件添加一个属性dtstyle

#define  DTUI_TIMEFORMAT	_T("timeformat")
namespace DuiLib
{
	//UpdateFlag
	enum EIPAddressUpdateFlag
	{
		E_IP_NONE = 0,
		E_IP_UPDATE = 1,
		E_IP_DELETE = 2,
		E_IP_KEEP = 3
	};

	class CIPAddressWnd;

	/// 时间选择控件
	class UILIB_API CIPAddressUI : public CLabelUI
	{
		friend class CIPAddressWnd;
	public:
		CIPAddressUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		DWORD GetIP();
		void SetIP(DWORD dwIP);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void UpdateText(EIPAddressUpdateFlag updateFlag);

		void DoEvent(TEventUI& event);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		DWORD	m_dwIP;
		bool       m_bReadOnly;
		EIPAddressUpdateFlag		m_nIPUpdateFlag;
		CIPAddressWnd* m_pWindow;
	};
}

#endif // __UIIPADDRESS_H__