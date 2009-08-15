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

#ifndef MENU_H
#define MENU_H

#include <string>
#include <list>
#include <map>
#include "keys.h"
#include "common.h"

class FontManager;
class Menu;
class MenuManager;

/*!
 * Menu text class.
 */
class MenuText {
public:
    int x_, y_;
    std::string text_;
    int size_;
    bool dark_;
    const char *to_;
    bool visible_;

    MenuText() : x_(0), y_(0), text_(""), size_(0), dark_(false), to_(NULL),
    visible_(true) {}

    MenuText(int x, int y, const char *text, int size, bool dark,
            const char *to, bool visible):x_(x), y_(y), text_(text),
        size_(size), dark_(dark), to_(to), visible_(visible) {
    }
};

/*!
 * Menu class.
 */
class Menu {
public:
    Menu(MenuManager *menuManager, const char *name, const char *showAnim,
            const char *leaveAnim);
    Menu(MenuManager *menuManager, const char *name, const char *parent);
    virtual ~Menu() {}

    const char *name() { return name_.c_str(); }

    void setClearArea(uint8 *background, int x, int y, int w, int h) {
        background_ = background;
        clear_x_ = x;
        clear_y_ = y;
        clear_w_ = w;
        clear_h_ = h;
    }

    virtual bool isSubMenu() {
        return leaveAnim_.size() == 0;
    }

    void show(bool playAnim = true);
    void leave(bool playAnim = true);
    void addStatic(int x, int y, const char *text, int size, bool dark);
    void addOption(int x, int y, const char *text, int size, Key key,
            const char *to = NULL, bool visible = true);

    void setParentMenu(const char *m) { parent_menu_ = m; }

    void keyEvent(Key key, KeyMod mod, bool pressed);
    void mouseMotionEvent(int x, int y, int state);
    void mouseDownEvent(int x, int y, int button);
    void mouseUpEvent(int x, int y, int button);

    void hideOption(Key key) {
        if (options_.find(key) == options_.end())
            return;

        options_[key].visible_ = false;
        redrawOptions();
    }

    void showOption(Key key) {
        if (options_.find(key) == options_.end())
            return;

        options_[key].visible_ = true;
        redrawOptions();
    }

    virtual void handleTick(int elapsed) {}
    virtual void handleUnknownKey(Key key, KeyMod mod, bool pressed) {}
    virtual void handleMouseDown(int x, int y, int button) {}
    virtual void handleMouseUp(int x, int y, int button) {}
    virtual void handleMouseMotion(int x, int y, int state) {}
    virtual void handleShow() {}
    virtual void handleLeave() {}
    virtual void handleShowLate() {}
    virtual void handleOption(Key key) {}

protected:
    MenuManager *menu_manager_;
    std::string name_;
    std::string showAnim_, leaveAnim_;
    std::list<MenuText> statics_;
    std::map<Key, MenuText> options_;
    bool hovering_;
    const char *parent_menu_;
    uint8 *background_;
    int clear_x_, clear_y_, clear_w_, clear_h_;

    void redrawOptions();
};

#endif
