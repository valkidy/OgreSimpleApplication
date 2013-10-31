/*
 | This file is code snippets for memory check utility 
 */
#include <stdio.h>
#include <crtdbg.h>
#include <assert.h>
#include <windows.h>

/*
This function describe MEMORY LEAK detect 
 */
void MemoryLeakDetect()
{ 
    /* debug flag */
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}


/*
This function describe HEAP CORRUPTION DETECTED
*/

//override 'new' operator
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW

void HeapCorruptionDetect()
{
    /* debug flag */
    _CrtSetDbgFlag((_CRTDBG_DELAY_FREE_MEM_DF) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));

    char *eror_cstring = new char[5];
    /* uncomment this line will cause heap corruption */
    //strcpy(eror_cstring, "heapcorruption");
    
    /* check memory : assert retrun false if heap corruption detect */
    assert(_CrtCheckMemory());
}

/*
This include header describe usage of visual leak detect

ref:http://www.dotblogs.com.tw/larrynung/archive/2011/12/27/63535.aspx
*/
//#include "vld.h"
