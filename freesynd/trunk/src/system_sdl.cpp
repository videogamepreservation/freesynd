/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#include "config.h"
#include "gfx/screen.h"
#include "system_sdl.h"
#include "app.h"
#include "sound/audio.h"
#include "utils/file.h"
#include "utils/log.h"

#include <SDL_image.h>

SDL_Joystick *joy = NULL;

const int SystemSDL::CURSOR_WIDTH = 24;

SystemSDL::SystemSDL(int depth) {
    depth_ = depth;
    keyModState_ = 0;
    screen_surf_ = NULL;
    temp_surf_ = NULL;
    cursor_surf_ = NULL;
}

SystemSDL::~SystemSDL() {
    if (temp_surf_) {
        SDL_FreeSurface(temp_surf_);
    }

    if (cursor_surf_) {
        SDL_FreeSurface(cursor_surf_);
    }

#ifdef HAVE_SDL_MIXER
    Audio::quit();
#endif

    // Destroy SDL_Image Lib
    IMG_Quit();

    SDL_Quit();
}

bool SystemSDL::initialize(bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO
#ifdef GP2X
                 | SDL_INIT_JOYSTICK
#endif
        ) < 0) {
        printf("Critical error, SDL could not be initialized!");
        return false;
    }
#ifdef GP2X
    if (SDL_NumJoysticks() > 0) {
        joy = SDL_JoystickOpen(0);
        if (!joy) {
            fprintf(stderr, "Couldn't open joystick 0: %s\n",
                    SDL_GetError());
        }
        printf("found joystick\n");
    }
#endif

    SDL_WM_SetCaption("FreeSynd", NULL);

    // Keyboard init
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    // Audio initialisation
    if (!Audio::init()) {
        LOG(Log::k_FLG_SND, "SystemSDL", "Init", ("Couldn't initialize Sound System : no sound will be played."))
    }

    // TODO(nobody): maybe use double buffering?
#ifdef GP2X
    screen_surf_ = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
    temp_surf_ =
        SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 8, 0, 0, 0, 0);
#else
    screen_surf_ =
        SDL_SetVideoMode(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, depth_,
                         SDL_DOUBLEBUF | SDL_HWSURFACE | (fullscreen ?
                                                          SDL_FULLSCREEN :
                                                          0));
    temp_surf_ =
        SDL_CreateRGBSurface(SDL_SWSURFACE, GAME_SCREEN_WIDTH,
                             GAME_SCREEN_HEIGHT, 8, 0, 0, 0, 0);

#endif

    cursor_surf_ = NULL;
    // Init SDL_Image library
    int sdl_img_flags = IMG_INIT_PNG;
    int initted = IMG_Init(sdl_img_flags);
    if ( (initted & sdl_img_flags) != sdl_img_flags ) {
        printf("Failed to init SDL_Image : %s\n", IMG_GetError());
    } else {
        // Load the cursor sprites
        if (loadCursorSprites()) {
            // Cursor movement is managed by the application
            SDL_ShowCursor(SDL_DISABLE);
        }
        // At first the cursor is hidden
        hideCursor();
        useMenuCursor();
    }

    return true;
}

