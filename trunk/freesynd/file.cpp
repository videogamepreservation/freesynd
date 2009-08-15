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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "file.h"
#include "dernc.h"

char File::_path[240] = "./data/";


const char *File::fileFullPath(const char *filename, bool uppercase) {
    static char buf[256];

    memset(buf, 0, 256);
    strcpy(buf, _path);

    int start = strlen(_path);
    int end = start + strlen(filename);

    for (int i = start; i < end; ++i) {
        buf[i] =
            uppercase ? toupper(filename[i - start]) :
            tolower(filename[i - start]);
    }

    return buf;
}

uint8 *File::loadFileToMem(const char *filename, int &filesize) {
    assert(filename);

    FILE *fp = fopen(fileFullPath(filename, false), "rb");      //try lowercase first
    if (!fp)                    //ok.. let's try uppercase
        fp = fopen(fileFullPath(filename, true), "rb");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        uint8 *mem = new uint8[filesize];
        fseek(fp, 0, SEEK_SET);
        fread(mem, 1, filesize, fp);
        fclose(fp);
        return mem;
    } else {
        printf("ERROR: Couldn't open file '%s' (using path: '%s')\n",
               filename, _path);
        exit(-1);
        //filesize = 0;
        //return NULL;
    }
}

FILE *File::loadTextFile(const char *filename) {
    assert(filename);

    FILE *fp = fopen(fileFullPath(filename, false), "r"); // try lowercase
    if (!fp)                    //ok.. let's try uppercase
        fp = fopen(fileFullPath(filename, true), "r");

    return fp;
}

void File::setPath(const char *path) {
    if (strlen(path) < 255) {
        printf("Changing path to: '%s'\n", path);
        strcpy((char *) _path, path);
    }
    else {
        printf("Warning: path '%s' too long, using CWD\n", _path);
        strcpy((char *) _path, "./");
    }
}

uint8 *File::loadFile(const char *filename, int &filesize) {
    uint8 *data = loadFileToMem(filename, filesize);
    if (data) {
        if (READ_BE_UINT32(data) == RNC_SIGNATURE) {    //File is RNC compressed
            filesize = rnc::unpackedLength(data);
            assert(filesize > 0);
            uint8 *buffer = new uint8[filesize];
            int result = rnc::unpack(data, buffer);
            delete[] data;

            if (result < 0) {
                printf("Error loading file: %s!\n",
                       rnc::errorString(result));
                filesize = 0;
                delete[] buffer;
            }

            if (result != filesize) {
                printf("Uncompressed size mismatch!\n");
                filesize = 0;
                delete[] buffer;
            }

            return buffer;
        }
    }
    return data;
}
