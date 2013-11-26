#include "Win32Application.h"
#include <stdio.h>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int)
{
    int argc = __argc;
    char** argv = __argv;
    void* pInstance = (void*)hInst;
    
    CHECK_MEMORY();

    //_CrtSetBreakAlloc(30565);

    COgreApplication app;

    try
    {
        if (true == app.ApplicationInit())
            app.Run();
    }
    catch(Ogre::Exception err)
    {
        MessageBoxA(NULL, err.what(), "An exception has occurred!", MB_ICONERROR | MB_TASKMODAL);
    }

    app.ApplicationTerminate();

    return 0;
}
