#include "Win32Utility.h"

#include <stdio.h>
#include <windows.h>

#ifdef _WIN32_WCE
void 
OutputDebugStringEx( const char*  str );
{
    static WCHAR  buf[8192];


    int sz = MultiByteToWideChar( CP_ACP, 0, str, -1, buf,
                                  sizeof ( buf ) / sizeof ( *buf ) );
    if ( !sz )
      lstrcpyW( buf, L"OutputDebugStringEx: MultiByteToWideChar failed" );

    OutputDebugStringW( buf );
} // End of OutputDebugStringEx
#else
    #define OutputDebugStringEx  OutputDebugStringA
#endif


void 
LOG(const char* fmt, ...)
{
	static char  buf[8192];
	va_list      ap;

	va_start( ap, fmt );
	vprintf( fmt, ap );
	/* send the string to the debugger as well */
	vsprintf_s( buf, fmt, ap );
	OutputDebugStringEx( buf );
	va_end( ap );

	OutputDebugStringEx( "\n" );
} // End of Log 
