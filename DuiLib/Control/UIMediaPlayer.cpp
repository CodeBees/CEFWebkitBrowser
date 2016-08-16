#include "StdAfx.h"
#include "UIMediaPlayer.h"
#include <windows.h>
#include <comdef.h>

namespace DuiLib
{
	CMediaPlayerUI::CMediaPlayerUI( ) :m_volume(100)
	{
		m_clsid = __uuidof(WMPLib::WindowsMediaPlayer);
		m_strUrl.Empty( );
	}

	CMediaPlayerUI::~CMediaPlayerUI( )
	{
		ReleaseControl( );
	}

	LPCTSTR CMediaPlayerUI::GetClass( ) const
	{
		return DUI_CTR_MEDIAPLAYER;
	}

	LPVOID CMediaPlayerUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_MEDIAPLAYER) == 0)
		{
			return static_cast<CMediaPlayerUI*>(this);
		}
		return CActiveXUI::GetInterface(pstrName);
	}
	void CMediaPlayerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("url")) == 0)
		{
			m_strUrl = pstrValue;
		}
		else if (_tcscmp(pstrName, _T("volume")) == 0)
		{
			TCHAR* pstr;
			long value = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			SetVolume(value);
		}
		else
		{
			CActiveXUI::SetAttribute(pstrName, pstrValue);
		}

	}

	void CMediaPlayerUI::SetVisible(bool bVisible)
	{
		CActiveXUI::SetInnerVisible(bVisible);
		if ((m_hwndHost != NULL) && (wmp_ != NULL))
		{
			if (bVisible)
			{
				::ShowWindow(m_hwndHost, SW_SHOW);
				wmp_->PutuiMode("None");//Full, Mini, None, Invisible
			}
			else
			{
				//::ShowWindow(m_hwndHost, SW_HIDE);
				wmp_->PutuiMode("Invisible");
			}
		}

	}
	void CMediaPlayerUI::SetInnerVisible(bool bVisible)
	{

		CControlUI::SetInnerVisible(bVisible);
		if ((m_hwndHost != NULL) && (wmp_ != NULL))
		{
			if (bVisible)
			{
				::ShowWindow(m_hwndHost, SW_SHOW);
				wmp_->PutuiMode("full");
			}
			else
			{
				wmp_->PutuiMode("Invisible");
			}
		}
	}
	bool CMediaPlayerUI::DoCreateControl( )
	{
		HRESULT hr;

		if (!CActiveXUI::DoCreateControl( ))
		{
			return false;
		}

		GetControl(__uuidof(WMPLib::IWMPPlayer4), (LPVOID*)&wmp_);

		if (wmp_)
		{
			//设置为无窗口模式
			hr = wmp_->put_windowlessVideo(VARIANT_TRUE);
			wmp_->put_stretchToFit(TRUE);
			if (!m_strUrl.IsEmpty( ))
			{

				hr = wmp_->put_URL(CComBSTR(m_strUrl.GetData( )));
			}

		}


		return true;

	}
	void CMediaPlayerUI::ReleaseControl( )
	{
		m_bCreated = false;
	}

	bool CMediaPlayerUI::Play(LPCWSTR pszUrl)
	{
		if (!wmp_)
		{
			return false;
		}

		wmp_->close( );
		wmp_->put_URL(CComBSTR(pszUrl));
		return true;
	}

	void CMediaPlayerUI::SetVolume(long value)
	{
		if (m_volume != value)
		{
			m_volume = value;
			CComQIPtr<WMPLib::IWMPSettings> wmpSet;
			GetControl(__uuidof(WMPLib::IWMPSettings), (LPVOID*)&wmpSet);
			if (wmpSet)
			{
				wmpSet->Putvolume(m_volume);
			}
		}

	}

	long CMediaPlayerUI::GetVolume( )
	{
		CComQIPtr<WMPLib::IWMPSettings> wmpSet;
		GetControl(__uuidof(WMPLib::IWMPSettings), (LPVOID*)&wmpSet);
		if (wmpSet)
		{
			return	wmpSet->Getvolume( );
		}

		return m_volume;
	}

	void CMediaPlayerUI::SetDelayCreate(bool bDelayCreate)
	{
		if (m_bDelayCreate == bDelayCreate) return;
		if (bDelayCreate == false) {
			if (m_bCreated == false && m_clsid != IID_NULL) DoCreateControl( );
		}
		m_bDelayCreate = bDelayCreate;
	}

	bool CMediaPlayerUI::CreateControl(const CLSID clsid)
	{
		ASSERT(clsid != IID_NULL);
		if (clsid == IID_NULL) return false;
		m_bCreated = false;
		m_clsid = clsid;
		if (!m_bDelayCreate) DoCreateControl( );
		return true;
	}

}