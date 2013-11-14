#include "GLNativeRender.h"
#include "GlutStuff.h"

void 
GLNativeRender::preRender()
{
    // save matrices
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity(); //Texture addressing should start out as direct.
  
	// save attribs
	glPushAttrib(GL_ALL_ATTRIB_BITS);

    // depth
    //GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	//GLboolean stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
	glDisable(GL_STENCIL_TEST);

	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}

void 
GLNativeRender::postRender()
{
    // restore
	//if (depthTestEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	//if (stencilTestEnabled)
	{
		glEnable(GL_STENCIL_TEST);
	}

    // restore original state
	glPopAttrib();

	// restore matrices
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}