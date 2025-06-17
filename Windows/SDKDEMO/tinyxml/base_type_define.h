
#ifndef __BASE_TYPE_DEFINE_
#define __BASE_TYPE_DEFINE_

#define _WIN32_WINNT 0x0700
#include <windows.h>
#include <assert.h>
#include <MMSystem.h>
#include <mmreg.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
typedef char			tchar, tint8;/////8bit signed
typedef unsigned char	tbyte, tuchar, tuint8; ///////8bit unsigned
typedef short			tint16; ////16bit  signed
typedef unsigned short  tuint16; ////16bit  unsigned
typedef int				tint32; /////32 bit  signed
typedef unsigned int	tuint32; /////32 bit unsigned
typedef __int64			tint64; //////64bit  signed
typedef unsigned __int64   tuint64; ////64bit  unsigned


/************************************************************************************
 *基本类型的扩展类型
************************************************************************************/
#if defined(OS_IS_64BIT)
    typedef tuint64     tuint_ptr;
    typedef tint64      tint_ptr;
#else
    typedef tuint32     tuint_ptr;
    typedef tint32      tint_ptr;
#endif /////


/************************************************************************/
/*                                                                      */
/************************************************************************/
//#define assert(X)      assert(X)

#define DEBUG_NEW  new

#if defined(min)
#undef min 
#endif

//#define  snprintf _snprintf 




/************************************************************************/
/*                                                                      */
/************************************************************************/
__inline bool operator < (const GUID& guidOne, const GUID& guidOther)
{
	return (memcmp(&guidOne, &guidOther, sizeof(GUID)) < 0);
}

__inline bool operator > (const GUID& guidOne, const GUID& guidOther)
{
	return (memcmp(&guidOne, &guidOther, sizeof(GUID)) > 0);
}


typedef DWORD TRHEAD_RETVALUE;

const FILETIME ZERO_FILE_TIME={0,0};
__inline bool operator < (FILETIME leftTime, FILETIME rightTime)
{
	ULONGLONG *pLeftTime=(ULONGLONG*)&leftTime;
	ULONGLONG *pRightTime=(ULONGLONG*)&rightTime;
	return ((*pLeftTime) < (*pRightTime));
}

__inline bool operator > (FILETIME leftTime, FILETIME rightTime)
{
	ULONGLONG *pLeftTime=(ULONGLONG*)&leftTime;
	ULONGLONG *pRightTime=(ULONGLONG*)&rightTime;
	return ((*pLeftTime) > (*pRightTime));
}

__inline bool operator == (FILETIME leftTime, FILETIME rightTime)
{
	return ((leftTime.dwHighDateTime== rightTime.dwHighDateTime) && 
		(leftTime.dwLowDateTime==rightTime.dwLowDateTime));
}

__inline bool operator != (FILETIME leftTime, FILETIME rightTime)
{
	return !(leftTime == rightTime);
}

__inline FILETIME & operator += (FILETIME &ftTime, const ULONGLONG &ullTime)
{
	ULONGLONG *pftTime = (ULONGLONG *)(&ftTime);
	*pftTime += ullTime;
	return ftTime;
}

__inline FILETIME & operator -= (FILETIME &ftTime, const ULONGLONG &ullTime)
{
	ULONGLONG *pftTime = (ULONGLONG *)(&ftTime);
	*pftTime -= ullTime;
	return ftTime;
}

__inline LONGLONG operator - (const FILETIME &ftTime1, const FILETIME &ftTime2)
{
	const LONGLONG *pllTime1 = (const LONGLONG *)(&ftTime1);
	const LONGLONG *pllTime2 = (const LONGLONG *)(&ftTime2);

	return *pllTime1 - *pllTime2;
}

#endif