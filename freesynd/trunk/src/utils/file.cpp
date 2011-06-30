/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2011  Joey Parrish  <joey.parrish@gmail.com>         *
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
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "file.h"
#include "dernc.h"
#include "log.h"
#include "portablefile.h"

char File::_path[240] = "./data/";

std::string File::homePath_ = "./";

/*!
 * The methods returns a new pointer to a string composed of the root path
 * and given file name. The absolute file name cannot exceed 256 characters.
 * No control is made on the result format or file existence.
 * \param filename The relative path to a file (must be null terminated).
 * \param uppercase If true, the resulting string will uppercased.
 * \see setPath()
 */
const char *File::fileFullPath(const char *filename, bool uppercase) {
    static char buf[256];

    memset(buf, 0, 256);
    // We're sure that _path is less than 255 because setPath method
    // controls its length
    strcpy(buf, _path);

    int start = strlen(_path);
    // Add 1 because strlen excludes the null caracter
    int end = start + strlen(filename) + 1;

    for (int i = start; i < end; ++i) {
        buf[i] =
            uppercase ? toupper(filename[i - start]) :
            tolower(filename[i - start]);
    }

    return buf;
}

void File::getFullPathForSaveSlot(int slot, std::string &path) {
    path.erase();

    path.append(homePath_).append("save");
#ifdef _WIN32
    path.append("\\");
#else
    path.append("/");
#endif

    std::ostringstream out;
    if (slot < 10) {
        out << "0";
    }
    out << slot << ".fsg";

    path.append(out.str());
}

/*!
 * \return NULL if file cannot be read.
 */
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
        size_t  n = fread(mem, 1, filesize, fp);
        if (n == 0) {
            FSERR(Log::k_FLG_IO, "File", "loadFileToMem", ("WARN: File '%s' (using path: '%s') is empty\n",
               filename, _path));
         }
        fclose(fp);
        return mem;
    }

    // If we're here, there's a problem
    FSERR(Log::k_FLG_IO, "File", "loadFileToMem", ("ERROR: Couldn't open file '%s' (using path: '%s')\n",
               filename, _path));

    filesize = 0;
    return NULL;
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
        LOG(Log::k_FLG_IO, "File", "setPath", ("Changing path to: '%s'\n", path));
        strcpy((char *) _path, path);
    }
    else {
        FSERR(Log::k_FLG_IO, "File", "setPath", ("Warning: path '%s' too long, using CWD\n", _path));
        strcpy((char *) _path, "./");
    }
}

void File::setHomePath(const char *path) {
    if (path) {
        homePath_.assign(path);
 
        LOG(Log::k_FLG_IO, "File", "setHomePath", ("set home path to %s", path));
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
                FSERR(Log::k_FLG_IO, "File", "loadFile", ("Error loading file: %s!", rnc::errorString(result)));
                filesize = 0;
                delete[] buffer;
            }

            if (result != filesize) {
                FSERR(Log::k_FLG_IO, "File", "loadFile", ("Uncompressed size mismatch for file %s!\n", filename));
                filesize = 0;
                delete[] buffer;
            }

            return buffer;
        }
    }
    return data;
}

/*!
 * Returns the list of names to display in load/save menu.
 * \param files 
 */
void File::getGameSavedNames(std::vector<std::string> &files) {
    std::string savePath(homePath_);
    savePath.append("save");

#ifdef _WIN32
    SECURITY_ATTRIBUTES	sa;
    WIN32_FIND_DATA File;
    HANDLE hSearch;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
    if (CreateDirectory(savePath.c_str(), &sa) == ERROR_PATH_NOT_FOUND) {
        FSERR(Log::k_FLG_IO, "File", "getGameSavedNames", ("Cannot create save directory in %s", homePath_.c_str()))
        return;
    }

    savePath.append("/*.fsg");
    hSearch = FindFirstFile(savePath.c_str(), &File);
    if (hSearch != INVALID_HANDLE_VALUE) {
        do {
            std::string filename(File.cFileName);
            size_t extPos = filename.find_last_of('.');
            std::string name = filename.substr(0, extPos);
            std::istringstream iss( name );
            int index;
            iss >> index;
            if (index < 10) {
                PortableFile infile;
                getFullPathForSaveSlot(index, filename);
                infile.open_to_read(filename.c_str());

                if (infile) {
                    // FIXME: detect original game saves
                    // Read version first
                    unsigned char vMaj = infile.read8();
                    unsigned char vMin = infile.read8();
                    format_version v(vMaj, vMin);
                    // Read slot name
                    if (v == 0x0100) {
                        files[index] = infile.read_string(25, true);
                    } else if (v == 0x0101) {
                        files[index] = infile.read_string(31, true);
                    }
                }
            }
        } while (FindNextFile(hSearch, &File));
    }

#else
	DIR * rep = opendir(savePath.c_str());
	struct dirent * ent;

	if (rep == NULL) {
		if (mkdir(savePath.c_str(), 0777) == -1) {  // Create the directory
        	FSERR(Log::k_FLG_IO, "File", "getGameSavedNames", ("Cannot create save directory in %s", homePath_.c_str()))
        	return;
     	}
		rep = opendir(savePath.c_str());
	}

	while ((ent = readdir(rep)) != NULL) {
		std::string filename(ent->d_name);
        size_t extPos = filename.find_last_of('.');
		if (extPos == std::string::npos) {
			continue;
		}
        std::string name = filename.substr(0, extPos);
		std::string ext = filename.substr(extPos);
		if (ext.compare(".fsg") == 0) {
        	std::istringstream iss( name );
	        int index;
	        iss >> index;
	        if (index < 10) {
	            PortableFile infile;
	            getFullPathForSaveSlot(index, filename);
	            infile.open_to_read(filename.c_str());

	            if (infile) {
	                // Read version first
	                unsigned char vMaj = infile.read8();
	                unsigned char vMin = infile.read8();
                    format_version v(vMaj, vMin);
	                // Read slot name
                    if (v == 0x0100) {
	                    files[index] = infile.read_string(25, true);
                    } else if (v == 0x0101) {
	                    files[index] = infile.read_string(31, true);
	                }
	            }
		}
        }
    }

	closedir(rep);

#endif
}
