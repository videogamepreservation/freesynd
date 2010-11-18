/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "debriefmenu.h"

DebriefMenu::DebriefMenu(MenuManager *m) : Menu(m, "debrief", "mdebrief.dat",
     "mdeout.dat") {
    int y = 35;
    int right_x = 310;
    int left_x = 20;
    addStatic(100, y, "MISSION DEBRIEFING", 3, true);
    y = 100;
    addStatic(left_x, y, "MISSION STATISTICS", 1, false);
    // TODO compute size dynamically
    separatorSize_ = 210;

    y = 120;
    addStatic(left_x, y, "MISSION STATUS", 1, false);
    txtStatusId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "AGENT USED", 1, false);
    txtUsedId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "AGENT TAKEN", 1, false);
    txtAgentCapturedId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "MISSION DURATION", 1, false);
    txtTimeId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "ENEMY KILLED", 1, false);
    txtAgentKilledId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "CRIMINAL KILLED", 1, false);
    txtCrimKilledId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "CIVIL KILLED", 1, false);
    txtCivilKilledId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "POLICE KILLED", 1, false);
    txtPoliceKilledId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "GARD KILLED", 1, false);
    txtGardKilledId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "PERSON CONVINCED", 1, false);
    txtConvincedId_ = addStatic(right_x, y, "", 1, false);
    y += 15;
    addStatic(left_x, y, "SHOOTING PRECISION", 1, false);
    txtPrecisionId_ = addStatic(right_x, y, "", 1, false);

    y = 352;
    addOption(43, y, "ACCEPT", 1, KEY_F5, "map");
    addOption(535, y, "MENU", 1, KEY_F6, "main");
}

/*!
 * This method sets the menu cursor and shows it and
 * updates infos based on mission statistics.
 */
void DebriefMenu::handleShow() {
    g_System.useMenuCursor();
    g_System.showCursor();

    Mission *pMission = g_Session.getMission();
    MissionStats *pStats = pMission->getStatistics();

    if (pMission->getStatus() == Mission::FAILED) {
        setStaticText(txtStatusId_, "MISSION FAILED");
    } else if (pMission->getStatus() == Mission::COMPLETED) {
        setStaticText(txtStatusId_, "MISSION COMPLETED");
    } else if (pMission->getStatus() == Mission::ABORTED) {
        setStaticText(txtStatusId_, "MISSION ABORTED");
    }

    setStaticTextFormated(txtUsedId_, "%i", pStats->agents);
    setStaticTextFormated(txtAgentCapturedId_, "%i", pStats->agentCaptured);

    int days = 0;
    int hours = 0;
    g_Session.getDayHourFromPeriod(pStats->mission_duration, days, hours);
    setStaticTextFormated(txtTimeId_, "%02d HOURS %d DAYS", hours, days);
    setStaticTextFormated(txtAgentKilledId_, "%i", pStats->ennemyKilled);
    setStaticTextFormated(txtCrimKilledId_, "%i", pStats->criminalKilled);
    setStaticTextFormated(txtCivilKilledId_, "%i", pStats->civilKilled);
    setStaticTextFormated(txtPoliceKilledId_, "%i", pStats->policeKilled);
    setStaticTextFormated(txtGardKilledId_, "%i", pStats->guardKilled);
    setStaticTextFormated(txtConvincedId_, "%i", pStats->convinced);

    if (pStats->nbOfShots == 0) {
        setStaticTextFormated(txtPrecisionId_, "NO BULLET FIRED");;
    } else {
        int precision = (pStats->nbOfHits * 100) / pStats->nbOfShots ;
        setStaticTextFormated(txtPrecisionId_, "%i%%", precision);
    }
}

void DebriefMenu::handleRender() {
    // Display team logo
    g_Screen.drawLogo(18, 14, g_App.getGameSession().getLogo(), g_App.getGameSession().getLogoColour());

    // Draws separator line between title and statistics
    g_Screen.drawLine(20, 117, 20 + separatorSize_, 117, 252);
    g_Screen.drawLine(20, 116, 20 + separatorSize_, 116, 252);
}

void DebriefMenu::handleLeave() {
    // We're leaving to the map menu
    // So at this point the current mission is no more needed
    // as a new one will be selected in the map menu
    g_Session.setMission(NULL);

    g_System.hideCursor();
}
