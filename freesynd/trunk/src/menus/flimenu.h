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

#ifndef FLIMENU_H
#define FLIMENU_H

#include "menus/menumanager.h"
#include "gfx/fliplayer.h"
#include "sound/sound.h"
#include "sound/music.h"

struct FrameEvent {
\t/*! frame Id.*/
\tuint16 frame;
\t/*! start music.*/
\tmsc::MusicTrack\tmusic;
\t/*! Game sound to play.*/
\tsnd::InGameSample\tsound;
\tuint8\tsndChan;
\t/*! Operation on subtitle.*/
\tconst char* subtitle;
};

/*!
 * A description of the fli animation to be played.
 */
struct FliDesc {
\t/*! Name of the file containing the animation.*/
\tstd::string name;
\t/*! Speed of animation : delay between 2 frames.*/
\tuint8 frameDelay;
\t/*! True means user has to press key or mouse to go to 
\t * next animation or next menu after the end of animation.*/
\tbool waitKeyPressed;
\t/*! True means animation can be skipped by pressing key or mouse.*/
\tbool skipable;
\tconst FrameEvent *evtList;
};

/*!
 * A Flimenu can display a set of fli animations before redirecting towards another menu.
 */
class FliMenu : public Menu {
public:
    FliMenu(MenuManager *m, int menuId);
    ~FliMenu();

    void handleTick(int elapsed);
    void handleShow();
    
    void handleLeave();
    
protected:
\tvoid handleRender(DirtyList &dirtyList);

\tbool handleMouseDown(int x, int y, int button, const int modKeys);

\tbool handleUnknownKey(Key key, const int modKeys);

\tbool loadNextFli();

\tvoid addFliDesc(const char *anim, uint8 frameDelay, bool waitKey, bool skipable, const FrameEvent *events);

protected:
\t/*! The Fli player.*/
\tFliPlayer fliPlayer_;
\t/*! The list of animation to play.*/
\tstd::vector<FliDesc> fliList_;
\t/*! Index of the next animation to play.*/
\tsize_t fliIndex_;
\tuint16 frameIndex_;
\t/*! Timer to control animation speed.*/
\tint frameDelay_;
\t/*! Content of current animation.*/
\tuint8 *pData_;
\t/*! A flag telling if an animation is being played.*/
\tbool playingFli_;
\t/*! Id of the menu to go after all animations have been played.*/
\tint nextMenu_;
\t/*! Content of a subtitle to draw during animation. Used in intro.*/
\tstd::string currSubTitle_;
};

#endif
