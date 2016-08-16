#ifndef XUNZIP_HPP
#define XUNZIP_HPP

#include "..\UnCompression.h"

class CDUIUnZip : public DuiLib::CUnCompression
{
public:
	CDUIUnZip() : m_db(NULL){}
	virtual BOOL Open(const TCHAR *filepath);
	virtual BOOL Open(void *z, unsigned int len);
	virtual BOOL IsOpen();
	virtual BOOL Close();

	virtual BOOL Find(const TCHAR *name, int* index, DWORD64 *size);
	virtual BOOL Get(int index, void *dst, DWORD64 len);
private:
	HANDLE m_db;
};
#endif