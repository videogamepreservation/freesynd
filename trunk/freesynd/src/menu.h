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

//! A text widget.
/*!
 * This class represents a text label.
 * This is the base class for widget : it displays a text
 * at a given location.
 * A widget can be visible or not, and the text is displayed
 * in dark or light green.
 */
class MenuText {
public:
    int x_, y_;
    /*! The text to be displayed.*/
    std::string text_;
    int size_;
    bool dark_;
    bool visible_;

    MenuText() : x_(0), y_(0), text_(""), size_(0), dark_(false),
    visible_(true) {}

    MenuText(int x, int y, const char *text, int size, bool dark,
            bool visible):x_(x), y_(y), text_(text),
        size_(size), dark_(dark), visible_(visible) {
    }

    virtual ~MenuText() {}

    //! Draw the widget on screen
    virtual void draw();
};
//! A button widget.
/*!
 * This class extends the MenuText class to represent a button on
 * the screen. A button can lead to another screen which name is
 * stored in the field "to".
 * It can have a widget in front of its text (like an arrow or a bullet).
 */
class Option : public MenuText {
public:
    /*! The name of the next menu.*/
    const char *to_;
    /*! 
     * The widget to display when button is dark.
     * When id is zero, there is no widget.
    */
    int dark_widget_id_;
    /*! 
     * The widget to display when button is highlighted.
     * When id is zero, there is no widget.
     */
    int light_widget_id_;

    //! Constructs a new button.
    Option() : MenuText() {
        to_ = NULL;
        dark_widget_id_ = 0;
        light_widget_id_ = 0;
    }

    //! Constructs a new button.
    Option(int x, int y, const char *text, int size,
            const char *to, bool visible, int dark_widget = 0, int light_widget = 0) : MenuText(x, y, text, size, true, visible) {
        to_ = to;
        dark_widget_id_ = dark_widget;
        light_widget_id_ = light_widget;
    }

    //! Draw the widget on screen
    void draw();

    //! Tells whether the pointer is over the button or not
    bool isMouseOver(int x, int y);

    ~Option() { to_ = NULL; }
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

    /*! Returns true if an animation has been defined when opening the menu.*/
    bool hasShowAnim() { return showAnim_.size() != 0; }
    /*! Returns the opening animation file name. */
    const char * getShowAnimName() { return showAnim_.c_str(); }
    /*! Returns true if an animation has been defined when leaving the menu.*/
    bool hasLeaveAnim() { return leaveAnim_.size() != 0; }
    /*! Returns the leaving animation file name. */
    const char * getLeaveAnimName() { return leaveAnim_.c_str(); }
    
    void render();
    //! Creates a new text label and returns its id
    int addStatic(int x, int y, const char *text, int size, bool dark);
    //! Creates a new button and returns its id
    void addOption(int x, int y, const char *text, int size, Key key,
            const char *to = NULL, bool visible = true, int dark_widget = 0, int light_widget = 0);

    void setParentMenu(const char *m) { parent_menu_ = m; }

    //! Handles key pressed
    void keyEvent(Key key, const int modKeys);
    //! Handles mouse moved
    void mouseMotionEvent(int x, int y, int state, const int modKeys);
    //! Handles mouse button pressed
    void mouseDownEvent(int x, int y, int button, const int modKeys);
    //! Handles mouse button released
    void mouseUpEvent(int x, int y, int button, const int modKeys);

    void setStaticText(int static_id, const char *text);

    //! Hides the given button
    /*!
     * Search for the option mapped to the given key
     * and disables its visible property.
     * \param key The key associated to the option.
     */
    void hideOption(Key key) {
        if (options_.find(key) == options_.end())
            return;

        if (options_[key].visible_) {
            options_[key].visible_ = false;
            needRendering();
        }
    }

    //! Shows the given button
    /*!
     * Search for the option mapped to the given key
     * and enables its visible property.
     * \param key The key associated to the option.
     */
    void showOption(Key key) {
        if (options_.find(key) == options_.end())
            return;

        if (!options_[key].visible_) {
            options_[key].visible_ = true;
            needRendering();
        }
    }

    virtual void handleTick(int elapsed) {}
    virtual void handleUnknownKey(Key key, const int modKeys) {}
    virtual void handleMouseDown(int x, int y, int button, const int modKeys) {}
    virtual void handleMouseUp(int x, int y, int button, const int modKeys) {}
    virtual void handleMouseMotion(int x, int y, int state, const int modKeys) {}

    //! Callback function : Childs can reimplement
    /*! 
     * Called just after the opening animation is played (if one has
     * been defined) and before the menu is rendered for the first time.
     */
    virtual void handleShow() {}

    //! Callback function : Childs can reimplement
    /*! 
     * Called each time a menu is rendered.
     */
    virtual void handleRender() {}

    //! Callback function : Childs can reimplement
    /*! 
     * Called just before the closing animation is played (if one has
     * been defined) and the menu closed.
     */
    virtual void handleLeave() {}
    virtual void handleShowLate() {}

    //! Callback function : Childs can reimplement
    /*! 
     * Called when an option has been activated.
     * \param key The key that was pressed.
     * \param modKeys The state of all modifiers buttons
     */
    virtual void handleOption(Key key, const int modKeys) {}

protected:
    MenuManager *menu_manager_;
    std::string name_;
    std::string showAnim_, leaveAnim_;
    std::list<MenuText> statics_;
    std::map<Key, Option> options_;
    const char *parent_menu_;
    uint8 *background_;
    int clear_x_, clear_y_, clear_w_, clear_h_;

    void redrawOptions();
    void needRendering();
    void addDirtyRect(int x, int y, int width, int height);
};

#endif
