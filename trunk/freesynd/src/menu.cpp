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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "app.h"
#include "gfx/fliplayer.h"
#include "gfx/screen.h"

Menu::Menu(MenuManager * menuManager, const char *menu_name,
           const char *showAnim,
           const char *leaveAnim):menu_manager_(menuManager),
name_(menu_name), showAnim_(showAnim), leaveAnim_(leaveAnim),
parent_menu_(NULL), background_(NULL)
{
    focusedWgId_ = -1;
    menuManager->addMenu(this);
}

Menu::Menu(MenuManager * menuManager, const char *menu_name,
           const char *parent):menu_manager_(menuManager),
name_(menu_name), showAnim_(""), leaveAnim_(""),
parent_menu_(parent), background_(NULL)
{
    focusedWgId_ = -1;
    menuManager->addMenu(this);
}

void Menu::redrawOptions()
{
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++) {
        Option & m = it->second;
        m.draw();
    }
    handleShowLate();
}

/*!
 * Adds a dirty rect the size of the screen.
 */
void Menu::needRendering() {
    menu_manager_->addRect(0, 0, g_Screen.gameScreenWidth(), g_Screen.gameScreenHeight());
}

/*!
 * Adds a dirty rect.
 * \param x
 * \param y
 * \param width
 * \param height
 */
void Menu::addDirtyRect(int x, int y, int width, int height) {
    menu_manager_->addRect(x, y, width, height);
}

void Menu::render()
{

    if (background_) {
        g_Screen.scale2x(clear_x_, clear_y_, clear_w_ / 2, clear_h_ / 2,
                         background_ + (clear_x_ / 2) +
                         (clear_y_ / 2) * 320, 320);
    }

    handleRender();

    for (std::list < MenuText >::iterator it = statics_.begin();
         it != statics_.end(); it++) {
        MenuText & m = *it;
        m.draw();
    }

    redrawOptions();
}

void Menu::leave() {
    if (focusedWgId_ != -1) {
        int i=0;
        for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++, i++) {

             if (focusedWgId_ == i) {
                Option & m = it->second;
                m.handleFocusLost();
                focusedWgId_ = -1;
                break;
             }
        }
    }
    handleLeave();
}

/*!
 * Creates and adds a label to the menu.
 * \param x X coordinate
 * \param y Y coordinate
 * \param text If text starts with a '#' then
 * text is a property in the current language file and it is
 * replaced by its value.
 * \param size Font size
 * \param dark True means text is not highlighted
 */
int Menu::addStatic(int x, int y, const char *text, FontManager::EFontSize size, bool dark)
{
    std::string lbl(text);
    // Find if string starts with '#' caracter
    if (lbl.find_first_of('#') == 0) {
        // Erase the # caracter
        lbl.erase(0, 1);
        // and looks for the message in the langage file
        menu_manager_->getMessage(lbl.c_str(), lbl);
    }
    MenuText m(x, y, lbl.c_str(), size, dark, true);
    statics_.push_back(m);
    return statics_.size() - 1;
}

int Menu::addStatic(int x, int y, int width, const char *text, FontManager::EFontSize size, bool dark) {
    MenuText m(x, y, width, text, size, dark, true);
    statics_.push_back(m);
    return statics_.size() - 1;
}

MenuText * Menu::getStatic(int staticId) {
    int i = 0;
    for (std::list < MenuText >::iterator it = statics_.begin();
         it != statics_.end(); it++) {
        MenuText & m = *it;
        
        if (i == staticId) {
            return &m;
        }

        i++;
    }

    return NULL;
}

/*!
 * Creates and adds a button to the menu.
 * \param x X coordinate
 * \param y Y coordinate
 * \param text Button label. If text starts with a '#' then
 * text is a property in the current language file and it is
 * replaced by its value.
 * \param size Font size
 * \param key Acceleration key
 * \param to Name of the next menu when button is clicked
 * \param visible True if button is visible on screen
 * \param dark_widget Widget drawn in front of the button when it's not highlighted
 * \param light_widget Widget drawn in front of the button when it's highlighted
 */
