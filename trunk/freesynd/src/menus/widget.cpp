#include "widget.h"
#include "app.h"

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

void Widget::setWidth(int width) {
    width_ = width;
}

void Widget::setHeight(int height) {
    height_ = height;
}

void Widget::setVisible(bool visible) {
    visible_ = visible;
}

/*! 
 * Modify the MenuText text.
 * If the given text starts with a '#', the remaining
 * text identifies a key in the language file.
 * \param text to set
 */
void MenuText::setText(const char *text) {
    std::string lbl(text);
    // Find if string starts with '#' caracter
    if (lbl.find_first_of('#') == 0) {
        // Erase the # caracter
        lbl.erase(0, 1);
        // and looks for the message in the langage file
        g_App.menus().getMessage(lbl.c_str(), lbl);
    }
    text_ = lbl;
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

void MenuText::setDark(bool dark) {
    dark_ = dark;
    redraw();
}

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