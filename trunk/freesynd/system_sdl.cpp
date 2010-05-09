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

#include "screen.h"
#include "system_sdl.h"
#include "app.h"
#include "audio.h"
#include "config.h"
#include "file.h"

#include "SDL_image.h"

extern bool want_fullscreen;
SDL_Joystick *joy = NULL;

const int SystemSDL::CURSOR_WIDTH = 24;

SystemSDL::SystemSDL(int depth)
:depth_(depth)
{
    if (SDL_Init(SDL_INIT_VIDEO
#ifdef GP2X
                 | SDL_INIT_JOYSTICK
#endif
        ) < 0) {
        printf("Critical error, SDL could not be initialized!");
        exit(1);
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

#ifdef HAVE_SDL_MIXER
    Audio::init();
#endif

    // TODO: maybe use double buffering?
#ifdef GP2X
    screen_surf_ = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
    temp_surf_ =
        SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 8, 0, 0, 0, 0);
#else
    screen_surf_ =
        SDL_SetVideoMode(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, depth,
                         SDL_DOUBLEBUF | SDL_HWSURFACE | (want_fullscreen ?
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
    if ( (initted&sdl_img_flags) != sdl_img_flags) {
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
}

SystemSDL::~SystemSDL()
{
    SDL_FreeSurface(temp_surf_);
    SDL_FreeSurface(cursor_surf_);

#ifdef HAVE_SDL_MIXER
    Audio::quit();
#endif

    // Destroy SDL_Image Lib
    IMG_Quit();

    SDL_Quit();
}

void SystemSDL::updateScreen()
{
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

void SystemSDL::handleEvents()
{
    static SDL_Event event;
    Key key;
    KeyMod keyMod;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            key = static_cast < Key > (event.key.keysym.sym);
            keyMod = static_cast < KeyMod > (event.key.keysym.mod);
            g_App.keyEvent(key, keyMod, event.type == SDL_KEYDOWN);
            break;
#ifdef GP2X
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            key = static_cast < Key > (event.jbutton.button);
            keyMod = KMD_NONE;
            g_App.keyEvent(key, keyMod, event.type == SDL_JOYBUTTONDOWN);
            break;
#endif

        case SDL_MOUSEBUTTONUP:
            g_App.mouseUpEvent(event.button.x, event.button.y,
                               event.button.button);
            break;

        case SDL_MOUSEBUTTONDOWN:
            g_App.mouseDownEvent(event.button.x, event.button.y,
                                 event.button.button);
            break;

        case SDL_MOUSEMOTION:
            update_cursor_ = true;
            cursor_x_ = event.motion.x;
            cursor_y_ = event.motion.y;
            g_App.mouseMotionEvent(event.motion.x, event.motion.y,
                                   event.motion.state);
            break;

        case SDL_QUIT:
            g_App.quit();
            break;

        default:
            break;
        }
    }
}

void SystemSDL::delay(int msec)
{
    SDL_Delay(msec);
}

bool like(int a, int b)
{
    return a == b || a == b - 1 || a == b + 1;
}

void SystemSDL::setPalette6b3(const uint8 * pal, int cols)
{
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

void SystemSDL::setPalette8b3(const uint8 * pal, int cols)
{
    static SDL_Color palette[256];

    for (int i = 0; i < cols; ++i) {
        palette[i].r = pal[i * 3 + 0];
        palette[i].g = pal[i * 3 + 1];
        palette[i].b = pal[i * 3 + 2];
    }

    SDL_SetColors(temp_surf_, palette, 0, cols);
}

void SystemSDL::setColor(uint8 index, uint8 r, uint8 g, uint8 b)
{
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
 * \returns False if the loading has failed. If it's the case, 
 * cursor_surf_ will be NULL.
 */
bool SystemSDL::loadCursorSprites()
{
    cursor_rect_.w = cursor_rect_.h = CURSOR_WIDTH;

    cursor_surf_ = IMG_Load(File::fileFullPath("cursors/cursors.png", false));

    if (!cursor_surf_) {
        printf("Cannot load cursors image: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

void SystemSDL::hideCursor()
{
    if (cursor_surf_ != NULL) {
        cursor_visible_ = false;
    } else {
        // Custom cursor surface doesn't
        // exists so use the default SDL Cursor
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void SystemSDL::showCursor()
{
    if (cursor_surf_ != NULL) {
        cursor_visible_ = true;
    } else {
        // Custom cursor surface doesn't
        // exists so use the default SDL Cursor
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void SystemSDL::useMenuCursor()
{
    update_cursor_ = true;
    cursor_rect_.x = cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 0;
}

void SystemSDL::usePointerCursor()
{
    update_cursor_ = true;
    cursor_rect_.x = 24;
    cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 0;
}

void SystemSDL::useTargetCursor()
{
    update_cursor_ = true;
    cursor_rect_.x = 48;
    cursor_rect_.y = 0;
    cursor_hs_x_ = cursor_hs_y_ = 10;
}

void SystemSDL::usePickupCursor()
{
    update_cursor_ = true;
    cursor_rect_.x = 0;
    cursor_rect_.y = 24;
    cursor_hs_x_ = cursor_hs_y_ = 2;
}
