/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef CONFMENU_H
#define CONFMENU_H

/*!
 * Configuration Menu class.
 */
class ConfMenu : public Menu {
public:
    ConfMenu(MenuManager *m);
    ~ConfMenu();

    virtual void handleRender(DirtyList &dirtyList);
\tvoid handleShow();
    void handleLeave();

    void handleAction(const int actionId, void *ctx, const int modKeys);

protected:

\tbool handleUnknownKey(Key key, const int modKeys);

\tvoid createPanels();
\tvoid showLogoPanel();
\tvoid showUserNamePanel();
\tvoid showCompanyNamePanel();
\tvoid showMainPanel();
\tvoid hideMainPanel();

\tvoid redrawLogo() { addDirtyRect(28, 22, 150, 110); }
\tvoid redrawPanel() { addDirtyRect(278, 20, 340 , 135); }

protected:
\t/*! 
\t * This enum identifies the panel which is currently displayed.
\t */
\tenum EPanel {
        PNL_MAIN = 0,
        PNL_LOGO = 1,
        PNL_USRNM = 2,
\t\tPNL_CMPNM = 3
    };

\t/*! This array holds the values for drawing a frame
\t *  around the ok and cancel button in the sub panels. */
\tuint8 butFrameData_[68 * 13];
\t/*! This array holds the values for drawing a frame
\t *  around the textfield in the user and company name panels. */
\tuint8 tfFrameData_[136 * 13];

\t/*! keep track of the current panel.*/
\tEPanel currPanel_;
\t/*! Button to open the change logo panel.*/
\tint logoButId_;
\t/*! Button to open the change company name panel.*/
\tint compNameButId_;
\t/*! Button to open the change user name panel.*/
\tint userNameButId_;
\t/*! Button in the main window to accept change.*/
\tint acceptButId_;
\t/*! Button in the main window to cancel change.*/
\tint menuButId_;
\tint panelMsgId_;

\t// Logo Panel
\tint leftColButId_;
\tint rightColButId_;
\tint leftLogoButId_;
\tint rightLogoButId_;
\tint logoStaticId_;
\tint colStaticId_;

\t// Change names panel
\tTextField *pUserNameTF_;
\tTextField *pCompNameTF_;

\t// Common buttons
\tint okButId_;
\tint cancelButId_;

\t/*! Id of the menu text that stores the username that will be accepted.*/
\tint toAcceptUsrNameTxtId_;
\t/*! Id of the menu text that stores the company name that will be accepted.*/
\tint toAcceptCmpNameTxtId_;
\tint toAcceptLogo_;
\tint toAcceptColourId_;
\tint tempLogo_;
\tint tempColourId_;
};

#endif
