/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *  Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net> *
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

#include "app.h"
#include "menus/flimenu.h"
#include "utils/file.h"
#include "gfx/screen.h"

FliMenu::FliMenu(MenuManager *m, int menuId, int nextMenu) : Menu(m, menuId, MENU_MAIN), fliPlayer_()
{
	fliIndex_ = 0;
	pData_ = NULL;
	playingFli_ = false;
	nextMenu_ = nextMenu;
}

FliMenu::~FliMenu()
{
	if (pData_) {
		delete[] pData_;
		pData_ = NULL;
	}
}

/*!
 * Adds a new description.
 * \param anim animation file name.
 * \param fps animation speed.
 * \param waitKey True to wait for the user input
 * \param skipable True means user can skip animation
 * \param music
 * \param sound
 */
void FliMenu::addFliDesc(const char *anim, uint8 fps, bool waitKey, bool skipable, msc::MusicTrack music, snd::InGameSample sound) {
	FliDesc desc;
	desc.name = anim;
	desc.fps = fps;
	desc.waitKeyPressed = waitKey;
	desc.sound = sound;
	desc.music = music;
	desc.skipable = skipable;

	fliList_.push_back(desc);
}

/*!
 * Loads the next animation if there is one still in the list.
 * \return True if an animation has been loaded.
 */
bool FliMenu::loadNextFli() {
	// Stop all music if one was being played
	g_App.music().stopPlayback();
	playingFli_ = false;
	// loads Fli
	if ( fliIndex_ < fliList_.size()) {
		int size = 0;

		if (pData_) {
			delete[] pData_;
			pData_ = NULL;
		}
		// Gets the fli description
		FliDesc desc = fliList_.at(fliIndex_);
		// Loads data from file
		pData_ = File::loadOriginalFile(desc.name, size);
		if (pData_) {
			fliPlayer_.loadFliData(pData_);
			if (fliPlayer_.hasFrames()) {
				g_Screen.clear(0);
				frameDelay_ = 0;
				fliIndex_++;
				return true;
			}
		}
	}

	// No animation was loaded
	return false;
}

void FliMenu::handleShow() 
{
	// Loads the first Fli
	if (!fliList_.empty()) {
		loadNextFli();
	}
}

void FliMenu::handleTick(int elapsed) 
{
	if (fliPlayer_.hasFrames()) {
		FliDesc desc = fliList_.at(fliIndex_ - 1);
		// There is a frame to display
		frameDelay_ += elapsed;
		if (frameDelay_ > desc.fps) {
			// read frame
			if (!fliPlayer_.decodeFrame()) {
				// Frame is not good -> quit
				menu_manager_->gotoMenu(nextMenu_);
				return;
			}

			fliPlayer_.copyCurrentFrameToScreen();
			// Add a dirty rect just to start the render routine
			addDirtyRect(0, 0, 1, 1);
			// Reset delay between frames
			frameDelay_ = 0;
		}

		// start music if fli is just begining
		if (!playingFli_) {
			if (desc.music != msc::NO_TRACK) {
				g_App.music().playTrack(desc.music);
			} else if (desc.sound != snd::NO_SOUND) {
				g_App.gameSounds().play(desc.sound);
			}
		}
		playingFli_ = true;
	} else if (playingFli_ ) {
		// A fli was being played but it has ended
		// so if user doesn't have to click end animation
		// trying to load next fli.
		playingFli_ = false;
		FliDesc desc = fliList_.at(fliIndex_ - 1);
		if (!desc.waitKeyPressed && !loadNextFli()) {
			// no more animation so quit
			menu_manager_->gotoMenu(nextMenu_);
		}
	}
}

void FliMenu::handleRender(DirtyList &dirtyList)
{}

/*!
 * Ends the animation.
 */
bool FliMenu::handleMouseDown(int x, int y, int button, const int modKeys)
{
	if (fliIndex_ > 0) {
		FliDesc desc = fliList_.at(fliIndex_ - 1);
		if ((playingFli_ && desc.skipable) || (!playingFli_ && desc.waitKeyPressed)) {
			// Fli can be ended by pressing a key
			if (!loadNextFli()) {
				// no fli next so quit
				menu_manager_->gotoMenu(nextMenu_);
			}
		}
	}

	return true;
}

/*!
 * Ends the animation.
 */
bool FliMenu::handleUnknownKey(Key key, const int modKeys) {
	if (fliIndex_ > 0) {
		FliDesc desc = fliList_.at(fliIndex_ - 1);
		if ((playingFli_ && desc.skipable) || (!playingFli_ && desc.waitKeyPressed)) {
			// Fli can be ended by pressing a key
			if (!loadNextFli()) {
				// no fli next so quit
				menu_manager_->gotoMenu(nextMenu_);
			}
		}
	}

	return true;
}
    
void FliMenu::handleLeave() 
{
	if (pData_) {
		delete[] pData_;
		pData_ = NULL;
	}

	fliList_.clear();
	fliIndex_ = 0;
}

