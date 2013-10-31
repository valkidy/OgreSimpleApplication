#include "COgreHeader.h"

SINGLETON_IMPLEMENT(COgreModuleManager)

void 
COgreModuleManager::Initialise()
{
    ChangeModule<CSampleModule>();
}

template<typename T> T* 
COgreModuleManager::ChangeModule()
{    
    std::string strCurrentModuleName;
    if (m_pCurrentModule)
    {
        strCurrentModuleName = typeid(m_pCurrentModule).name();

        m_pCurrentModule->destroyScene();
        delete m_pCurrentModule;
    } // End if

    m_pCurrentModule = new T;
    m_pCurrentModule->createScene();

    const std::string& strNextModuleName = typeid(static_cast<T*>(m_pCurrentModule)).name();

    LOG("ChangeModule from %s to %s", strCurrentModuleName.c_str(), strNextModuleName.c_str());

    return static_cast<T*>(m_pCurrentModule);
}

void 
COgreModuleManager::UpdateModule(float fDeltaFime)
{
    if (m_pCurrentModule)
    {
        m_pCurrentModule->updateScene(fDeltaFime);
    } // End if
}