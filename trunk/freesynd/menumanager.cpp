/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
#include "file.h"
#include "mainmenu.h"
#include "confmenu.h"
#include "mapmenu.h"
#include "briefmenu.h"
#include "selectmenu.h"
#include "researchmenu.h"
#include "loadsavemenu.h"
#include "loadingmenu.h"
#include "gameplaymenu.h"
#include "debriefmenu.h"
#include "misswinmenu.h"
#include "misslosemenu.h"
#include "logoutmenu.h"

MenuManager::MenuManager():current_(NULL),
menu_main_(NULL), menu_conf_(NULL), menu_load_save_(NULL),
menu_map_(NULL), menu_brief_(NULL), menu_select_(NULL),
menu_research_(NULL), menu_loading_(NULL), menu_gameplay_(NULL),
menu_debrief_(NULL), menu_miss_win_(NULL), menu_miss_lose_(NULL),
menu_logout_(NULL)
{
}

MenuManager::~MenuManager()
{
    if (menu_main_)
        delete menu_main_;
    if (menu_conf_)
        delete menu_conf_;
    if (menu_load_save_)
        delete menu_load_save_;
    if (menu_map_)
        delete menu_map_;
    if (menu_brief_)
        delete menu_brief_;
    if (menu_select_)
        delete menu_select_;
    if (menu_research_)
        delete menu_research_;
    if (menu_loading_)
        delete menu_loading_;
    if (menu_gameplay_)
        delete menu_gameplay_;
    if (menu_debrief_)
        delete menu_debrief_;
    if (menu_miss_win_)
        delete menu_miss_win_;
    if (menu_miss_lose_)
        delete menu_miss_lose_;
    if (menu_logout_)
        delete menu_logout_;
}

void MenuManager::changeCurrentMenu(const char *name)
{
    if (name == NULL)
        name = "main";
    if (menus_.find(name) == menus_.end()) {
        printf("Failed to find menu: %s\n", name);
        return;
    }
    Menu *m = menus_[name];
    bool currentWasSubMenu = false;
    if (current_) {
        currentWasSubMenu = current_->isSubMenu();
        current_->leave(!m->isSubMenu());
    }
    current_ = m;
    if (m)
        m->show(!currentWasSubMenu);
}

void MenuManager::keyEvent(Key key, KeyMod mod, bool pressed)
{
    if (current_)
        current_->keyEvent(key, mod, pressed);
}

void MenuManager::mouseMotionEvent(int x, int y, int state)
{
    if (current_)
        current_->mouseMotionEvent(x, y, state);
}

void MenuManager::mouseDownEvent(int x, int y, int button)
{
    if (current_)
        current_->mouseDownEvent(x, y, button);
}

void MenuManager::mouseUpEvent(int x, int y, int button)
{
    if (current_)
        current_->mouseUpEvent(x, y, button);
}

void MenuManager::createAllMenus()
{
    menu_main_ = new MainMenu(this);

    menu_conf_ = new ConfMenu(this);
    menu_load_save_ = new LoadSaveMenu(this);
    menu_map_ = new MapMenu(this);
    menu_brief_ = new BriefMenu(this, menu_map_);
    menu_select_ = new SelectMenu(this);
    menu_research_ = new ResearchMenu(this);
    menu_loading_ = new LoadingMenu(this, menu_brief_);
    menu_gameplay_ = new GameplayMenu(this, menu_loading_, menu_map_);
    menu_debrief_ = new DebriefMenu(this);
    menu_miss_win_ = new MissWinMenu(this);
    menu_miss_lose_ = new MissLoseMenu(this);
    menu_logout_ = new LogoutMenu(this);

    changeCurrentMenu("main");

    // still to go:  mendlose.dat, mendwin.dat
}
