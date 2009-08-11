/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

extern bool want_fullscreen;
SDL_Joystick *joy = NULL;

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
}

SystemSDL::~SystemSDL()
{
    SDL_FreeSurface(temp_surf_);
#ifdef HAVE_SDL_MIXER
    Audio::quit();
#endif
    SDL_Quit();
}

void SystemSDL::updateScreen()
{
    if (g_Screen.dirty()) {
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

void SystemSDL::hideCursor()
{
    // ya gotta wonder why SDL_ShowCursor is so complicated.
    if (SDL_ShowCursor(-1))
        SDL_ShowCursor(SDL_DISABLE);
}

void SystemSDL::showCursor()
{
    if (!SDL_ShowCursor(-1))
        SDL_ShowCursor(SDL_ENABLE);
}

static SDL_Cursor *origCursor = NULL;

void SystemSDL::usePointerCursor()
{
    SDL_SetCursor(origCursor);
}

void SystemSDL::useTargetCursor()
{
    static SDL_Cursor *targetCursor = NULL;

    if (targetCursor == NULL) {
        int b = 0;
        int w = 12;
        int t = 255;
        // TODO: be nice to use the real sprite, but I can't find it.
        uint8 target[21 * 21] = {
            t, t, t, t, t, t, t, t, t, t, b, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, b, w, b, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, b, b, w, b, b, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, b, b, w, w, w, w, w, b, b, t, t, t, t, t, t,
            t, t, t, t, t, b, w, w, b, b, w, b, b, w, w, b, t, t, t, t, t,
            t, t, t, t, b, w, b, b, t, b, w, b, t, b, b, w, b, t, t, t, t,
            t, t, t, b, w, b, t, t, t, b, w, b, t, t, t, b, w, b, t, t, t,
            t, t, t, b, w, b, t, t, t, b, t, b, t, t, t, b, w, b, t, t, t,
            t, t, b, w, b, t, t, t, t, t, t, t, t, t, t, t, b, w, b, t, t,
            t, b, b, w, b, b, b, b, t, t, t, t, t, b, b, b, b, w, b, b, t,
            b, w, w, w, w, w, w, t, t, t, t, t, t, t, w, w, w, w, w, w, b,
            t, b, b, w, b, b, b, b, t, t, t, t, t, b, b, b, b, w, b, b, t,
            t, t, b, w, b, t, t, t, t, t, t, t, t, t, t, t, b, w, b, t, t,
            t, t, t, b, w, b, t, t, t, b, t, b, t, t, t, b, w, b, t, t, t,
            t, t, t, b, w, b, t, t, t, b, w, b, t, t, t, b, w, b, t, t, t,
            t, t, t, t, b, w, b, b, t, b, w, b, t, b, b, w, b, t, t, t, t,
            t, t, t, t, t, b, w, w, b, b, w, b, b, w, w, b, t, t, t, t, t,
            t, t, t, t, t, t, b, b, w, w, w, w, w, b, b, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, b, b, w, b, b, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, b, w, b, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, b, t, t, t, t, t, t, t, t, t, t,
        };

        uint8 data[4 * 32], mask[4 * 32];
        memset(data, 0, sizeof(data));
        memset(mask, 0, sizeof(mask));
        for (int j = 0; j < 32; j++)
            for (int i = 0; i < 32; i++) {
                if (j < 6 || i < 6 || j >= 26 || i >= 26)
                    continue;
                if (target[(i - 6) + (j - 6) * 21] == t)
                    continue;
                if (target[(i - 6) + (j - 6) * 21] == b)
                    data[(i / 8) + j * 4] |= 128 >> (i % 8);
                mask[(i / 8) + j * 4] |= 128 >> (i % 8);
            }

        if (origCursor == NULL)
            origCursor = SDL_GetCursor();
        targetCursor =
            SDL_CreateCursor(data, mask, 32, 32, 6 + 11, 6 + 11);
    }

    SDL_SetCursor(targetCursor);
}

void SystemSDL::usePickupCursor()
{
    static SDL_Cursor *pickupCursor = NULL;

    if (pickupCursor == NULL) {
        int b = 0;
        int w = 12;
        int t = 255;
        // TODO: be nice to use the real sprite, but I can't find it.
        uint8 pickup[21 * 21] = {
            t, t, t, t, b, b, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, b, w, w, b, b, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, b, b, w, w, b, b, t, t, t, t, t, t, t, t, t, t, t,
            t, b, t, t, t, t, b, b, w, w, b, b, t, t, t, t, t, t, t, t, t,
            b, w, b, t, t, t, t, t, b, b, w, w, b, b, t, t, t, t, t, t, t,
            b, w, b, t, t, t, t, t, t, t, b, w, w, w, b, t, t, t, t, t, t,
            t, b, w, b, t, t, t, t, t, t, t, b, w, w, w, b, t, t, t, t, t,
            t, b, w, b, t, t, t, t, t, t, t, t, b, w, w, b, t, t, t, t, t,
            t, t, b, w, b, t, t, t, t, t, t, b, w, w, w, b, t, t, t, t, t,
            t, t, b, w, b, t, t, t, t, t, b, b, b, w, w, b, t, t, t, t, t,
            t, t, t, b, w, b, t, t, t, b, w, w, w, w, w, b, t, t, t, t, t,
            t, t, t, b, w, w, b, t, b, b, w, w, b, w, b, b, t, t, t, t, t,
            t, t, t, t, b, w, w, b, w, b, w, b, b, w, b, t, t, t, t, t, t,
            t, t, t, t, b, w, w, w, w, w, w, w, w, w, b, t, t, t, t, t, t,
            t, t, t, t, t, b, w, w, w, w, w, b, b, b, t, t, t, t, t, t, t,
            t, t, t, t, t, b, b, b, b, b, b, b, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
            t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t, t,
        };

        uint8 data[4 * 32], mask[4 * 32];
        memset(data, 0, sizeof(data));
        memset(mask, 0, sizeof(mask));
        for (int j = 0; j < 32; j++)
            for (int i = 0; i < 32; i++) {
                if (j < 6 || i < 6 || j >= 26 || i >= 26)
                    continue;
                if (pickup[(i - 6) + (j - 6) * 21] == t)
                    continue;
                if (pickup[(i - 6) + (j - 6) * 21] == b)
                    data[(i / 8) + j * 4] |= 128 >> (i % 8);
                mask[(i / 8) + j * 4] |= 128 >> (i % 8);
            }

        if (origCursor == NULL)
            origCursor = SDL_GetCursor();
        pickupCursor =
            SDL_CreateCursor(data, mask, 32, 32, 6 + 11, 6 + 11);
    }

    SDL_SetCursor(pickupCursor);
}
