#ifndef DUILIB_CUNCOMPRESSION_H_
#define DUILIB_CUNCOMPRESSION_H_

#pragma once

namespace DuiLib
{

	class CUnCompression
	{
	public:
		virtual BOOL Open(const TCHAR *filepath) = 0;
		virtual BOOL Open(void *z, unsigned int len) = 0;
		virtual BOOL IsOpen( ) = 0;
		virtual BOOL Close( ) = 0;

		virtual BOOL Find(const TCHAR *name, int* index, DWORD64 *size) = 0;
		virtual BOOL Get(int index, void *dst, DWORD64 len) = 0;
	};

}
#endif