#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};


#pragma region CPoint
	class UILIB_API CPoint : public tagPOINT
	{
	public:
		CPoint();
		CPoint(const POINT& src);
		CPoint(int x, int y);
		CPoint(LPARAM lParam);
	};

#pragma endregion
#pragma region CSize

	class UILIB_API CSize : public tagSIZE
	{
	public:
		CSize();
		CSize(const SIZE& src);
		CSize(const RECT rc);
		CSize(int cx, int cy);
	};

#pragma endregion
#pragma region CDuiRect

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		//区域合并
		void Join(const RECT& rc);
		
		void ResetOffset();
		//格式化(left<right,top<bottom)
		void Normalize();
		void Offset(int cx, int cy);
		//膨胀
		void Inflate(int cx, int cy);
		//收缩
		void Deflate(int cx, int cy);
		//并操作
		void Union(CDuiRect& rc);
	};
#pragma endregion
#pragma region CStdPtrArray

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;	  //指针数量
		int m_nAllocated; //已分配内存
	};

	template<typename T = LPVOID>
	class  TStdPtrArray : public CStdPtrArray
	{
	public:
		TStdPtrArray(int iPreallocSize = 0):CStdPtrArray(iPreallocSize){};
		TStdPtrArray(const TStdPtrArray& src):CStdPtrArray(src){};
		int Find(T iIndex) const {return CStdPtrArray::Find(iIndex);};
		bool Add(T pData){return CStdPtrArray::Add(pData);};
		bool SetAt(int iIndex, T pData){return CStdPtrArray::SetAt(iIndex,pData);};
		bool InsertAt(int iIndex, T pData){return CStdPtrArray::InsertAt(iIndex,pData);};
		bool Remove(int iIndex,bool bDeleteObj = false){
			if(bDeleteObj){
				T p = GetAt(iIndex);
				if(p)
					delete p;
			}
			return CStdPtrArray::Remove(iIndex);
		}
		T* GetData(){return static_cast<T>(CStdPtrArray::GetData());};
		T GetAt(int iIndex) const {return static_cast<T>(CStdPtrArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return static_cast<T>(CStdPtrArray::operator[](nIndex));};
	};


#pragma endregion
#pragma region CStdValArray

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	template<typename T = LPVOID,typename T1 = T>
	class  TStdValArray : public CStdValArray
	{
	public:
		TStdValArray(int iElementSize = sizeof(T), int iPreallocSize = 0):CStdValArray(iElementSize,iPreallocSize){};
		bool Add(const T1 pData)
		{
			return CStdValArray::Add((LPCVOID)&pData);
		};
		bool InsertAt(int iIndex,const T pData)
		{
			if (iIndex == m_nCount)
			{
				return Add(pData);
			}
			if (iIndex < 0 || iIndex > m_nCount)
			{
				return false;
			}
			if( ++m_nCount >= m_nAllocated) 
			{
				int nAllocated = m_nAllocated * 2;
				if( nAllocated == 0 ) nAllocated = 11;
				LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
				if( pVoid != NULL )
				{
					m_nAllocated = nAllocated;
					m_pVoid = pVoid;
				}
				else 
				{
					--m_nCount;
					return false;
				}
			}

			memmove(&m_pVoid + (iIndex+1) * m_iElementSize, &m_pVoid + iIndex * m_iElementSize,m_iElementSize);
			::CopyMemory(m_pVoid + (iIndex * m_iElementSize), &pData, m_iElementSize);
			return true;
		}
		T GetData()
		{
			return static_cast<T>(CStdValArray::GetData());
		};
		T GetAt(int iIndex) const 
		{
			return *static_cast<T1*>(CStdValArray::GetAt(iIndex));
		};
		T operator[] (int nIndex) const
		{
			return (T)CStdValArray:::operator[](nIndex);
		};
	};

#pragma endregion
#pragma region CDuiString

	/*
	template<typename T>
	class CTDUIString
	{
	public:
	enum { MAX_LOCAL_STRING_LEN = 63 };
	//构造函数
	CTDUIString();
	CTDUIString(const T ch);
	CTDUIString(const CTDUIString& src);
	CTDUIString(const T* lpsz, int nLen = -1);
	~CTDUIString();

	protected:
	T* m_pstr;
	T m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};



	CTDUIString<TCHAR> mytstring;
	CTDUIString<WCHAR> mywtstring;
	CTDUIString<CHAR> mystring;
	*/
	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		//构造函数
		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		int SafeStrlen(LPCTSTR lpsz);

		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		TCHAR GetAt(int nIndex) const;
		void SetAt(int nIndex, TCHAR ch);
		//插入一个字符,
		void Insert(int nIndex, TCHAR ch); 
		void Insert(int nIndex, LPCTSTR pstr);

		//类型转换
		operator LPCTSTR() const;
		//操作符重载
		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& CDuiString::operator=(LPCSTR lpStr);
		const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
		const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
		const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();
		void MakeReverse();
		//裁剪
		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;
		//查找
		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0, bool casesensitive=true) const;
		int ReverseFind(TCHAR ch) const;
		//替换
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
		//修剪字符串
		void TrimLeft(LPCTSTR lpszTargets);
		void TrimLeft(TCHAR chTarget);
		void TrimLeft();
		void TrimRight(LPCTSTR lpszTargetList);
		void TrimRight(TCHAR chTarget);
		void TrimRight();
		void Trim(LPCTSTR lpszTargetList);
		void Trim(TCHAR chTarget);
		void Trim();
		//
		int Remove(TCHAR chRemove);
		//最长63位的字符串格式化
		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl Format(LPCTSTR pstrFormat, va_list Args);
		//一次格式化1024长度的字符串格式化
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};


	bool operator==(const DuiLib::CDuiString& s1, const DuiLib::CDuiString& s2);
	bool operator==(const DuiLib::CDuiString& s1, LPCTSTR  s2);
	bool operator==(LPCTSTR s1, const DuiLib::CDuiString& s2);

	inline bool operator==(const CDuiString& s1, const CDuiString& s2)
	{ return s1.Compare(s2.GetData()) == 0; }
	inline bool operator==(const CDuiString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) == 0; }
	inline bool operator==(LPCTSTR s1, const CDuiString& s2)
	{ return s2.Compare(s1) == 0; }

#pragma endregion

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

#pragma region CStdStringPtrMap
	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;//容器容量
		int m_nCount;
	};

	template<typename T = LPVOID>
	class UILIB_API TStdStringPtrMap : public CStdStringPtrMap
	{
	public:
		TStdStringPtrMap(int nSize = 83):CStdStringPtrMap(nSize){};
		T GetAtObj(int iIndex) const {
			LPCTSTR nkey = GetAt(iIndex);
			if(!nkey)
				return NULL;
			return Find(nkey);
		}
		T Find(LPCTSTR key, bool optimize = true) const{return static_cast<T>(CStdStringPtrMap::Find(key,optimize));};
		bool Insert(LPCTSTR key,T pData){return CStdStringPtrMap::Insert(key,pData);};
		T Set(LPCTSTR key,T pData){return static_cast<T>(CStdStringPtrMap::Set(key,pData));};
	};

#pragma endregion 
#pragma region CWaitCursor
	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

#pragma endregion 
#pragma region CVariant

	class UILIB_API CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};
#pragma endregion


}// namespace DuiLib




#endif // __UTILS_H__