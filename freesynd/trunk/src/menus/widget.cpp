#include "widget.h"
#include "app.h"

int Widget::widgetCnt = 0;

/*!
 * Utility method to add a dirty rect to the menu.
 * Adds rect only if widget is visible.
 */
void Widget::redraw() {
    if (visible_) {
        g_App.menus().addRect(x_, y_, width_, height_);
    }
}

void Widget::setLocation(int x, int y) {
    x_ = x;
    y_ = y;
}

void Widget::setVisible(bool visible) {
    if (visible != visible_) {
        redraw();
        visible_ = visible;
    }
}

MenuText::MenuText(int x, int y, const char *text, FontManager::EFontSize size, 
                   bool dark, bool visible): 
            Widget(x, y, 0, 0, visible), dark_(dark), size_(size) {

        centered_ = false;
        anchorX_ = x;
        anchorY_ = y;
        // Height is fixed by font size
        height_ = g_App.fonts().textHeight(size_);

        updateText(text);
}

MenuText::MenuText(int x, int y, int width, const char *text, FontManager::EFontSize size, 
                   bool dark, bool visible, bool centered): 
            Widget(x, y, width, 0, visible), dark_(dark), size_(size) {
        
        centered_ = centered;
        anchorX_ = x;
        anchorY_ = y;
        // Height is fixed by font size
        height_ = g_App.fonts().textHeight(size_);

        updateText(text);
    }

void MenuText::updateText(const char *text) {
    std::string lbl(text);
    // Find if string starts with '#' caracter
    if (lbl.find_first_of('#') == 0) {
        // Erase the # caracter
        lbl.erase(0, 1);
        // and looks for the message in the langage file
        g_App.menus().getMessage(lbl.c_str(), lbl);
    }
    text_ = lbl;

    int textWidth = g_App.fonts().textWidth(text_.c_str(), false, size_);
    if (textWidth > width_) {
        width_ = textWidth;
    }

    if (centered_) {
        anchorX_ = (x_ + x_ + width_) / 2  - textWidth / 2;
    }
}

/*! 
 * Modify the MenuText text.
 * If the given text starts with a '#', the remaining
 * text identifies a key in the language file.
 * \param text to set
 */
void MenuText::setText(const char *text) {
    updateText(text);
    redraw();
}

/*! 
 * Modify the MenuText text.
 * The given text can be a formated string.
 * If it starts with a '#', the remaining
 * text identifies a key in the language file.
 * \param format message format
 */
void MenuText::setTextFormated(const char * format, ...) {

    char tmp[200];
    va_list list;

    std::string lbl(format);
    // Find if string starts with '#' caracter
    if (lbl.find_first_of('#') == 0) {
        // Erase the # caracter
        lbl.erase(0, 1);
        // and looks for the message in the langage file
        g_App.menus().getMessage(lbl.c_str(), lbl);
    }

    va_start(list, format);
    vsprintf(tmp, lbl.c_str(), list);
    va_end(list);

    setText(tmp);
}

void MenuText::setLocation(int x, int y) {
    int dx = x - x_;
    int dy = y - y_;
    x_ = x;
    y_ = y;
    anchorX_ += dx;
    anchorY_ += dy;
}

void MenuText::setDark(bool dark) {
    dark_ = dark;
    redraw();
}

/*!
 * Draw the widget at the current position.
 * Actually, only a text is drawn (see Font). The borders are
 * already drawn on the background image.
 */
void MenuText::draw() {
    g_App.fonts().drawText(anchorX_, anchorY_, text_.c_str(), false, size_, dark_);
}

bool ActionWidget::isMouseOver(int x, int y) {
 
    return (x > x_  && 
            x < x_ + width_ && 
            y >= y_ && 
            y < y_ + height_);
}

void ActionWidget::setenabled(bool enabled) {
	if (enabled != enabled_) {
		enabled_ = enabled;
		redraw();
	}
}

