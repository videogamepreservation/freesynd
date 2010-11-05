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
#include <string.h>
#include <assert.h>

#include "app.h"
#include "fliplayer.h"
#include "screen.h"

/*!
 * Draw the widget at the current position and only if it's
 * visible.
 * Actually, only a text is drawn (see Font). The borders are
 * already drawn on the background image.
 */
void MenuText::draw() {
    if (visible_)
        g_App.fonts().drawText(x_, y_, text_.c_str(), size_, dark_);
}

/*!
 * Draw the widget at the current position and only if it's
 * visible.
 * Actually, only a text is drawn (see Font). The borders are
 * already drawn on the background image.
 */
void Option::draw() {
    if (visible_) {
        int x = x_;
        if (dark_ && dark_widget_id_ != 0) {
            Sprite *widget = g_App.menuSprites().sprite(dark_widget_id_);
            widget->draw(x, y_ - 4, 0, false, true);
            x += widget->width() * 2 + 8;
        } else if (light_widget_id_ != 0) {
            Sprite *widget = g_App.menuSprites().sprite(light_widget_id_);
            widget->draw(x, y_ - 4, 0, false, true);
            x += widget->width() * 2 + 8;
        }

        g_App.fonts().drawText(x, y_, text_.c_str(), size_, dark_);
    }
}

bool Option::isMouseOver(int x, int y) {
    int width = 0;
    int height = 0;
    
    if (dark_widget_id_ != 0) {
        // We consider that the dark and light widget have the same size
        Sprite *widget = g_App.menuSprites().sprite(dark_widget_id_);
        width = widget->width() * 2;
        height = widget->height() * 2;

        if (text_.size() != 0) {
            // This is the space between the widget and the text
            width += 8 + g_App.fonts().textWidth(text_.c_str(), size_);
        }
    } else {
        width = g_App.fonts().textWidth(text_.c_str(), size_);
        height = g_App.fonts().textHeight(size_);
    }

    return (x > x_ - 2 && 
            x < x_ + width && 
            y >= y_ && 
            y < y_ + height - 2);
}

Menu::Menu(MenuManager * menuManager, const char *menu_name,
           const char *showAnim,
           const char *leaveAnim):menu_manager_(menuManager),
name_(menu_name), showAnim_(showAnim), leaveAnim_(leaveAnim),
parent_menu_(NULL), background_(NULL)
{
    menuManager->addMenu(this);
    needRendering();
}

Menu::Menu(MenuManager * menuManager, const char *menu_name,
           const char *parent):menu_manager_(menuManager),
name_(menu_name), showAnim_(""), leaveAnim_(""),
parent_menu_(parent), background_(NULL)
{
    menuManager->addMenu(this);
    needRendering();
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
    
    need_rendering_ = false;
}

/*!
 * 
 */
int Menu::addStatic(int x, int y, const char *text, int size, bool dark)
{
    MenuText m(x, y, text, size, dark, true);
    statics_.push_back(m);
    return statics_.size() - 1;
}

void Menu::setStaticText(int static_id, const char *text){
    int i = 0;
    for (std::list < MenuText >::iterator it = statics_.begin();
         it != statics_.end(); it++) {
        MenuText & m = *it;
        
        if (i == static_id) {
            m.text_ = text;
            needRendering();
            return;
        }

        i++;
    }
}

void Menu::addOption(int x, int y, const char *text, int size, Key key,
                     const char *to, bool visible, int dark_widget, int light_widget)
{
    Option m(x, y, text, size, to, visible, dark_widget, light_widget);
    options_[key] = m;
}

/*!
 * Handles the key pressed event.
 * \param key The key that was pressed
 * \param modKeys State of all modifier keys
 */
void Menu::keyEvent(Key key, const int modKeys)
{

    if (key == KEY_ESCAPE) {
        menu_manager_->changeCurrentMenu(parent_menu_);
        return;
    }

    // Please comment
    std::vector < MenuText * >options_vec;
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++)
        options_vec.push_back(&it->second);

    if (options_vec.size()) {
        unsigned int curOpt = 0;
        for (unsigned int i = 0; i < options_vec.size(); i++)
            if (!options_vec[i]->dark_) {
                curOpt = i;
                break;
            }
        if (key == VK_UP || key == KEY_UP) {
            options_vec[curOpt]->dark_ = true;
            if (curOpt == 0)
                curOpt = options_vec.size() - 1;
            else
                curOpt--;
            options_vec[curOpt]->dark_ = false;
            render();
        }
        if (key == VK_DOWN || key == KEY_DOWN) {
            if (!options_vec[curOpt]->dark_) {
                options_vec[curOpt]->dark_ = true;
                curOpt++;
                if (curOpt >= options_vec.size())
                    curOpt = 0;
            }
            options_vec[curOpt]->dark_ = false;
            render();
        }
        if (key == VK_FB || key == KEY_RETURN || key == KEY_KP_ENTER) {
            for (std::map < Key, Option >::iterator it =
                 options_.begin(); it != options_.end(); it++)
                if (&it->second == options_vec[curOpt])
                    key = it->first;
        }
    }

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

    // See if the mouse is hovering a button
    for (std::map < Key, Option >::iterator it = options_.begin();
         it != options_.end(); it++) {
        Option & m = it->second;

        if (!m.visible_) {
            // Button is not visible so it doesn't count
            continue;
        }

        if (m.isMouseOver(x, y)) {
            if ( m.dark_ ) {
                // The button was dark but is now highlighted
                // since the mouse is over it
                m.dark_ = false;
                needRendering();
            }
        } else {
            if (!m.dark_) {
                // The button was highlighted but not anymore : refresh
                needRendering();
            }
            m.dark_ = true;
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

        if (!m.visible_) {
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
