#pragma once

#include "Win32Utility.h"
#include "COgreHeader.h"

class CPanelManager :
    public TSingleton<CPanelManager>
{
    SINGLETON_DECLARE(CPanelManager)
public:
    typedef void (*PANEL_FUNCTION)(Widget*);
     
    typedef std::pair<Widget*, PANEL_FUNCTION> WIDGET_PAIR;

    typedef std::map<std::string, WIDGET_PAIR> WidgetBindingMAP;
    typedef WidgetBindingMAP::iterator WidgetBindingMAP_ITER;
    typedef WidgetBindingMAP::const_iterator WidgetBindingMAP_CITER;

public:
    CPanelManager() : mWidgetLayer(NULL){}
    virtual ~CPanelManager() {}
    virtual void Finalize() {DetechAllWidgets();}
    
        template <typename T>
    T* CreateWidget(const std::string& name, PANEL_FUNCTION _func)
    {
        WidgetBindingMAP_ITER found = m_mapWidgetDelegate.find(name);
        if (found != m_mapWidgetDelegate.end())
            return dynamic_cast<T*>(found->second.first);

        T* _Widget = new T(name);
    
        AttachWidget(_Widget);

        // binding function
        m_mapWidgetDelegate[ name ] = std::make_pair(_Widget, _func);
        
        return _Widget;
    } // End for CreateWidget

    
    void UpdateWidgets();    
    Widget* FindWidget(const std::string& name);

protected:    
    void AttachWidget(Widget* _widget);
    void DetechAllWidgets();

    WidgetBindingMAP m_mapWidgetDelegate;
    Ogre::Overlay* mWidgetLayer;
};

/*=============================================================================
|  widegt function for stats 
=============================================================================*/
void UpdateStats(Widget* _pWidget);

