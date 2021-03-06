#pragma once

#include "Win32Utility.h"

class COgreModule;
class COgreModuleManager :
    public TSingleton<COgreModuleManager>
{
    SINGLETON_DECLARE(COgreModuleManager)

public:
    COgreModuleManager() : 
        m_pCurrentModule(NULL) {}
        virtual ~COgreModuleManager() {}

    void Initialise();
    void Finalize();

    template<typename T>
    T* ChangeModule();

    void UpdateModule(float fDeltaFime);
    COgreModule* GetCurrentModule() {return m_pCurrentModule;}

protected:
    COgreModule* m_pCurrentModule;
};