void SystemSDL::updateScreen() {
    if (g_Screen.dirty()|| (cursor_visible_ && update_cursor_)) {
        SDL_LockSurface(temp_surf_);
#ifdef GP2X
        const uint8 *pixeldata = g_Screen.pixels();
        uint8 *screen = (uint8 *) temp_surf_->pixels;
        for (int j = 0; j < 240; j++)
            for (int i = 0; i < 320; i++) {
                int tx = i * GAME_SCREEN_WIDTH / 320;
                int ty = j * GAME_SCREEN_HEIGHT / 240;

                uint8 c = pixeldata[ty * GAME_SCREEN_WIDTH + tx];
                screen[j * 320 + i] = c;
            }
#else
        memcpy(temp_surf_->pixels, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
#endif
        SDL_UnlockSurface(temp_surf_);

        g_Screen.clearDirty();

        SDL_BlitSurface(temp_surf_, NULL, screen_surf_, NULL);

        if (cursor_visible_) {
            SDL_Rect dst;

            dst.x = cursor_x_ - cursor_hs_x_;
            dst.y = cursor_y_ - cursor_hs_y_;
            SDL_BlitSurface(cursor_surf_, &cursor_rect_, screen_surf_, &dst);
            update_cursor_ = false;
        }

        SDL_Flip(screen_surf_);
    }
}

/*!
 * Using the keysym parameter, verify if this is a key that is 
 * used in the game and returns the corresponding entry in the Key enumaration.
 * \ returns If key code is not valid, returns KEY_UNKNOWN.
 */
Key SystemSDL::checkValidKey(SDL_keysym keysym) {
    Key key = KEY_UNKNOWN;
    switch(keysym.sym) {
        case SDLK_ESCAPE: key = KEY_ESCAPE; break;
        case SDLK_BACKSPACE: key = KEY_BACKSPACE; break;
        case SDLK_SPACE: key = KEY_SPACE; break;
        case SDLK_BACKQUOTE: key = KEY_BACKQUOTE; break;
        case SDLK_DELETE: key = KEY_DELETE; break;
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_RIGHT:
        case SDLK_LEFT:
        case SDLK_HOME:
        case SDLK_END:
        case SDLK_PAGEUP:
        case SDLK_PAGEDOWN:
            key = static_cast < Key > (KEY_UP + (keysym.sym - SDLK_UP));
            break;
        case SDLK_F1:
        case SDLK_F2:
        case SDLK_F3:
        case SDLK_F4:
        case SDLK_F5:
        case SDLK_F6:
        case SDLK_F7:
        case SDLK_F8:
        case SDLK_F9:
        case SDLK_F10:
        case SDLK_F11:
        case SDLK_F12:
            key = static_cast < Key > (KEY_F1 + (keysym.sym - SDLK_F1));
            break;

        case SDLK_a:case SDLK_b:case SDLK_c:case SDLK_d:case SDLK_e:
        case SDLK_f:case SDLK_g:case SDLK_h:case SDLK_i:case SDLK_j:
        case SDLK_k:case SDLK_l:case SDLK_m:case SDLK_n:case SDLK_o:
        case SDLK_p:case SDLK_q:case SDLK_r:case SDLK_s:case SDLK_t:
        case SDLK_u:case SDLK_v:case SDLK_w:case SDLK_x:case SDLK_y:case SDLK_z:
            key = static_cast < Key > (KEY_a + (keysym.sym - SDLK_a));
            break;

        case SDLK_0:case SDLK_1:case SDLK_2:case SDLK_3:case SDLK_4:
        case SDLK_5:case SDLK_6:case SDLK_7:case SDLK_8:case SDLK_9:
            key = static_cast < Key > (KEY_0 + (keysym.sym - SDLK_0));
            break;

        case SDLK_KP0:case SDLK_KP1:case SDLK_KP2:case SDLK_KP3:case SDLK_KP4:
        case SDLK_KP5:case SDLK_KP6:case SDLK_KP7:case SDLK_KP8:case SDLK_KP9:
            key = static_cast < Key > (KEY_0 + (keysym.sym - SDLK_KP0));
            break;

        case SDLK_PLUS:case SDLK_COMMA:case SDLK_MINUS:case SDLK_PERIOD:
        case SDLK_SLASH:
            key = static_cast < Key > (KEY_PLUS + (keysym.sym - SDLK_PLUS));
            break;
        default:
            // unused key
            break;
    }

    return key;
}

/*!
 * Watch the event queue and dispatch events.
 * - keyboard events : when a modifier key is pressed,
 * the system does not dispatch the event to the application:
 * it stores the key state and then passes the complete state
 * when a regular key is pressed. So that the application knows
 * if multiple modifier keys are pressed at the same time (ie Ctrl/Shift)
 */
void SystemSDL::handleEvents() {
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            {
            // Check if key pressed is a modifier
            bool isKeyMod = false;
            switch(event.key.keysym.sym) {
                case SDLK_RSHIFT:
                    keyModState_ = keyModState_ | KMD_RSHIFT; 
                    isKeyMod = true;
                    break;
                case SDLK_LSHIFT:
                    keyModState_ = keyModState_ | KMD_LSHIFT; 
                    isKeyMod = true;
                    break;
                case SDLK_RCTRL:
                    keyModState_ = keyModState_ | KMD_RCTRL; 
                    isKeyMod = true;
                    break;
                case SDLK_LCTRL:
                    keyModState_ = keyModState_ | KMD_LCTRL; 
                    isKeyMod = true;
                    break;
                case SDLK_RALT:
                    keyModState_ = keyModState_ | KMD_RALT; 
                    isKeyMod = true;
                    break;
                case SDLK_LALT:
                    keyModState_ = keyModState_ | KMD_LALT; 
                    isKeyMod = true;
                    break;
                default:
                    break;
            }

            // We pass the event only if it's not a allowed modifier key
            // Plus, the application receives event only when key is pressed
            // not released.
            if (!isKeyMod) {
                Key key = checkValidKey(event.key.keysym);
                if (key != KEY_UNKNOWN) {
                    g_App.keyEvent(key, keyModState_);
                }
            }
            }
            break;
        case SDL_KEYUP:
            {
            switch(event.key.keysym.sym) {
                case SDLK_RSHIFT:
                    keyModState_ = keyModState_ & !KMD_RSHIFT;
                    break;
                case SDLK_LSHIFT:
                    keyModState_ = keyModState_ & !KMD_LSHIFT;
                    break;
                case SDLK_RCTRL:
                    keyModState_ = keyModState_ & !KMD_RCTRL;
                    break;
                case SDLK_LCTRL:
                    keyModState_ = keyModState_ & !KMD_LCTRL;
                    break;
                case SDLK_RALT:
                    keyModState_ = keyModState_ & !KMD_RALT;
                    break;
                case SDLK_LALT:
                    keyModState_ = keyModState_ & !KMD_LALT;
                    break;
                default:
                    break;
            }
            }
            break;
#ifdef GP2X
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            Key key = static_cast < Key > (event.jbutton.button);
            keyMod = KMD_NONE;
            g_App.keyEvent(key, KMD_NONE);
            break;
#endif

        case SDL_MOUSEBUTTONUP:
            g_App.mouseUpEvent(event.button.x, event.button.y,
                               event.button.button, keyModState_);
            break;

        case SDL_MOUSEBUTTONDOWN:
            g_App.mouseDownEvent(event.button.x, event.button.y,
                                 event.button.button, keyModState_);
            break;

        case SDL_MOUSEMOTION:
            update_cursor_ = true;
            cursor_x_ = event.motion.x;
            cursor_y_ = event.motion.y;
            g_App.mouseMotionEvent(event.motion.x, event.motion.y,
                                   event.motion.state, keyModState_);
            break;

        case SDL_QUIT:
            g_App.quit();
            break;

        default:
            break;
        }
    }
}

