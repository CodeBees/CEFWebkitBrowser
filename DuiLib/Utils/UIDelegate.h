#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once

//#include "StdAfx.h"

namespace DuiLib {

	typedef struct tagTEventUI TEventUI;
	typedef struct tagTNotifyUI TNotifyUI;

	typedef bool (*FunVoid)(void* pParam);
	typedef bool (*FunTEvent)(TEventUI* pTEventUI);
	typedef bool (*FunTNotify)(TNotifyUI* pTNotifyUI);

	class CDelegateBase;

	class UILIB_API IDelegate
	{
	public:
		typedef union {
			FunVoid pFunVoid;
			FunTEvent pFunTEvent;
			FunTNotify pFunTNotify;
		}FnType;

	public:
		virtual FunVoid GetFunVoid() const = 0;
		virtual FunTEvent GetFunTEvent() const = 0;
		virtual FunTNotify GetFunTNotify() const = 0;

	public:
		virtual CDelegateBase* Copy() const = 0;
		virtual bool Invoke(void* pParam) = 0;
		virtual bool Invoke(TEventUI* pTEventUI) = 0;
		virtual bool Invoke(TNotifyUI* pTNotifyUI) = 0;
	};

	class UILIB_API CDelegateBase : public IDelegate
	{
	public:
		CDelegateBase(void* pObject, FunVoid pFn);
		CDelegateBase(void* pObject, FunTEvent pFn,UINT _iEventType);
		CDelegateBase(void* pObject, FunTNotify pFn,LPCTSTR _sNotifyTypeName);
		CDelegateBase(const CDelegateBase& rhs);

		virtual ~CDelegateBase();
		FunVoid GetFunVoid() const{return m_unionFnType.pFunVoid;};
		FunTEvent GetFunTEvent() const{return m_unionFnType.pFunTEvent;};
		FunTNotify GetFunTNotify() const{return m_unionFnType.pFunTNotify;};

		bool Equals(const CDelegateBase& rhs) const;
		bool operator() (void* param){return Invoke(param);};
		bool operator() (TEventUI* pTEventUI,UINT _iEventType){return Invoke(pTEventUI);};
		bool operator() (TNotifyUI* pTNotifyUI,LPCTSTR _sNotifyTypeName){return Invoke(pTNotifyUI);};

		void* GetObj() const {return m_pObject;};
	protected:
		void SetEventType(UINT _iEventType){m_iEventType = _iEventType;};
		void SetNotifyTypeName(CDuiString& _sNotifyTypeName){m_sNotifyTypeName = _sNotifyTypeName.GetData();};
		UINT GetEventType(){return m_iEventType;};
		CDuiString GetNotifyTypeName(){return m_sNotifyTypeName.GetData();};

	private:
		void* m_pObject;
		FnType m_unionFnType;
	protected:
		UINT m_iEventType;
		CDuiString m_sNotifyTypeName;
	};


	class UILIB_API CDelegateStatic: public CDelegateBase
	{
	public:
		CDelegateStatic(FunVoid pFunVoid) : CDelegateBase(NULL, pFunVoid) { } 
		CDelegateStatic(FunTEvent pFunTEvent,UINT _iEventType) : CDelegateBase(NULL, pFunTEvent,_iEventType) { } 
		CDelegateStatic(FunTNotify pFunTNotify,LPCTSTR _sNotifyTypeName) : CDelegateBase(NULL, pFunTNotify,_sNotifyTypeName) { } 
		CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
		virtual CDelegateBase* Copy() const { return new CDelegateStatic(*this); }

	protected:
		virtual bool Invoke(void* param)
		{
			FunVoid pFunVoid = GetFunVoid();
			return pFunVoid(param); 
		}

		virtual bool Invoke(TEventUI* pTEventUI)
		{
			FunTEvent pFunTEvent = GetFunTEvent();
			return !pFunTEvent(pTEventUI); 
		};

		virtual bool Invoke(TNotifyUI* pTNotifyUI)
		{
			FunTNotify pFunTNotify = GetFunTNotify();
			return pFunTNotify(pTNotifyUI);
		};
	};

