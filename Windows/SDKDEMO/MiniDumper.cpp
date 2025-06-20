#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <tchar.h>
#include <dbghelp.h>
#include "miniDumper.h"
#include "direct.h"
#pragma comment(lib, "Dbghelp.lib")

CMiniDumper g_miniDumper( true );

#ifdef UNICODE
#define _tcssprintf wsprintf
#define tcsplitpath _wsplitpath
#else
#define _tcssprintf sprintf
#define tcsplitpath _splitpath
#endif

const int USER_DATA_BUFFER_SIZE = 4096;

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
CMiniDumper* CMiniDumper::s_pMiniDumper = NULL;
LPCRITICAL_SECTION CMiniDumper::s_pCriticalSection = NULL;


//-----------------------------------------------------------------------------
// Name: CMiniDumper()
// Desc: Constructor
//-----------------------------------------------------------------------------
CMiniDumper::CMiniDumper( bool bPromptUserForMiniDump )
{
	// Our CMiniDumper should act alone as a singleton.
	assert( !s_pMiniDumper );

	s_pMiniDumper = this;
	m_bPromptUserForMiniDump = bPromptUserForMiniDump;

	// The SetUnhandledExceptionFilter function enables an application to 
	// supersede the top-level exception handler of each thread and process.
	// After calling this function, if an exception occurs in a process 
	// that is not being debugged, and the exception makes it to the 
	// unhandled exception filter, that filter will call the exception 
	// filter function specified by the lpTopLevelExceptionFilter parameter.
	::SetUnhandledExceptionFilter( unhandledExceptionHandler );

	// Since DBGHELP.dll is not inherently thread-safe, making calls into it 
	// from more than one thread simultaneously may yield undefined behavior. 
	// This means that if your application has multiple threads, or is 
	// called by multiple threads in a non-synchronized manner, you need to  
	// make sure that all calls into DBGHELP.dll are isolated via a global
	// critical section.
	s_pCriticalSection = new CRITICAL_SECTION;

	if( s_pCriticalSection )
		InitializeCriticalSection( s_pCriticalSection );
}

//-----------------------------------------------------------------------------
// Name: ~CMiniDumper()
// Desc: Destructor
//-----------------------------------------------------------------------------
CMiniDumper::~CMiniDumper( void )
{
	if( s_pCriticalSection )
	{
		DeleteCriticalSection( s_pCriticalSection );
		delete s_pCriticalSection;
	}
}

//-----------------------------------------------------------------------------
// Name: unhandledExceptionHandler()
// Desc: Call-back filter function for unhandled exceptions
//-----------------------------------------------------------------------------
LONG CMiniDumper::unhandledExceptionHandler( _EXCEPTION_POINTERS *pExceptionInfo )
{
	if( !s_pMiniDumper )
		return EXCEPTION_CONTINUE_SEARCH;

	return s_pMiniDumper->writeMiniDump( pExceptionInfo );
}

//-----------------------------------------------------------------------------
// Name: setMiniDumpFileName()
// Desc: 
//-----------------------------------------------------------------------------
void CMiniDumper::setMiniDumpFileName( void )
{
	time_t currentTime;
	time(&currentTime);

	strcat(m_szAppPath,"dump\\");
	_mkdir(m_szAppPath);  

	TCHAR chTmp[100];  
	memset(chTmp, 0, sizeof(chTmp));  
	struct tm *p;     
	p = localtime(&currentTime );  
	if(p == NULL) return;  
	p->tm_year = p->tm_year + 1900;  
	p->tm_mon = p->tm_mon + 1;  

	_tcssprintf(chTmp, _T("%04d%02d%02d_%02d%02d%02d"),  
		p->tm_year, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);  

	_tcssprintf( m_szMiniDumpPath,  
		_T("%s%s_%s.dmp"),  
		m_szAppPath,  
		m_szAppBaseName,  
		chTmp ); 
}

