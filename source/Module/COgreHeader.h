#ifndef __COGRE_HEADER__
#define __COGRE_HEADER__

#ifdef _DEBUG
    #pragma comment(lib , "OgreMain_d.lib")
    #pragma comment(lib , "OIS_d.lib")
#else
    #pragma comment(lib , "OgreMain.lib")
    #pragma comment(lib , "OIS.lib")
#endif

#pragma warning(disable: 4010)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4193)
#pragma warning(disable: 4275)

#include <Ogre.h>
#include <OIS/OIS.h>

// simple widget
#include "CParamPanel.h"
#include "CParamPanelManager.h"

// module
#include "COgreModule.h"
#include "COgreModuleManager.h"
#include "COgreModuleTemplate.h"

// module instance
#include "CSampleModule.h"

#endif