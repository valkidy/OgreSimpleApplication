#include <stdio.h>
#include <windows.h>

#include "Win32Platform.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int)
{
    int argc = __argc;
    char** argv = __argv;
    void* pInstance = (void*)hInst;
    
    CheckMemory();
    
    COgreApplication app;
    try
    {
        //COgreApplication app;
        if (true == app.ApplicationInit())
            app.Run();

        //app.ApplicationTerminate();
    }
    catch(Ogre::Exception err)
    {
        MessageBoxA(NULL, err.what(), "An exception has occurred!", MB_ICONERROR | MB_TASKMODAL);
    }
    app.ApplicationTerminate();

    return 0;
}
