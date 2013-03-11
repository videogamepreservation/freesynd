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

#include "menus/agentselectorrenderer.h"
#include "gfx/screen.h"

const int AgentSelectorRenderer::kIpaBarWidth = 56;
const int AgentSelectorRenderer::kIpaBarHeight = 10;
const int AgentSelectorRenderer::kIpaBarLeft13 = 4;
const int AgentSelectorRenderer::kIpaBarLeft24 = 68;
const int AgentSelectorRenderer::kIpaBarTop12  = 48;
const int AgentSelectorRenderer::kIpaBarTop34  = 148;
const int AgentSelectorRenderer::kIpaYOffset = 14;

AgentSelectorRenderer::IPAMouseEvent * AgentSelectorRenderer::scanCoordsForIPA(int x, int y)
{
    IPAStim::IPAType type;
    for(size_t a = 0; a < 4; a++)
    {
        int barLeft = getIpaBarLeftForAgent(a);
        if(x >= barLeft && x <= barLeft + kIpaBarWidth)
        {
            // try for Adrenaline bar
            type = IPAStim::Adrenaline;
            int barTop = getIpaBarTop(a, type);
            if( y>= barTop && y <= barTop + kIpaBarHeight)
                return new IPAMouseEvent(a, type, get_percentage(a, x));

            // try for Perception bar
            type = IPAStim::Perception;
            barTop = getIpaBarTop(a, type);
            if( y>= barTop && y <= barTop + kIpaBarHeight)
                return new IPAMouseEvent(a, type, get_percentage(a, x));

            // try for Intelligence bar
            type = IPAStim::Intelligence;
            barTop = getIpaBarTop(a, type);
            if( y>= barTop && y <= barTop + kIpaBarHeight)
                return new IPAMouseEvent(a, type, get_percentage(a, x));
        }
    }

    // No IPA bar was clicked
    return NULL;
}

// helper to render the IPA bars. Draws all the elements
    // for one bar
void AgentSelectorRenderer::drawIPABar(int agent, IPAStim *stim)
{
    // Convert those percentages to pixels
    int amount_x = (float)kIpaBarWidth * ((float)stim->getAmount()/100.0);
    int effect_x = (float)kIpaBarWidth * ((float)stim->getEffect()/100.0);
    int dependency_x = (float)kIpaBarWidth * ((float)stim->getDependency()/100.0);

    IPAStim::IPAType type = stim->getType();
    
    // Draw a bar between the current level and the dependency marker
    // x needs to be leftmost...
    int left, width;
    boxify(left, width, amount_x, dependency_x);
    if(width > 0) {
        g_Screen.drawRect(getIpaBarLeftForAgent(agent) + left,
                          getIpaBarTop(agent, type),
                          width, kIpaBarHeight, colourForIpaType(type));
    }
    
    // NB: this bar stops rendering when it's neck-a-neck with 'amount'
    if(amount_x != effect_x)
    {
        boxify(left, width, effect_x, dependency_x);
        if(width > 0) {
            g_Screen.drawRect(getIpaBarLeftForAgent(agent) + left,
                              getIpaBarTop(agent, type),
                              width, kIpaBarHeight, dim_colour(type));
        }
    }
    
    // Draw a vertical white line to mark the dependency level
    g_Screen.drawVLine(getIpaBarLeftForAgent(agent) + dependency_x,
                      getIpaBarTop(agent, type), kIpaBarHeight, 12);
}