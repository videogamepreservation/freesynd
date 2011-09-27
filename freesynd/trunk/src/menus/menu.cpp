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
	pCaptureInput_ = NULL;
    menuManager->addMenu(this);
}

Menu::Menu(MenuManager * menuManager, const char *menu_name,
           const char *parent):menu_manager_(menuManager),
name_(menu_name), showAnim_(""), leaveAnim_(""),
parent_menu_(parent), background_(NULL)
{
    focusedWgId_ = -1;
	pCaptureInput_ = NULL;
    menuManager->addMenu(this);
}

Menu::~Menu() {
    for (std::list < ActionWidget * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {
        delete (*it);
    }

    actions_.clear();
}

void Menu::redrawOptions()
{
    for (std::list < ActionWidget * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {
            if ( (*it)->isVisible()) {
                (*it)->draw();
            }
    }
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
        if ( m.isVisible()) {
            m.draw();
        }
    }

    redrawOptions();
}

/*!
 * This method does some common actions before given handle to the 
 * current menu instance via handleLeave().
 */
void Menu::leave() {
    // Reset focus if an action widget had one.
    if (focusedWgId_ != -1) {
        ActionWidget *pAction = getActionWidget(focusedWgId_);

        pAction->handleFocusLost();
        focusedWgId_ = -1;
    }

	// Reset capture
	if (pCaptureInput_ != NULL) {
		pCaptureInput_->handleCaptureLost();
		pCaptureInput_ = NULL;
	}

    // Give control to menu instance
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
 * \returns The newly created widget id.
 */
int Menu::addStatic(int x, int y, const char *text, FontManager::EFontSize size, bool dark) {

    MenuText m(x, y, text, size, dark, true);
    statics_.push_back(m);
    return m.getId();
}

/*!
 * Creates and adds a label with a fixed width to the menu.
 * Height will be equals to text height in the given font.
 * \param x X coordinate
 * \param y Y coordinate
 * \param width Width of the widget
 * \param text If text starts with a '#' then
 * text is a property in the current language file and it is
 * replaced by its value.
 * \param size Font size
 * \param dark True means text is not highlighted
 * \returns The newly created widget id.
 */
int Menu::addStatic(int x, int y, int width, const char *text, FontManager::EFontSize size, bool dark) {
    MenuText m(x, y, width, text, size, dark, true);
    statics_.push_back(m);
    return m.getId();
}

/*!
 * Returns the MenuText widget with the given id.
 * \return NULL if no widget is found
 */
MenuText * Menu::getStatic(int staticId) {
    for (std::list < MenuText >::iterator it = statics_.begin();
         it != statics_.end(); it++) {
        MenuText & m = *it;
        
        if (m.getId() == staticId) {
            return &m;
        }
    }

    return NULL;
}

/*! 
 * Creates a new button that has no text but an image.
 * Widget's size will be the same as the image used. Dark image
 * and light image should be the same size.
 * \param x X coordinate
 * \param y Y coordinate
 * \param key Acceleration key
 * \param dark_widget Widget drawn in front of the button when it's not highlighted
 * \param light_widget Widget drawn in front of the button when it's highlighted
 * \param visible True if button is visible on screen
 * \returns The newly created widget id.
 */
int Menu::addImageOption(int x, int y, Key key, int dark_widget, int light_widget, bool visible) {

    Sprite *spr = g_App.menuSprites().sprite(dark_widget);
   
    Option *m = new Option(this, x, y, spr->width() * 2, spr->height() * 2, "", 
                FontManager::SIZE_1, NULL, visible, true, dark_widget, light_widget);
    hotKeys_[key] = m;
    actions_.push_back(m);

    return m->getId();
}

/*!
 * Creates and adds a button to the menu.
 * \param x X coordinate
 * \param y Y coordinate
 * \param width Button width
 * \param height Button height
 * \param text Button label. If text starts with a '#' then
 * text is a property in the current language file and it is
 * replaced by its value.
 * \param size Font size
 * \param key Acceleration key
 * \param to Name of the next menu when button is clicked
 * \param visible True if button is visible on screen
 * \param centered True if text must centered regarding button width
 * \param dark_widget Widget drawn in front of the button when it's not highlighted
 * \param light_widget Widget drawn in front of the button when it's highlighted
 */
int Menu::addOption(int x, int y, int width, int height, const char *text, FontManager::EFontSize size, Key key,
            const char *to, bool visible, bool centered, int dark_widget, int light_widget) {
    
    Option *m = new Option(this, x, y, width, height, text, size, to, visible, centered, dark_widget, light_widget);
    hotKeys_[key] = m;
    actions_.push_back(m);

    return m->getId();
}

/*!
 * Returns a pointer to the action widget with the given id.
 * \param id The id of the widget.
 * \return NULL if no widget has that id.
 */
ActionWidget * Menu::getActionWidget(int id) {
    for (std::list < ActionWidget * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {

        if (id == (*it)->getId()) {
            return *it;
        }
    }

    return NULL;
}

Option * Menu::getOption(int optionId) {
    ActionWidget *pAction = getActionWidget(optionId);

    if (pAction) {
        return dynamic_cast<Option *> (pAction);
    }

    return NULL;
}

int Menu::addToggleAction(int x, int y, int width, int height, const char *text, FontManager::EFontSize size, Key key, bool selected) {
    ToggleAction *a = new ToggleAction(this, x, y, width, height, text, size, selected, &group_);
    group_.addButton(a);
    hotKeys_[key] = a;
    actions_.push_back(a);

    return a->getId();
}

/*!
 * Creates and adds a list box to the menu.
 * \param x X coordinate
 * \param y Y coordinate
 * \param width widget width
 * \param height widget height
 * \param visible True if widget is visible on screen
 * \return A pointer on the widget.
 */
ListBox * Menu::addListBox(int x, int y, int width, int height, bool visible) {
    ListBox *pBox = new ListBox(this, x, y, width, height, visible);
    actions_.push_back(pBox);

    return pBox;
}

/*!
 * Creates and adds a team list box to the menu.
 * \param x X coordinate
 * \param y Y coordinate
 * \param width widget width
 * \param height widget height
 * \param visible True if widget is visible on screen
 * \return A pointer on the widget.
 */
TeamListBox * Menu::addTeamListBox(int x, int y, int width, int height, bool visible) {
    TeamListBox *pBox = new TeamListBox(this, x, y, width, height, visible);
    actions_.push_back(pBox);

    return pBox;
}

TextField * Menu::addTextField(int x, int y, int width, int height, FontManager::EFontSize size, int maxSize, bool displayEmpty, bool visible) {
	TextField *pTextField = new TextField(this, x, y, width, height, size, maxSize, displayEmpty, visible);
	actions_.push_back(pTextField);

	return pTextField;
}

void Menu::captureInputBy(TextField *pTextfield) {
	if (pTextfield != pCaptureInput_ && pCaptureInput_ != NULL) {
		pCaptureInput_->handleCaptureLost();
	}

	pCaptureInput_ = pTextfield;
}

/*!
 * Handles the key pressed event.
 * \param key The key that was pressed
 * \param modKeys State of all modifier keys
 */
void Menu::keyEvent(Key key, const int modKeys)
{
	// first pass the event to the textfield that has the cursor
	if (pCaptureInput_ != NULL) {
		if (pCaptureInput_->handleKey(key, modKeys)) {
			return;
		}
	}

	// Then look for a mapped key to execute an action
    if (hotKeys_.find(key) != hotKeys_.end()) {
        Option *opt = hotKeys_[key];
        
        opt->executeAction(modKeys);

        return;
    }

    // Pressing Escape changes the current menu to its parent(like a back)
    // if menu has not already consummed key event
    if (!handleUnknownKey(key, modKeys) && key == KEY_ESCAPE) {
        menu_manager_->changeCurrentMenu(parent_menu_);
        return;
    }
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
        ActionWidget *pAction = getActionWidget(focusedWgId_);
        
        if (!pAction->isMouseOver(x, y) || !pAction->isVisible()) {
            pAction->handleFocusLost();
            focusedWgId_ = -1;
        }
    }

    // See if the mouse is hovering a button
    for (std::list < ActionWidget * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {
        ActionWidget *m = *it;

        if (!m->isVisible()) {
            // Button is not visible so it doesn't count
            continue;
        }

        // Mouse is over a widget
        if (m->isMouseOver(x, y)) {
            if (m->getId() != focusedWgId_) {
                // Widget has now the focus : handle the event
                m->handleFocusGained();
                focusedWgId_ = m->getId();
            }

            // Pass the event to the widget
            m->handleMouseMotion(x, y, state, modKeys);

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
	// First pass the event to the menu
	if (handleMouseDown(x, y, button, modKeys)) {
		// Menu has processed the event, so don't pass it to widget
		return;
	}

	// The event was not processed by the menu, so give a chance to a widget
    for (std::list < ActionWidget * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {
        ActionWidget *m = *it;

        if (!m->isVisible()) {
            // Widget is not visible so it doesn't count
            continue;
        }

        if (m->isMouseOver(x, y)) {
            m->handleMouseDown(x, y, button, modKeys);
            return;
        }
    }
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
