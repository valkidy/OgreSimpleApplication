#pragma once

/*---------------------------------------------------------------------------
 | Simple overlay widget from Ogre3d example
 ---------------------------------------------------------------------------*/
#include "OgreFontManager.h"
#include "OgreBorderPanelOverlayElement.h"
#include "OgreTextAreaOverlayElement.h"

#if OGRE_UNICODE_SUPPORT
	#define DISPLAY_STRING_TO_STRING(DS) (DS.asUTF8())
#else
	#define DISPLAY_STRING_TO_STRING(DS) (DS)
#endif

/*=============================================================================
| Abstract base class for all widgets.
=============================================================================*/
class Widget
{
public:		
	Widget() { mElement = 0;}
	virtual ~Widget() {}

	Ogre::OverlayElement* getOverlayElement() { return mElement;}
	const Ogre::String& getName() { return mElement->getName();}
    void setVisible(bool bVisible) { (bVisible) ? mElement->show() : mElement->hide();}
	bool isVisible() { return mElement->isVisible(); }	
protected:
	Ogre::OverlayElement* mElement;		
};

/*=============================================================================
| Basic parameters panel widget for 
=============================================================================*/
class ParamsPanel : public Widget
{
public:
	// Do not instantiate any widgets directly. Use SdkTrayManager.
	ParamsPanel(const Ogre::String& name)
	{
		mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("SdkTrays/ParamsPanel", "BorderPanel", name);
		Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
		mNamesArea = (Ogre::TextAreaOverlayElement*)c->getChild(getName() + "/ParamsPanelNames");
		mValuesArea = (Ogre::TextAreaOverlayElement*)c->getChild(getName() + "/ParamsPanelValues");
	}

    virtual ~ParamsPanel() {}

    // geometry attribute
    void setPanelWidth(Ogre::Real width) {mElement->setWidth(width);}
    void setPanelAlignment(Ogre::GuiHorizontalAlignment _h, Ogre::GuiVerticalAlignment _v)
    {
        if (Ogre::GMM_PIXELS == mElement->getMetricsMode())
        {
            if (Ogre::GHA_RIGHT == _h)
                mElement->setLeft(-mElement->getWidth());
            if (Ogre::GHA_CENTER == _h)
                mElement->setLeft(-mElement->getWidth()/2.0f);
            if (Ogre::GVA_BOTTOM == _v)
                mElement->setTop(-mElement->getHeight());
            if (Ogre::GHA_CENTER == _v)
                mElement->setTop(-mElement->getHeight()/2.0f);
        
        }
        
        mElement->setHorizontalAlignment(_h);
        mElement->setVerticalAlignment(_v);
    }

    // param attribute
	void setAllParamNames(const Ogre::StringVector& paramNames)
	{
		mNames = paramNames;
		mValues.clear();
		mValues.resize(mNames.size(), "");        
		mElement->setHeight(mNamesArea->getTop() * 2 + mNames.size() * mNamesArea->getCharHeight());
		updateText();
	}

    const Ogre::StringVector& getAllParamNames() 
    { 
        return mNames;
    }

	void setAllParamValues(const Ogre::StringVector& paramValues)
	{
		mValues = paramValues;
		mValues.resize(mNames.size(), "");
		updateText();
	}

	const Ogre::StringVector& getAllParamValues() 
    { 
        return mValues;
    }

protected:
	/*-----------------------------------------------------------------------------
	| Internal method - updates text areas based on name and value lists.
	-----------------------------------------------------------------------------*/
	void updateText()
	{
		Ogre::DisplayString namesDS;
		Ogre::DisplayString valuesDS;

		for (unsigned int i = 0; i < mNames.size(); i++)
		{
			namesDS.append(mNames[i] + ":\n");
			valuesDS.append(mValues[i] + "\n");
		}

		mNamesArea->setCaption(namesDS);
		mValuesArea->setCaption(valuesDS);
	}

	Ogre::TextAreaOverlayElement* mNamesArea;
	Ogre::TextAreaOverlayElement* mValuesArea;
	Ogre::StringVector mNames;
	Ogre::StringVector mValues;
};