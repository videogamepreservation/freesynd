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

#include <memory>

#include <cstdio>
#include <cstdlib>

#include "common.h"
#include "utils/file.h"
#include "app.h"
#include "utils/log.h"
#include "default_ini.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
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

void cleanupLeakDetection() {
    int unfreed = 0;
    int mfreed = 0;

    for (int i = 0; i < BLKSZ; i++) {
        if (blocks[i] > 0) {
            unfreed++;
            printf("%x ", i);
        }
        else if (blocks[i] < 0) {
            mfreed++;
            printf("m%x ", i);
        }
    }

    printf("\n%i unfreed blocks\n", unfreed);
    printf("%i multiple freed blocks\n", mfreed);
}

void initLeakDetection() {
    int ginits = 0;

    for (int i = 0; i < BLKSZ; i++) {
        if (blocks[i] > 0)
            ginits++;
        else
            assert(blocks[i] == 0);
    }

    printf("%i blocks allocated before main\n", ginits);
    atexit(&cleanupLeakDetection);
}

void *operator new(size_t n) {
    void *p = malloc(n);
    int x = ((int) p) % BLKSZ;

    if (x == 0x1ffff) {         // plug a reported hash in here and set a breakpoint
        printf("boo\n");
    }

    blocks[x]++;
    return p;
}

void operator delete(void *p) {
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

void print_usage() {
    printf("usage: freesynd [-h|--help] [-p|--path path-to-data] [-f|--full-screen]\n");
}

int main(int argc, char *argv[]) {

#ifdef CHEAP_LEAK_DETECTION
    initLeakDetection();
#endif

#ifdef _WIN32
    srand(GetTickCount());
#else
    srand((unsigned) time(NULL));
#endif

    // This variable stores the index of the cheat code param on
    // the command line
    int cheatCodeIndex = -1;
    // If different from -1, the game will start directly on mission
    // with the given id
    int  start_mission = -1;

    for (int i = 1; i < argc; ++i) {

#ifdef _DEBUG
        // This parameter is used in debug phase to accelerate the starting
        // of a game and to jump directly to a mission
        // Note : the argument is the index of the block in the structure g_MissionNumbers 
        // as defined in briefmenu.cpp and not the mission number itself.
        if (0 == strcmp("-m", argv[i]) || 0 == strcmp("--mission", argv[i])) {
            int mission = atoi(argv[i + 1]);
            if (mission >= 0 && mission < 50) {
                start_mission = mission;
            }
            i++;
        }

        // This parameter is used to specify cheat codes on command line.
        // You can specify multiple codes using the ':' as a separator.
        // See available cheat codes in App::setCheatCode()
        // exemple -c "DO IT AGAIN:NUK THEM"
        if (0 == strcmp("-c", argv[i]) || 0 == strcmp("--cheat", argv[i])) {
            cheatCodeIndex = i + 1;
            i++;
        }
#endif
    }

#ifdef _DEBUG
    // Initialize log
    Log::initialize(Log::k_FLG_ALL, "game.log");
#endif

    // The variable stores the directory where
    // Freesynd configuration file should be
    std::string confPath;

#ifdef _WIN32
    // Under windows config file is in the same directory
    // that the freesynd.exe file
    confPath.append(argv[0]);
    size_t pos = confPath.find_last_of('\\');
    confPath.erase(pos + 1);
#elif __APPLE__
    // on OS X, applications tend to store config files in this sort of path
    confPath.assign(getenv("HOME"));
    confPath.append("/Library/Application Support/FreeSynd/");
#else
    // Under unix it's in the user home directory
    confPath.assign(getenv("HOME"));
    confPath.append("/.freesynd/");
#endif

	// create dir if it does not exist
#ifdef _WIN32
    // FIXME: create directory on windows
#else
	DIR * rep = opendir(confPath.c_str());
	if (rep == NULL) {
		if (mkdir(confPath.c_str(), 0777) == -1) {
			FSERR(Log::k_FLG_IO, "Freesynd", "main", ("Cannot create home directory in %s", confPath.c_str()))
			return -1;
	 	}
	} else {
		closedir(rep);
	}
#endif
	// create the ini file if it doesn't exist.
	std::string iniPath = confPath + "freesynd.ini";
#ifdef _WIN32
    if (_access(iniPath.c_str(), 0) != 0) {
#else
	struct stat st;
	if (stat(iniPath.c_str(), &st)) {
#endif
		FILE *f = fopen(iniPath.c_str(), "w");
		if (!f) {
			FSERR(Log::k_FLG_IO, "Freesynd", "main", ("Cannot create default ini file at %s", iniPath.c_str()))
			return -1;
		}

		fprintf(f, "%s", embedded_default_ini);
		fclose(f);
	}
    
    LOG(Log::k_FLG_INFO, "Main", "main", ("Initializing application..."))
    std::auto_ptr<App> app(new App());

    if (app->initialize(confPath.c_str())) {
        // setting the cheat codes
        if (cheatCodeIndex != -1) {
            char s[50];
            strcpy(s, argv[cheatCodeIndex]);
            char *token = strtok(s, ":");
            while ( token != NULL ) {
                LOG(Log::k_FLG_INFO, "Main", "main", ("Cheat code activated : %s", token))
                app->setCheatCode(token);
                token = strtok(NULL, ":");
            }
        }

        LOG(Log::k_FLG_INFO, "Main", "main", ("Initializing application completed"))

        app->run(confPath.c_str(), start_mission);
    } else {
        LOG(Log::k_FLG_INFO, "Main", "main", ("Initializing application failed"))
    }

    // Destroy application
    app->destroy();

#ifdef _DEBUG
    // Close log
    Log::close();
#endif

    return 0;
}
