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
    virtual ~COgreModuleManager() { if (m_pCurrentModule) delete m_pCurrentModule;}

    void Initialise();

    template<typename T>
    T* ChangeModule();

    void UpdateModule(float fDeltaFime);
    COgreModule* GetCurrentModule() {return m_pCurrentModule;}

protected:
    COgreModule* m_pCurrentModule;
};