void SystemSDL::delay(int msec) {
    SDL_Delay(msec);
}

bool like(int a, int b) {
    return a == b || a == b - 1 || a == b + 1;
}

void SystemSDL::setPalette6b3(const uint8 * pal, int cols) {
    static SDL_Color palette[256];

    for (int i = 0; i < cols; ++i) {
        uint8 r = pal[i * 3 + 0];
        uint8 g = pal[i * 3 + 1];
        uint8 b = pal[i * 3 + 2];

        // multiply by 255 divide by 63 isn't good enough?
        palette[i].r = (r << 2) | (r >> 4);
        palette[i].g = (g << 2) | (g >> 4);
        palette[i].b = (b << 2) | (b >> 4);

#if 0
        if (like(palette[i].r, 28) && like(palette[i].g, 144)
            && like(palette[i].b, 0))
            printf("col %i = %i, %i, %i\n", i, palette[i].r, palette[i].g,
                   palette[i].b);
#endif
    }

    SDL_SetColors(temp_surf_, palette, 0, cols);
}

void SystemSDL::setPalette8b3(const uint8 * pal, int cols) {
    static SDL_Color palette[256];

    for (int i = 0; i < cols; ++i) {
        palette[i].r = pal[i * 3 + 0];
        palette[i].g = pal[i * 3 + 1];
        palette[i].b = pal[i * 3 + 2];
    }

    SDL_SetColors(temp_surf_, palette, 0, cols);
}

void SystemSDL::setColor(uint8 index, uint8 r, uint8 g, uint8 b) {
    static SDL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;

    SDL_SetColors(temp_surf_, &color, index, 1);
}

/*!
 * This method uses the SDL_Image library to load a file called
 * cursors/cursors.png under the root path.
 * The file is loaded into the cursor surface.
 * \return False if the loading has failed. If it's the case, 
 * cursor_surf_ will be NULL.
 */
bool SystemSDL::loadCursorSprites() {
    cursor_rect_.w = cursor_rect_.h = CURSOR_WIDTH;

    cursor_surf_ = IMG_Load(File::dataFullPath("cursors/cursors.png").c_str());

    if (!cursor_surf_) {
        printf("Cannot load cursors image: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

/*! 
 * Returns the mouse pointer coordinates using SDL_GetMouseState. 
 * \param x The x coordinate.
 * \param y The y coordinate.
 * \return See SDL_GetMouseState.
 */
int SystemSDL::getMousePos(int *x, int *y) {
    return SDL_GetMouseState(x, y);
}

void SystemSDL::hideCursor() {
    if (cursor_surf_ != NULL) {
        cursor_visible_ = false;
    } else {
        // Custom cursor surface doesn't
        // exists so use the default SDL Cursor
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void SystemSDL::showCursor() {
    if (cursor_surf_ != NULL) {
        cursor_visible_ = true;
    } else {
        // Custom cursor surface doesn't
        // exists so use the default SDL Cursor
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void SystemSDL::useMenuCursor() {
    update_cursor_ = true;
    cursor_rect_.x = cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 0;
}

void SystemSDL::usePointerCursor() {
    update_cursor_ = true;
    cursor_rect_.x = 24;
    cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 0;
}

void SystemSDL::useTargetCursor() {
    update_cursor_ = true;
    cursor_rect_.x = 48;
    cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 10;
}

void SystemSDL::usePickupCursor() {
    update_cursor_ = true;
    cursor_rect_.x = 0;
    cursor_rect_.y = 24;
    cursor_hs_x_ = cursor_hs_y_ = 2;
}
