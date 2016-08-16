#include "StdAfx.h"
#include "XUnZip.h"
#include "XUnZipBase.h"


BOOL CDUIUnZip::Open(const TCHAR *filepath)
{

	if (filepath == NULL)
	{
		return FALSE;
	}

	TUnzip *pHandle = new TUnzip( );
	ASSERT(pHandle != NULL);
	if (ZR_OK != pHandle->Open((void*)(filepath), 0, ZIP_FILENAME))
	{
		delete pHandle;
		pHandle = NULL;
		return FALSE;
	}

	m_db = (HANDLE)pHandle;

	return TRUE;
}

BOOL CDUIUnZip::Open(void *z, unsigned int len)
{
	TUnzip *pHandle = new TUnzip( );
	ASSERT(pHandle != NULL);
	if (ZR_OK != pHandle->Open(z, len, ZIP_MEMORY))
	{
		delete pHandle;
		pHandle = NULL;
		return FALSE;
	}
	m_db = (HANDLE)pHandle;

	return TRUE;
}

BOOL CDUIUnZip::IsOpen( )
{
	return m_db == NULL;
}

BOOL  CDUIUnZip::Close( )
{
	if (m_db == NULL)
		return TRUE;

	TUnzip *pHandle = (TUnzip *)m_db;
	if (ZR_OK == pHandle->Close( ))
	{
		delete pHandle;
		pHandle = NULL;
		m_db = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL CDUIUnZip::Find(const TCHAR *name, int* index, DWORD64 *size)
{
	ZIPENTRY ze;
	TUnzip *pHandle = (TUnzip *)m_db;
	ASSERT(pHandle != NULL);
	if (pHandle == NULL)
	{
		return FALSE;
	}
	if (ZR_OK == pHandle->Find(name, false, index, &ze))
	{
		*size = ze.unc_size;
		return TRUE;
	}
	return FALSE;
}

BOOL CDUIUnZip::Get(int index, void *dst, DWORD64 len)
{
	TUnzip *pHandle = (TUnzip *)m_db;
	ASSERT(pHandle != NULL);
	ZRESULT res = pHandle->Unzip(index, dst, len, ZIP_MEMORY);
	if (ZR_OK != res && ZR_MORE != res)
	{
		return FALSE;
	}
	return TRUE;
}