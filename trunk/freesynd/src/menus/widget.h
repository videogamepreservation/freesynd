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

    virtual void setLocation(int x, int y);
    int getX() { return x_; }
    int getY() { return y_; }

    int getWidth() { return width_; }
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
 * Text label can be left aligned or centered.
 */
class MenuText : public Widget {
public:
    /*!
     * Use this constructor to left align text. Widget width will depend on text and font.
     */
    MenuText(int x, int y, const char *text, FontManager::EFontSize size, bool dark,
            bool visible = true);

    /*!
     * Use this constructor to specify text alignment. Widget width will be fixed but text position
     * will depend of font size and text length.
     */
    MenuText(int x, int y, int width, const char *text, FontManager::EFontSize size, bool dark,
            bool visible = true, bool centered = true);

    virtual ~MenuText() {}

    //! Draw the widget on screen
    void draw();

    void setLocation(int x, int y);

    void setText(const char * text);
    void setTextFormated(const char * format, ...);
    std::string getText() { return text_; }

    void setDark(bool dark);
    bool isDark() { return dark_; }

protected:
    /*!
     * Utility method to update text and compute associated data.
     */
    void updateText(const char *text);

protected:
    /*! The text to be displayed.*/
    std::string text_;
    bool dark_;
    /*! True means the text is centered, false the text is anchored
     * to the left.
     */
    bool centered_;
    /*! Real location of text.*/
    int anchorX_;
    /*! Real location of text.*/
    int anchorY_;
    /*! Size of text font. */
    FontManager::EFontSize size_;
};

class Sprite;

//! A button widget.
/*!
 * This class represents a button. A button can lead to another screen 
 * which name is stored in the field "to".
 * It can have a widget in front of its text (like an arrow or a bullet).
 */
class Option : public Widget {
public:

    /*! The name of the next menu.*/
    const char *to_;

    //! Constructs a new button.
    Option() : Widget(), text_(0, 0, "", FontManager::SIZE_1, true, true) {
        to_ = NULL;
        darkWidget_ = NULL;
        lightWidget_ = NULL;
    }

    //! Constructs a new button.
    Option(int x, int y, int width, int height, const char *text, FontManager::EFontSize size,
            const char *to, bool visible, bool centered = true, int dark_widget = 0, int light_widget = 0);

    ~Option() { to_ = NULL; }

    //! Draw the widget on screen
    void draw();

    //! Tells whether the pointer is over the button or not
    bool isMouseOver(int x, int y);

    virtual void handleMouseMotion(int x, int y, int state, const int modKeys) {}

    virtual void handleFocusGained();
    virtual void handleFocusLost();

protected:
    MenuText text_;
    /*! 
     * The widget to display when button is dark.
     * When id is zero, there is no widget.
    */
    Sprite *darkWidget_;
    /*! 
     * The widget to display when button is highlighted.
     * When id is zero, there is no widget.
     */
    Sprite *lightWidget_;
};

#endif // WIDGET_H