//-----------------------------------------------------------------------------
// Name: getImpersonationToken()
// Desc: The method acts as a potential workaround for the fact that the 
//       current thread may not have a token assigned to it, and if not, the 
//       process token is received.
//-----------------------------------------------------------------------------
bool CMiniDumper::getImpersonationToken( HANDLE* phToken )
{
	*phToken = NULL;

	if( !OpenThreadToken( GetCurrentThread(),
		TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
		TRUE,
		phToken) )
	{
		if( GetLastError() == ERROR_NO_TOKEN )
		{
			// No impersonation token for the current thread is available. 
			// Let's go for the process token instead.
			if( !OpenProcessToken( GetCurrentProcess(),
				TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
				phToken) )
				return false;
		}
		else
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: enablePrivilege()
// Desc: Since a MiniDump contains a lot of meta-data about the OS and 
//       application state at the time of the dump, it is a rather privileged 
//       operation. This means we need to set the SeDebugPrivilege to be able 
//       to call MiniDumpWriteDump.
//-----------------------------------------------------------------------------
BOOL CMiniDumper::enablePrivilege( LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld )
{
	BOOL bOk = FALSE;

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bOk = LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid );

	if( bOk )
	{
		DWORD cbOld = sizeof(*ptpOld);
		bOk = AdjustTokenPrivileges( hToken, FALSE, &tp, cbOld, ptpOld, &cbOld );
	}

	return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

//-----------------------------------------------------------------------------
// Name: restorePrivilege()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMiniDumper::restorePrivilege( HANDLE hToken, TOKEN_PRIVILEGES* ptpOld )
{
	BOOL bOk = AdjustTokenPrivileges(hToken, FALSE, ptpOld, 0, NULL, NULL);
	return ( bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()) );
}

//-----------------------------------------------------------------------------
// Name: writeMiniDump()
// Desc: 
//-----------------------------------------------------------------------------
LONG CMiniDumper::writeMiniDump( _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	m_pExceptionInfo = pExceptionInfo;

	HANDLE hImpersonationToken = NULL;
	if( !getImpersonationToken( &hImpersonationToken ) )
		return FALSE;
	
	if( GetModuleFileName( NULL, m_szAppPath, _MAX_PATH ) )
	{	
		TCHAR *pSlash = _tcsrchr( m_szAppPath, '\\' );  

		if( pSlash )  
		{  
			_tcscpy( m_szAppBaseName, pSlash + 1);  
			*(pSlash+1) = 0;  
		}  
	}

	LPCTSTR szResult = NULL;


	TCHAR szScratch[USER_DATA_BUFFER_SIZE];

	setMiniDumpFileName();

	// Ask the user if he or she wants to save a mini-dump file...
	wsprintf( szScratch,
		_TEXT("There was an unexpected error:\n\nWould you ")
		_TEXT("like to create a mini-dump file?\n\n%s "),
		m_szMiniDumpPath);

	// Create the mini-dump file...
	HANDLE hFile = ::CreateFile( m_szMiniDumpPath, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL );

	if( hFile != INVALID_HANDLE_VALUE )
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId          = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers    = NULL;

		// We need the SeDebugPrivilege to be able to run MiniDumpWriteDump
		TOKEN_PRIVILEGES tp;
		BOOL bPrivilegeEnabled = enablePrivilege( SE_DEBUG_NAME, hImpersonationToken, &tp );

		BOOL bOk;

		// DBGHELP.dll is not thread-safe, so we need to restrict access...
		EnterCriticalSection( s_pCriticalSection );
		{
			// Write out the mini-dump data to the file...
			bOk = ::MiniDumpWriteDump( GetCurrentProcess(),
				GetCurrentProcessId(),
				hFile,
				MiniDumpNormal,
				&ExInfo,
				NULL,
				NULL );
		}
		LeaveCriticalSection( s_pCriticalSection );

		// Restore the privileges when done
		if( bPrivilegeEnabled )
			restorePrivilege( hImpersonationToken, &tp );

		if( bOk )
		{
			szResult = NULL;
			retval = EXCEPTION_EXECUTE_HANDLER;
		}
		else
		{
			wsprintf( szScratch,
				_TEXT("Failed to save the mini-dump file to '%s' (error %d)"),
				m_szMiniDumpPath,
				GetLastError() );

			szResult = szScratch;
		}

		::CloseHandle( hFile );
	}
	else
	{
		wsprintf( szScratch,
			_TEXT("Failed to create the mini-dump file '%s' (error %d)"),
			m_szMiniDumpPath,
			GetLastError() );

		szResult = szScratch;
	}

	if( szResult && m_bPromptUserForMiniDump )
		::MessageBox( NULL, szResult, NULL, MB_OK );

	::TerminateProcess( GetCurrentProcess(), 0 );

	return retval;
}

