#include "CParamPanelManager.h"

SINGLETON_IMPLEMENT(CPanelManager)

void 
CPanelManager::DetechAllWidgets()
{
    if (mWidgetLayer)
    {
        for (WidgetBindingMAP_ITER iter=m_mapWidgetDelegate.begin() ; iter!=m_mapWidgetDelegate.end() ; ++iter)
        {
            delete iter->second.first;
        } // End for

        m_mapWidgetDelegate.clear();
        
        Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
        om.destroy(mWidgetLayer);
        
        mWidgetLayer = NULL;
    } // End if
}

void
CPanelManager::AttachWidget(Widget* _widget)
{
    assert(_widget);

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    if (!mWidgetLayer)
    {
        mWidgetLayer = om.create("WidgetsLayer");        
        mWidgetLayer->show();
    } // End if
    
    mWidgetLayer->add2D((Ogre::OverlayContainer*)(_widget->getOverlayElement()));
}
 
void
CPanelManager::UpdateWidgets()
{
    for (WidgetBindingMAP_ITER iter=m_mapWidgetDelegate.begin() ; iter!=m_mapWidgetDelegate.end() ; ++iter)
    {
        Widget* _widget = iter->second.first;

        if (_widget->isVisible())
            (*(iter->second.second))(_widget);
    } // End for
}

Widget*
CPanelManager::FindWidget(const std::string& name)
{
    WidgetBindingMAP_ITER found = m_mapWidgetDelegate.find(name);
    if (found == m_mapWidgetDelegate.end())
        return NULL;
    
    return found->second.first;
}


//=============================================================================
void UpdateStats(Widget* _widget)
{
    ParamsPanel* _panel = dynamic_cast<ParamsPanel*>(_widget);
    assert(_panel);

    // param names
    const Ogre::StringVector& params = _panel->getAllParamNames();
    if (params.empty())
    {
        Ogre::String ParamNames[] = {"Camera Pos","Last FPS","Average FPS","Best FPS","Worst FPS","Triangles","Batches","ProcessMemory"};
        //stat
	    Ogre::StringVector stats(ParamNames, ParamNames + sizeof(ParamNames)/sizeof(Ogre::String));

        _panel->setPanelWidth(250);
        _panel->setAllParamNames(stats);
        _panel->setPanelAlignment(Ogre::GHA_RIGHT, Ogre::GVA_BOTTOM);
    } // End if
    
    // param values
    if (_panel->getOverlayElement()->isVisible())    
	{		
        Ogre::StringVector values;

        Ogre::SceneManager* pSceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
        assert(pSceneMgr);
        Ogre::Camera* pCamera = pSceneMgr->getCamera("DefaultCamera");
        assert(pCamera);
        const Ogre::Vector3& vPosition = pCamera->getPosition();

        char buf[64];
        sprintf_s(buf, sizeof(buf), "%.2f, %.2f, %.2f", vPosition.x, vPosition.y, vPosition.z);
        values.push_back(buf);

        Ogre::RenderWindow* _pRenderWindow = (Ogre::RenderWindow*)Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window");
        assert(_pRenderWindow);    
        const Ogre::RenderTarget::FrameStats& stats = _pRenderWindow->getStatistics();

        // fps info
		std::ostringstream oss;
        Ogre::String str;

        float fps[] = {stats.lastFPS, stats.avgFPS, stats.bestFPS, stats.worstFPS};
        int size = sizeof(fps)/sizeof(float);
        for (int j = 0 ; j < size ; ++j)
        {
            oss.str("");
            oss << std::fixed << std::setprecision(1) << fps[j];
            str = oss.str();
		    for (int i = str.length() - 5; i > 0; i -= 3) { str.insert(i, 1, ','); }
		    values.push_back(str);
        } // End for
        
        // triangle info
		str = Ogre::StringConverter::toString(stats.triangleCount);
		for (int i = str.length() - 3; i > 0; i -= 3) { str.insert(i, 1, ','); }
		values.push_back(str);

		str = Ogre::StringConverter::toString(stats.batchCount);
		for (int i = str.length() - 3; i > 0; i -= 3) { str.insert(i, 1, ','); }
		values.push_back(str);

/*
 * calculate total triangle count for all render targets
 *
        size_t uiTotalTriangleCount = 0, uiTotalBatchCount = 0;
        Ogre::RenderSystem* _pRenderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();
        assert(_pRenderSystem);
        Ogre::RenderSystem::RenderTargetIterator it = _pRenderSystem->getRenderTargetIterator();
	    while(it.hasMoreElements())
	    {
            uiTotalTriangleCount += it.current()->second->getTriangleCount();
		    uiTotalBatchCount += it.current()->second->getBatchCount();

            it.getNext();
	    } // End while
*/
        // memory info
        oss.str("");
		oss << std::fixed << std::setprecision(3) << CalculateProcessMemory()/(1024.0f*1024.0f);
        str = oss.str();
 		values.push_back(str);

		_panel->setAllParamValues(values);
	}
}