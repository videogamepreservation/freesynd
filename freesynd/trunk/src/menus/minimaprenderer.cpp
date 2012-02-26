#include "minimaprenderer.h"
#include "mission.h"
#include "gfx/screen.h"

MinimapRenderer::MinimapRenderer() {
    scroll_step_ = 0;
}

void MinimapRenderer::init(Mission *pMission, uint8 enh_level)
{
    // Initialize minimap origin by looking for the position
    // of the first found agent on the map
    p_mission_ = pMission;
    setEnhancementLevel(enh_level);

    bool found = false;
    int maxx = pMission->mmax_x_;
    int maxy = pMission->mmax_y_;

    for (int x = 0; x < maxx && (!found); x++) {
        for (int y = 0; y < maxy && (!found); y++) {
            if (pMission->getMinimapOverlay(x, y) == 1) {
                // We found a tile with an agent on it
                // stop searching and memorize position
                minimap_scroll_x_ = x;
                minimap_scroll_y_ = y;
                found = true;
            }
        }
    }

    minimap_blink_ticks_ = 0;
    minimap_blink_ = 0;
}

void MinimapRenderer::setEnhancementLevel(uint8 enh_level) { 
    enh_level_ = enh_level;
    // scrolling step depends on the level of details of the minimap
    scroll_step_ = 30 / (10 - (enh_level << 1));
}

bool MinimapRenderer::handleTick(int elapsed)
{
    minimap_blink_ticks_ += elapsed;

    if (minimap_blink_ticks_ > 500) {
        minimap_blink_ticks_ = 0;
        minimap_blink_ = 1 - minimap_blink_;
        return true;
    }

    return false;
}

void MinimapRenderer::scrollRight() {
    minimap_scroll_x_ += scroll_step_;
    if (minimap_scroll_x_ > p_mission_->mmax_x_)
        minimap_scroll_x_ = p_mission_->mmax_x_ - 1;
}

void MinimapRenderer::scrollLeft() {
    minimap_scroll_x_ -= scroll_step_;
    if (minimap_scroll_x_ < 0)
        minimap_scroll_x_ = 0;
}

void MinimapRenderer::scrollUp() {
    minimap_scroll_y_ -= scroll_step_;
    if (minimap_scroll_y_ < 0)
        minimap_scroll_y_ = 0;
}

void MinimapRenderer::scrollDown() {
    minimap_scroll_y_ += scroll_step_;
    if (minimap_scroll_y_ > p_mission_->mmax_y_)
        minimap_scroll_y_ = p_mission_->mmax_y_ - 1;
}

void MinimapRenderer::render() 
{
    int maxx = p_mission_->mmax_x_;
    int maxy = p_mission_->mmax_y_;
    bool addenemies = (enh_level_ == 4);

    unsigned char pixperblock = 10 - (enh_level_ << 1);
    short fullblocks = 120 / pixperblock;
    short halfblocks = fullblocks / 2;
    short modblocks = fullblocks % 2;
    short bxl = minimap_scroll_x_ - halfblocks + 1;
    short bxr = minimap_scroll_x_ + halfblocks + modblocks;
    short byl = minimap_scroll_y_ - halfblocks + 1;
    short byr = minimap_scroll_y_ + halfblocks + modblocks;

    // checking borders for correctness, map will be always on center
    if (bxl < 0) {
        bxl = 0;
        if (bxr >= maxx) {
            bxr = maxx - 1;
        } else {
            bxr = fullblocks >= maxx ? maxx - 1 : (fullblocks) - 1;
        }
    }
    if (bxr >= maxx) {
        bxr = maxx - 1;
        bxl = (maxx - (fullblocks)) < 0 ? 0 : (maxx - (fullblocks));
    }

    if (byl < 0) {
        byl = 0;
        if (byr >= maxy) {
            byr = maxy - 1;
        } else {
            byr = fullblocks >= maxy ? maxy - 1 : (fullblocks) - 1;
        }
    }
    if (byr >= maxy) {
        byr = maxy - 1;
        byl = (maxy - (fullblocks)) < 0 ? 0 : (maxy - (fullblocks));
    }

    short sx = 504;
    short sy = 220;
    if ((bxr - bxl + 1) < (fullblocks)) {
        sx += ((fullblocks - (bxr - bxl + 1)) >> 1) * pixperblock;
    }
    if ((byr - byl + 1) < (fullblocks)) {
        sy += ((fullblocks - (byr - byl + 1)) >> 1) * pixperblock;
    }

    for (short x = bxl; x <= bxr; x++) {
        short xc = sx + (x - bxl) * pixperblock;
        for (short y = byl; y <= byr; y++) {
            unsigned char c = p_mission_->getMinimapOverlay(x, y);
            switch (c) {
                case 0:
                    c = p_mission_->getMinimapColour(x, y);
                    break;
                case 1:
                    c = minimap_blink_ ? 14 : 12;
                    break;
                case 2:
                    if (addenemies)
                        c = minimap_blink_ ? 14 : 5;
                    else
                        c = p_mission_->getMinimapColour(x, y);
            }
            g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                pixperblock, c);
        }
    }
}