void Menu::addOption(int x, int y, int width, int height, const char *text, FontManager::EFontSize size, Key key,
            const char *to, bool visible, bool centered, int dark_widget, int light_widget)
{
    std::string lbl(text);
    // Find if string starts with '#' caracter
    if (lbl.find_first_of('#') == 0) {
        // Erase the # caracter
        lbl.erase(0, 1);
        // and looks for the message in the langage file
        menu_manager_->getMessage(lbl.c_str(), lbl);
    }
    Option m(x, y, width, height, lbl.c_str(), size, to, visible, centered, dark_widget, light_widget);
    options_[key] = m;
}

/*!
 * Handles the key pressed event.
 * \param key The key that was pressed
 * \param modKeys State of all modifier keys
 */
void Menu::keyEvent(Key key, const int modKeys)
{

    // Pressing Escape changes the current menu to its parent(like a back)
    if (key == KEY_ESCAPE) {
        menu_manager_->changeCurrentMenu(parent_menu_);
        return;
    }

    // Transform option map into a vector
    std::vector < Option * >options_vec;
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++)
        options_vec.push_back(&it->second);

/*    if (options_vec.size()) {
        unsigned int curOpt = 0;
        for (unsigned int i = 0; i < options_vec.size(); i++)
            if (!options_vec[i]->isDark()) {
                curOpt = i;
                break;
            }
        if (key == VK_UP || key == KEY_UP) {
            options_vec[curOpt]->setDark(true);
            if (curOpt == 0)
                curOpt = options_vec.size() - 1;
            else
                curOpt--;
            options_vec[curOpt]->setDark(false);
            needRendering();
        }
        if (key == VK_DOWN || key == KEY_DOWN) {
            if (!options_vec[curOpt]->isDark()) {
                options_vec[curOpt]->setDark(true);
                curOpt++;
                if (curOpt >= options_vec.size())
                    curOpt = 0;
            }
            options_vec[curOpt]->setDark(false);
            needRendering();
        }
        if (key == VK_FB || key == KEY_RETURN || key == KEY_KP_ENTER) {
            for (std::map < Key, Option >::iterator it =
                 options_.begin(); it != options_.end(); it++)
                if (&it->second == options_vec[curOpt])
                    key = it->first;
        }
    }
*/
    if (options_.find(key) == options_.end()) {
        handleUnknownKey(key, modKeys);
        return;
    }
    handleOption(key, modKeys);
    if (options_[key].to_)
        menu_manager_->changeCurrentMenu(options_[key].to_);
}

/*!
 * Handles the mouse motion event.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param state If button is pressed during mouse motion.
 * \param modKeys State of all modifier keys
 */
void Menu::mouseMotionEvent(int x, int y, int state, const int modKeys)
{
    handleMouseMotion(x, y, state, modKeys);

    // Check focus is lost for currently focused widget
    if (focusedWgId_ != -1) {
        int i=0;
        for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++, i++) {

             if (focusedWgId_ == i) {
                Option & m = it->second;
                if (!m.isMouseOver(x, y)) {
                    m.handleFocusLost();
                    focusedWgId_ = -1;
                }
                break;
             }
        }
    }

    // See if the mouse is hovering a button
    int i=0;
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++, i++) {
        Option & m = it->second;

        if (!m.isVisible()) {
            // Button is not visible so it doesn't count
            continue;
        }

        // Mouse is over a widget
        if (m.isMouseOver(x, y)) {
            if (i != focusedWgId_) {
                // Widget has now the focus : handle the event
                m.handleFocusGained();
                focusedWgId_ = i;
            }

            // Pass the event to the widget
            m.handleMouseMotion(x, y, state, modKeys);

            return;
        }
    }
}

/*!
 * Handles the mouse down event.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was pressed
 * \param modKeys State of all modifier keys
 */
void Menu::mouseDownEvent(int x, int y, int button, const int modKeys)
{
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++) {
        Option & m = it->second;

        if (!m.isVisible()) {
            // Button is not visible so it doesn't count
            continue;
        }

        if (m.isMouseOver(x, y)) {
            keyEvent(it->first, modKeys);
            return;
        }
    }
    handleMouseDown(x, y, button, modKeys);
}

/*!
 * Handles the mouse up event.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was released
 * \param modKeys State of all modifier keys
 */
void Menu::mouseUpEvent(int x, int y, int button, const int modKeys)
{
    handleMouseUp(x, y, button, modKeys);
}
