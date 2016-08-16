#include "StdAfx.h"
#include "UIDelegate.h"

namespace DuiLib {

	CDelegateBase::CDelegateBase( void* pObject, FunVoid pFn )
	{
		m_pObject				= pObject;
		m_unionFnType.pFunVoid	= pFn;
		m_iEventType			= UIEVENT__ALL;
		m_sNotifyTypeName.Empty();
	}

	CDelegateBase::CDelegateBase( void* pObject, FunTEvent pFn,UINT _iEventType /*= UIEVENT__ALL*/ )
	{
		m_pObject					= pObject;
		m_unionFnType.pFunTEvent	= pFn;
		m_iEventType				= _iEventType;
		m_sNotifyTypeName.Empty();
	}

	CDelegateBase::CDelegateBase( void* pObject, FunTNotify pFn,LPCTSTR _sNotifyTypeName /*= NULL*/)
	{
		m_pObject					= pObject;
		m_unionFnType.pFunTNotify	= pFn;
		m_iEventType				= UIEVENT__ALL;

		if(NULL != _sNotifyTypeName)
			m_sNotifyTypeName = _sNotifyTypeName;
	}


	CDelegateBase::CDelegateBase( const CDelegateBase& rhs )
	{
		m_pObject				= rhs.m_pObject;
		m_unionFnType.pFunVoid	= rhs.m_unionFnType.pFunVoid;
		m_iEventType			= rhs.m_iEventType;
		m_sNotifyTypeName		= rhs.m_sNotifyTypeName.GetData();
	}

	CDelegateBase::~CDelegateBase()
	{
		if(!m_sNotifyTypeName.IsEmpty())
			m_sNotifyTypeName.Empty();
	}

	bool CDelegateBase::Equals(const CDelegateBase& rhs) const 
	{
		return m_pObject == rhs.m_pObject && m_unionFnType.pFunVoid	== rhs.m_unionFnType.pFunVoid && m_iEventType == rhs.m_iEventType && m_sNotifyTypeName == rhs.m_sNotifyTypeName.GetData(); 
	}

	//

	CEventSource::CEventSource()
	{
		m_aDelegates.Empty();
	}

	CEventSource::~CEventSource()
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject) delete pObject;
			pObject = NULL;
		}
	}

	CEventSource::operator bool()
	{
		return m_aDelegates.GetSize() > 0;
	}

	void CEventSource::operator+= (const CDelegateBase& d)
	{ 
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && pObject->Equals(d) ) return;
		}

		m_aDelegates.Add(d.Copy());
	}

	void CEventSource::operator-= (const CDelegateBase& d) 
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && pObject->Equals(d) ) {
				delete pObject;
				m_aDelegates.Remove(i);
				return;
			}
		}
	}

	bool CEventSource::operator() (void* param) 
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && !pObject->Invoke(param) ) return false;
		}
		return true;
	}

	bool CEventSource::operator() (TEventUI* pTEventUI) 
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && !pObject->Invoke(pTEventUI) ) return false;
		}
		return true;
	}

	bool CEventSource::operator() (TNotifyUI* pTNotifyUI) 
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
			CDelegateBase* pObject = m_aDelegates.GetAt(i);

			if( pObject && !pObject->Invoke(pTNotifyUI) ) return false;
		}
		return true;
	}



}