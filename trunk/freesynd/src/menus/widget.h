/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
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

#ifndef WIDGET_H
#define WIDGET_H

#include <string>

#include "gfx/fontmanager.h"

/*!
 * This is a graphical component. It's the base class
 * for all widgets (text, button, ...).
 * A widget has a size and location and can be visible or not.
 */
class Widget {
public:

    /*!
     * Default constructor.
     */
    Widget() : x_(0), y_(0), width_(0), height_(0), visible_(true) {}

    /*!
     * Constructs a widget with given size and location.
     */
    Widget(int x, int y, int width, int height, bool visible):
            x_(x), y_(y), width_(width), height_(height), visible_(visible) {}

    /*!
     * Default constructor.
     */
    virtual ~Widget() {}

    //! Draw the widget on screen. All subclass must implement this method.
    virtual void draw() = 0;

    void setLocation(int x, int y);
    int getX() { return x_; }
    int getY() { return y_; }

    void setWidth(int width);
    int getWidth() { return width_; }

    void setHeight(int height);
    int getHeight() { return height_; }

    void setVisible(bool visible);
    bool isVisible() { return visible_; }

protected:
    void redraw();

protected:
    /*! lower left coordinates of the widget.*/
    int x_, y_;
    /*! Width of the widget. */
    int width_;
    /*! Height of the widget. */
    int height_;
    /*! True if the widget is displayed on screen. */
    bool visible_;

};

//! A text widget.
/*!
 * This class represents a static text label : it displays a text
 * at a given location.
 * The text is displayed in dark or light green.
 */
class MenuText : public Widget {
public:
    MenuText(int x, int y, const char *text, FontManager::EFontSize size, bool dark,
            bool visible): Widget(x, y, 0, 0, visible), 
            text_(text), dark_(dark), size_(size) {}

    virtual ~MenuText() {}

    //! Draw the widget on screen
    void draw();

    void setText(const char * text);
    void setTextFormated(const char * format, ...);
    std::string getText() { return text_; }

    void setDark(bool dark);
    bool isDark() { return dark_; }

protected:
    /*! The text to be displayed.*/
    std::string text_;
    bool dark_;
    /*! Size of text font. */
    FontManager::EFontSize size_;
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
    Option() : MenuText(0, 0, "", FontManager::SIZE_1, true, true) {
        to_ = NULL;
        dark_widget_id_ = 0;
        light_widget_id_ = 0;
    }

    //! Constructs a new button.
    Option(int x, int y, const char *text, FontManager::EFontSize size,
            const char *to, bool visible, int dark_widget = 0, int light_widget = 0) : MenuText(x, y, text, size, true, visible) {
        to_ = to;
        dark_widget_id_ = dark_widget;
        light_widget_id_ = light_widget;
    }

    ~Option() { to_ = NULL; }

    //! Draw the widget on screen
    void draw();

    //! Tells whether the pointer is over the button or not
    bool isMouseOver(int x, int y);
};

#endif // WIDGET_H