Option::Option(Menu *peer, int x, int y, int width, int height, const char *text, FontManager::EFontSize size,
            const char *to, bool visible, bool centered, int darkWidgetId, int lightWidgetId) 
            : ActionWidget(peer, x, y, width, height, visible), text_(x, y, width - 4, text, size, true, true, centered) {
        to_ = to;
        darkWidget_ = NULL;
        lightWidget_ = NULL;

        // Position the button text in the middle of height
        // add 1 pixel to height to compensate lost of division
        text_.setLocation(text_.getX(), y_ + (height_ / 2) - (text_.getHeight() / 2) + 1);

        if (darkWidgetId != 0) {
            darkWidget_ = g_App.menuSprites().sprite(darkWidgetId);
            lightWidget_ = g_App.menuSprites().sprite(lightWidgetId);
            // there's a small pad between heading widget ant text
            text_.setLocation(text_.getX() + darkWidget_->width() * 2 + 8, text_.getY());
        }
}

Option::~Option() {
    to_ = NULL; 
}
/*!
 * Draw the widget at the current position.
 * Actually, only a text is drawn (see Font). The borders are
 * already drawn on the background image.
 */
void Option::draw() {
    int x = x_;
    if (text_.isDark() && darkWidget_ != NULL) {
        darkWidget_->draw(x, y_, 0, false, true);
    } else if (lightWidget_ != NULL) {
        lightWidget_->draw(x, y_, 0, false, true);
    }

    text_.draw();
}

/*!
 * Calls Menu::handleAction() then redirect to
 * another menu if the field "to" has been set.
 */
void Option::executeAction(const int modKeys) {
    if (peer_ && this->isVisible()) {
        peer_->handleAction(getId(), NULL, modKeys);
    }

    if (to_) {
        g_App.menus().changeCurrentMenu(to_);
        return;
    }
}

void Option::handleFocusGained() {
    text_.setDark(false);
    redraw();
}
void Option::handleFocusLost() {
    text_.setDark(true);
    redraw();
}

void Option::handleMouseDown(int x, int y, int button, const int modKeys) {
    executeAction(modKeys);
}

Group::~Group() {
    actions_.clear();
}

void Group::addButton(ToggleAction *pAction) {
    actions_.push_back(pAction);
}

void Group::selectButton(int id) {
    for (std::list < ToggleAction * >::iterator it = actions_.begin();
         it != actions_.end(); it++) {
        ToggleAction *pAction = *it;

        if (pAction->getId() != id ) {
            pAction->handleSelectionLost();
        } else
            pAction->handleSelectionAquire();
    }
}

ToggleAction::ToggleAction(Menu *peer, int x, int y, int width, int height, 
                            const char *text, FontManager::EFontSize size, bool selected, Group *pGroup)
: Option(peer, x, y, width, height, text, size, NULL, true) {
    group_ = pGroup;
    setSelected(selected);
}

void ToggleAction::setSelected(bool isSelected) {
    selected_ = isSelected;
    // When a ToggleAction is selected it lighted
    text_.setDark(!selected_);
    redraw();
}

void ToggleAction::executeAction(const int modKeys) {
    // Deselect all other buttons of the group and select current
    group_->selectButton(getId());
    Option::executeAction(modKeys);
}

void ToggleAction::handleFocusLost() {
    // Toggle buttons get dark only
    // if they are not pushed
    if (!selected_) {
        Option::handleFocusLost();
    }
}

void ToggleAction::handleSelectionLost() {
    setSelected(false);
}

void ToggleAction::handleSelectionAquire() {
    setSelected(true);
}

ListBox::ListBox(Menu *peer, int x, int y, int width, int height, bool visible) :
        ActionWidget(peer, x, y, width, height, visible) {
    focusedLine_ = -1;
    pModel_ = NULL;
}

ListBox::~ListBox() {
    labels_.clear();

    if (pModel_) {
        pModel_->removeModelListener(this);
    }
}

void ListBox::setModel(SequenceModel *pModel) {
    pModel_ = pModel;
    labels_.clear();
    pModel_->getLabels(labels_);
    pModel_->addModelListener(this);
}

void ListBox::handleModelChanged() {
    labels_.clear();
    pModel_->getLabels(labels_);
    redraw();
}

//! Draw the widget on screen
void ListBox::draw() {
    int i=0;
    for (std::list < std::string >::iterator it = labels_.begin();
         it != labels_.end(); it++, i++) {
             g_App.fonts().drawText(getX(), getY() + i * 12, (*it).c_str(), false, FontManager::SIZE_1, focusedLine_ != i);
    }
}

