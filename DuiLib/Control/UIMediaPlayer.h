#ifndef __UIMEDIAPLAYER_H_
#define __UIMEDIAPLAYER_H_

#pragma once
#include <atlcomcli.h>
//class CActiveXCtrl;
//class CActiveXUI;
#include "../Utils/wmp.tlh"

namespace DuiLib
{
	class UILIB_API CMediaPlayerUI :public CActiveXUI
	{
	public:
		CMediaPlayerUI( );
		virtual ~CMediaPlayerUI( );


		virtual LPCTSTR GetClass( ) const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void SetVisible(bool bVisible = true);
		virtual void SetInnerVisible(bool bVisible = true);
		virtual bool DoCreateControl( );
		virtual void ReleaseControl( );

		virtual	bool CreateControl(const CLSID clsid);
		virtual	void SetDelayCreate(bool bDelayCreate);

	public:
		bool Play(LPCWSTR pszUrl);
		void SetVolume(long value);
		long GetVolume( );
	protected:
		
		CDuiString m_strUrl;
		CComQIPtr<WMPLib::IWMPPlayer4> wmp_;
	private:
		long	m_volume;
	};


}


#endif
