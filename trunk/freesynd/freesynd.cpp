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

#include <memory>

#include <cstdio>
#include <cstdlib>

#include "common.h"
#include "file.h"
#include "app.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#ifdef SYSTEM_SDL
#ifdef _WIN32
#undef main
#else
#include <SDL_main.h>           //This is required on OSX for remapping of main()
#endif
#endif

//#define CHEAP_LEAK_DETECTION

#ifdef CHEAP_LEAK_DETECTION

#define BLKSZ 65536
int blocks[BLKSZ];

void cleanupLeakDetection()
{
    int unfreed = 0;
    int mfreed = 0;
    for (int i = 0; i < BLKSZ; i++)
        if (blocks[i] > 0) {
            unfreed++;
            printf("%x ", i);
        } else if (blocks[i] < 0) {
            mfreed++;
            printf("m%x ", i);
        }
    printf("\n%i unfreed blocks\n", unfreed);
    printf("%i multiple freed blocks\n", mfreed);
}

void initLeakDetection()
{
    int ginits = 0;
    for (int i = 0; i < BLKSZ; i++)
        if (blocks[i] > 0)
            ginits++;
        else
            assert(blocks[i] == 0);
    printf("%i blocks allocated before main\n", ginits);
    atexit(&cleanupLeakDetection);
}

void *operator  new(size_t n)
{
    void *p = malloc(n);
    int x = ((int) p) % BLKSZ;
    if (x == 0x1ffff) {         // plug a reported hash in here and set a breakpoint
        printf("boo\n");
    }
    blocks[x]++;
    return p;
}

void operator  delete(void *p)
{
    if (p == 0) {
        printf("freeing NULL\n");
        return;
    }
    int x = ((int) p) % BLKSZ;
    if (x == 0x1ffff) {         // plug a reported hash in here and set a breakpoint
        printf("mboo\n");
    }
    blocks[x]--;
    free(p);
}
#endif

bool want_fullscreen = false;

void print_usage()
{
    printf
        ("usage: freesynd [-h|--help] [-p|--path path-to-data] [-f|--full-screen]\n");
}

int main(int argc, char *argv[])
{
#ifdef CHEAP_LEAK_DETECTION
    initLeakDetection();
#endif

#ifdef _WIN32
    srand(GetTickCount());
#else
    srand((unsigned) time(NULL));
#endif
    for (int i = 1; i < argc; ++i) {
        if (0 == strcmp("-p", argv[i]) || 0 == strcmp("--path", argv[i])) {
            if (argv[i + 1])
                File::setPath(argv[i + 1]);
            i++;
        }
        if (0 == strcmp("-f", argv[i])
            || 0 == strcmp("--full-screen", argv[i])) {
            want_fullscreen = true;
        }
        if (0 == strcmp("-h", argv[i]) || 0 == strcmp("--help", argv[i])) {
            print_usage();
            return 0;
        }
    }

    std::auto_ptr < App > app(new App());

    app->run();

    return 0;
}
