/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/


#ifndef COAJNR_DEBUGOVERLAYFRAMELISTENER_INCLUDED
#define COAJNR_DEBUGOVERLAYFRAMELISTENER_INCLUDED

#include "Dependencies.h"

#include "GraphicsManager.h"

namespace CoAJnR
{

/**
 * overlay displaying frame rates and ogre logo
 *
 * this overlay is used by the GraphicsManager to render an ogre logo and a
 * small window containing the current updates per second (FPS) for the
 * PhysicsManager and GraphicsManager, 
 * worst value for both and the current batch and triangle count.
 *
 * worst values will be resetted every 5 seconds.
 * 
 * @note based on the overlay code from the ogre samples
 */
class DebugOverlayFrameListener : public Ogre::FrameListener
{
    /// a debug text in the overlay
	std::string m_debugText;

    /// the overlay object itself
	Ogre::Overlay* m_debugOverlay;

    /// the window the overlay is rendered in (needed to retrieve statistics)
	Ogre::RenderWindow* m_window;

    /// the FPS reported by the PhysicsManager
    double m_physicsFPS;
    
    /// the worst FPS reported by the PhysicsManager
    double m_worstPhysicsFPS;

    /**
     * update the stats data on the overlay.
     */
	void updateStats(void)
	{
		static Ogre::String currFps = "Graphics FPS: ";
		static Ogre::String phyFps = "Physics FPS: ";

		static Ogre::String worstPhysicsFPS = "Worst Physics FPS: ";
		static Ogre::String worstGraphicsFPS = "Worst Graphics FPS: ";

		static Ogre::String tris = "Triangle Count: ";
		static Ogre::String batches = "Batch Count: ";

		try {
			Ogre::OverlayElement* guiPhy = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/AverageFps");
			Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/CurrFps");

			Ogre::OverlayElement* guiBest = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/BestFps");
			Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/WorstFps");

			Ogre::OverlayElement* guiTris = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/NumTris");
			Ogre::OverlayElement* guiBatches = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/NumBatches");
			Ogre::OverlayElement* guiDbg = Ogre::OverlayManager::
			        getSingleton().getOverlayElement("Core/DebugText");

			const Ogre::RenderTarget::FrameStats& stats = 
			        m_window->getStatistics();
			        
			guiPhy->setCaption(phyFps + 
			        Ogre::StringConverter::toString((Ogre::Real)m_physicsFPS));
			guiCurr->setCaption(currFps + 
			        Ogre::StringConverter::toString(1.0f /
	                (Ogre::Real)GraphicsManager::get().lastFrameTime()));

			guiBest->setCaption(worstPhysicsFPS + 
			        Ogre::StringConverter::toString(
			        (Ogre::Real)m_worstPhysicsFPS));
			        
			guiWorst->setCaption(worstGraphicsFPS + 
			        Ogre::StringConverter::toString(1.0f /
	                (Ogre::Real)GraphicsManager::get().worstFrameTime()));

			guiTris->setCaption(tris + 
			        Ogre::StringConverter::toString(stats.triangleCount));
			guiBatches->setCaption(batches + 
			        Ogre::StringConverter::toString(stats.batchCount));

			guiDbg->setCaption(m_debugText);
		}
		catch(...) { /* ignore */ }
	}

public:
    /**
     * ctor.
     *
     * @param window the window to retrieve graphics statistics from.
     *
     * @note this method is not thread safe and can only be used by the
     * GraphicsManager thread.
     */
	DebugOverlayFrameListener(Ogre::RenderWindow* window)
	{
	    m_window = window;
	
		m_debugOverlay = Ogre::OverlayManager::getSingleton().
		        getByName("Core/DebugOverlay");

		showDebugOverlay(true);
		
		m_physicsFPS = 0;
		m_worstPhysicsFPS = 0;
		
		m_debugText = "CoA Jump 'n' Run";
	}

    /**
     * dtor.
     */
	virtual ~DebugOverlayFrameListener()
	{

	}

    /**
     * toggle the debug overlay, turn it on or off.
     *
     * @note this method is not thread safe and can only be used by the
     * GraphicsManager thread.
     */
	void showDebugOverlay(bool show)
	{
		if (m_debugOverlay)
		{
			if (show)
				m_debugOverlay->show();
			else
				m_debugOverlay->hide();
		}
	}

    /**
     * hook which gets called by ogre, updates the overlay after every frame.
     *
     * @note this method is not thread safe and can only be used by the
     * GraphicsManager thread.
     */
	bool frameEnded(const Ogre::FrameEvent& evt)
	{
        updateStats();	
		return true;
	}

    /**
     * set the current FPS value of the PhysicsManager.
     *
     * the PhysicsManager reports this value via a DeferredCall.
     *
     * @note this method is not thread safe and can only be used by the
     * GraphicsManager thread.
     */
    void setPhysicsFPS(double fps)
    {
        m_physicsFPS = fps;
    }
    
    /**
     * set the worst FPS value of the PhysicsManager.
     *
     * the PhysicsManager reports this value via a DeferredCall.
     *
     * @note this method is not thread safe and can only be used by the
     * GraphicsManager thread.
     */
    void setWorstPhysicsFPS(double fps)
    {
        m_worstPhysicsFPS = fps;
    }

};

}

#endif

