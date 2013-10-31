#include "Win32Utility.h"

#include <stdio.h>
#include <windows.h>

// for memory usage
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
/*=============================================================================
| implement of OutputDebugString
=============================================================================*/
#ifdef _WIN32_WCE
void 
OutputDebugStringEx( const char*  str );
{
#define MAX_BUFFER_SIZE (8192)
    static WCHAR buf[MAX_BUFFER_SIZE];

    int sz = MultiByteToWideChar( CP_ACP, 0, str, -1, buf, sizeof( buf )/sizeof( *buf ) );
    
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
#define MAX_BUFFER_SIZE (8192)
	static char  buf[MAX_BUFFER_SIZE];
	va_list      ap;

	va_start( ap, fmt );
	vprintf( fmt, ap );

	/* send the string to the debugger as well */
	vsprintf_s( buf, fmt, ap );
	OutputDebugStringEx( buf );
	va_end( ap );

	OutputDebugStringEx( "\n" );
} // End of Log 

size_t 
CalculateProcessMemory()
{
    size_t mem_usage = 0;
#if defined(_MSC_VER)
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (hProcess)
    {
        PROCESS_MEMORY_COUNTERS meminfo;
        GetProcessMemoryInfo(hProcess, &meminfo, sizeof(meminfo));

        mem_usage = meminfo.WorkingSetSize;

        CloseHandle(hProcess);
    }
#else	
	#warning ("Warning : CalcProcessMemory not work instead compiler VC++" )
#endif

    return mem_usage;
} // End of CalcProcessMemory