	template <class O, class T>
	class  CDelegate : public CDelegateBase
	{
		typedef bool (T::*CMFunVoid)(void* pParam);
		typedef bool (T::*CMFunTEvent)(TEventUI* pTEventUI);
		typedef bool (T::*CMFunTNotify)(TNotifyUI* pTNotifyUI);
	public:
		CDelegate(O* pObj, CMFunVoid pCMFunVoid) : CDelegateBase(pObj, *(FunVoid*)&pCMFunVoid), m_pCMFunVoid(pCMFunVoid),m_pCMFunTEvent(NULL),m_pCMFunTNotify(NULL){

		}
		CDelegate(O* pObj, CMFunTEvent pCMFunTEvent,UINT _iEventType) : CDelegateBase(pObj, *(FunTEvent*)&pCMFunTEvent,_iEventType), m_pCMFunVoid(NULL),m_pCMFunTEvent(pCMFunTEvent),m_pCMFunTNotify(NULL){
		}
		CDelegate(O* pObj, CMFunTNotify pCMFunTNotify,LPCTSTR _sNotifyTypeName ) : CDelegateBase(pObj, *(FunTNotify*)&pCMFunTNotify,_sNotifyTypeName), m_pCMFunVoid(NULL),m_pCMFunTEvent(NULL),m_pCMFunTNotify(pCMFunTNotify){
		}
		CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) {
			m_pCMFunVoid = rhs.m_pCMFunVoid;m_pCMFunTEvent = rhs.m_pCMFunTEvent;m_pCMFunTNotify = rhs.m_pCMFunTNotify;} 
		virtual CDelegateBase* Copy() const { return new CDelegate(*this); }
		virtual ~CDelegate(){

		}

	protected:
		virtual bool Invoke(void* param)
		{
			O* pObject = (O*) GetObj();
			if(pObject && m_pCMFunVoid)
				return (pObject->*m_pCMFunVoid)(param);
			else if(pObject && m_pCMFunTEvent)
				return Invoke((TEventUI*)param);
			else if(pObject && m_pCMFunTNotify)
				return Invoke((TNotifyUI*)param);

			return true;
		} 

		virtual bool Invoke(TEventUI* pTEventUI)
		{
			O* pObject = (O*) GetObj();
			if(pObject && pTEventUI && GetEventType() == 0)
				return (pObject->*m_pCMFunTEvent)(pTEventUI); 
			else if(pObject && pTEventUI && pTEventUI->Type == GetEventType())
				return (pObject->*m_pCMFunTEvent)(pTEventUI);

			return true;
		};

		virtual bool Invoke(TNotifyUI* pTNotifyUI)
		{
			O* pObject = (O*) GetObj();
			if(pObject && GetNotifyTypeName().IsEmpty())
				return (pObject->*m_pCMFunTNotify)(pTNotifyUI);
			else if(pObject && pTNotifyUI && pTNotifyUI->sType == GetNotifyTypeName())
				return (pObject->*m_pCMFunTNotify)(pTNotifyUI);

			return true;
		};

	private:
		CMFunVoid m_pCMFunVoid;
		CMFunTEvent m_pCMFunTEvent;
		CMFunTNotify m_pCMFunTNotify;
	};


	template <class O, class T>
	CDelegate<O, T>  MakeDelegate(O* pObject, bool (T::* pFn)(void*))
	{
		return CDelegate<O, T>(pObject, pFn);
	}

	template <class O, class T>
	CDelegate<O, T>  MakeDelegate(O* pObject, bool (T::* pFn)(TEventUI*),UINT _iEventType)
	{
		return CDelegate<O, T>(pObject, pFn,_iEventType);
	}

	template <class O, class T>
	CDelegate<O, T>  MakeDelegate(O* pObject, bool (T::* pFn)(TNotifyUI*),LPCTSTR _sNotifyTypeName)
	{
		return CDelegate<O, T>(pObject, pFn,(LPCTSTR)_sNotifyTypeName);
	}

	inline CDelegateStatic MakeDelegate(FunVoid pFunVoid)
	{
		return CDelegateStatic(pFunVoid);
	}

	inline CDelegateStatic MakeDelegate(FunTEvent pFunTEvent,UINT _iEventType)
	{
		return CDelegateStatic(pFunTEvent,_iEventType); 
	}

	inline CDelegateStatic MakeDelegate(FunTNotify pFunTNotify,LPCTSTR _sNotifyTypeName)
	{
		return CDelegateStatic(pFunTNotify,_sNotifyTypeName); 
	}

	class UILIB_API CEventSource
	{
	public:
		CEventSource();
		~CEventSource();
		operator bool();
		void operator+= (const CDelegateBase& d); // add const for gcc
		void operator+= (FunVoid pFunVoid){
			(*this) += MakeDelegate(pFunVoid);
		};
		void operator-= (const CDelegateBase& d);
		void operator-= (FunVoid pFunVoid){(*this) -= MakeDelegate(pFunVoid);};
		bool operator() (void* param);
		bool operator() (TEventUI* pTEventUI);
		bool operator() (TNotifyUI* pTNotifyUI);

	protected:
		TStdPtrArray<CDelegateBase*> m_aDelegates;
	};

}


#endif // __UIDELEGATE_H__