void ListBox::handleMouseMotion(int x, int y, int state, const int modKeys) {
    
    if (pModel_) {
        // Gets the line pointed by the mouse
        unsigned int i = (y - getY()) / 12;
        if (i < pModel_->size()) {
            if (pModel_->getElement(i)) {
                // If line contains something, highlight it
                if (focusedLine_ != (int)i) {
                    redraw();
                    focusedLine_ = i;
                }
            } else {
                if (focusedLine_ != -1) {
                    redraw();
                    focusedLine_ = -1;
                }
            }
        }  else if (focusedLine_ != -1) {
            // A line was highlighted but not anymore
            redraw();
            focusedLine_ = -1;
        }
    }
}

void ListBox::handleMouseDown(int x, int y, int button, const int modKeys) {
    if (focusedLine_ != -1 && pModel_) {
        if (peer_) {
            // call the peer handleAction method giving the index of pressed line.
            std::pair<int, void *> tuple = std::make_pair(focusedLine_, pModel_->getElement(focusedLine_));
            peer_->handleAction(getId(), &tuple, modKeys);
            
        }
    }
}

void ListBox::handleFocusLost() {
    focusedLine_ = -1;
    redraw();
}

const int TeamListBox::LINE_OFFSET = 20;

TeamListBox::TeamListBox(Menu *peer, int x, int y, int width, int height, bool visible) :
        ListBox(peer, x, y, width, height, visible) {
    pTitle_ = new MenuText(x, y, width, "#SELECT_CRYO_TITLE", FontManager::SIZE_1, false);
    lUnderline_ = g_App.fonts().textWidth(pTitle_->getText().c_str(), false, FontManager::SIZE_1);
    xUnderline_ = (x + x + width) / 2  - lUnderline_ / 2;
    yUnderline_ = y + g_App.fonts().textHeight(FontManager::SIZE_1);
    yOrigin_ = yUnderline_ + 2;
    for (int i=0; i<4; i++) {
        squadLines_[i] = i;
    }

    g_App.menus().getMessage("MENU_LB_EMPTY", emptyLbl_);
}

TeamListBox::~TeamListBox() {
    delete pTitle_;
    pTitle_ = NULL;
}

//! Draw the widget on screen
void TeamListBox::draw() {

    pTitle_->draw();
    g_Screen.drawRect(xUnderline_, yUnderline_, lUnderline_, 2, 252);

    int i=0;
    for (std::list < std::string >::iterator it = labels_.begin();
         it != labels_.end(); it++, i++) {
             if ((*it).size() != 0) {
                 for (int ln=0; ln<4; ln++) {
                     if (squadLines_[ln] ==  i) {
                        char tmp[5];
                        sprintf(tmp, "%d", ln+1);
                        g_App.fonts().drawText(getX(), yOrigin_ + i * 12, tmp, false, FontManager::SIZE_1, focusedLine_ != i);
                        break;
                     }
                 }
                 g_App.fonts().drawText(getX() + LINE_OFFSET, yOrigin_ + i * 12, (*it).c_str(), false, FontManager::SIZE_1, focusedLine_ != i);
             } else {
                 g_App.fonts().drawText(getX() + LINE_OFFSET, yOrigin_ + i * 12, emptyLbl_.c_str(), false, FontManager::SIZE_1, focusedLine_ != i);
             }
    }
}

void TeamListBox::handleMouseMotion(int x, int y, int state, const int modKeys) {
    
    if (pModel_) {
        // Gets the line pointed by the mouse
        int i = (y - yOrigin_) / 12;
        if (i >= 0 && i < (int) pModel_->size()) {
            if (pModel_->getElement(i)) {
                // If line contains something, highlight it
                if (focusedLine_ != i) {
                    redraw();
                    focusedLine_ = i;
                }
            } else {
                if (focusedLine_ != -1) {
                    redraw();
                    focusedLine_ = -1;
                }
            }
        }  else if (focusedLine_ != -1) {
            // A line was highlighted but not anymore
            redraw();
            focusedLine_ = -1;
        }
    }
}

void TeamListBox::setSquadLine(int squadSlot, unsigned int line) {
    if (pModel_ && line < pModel_->size() && squadSlot >= 0 && squadSlot < 4) {
        squadLines_[squadSlot] = line;
        redraw();
    }
}

// By default there's no empty label
std::string TextField::emptyLbl_ = "";

