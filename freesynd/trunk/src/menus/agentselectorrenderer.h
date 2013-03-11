/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;  without even  the implied  warranty of  *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project's  web  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#ifndef MENUS_AGENTSELECTORRENDERER_H_
#define MENUS_AGENTSELECTORRENDERER_H_

#include "ipastim.h"


class AgentSelectorRenderer
{
public:
    /*!
     * This structure holds informations on where the player
     * clicked when he clicked on the IPA meters.
     */
    struct IPAMouseEvent
    {
        IPAMouseEvent(int a, IPAStim::IPAType aType, int aPercentage) {
            agent = a;
            type = aType;
            percentage = aPercentage;
        }

        /*! The type of IPA selected.*/
        IPAStim::IPAType type;
        /*! What agent was clicked.*/
        int agent;
        /*!*/
        int percentage;
    };

    /*!
     * Return an event to describe the IPA the player has clicked.
     * \return NULL if the player didn't click on a IPA meter.
     */
    IPAMouseEvent * scanCoordsForIPA(int x, int y);

    /*!
     * helper to render the IPA bars. Draws all the elements
     * for one bar
     */
    void drawIPABar(int agent, IPAStim *stim);

private:
    static const int kIpaBarWidth;
    static const int kIpaBarHeight;
    static const int kIpaBarLeft13;
    static const int kIpaBarLeft24;
    static const int kIpaBarTop12;
    static const int kIpaBarTop34;
    static const int kIpaYOffset;

    /*!
     * Returns the Y coord of the top left corner of the IPA bar for a given agent slot and IPA Type.
     */
    int getIpaBarTop(size_t agent, IPAStim::IPAType type)
    {
        int top;
        if(agent > 1)
            top = kIpaBarTop34;
        else
            top = kIpaBarTop12;
        
        switch (type) {
            case IPAStim::Adrenaline:
                break;
            case IPAStim::Perception:
                top += kIpaYOffset;
                break;
            case IPAStim::Intelligence:
                top += 2 * kIpaYOffset;
                break;
            default:
                assert(false);
        }
        return top;
    }
    
    /*!
     * Returns the X coord for all IPA bars for a given agent slot.
     */
    int getIpaBarLeftForAgent(size_t agent)
    {
        if(agent % 2 == 1)
            return kIpaBarLeft24;
        else
            return kIpaBarLeft13;
    }
    
    /*!
     * Returns the color for drawing the IPA bar of given type.
     */
    int colourForIpaType(IPAStim::IPAType type)
    {
        switch (type)
        {
            case IPAStim::Adrenaline:
                return fs_cmn::kColorLightRed;
            case IPAStim::Perception:
                return fs_cmn::kColorBlue;
            case IPAStim::Intelligence:
                // TODO: this color needs to be defined
                return 13;
            default:
                assert(false);
        }
        return -1;
    }

    int dim_colour(IPAStim::IPAType type)
    {
        switch (type)
        {
            case IPAStim::Adrenaline:
                return fs_cmn::kColorDarkRed;
            case IPAStim::Perception:
                return fs_cmn::kColorBlueGrey;
            case IPAStim::Intelligence:
                // TODO: this color needs to be defined
                return 5;
            default:
                assert(false);
        }
        return -1;
    }
    
    
    /*!
     *
     */
    int get_percentage(size_t agent, int x)
    {
        int left = getIpaBarLeftForAgent(agent);
        int offset = x - left;
        return (int)(((float)offset / (float) kIpaBarWidth) * 100.0);
    }
};

#endif  // MENUS_AGENTSELECTORRENDERER_H_