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
#include <list>

#include "gfx/fontmanager.h"

/*!
 * This is a graphical component. It's the base class
 * for all widgets (text, button, ...).
 * A widget has a size and location and can be visible or not.
 * Each widget has a unique id.
 */
class Widget {
public:

    /*!
     * Default constructor.
     */
    Widget() : 
      id_(++widgetCnt), x_(0), y_(0), width_(0), height_(0), visible_(true) {}

    /*!
     * Constructs a widget with given size and location.
     * \param x X coordinate
     * \param y Y coordinate
     * \param widht Widget width
     * \param height Widget height
     * \param visible True means the widget is drawn
     */
    Widget(int x, int y, int width, int height, bool visible):
            id_(++widgetCnt), x_(x), y_(y), width_(width), 
            height_(height), visible_(visible) {}

    /*!
     * Default constructor.
     */
    virtual ~Widget() {}

    //! Draw the widget on screen. All subclass must implement this method.
    virtual void draw() = 0;

    /*!
     * Returns the widget id.
     */
    int getId() { return id_; }

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

private:
    /*! A counter to have unique widget IDs.*/
    static int widgetCnt;

    /*! A unique ID identifying the widget.*/
    int id_;
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

class Menu;
class Sprite;

/*!
 * A widget that responds to user interactions.
 */
class ActionWidget : public Widget {
public:
    ActionWidget() : Widget() {}
    ActionWidget(int x, int y, int width, int height, bool visible) : Widget(x, y, width, height, visible) {}

    //! Tells whether the pointer is over the widget or not
    bool isMouseOver(int x, int y);

    //! Callback method to respond to mouse motion event
    virtual void handleMouseMotion(int x, int y, int state, const int modKeys) {}
    //! Callback method to respond to mouse down event
    virtual void handleMouseDown(int x, int y, int button, const int modKeys) {}

    //! Callback method called when the mouse is over the widget
    virtual void handleFocusGained() {}
    //! Callback method called when the mouse leaves the widget
    virtual void handleFocusLost() {}
};

//! A button widget.
/*!
 * This class represents a button. A button can lead to another screen 
 * which name is stored in the field "to".
 * It can have a widget in front of its text (like an arrow or a bullet).
 */
class Option : public ActionWidget {
public:

    /*! The name of the next menu.*/
    const char *to_;

    //! Constructs a new button.
    Option() : ActionWidget(), text_(0, 0, 0, "", FontManager::SIZE_1, true, true) {
        to_ = NULL;
        darkWidget_ = NULL;
        lightWidget_ = NULL;
    }

    //! Constructs a new button.
    Option(Menu *peer, int x, int y, int width, int height, const char *text, FontManager::EFontSize size,
            const char *to, bool visible, bool centered = true, int dark_widget = 0, int light_widget = 0);

    ~Option();

    //! Draw the widget on screen
    void draw();

    void handleMouseDown(int x, int y, int button, const int modKeys);

    void handleFocusGained();
    void handleFocusLost();

    //! Runs the action for this widget
    virtual void executeAction(const int modKeys);

protected:
    Menu *peer_;

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

class ToggleAction;

/*!
 * A group is used to track the 
 * mutual exclusion between toggle actions.
 * Each time a button is selected, the others
 * must be unselected.
 */
class Group {
public:
    Group() {}
    ~Group();

    //! Adds a button to the group
    void addButton(ToggleAction *action);
    //! A toggle button has been selected
    void selectButton(int id);

private:
    /*! List of buttons that belongs to the group.*/
    std::list<ToggleAction *> actions_;
};

/*!
 * ToggleAction are buttons that stay pushed when clicked
 * but gets deselected when another button from the same group
 * gets selected.
 */
class ToggleAction : public Option {
public:
    //! Constructs a new button.
    ToggleAction(Menu *peer, int x, int y, int width, int height, 
                    const char *text, FontManager::EFontSize size, bool selected, Group *pGroup);

    void executeAction(const int modKeys);

    void handleFocusLost();

    void handleSelectionLost();
protected:
    //! True to select the button
    void setSelected(bool isSelected);
protected:
    /*! State of the button. True means the button is on.*/
    bool selected_;
    /*! Group of the button.*/
    Group *group_;
};

/*!
 * A list box is a widget that displays a list of lines. Each line corresponds to
 * a game object (team members, equips, mods, ...). A list box reacts to user actions
 * and calls Menu::handleAction() if one of its items has been pressed.
 * A list box can have a title, displayed at the top of the list and can print a default
 * label for all empty lines.
 */
class ListBox : public ActionWidget {
public:
    //! Constructs a new list box.
    ListBox(Menu *peer, int x, int y, int width, int height, int maxLine, 
                bool visible = true, const char *title = NULL, bool displayEmpty = false);

    ~ListBox();

    //! Draw the widget on screen
    void draw();
    //! Callback method to respond to mouse motion event
    void handleMouseMotion(int x, int y, int state, const int modKeys);
    //! Callback method to respond to mouse down event
    void handleMouseDown(int x, int y, int button, const int modKeys);
    void handleFocusLost();

    //! Returns the number of lines in the list box.
    int getMaxLine() { return maxLine_; }

    //! Adds a new line at the first emmty place
    void add(std::string label, int itemId);
    //! Adds a new line at the given emmty place
    void addAt(std::string label, int itemId, int i);
    //! Modifies the given line label 
    void setLabel(std::string label, int i);

    //! Returns the item id associated to the given line
    int getItemIdAt(int index);

    //! Returns the first line with the given associated id
    int getIndexWithItemId(int itemId);
    //! Returns true if there is a non empty line at given index
    bool existsAt(int index);
    //! Removes line contents
    void remove(int index);

protected:
    struct ListEntry {
        std::string label_;
        int itemId_;

        ListEntry() {
            itemId_ = -1;
        }
    };

    Menu *peer_;

    /*! Title of the list box. Optional.*/
    MenuText *pTitle_;
    /*! The label for an empty line.*/
    std::string emptyLbl_;
    /*! True will display a label when entry is null.*/
    bool displayEmpty_;

    std::vector<ListEntry *> entries_;
    /*! X coord for title underline.*/
    int xUnderline_;
    /*! Y coord for title underline.*/
    int yUnderline_;
    /*! Length of title underline.*/
    int lUnderline_;
    /*! Starting coordinate of content list.*/
    int yOrigin_;
    /*! Maximum number of line in the list box.*/
    int maxLine_;
    /*! The line that the mouse is on. -1 if no line is hovered.*/
    int focusedLine_;
};

#endif // WIDGET_H