TextField::TextField(Menu *peer, int x, int y, int width, int height, FontManager::EFontSize size,
            int maxSize, bool displayEmpty, bool visible) 
            : ActionWidget(peer, x, y, width, height, visible), text_(x, y, width, "", size, true, visible, false) {

    // Position the button text in the middle of height
    // add 1 pixel to height to compensate lost of division
    text_.setLocation(text_.getX(), y_ + (height_ / 2) - (text_.getHeight() / 2) + 1);
	isDisplayEmpty_ = displayEmpty;
	caretPosition_ = 0;
	isInEdition_ = false;
	maxSize_ = maxSize;
}

TextField::~TextField() {
}

/*!
 * Draw the widget at the current position.
 * Actually, only a text is drawn (see Font). The borders are
 * already drawn on the background image.
 */
void TextField::draw() {
    text_.draw();
	if (isInEdition_) {
		text_.draw();
		drawCaret();
	} else if (isDisplayEmpty_ && text_.getText().size() == 0) {
		g_App.fonts().drawText(getX(), text_.getY(), emptyLbl_.c_str(), false, FontManager::SIZE_2, true);
    } else {
        text_.draw();
    }
}

void TextField::handleCaptureLost() {
	isInEdition_ = false;
	redraw();
}

bool TextField::handleKey(Key key, const int modKeys) {
	if (key == KEY_LEFT) {
		// Move caret to the left until start of the text
        if (caretPosition_ > 0) {
            caretPosition_--;
        }
    } else if (key == KEY_RIGHT) {
		// Move caret to the right until the end of the text
		if (caretPosition_ < text_.getText().size()) {
            caretPosition_++;
        }
    } else if (key == KEY_BACKSPACE) {
		// Erase one character before caret
        if (caretPosition_ > 0) {
			std::string str = text_.getText();
            str.erase(caretPosition_ - 1, 1);
			text_.setText(str.c_str());
            caretPosition_--;
        }
    } else if (key == KEY_DELETE) {
        if (caretPosition_ < text_.getText().size()) {
			std::string str = text_.getText();
			str.erase(caretPosition_, 1);
            //text_.getText().erase(caretPosition_, 1);
			text_.setText(str.c_str());
        }
    } else if (key == KEY_HOME) {
        caretPosition_ = 0;
    } else if (key == KEY_END) {
        caretPosition_ = text_.getText().size();
    } else if (MenuManager::isPrintableKey(key)) {
        if (text_.getText().size() < maxSize_) {
            std::string str = text_.getText().substr(0, caretPosition_);
            str += MenuManager::getKeyAsChar(key);
			str += text_.getText().substr(caretPosition_, text_.getText().size());
			text_.setText(str.c_str());
            caretPosition_++;
        }
    } else {
        return false;
    }

	redraw();
	return true;
}

void TextField::handleMouseDown(int x, int y, int button, const int modKeys) {
	isInEdition_ = true;
	peer_->captureInputBy(this);

	int size = g_App.fonts().textWidth(text_.getText().c_str(), false, text_.getSize());

    // computes caret position
    if (x > size + getX()) {
        // Clicked after the text so caret is at the end
        caretPosition_ = text_.getText().size();
    } else {
        // Clicked in the text so computes what exact letter
        size_t pos = 1;
        for (unsigned int i=0; i<text_.getText().size(); i++, pos++) {
            std::string sub = text_.getText().substr(0, pos);
			if (x < getX() + g_App.fonts().textWidth(sub.c_str(), false, text_.getSize())) {
                caretPosition_ = pos - 1;
                break;
            }
        }
    }

	redraw();
}

void TextField::setText(const char* text) {
	text_.setText(text);
	redraw();
}

void TextField::drawCaret() {
        std::string start = text_.getText().substr(0, caretPosition_);
		int x = getX() + g_App.fonts().textWidth(start.c_str(), false, text_.getSize()) + 1;
        int y = text_.getY() + g_App.fonts().textHeight(text_.getSize(), true);

        // width of caret is the same of the letter above
        int length = 10;
        if (caretPosition_ < text_.getText().size()) {
            length = g_App.fonts().textWidth(text_.getText().substr(caretPosition_, 1).c_str(), false, text_.getSize());
        }

        // Draw caret
        g_Screen.drawRect(x, y, length, 2, 252);
}