/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
#include <assert.h>
#include "modmanager.h"

ModManager::ModManager()
{
}

ModManager::~ModManager() {
    destroy();
}

void ModManager::destroy() {
    // Delete all mods from the cache
    for (unsigned int i = 0; i != preFetch_.size(); ++i) {
       delete preFetch_[i];
    }

    // Delete all available mods 
    for (unsigned int i = 0; i < mods_.size(); i++) {
        delete mods_.get(i);
    }

    preFetch_.clear();
    mods_.clear();
}

void ModManager::reset() {
    destroy();

    Mod::EModType types[] = {Mod::MOD_LEGS, Mod::MOD_ARMS, Mod::MOD_CHEST,
        Mod::MOD_HEART, Mod::MOD_EYES, Mod::MOD_BRAIN};
    
    for (int t=0; t<6; t++) {
        enableMod(types[t], Mod::MOD_V1);
    }
}

void ModManager::cheatEnableAllMods() {
    Mod::EModType types[] = {Mod::MOD_LEGS, Mod::MOD_ARMS, Mod::MOD_CHEST,
        Mod::MOD_HEART, Mod::MOD_EYES, Mod::MOD_BRAIN};
    Mod::EModVersion versions[] = {Mod::MOD_V1, Mod::MOD_V2, Mod::MOD_V3};

    for (int t=0; t<6; t++) {
        for (int v=0; v<3; v++) {
            enableMod(types[t], versions[v]);
        }
    }
}

void ModManager::enableMod(Mod::EModType mt, Mod::EModVersion ver) {
    // First check if mod is not already available
    for (unsigned i = 0; i != mods_.size(); ++i) {
        if (mt == mods_.get(i)->getType() && ver == mods_.get(i)->getVersion())
            return;
    }

    // Then get mod
    Mod *pMod = getMod(mt, ver);

    // removes it from cache
    for (std::vector < Mod * >::iterator it = preFetch_.begin();
         it != preFetch_.end(); it++) {
             if (pMod == *it) {
                preFetch_.erase(it);
                break;
             }
    }

    // make it available
    for (unsigned i = 0; i != mods_.size(); ++i) {
        Mod *pOther = mods_.get(i);
        if (pMod->getType() < pOther->getType()) {
            // The new mod is inserted in the order of the type and version
            mods_.insertAt(i, pMod);
            return;
        }
    }
    // If we're here, it's because the new mod comes at the end of the list
    mods_.add(pMod);
}

Mod *ModManager::getMod(Mod::EModType mt, Mod::EModVersion ver) {
    // Search in prefetched mods first
    for (unsigned int i = 0; i < preFetch_.size(); i++) {
        if (mt == preFetch_[i]->getType() && ver == preFetch_[i]->getVersion())
            return preFetch_[i];
    }

    // Then search in available mods
    for (unsigned i = 0; i != mods_.size(); ++i) {
        if (mt == mods_.get(i)->getType() && ver == mods_.get(i)->getVersion())
            return mods_.get(i);
    }

    // Mod was not found so loads it
    Mod *pMod = loadMod(mt, ver);
    // Stores it in cache
    preFetch_.push_back(pMod);
    
    return pMod;
}

Mod *ModManager::loadMod(Mod::EModType mt, Mod::EModVersion ver) {
    if (mt == Mod::MOD_LEGS) {
        if (ver == Mod::MOD_V1) {
            return new Mod("LEGS V1", Mod::MOD_LEGS, Mod::MOD_V1, 6000,
                        "REPLACEMENT\nMETAL LEGS.\nSERVO ASSISTED\nHYDRAULICS\nALLOW QUICKER\nMOVEMENT.",
                        33, 37);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("LEGS V2", Mod::MOD_LEGS, Mod::MOD_V2, 30000,
                      "PLASTEEL LEGS.\nLIGHTER AND\nTOUGHER THAN\nMETAL LEGS.\nENHANCED\nHYDRAULIC\nSYSTEM AND\nBALANCE\nCONTROL.",
                      34, 38);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("LEGS V3", Mod::MOD_LEGS, Mod::MOD_V3, 60000,
                      "CYBERMESH LEGS.\nPLASTEEL CORE\nWITH SYNTHETIC\nMUSCLE FIBRE.\nSUPERB\nRESPONSE AND\nBALANCE\nCOUPLED WITH\nHIGH SPEED.",
                      35, 39);
        }
    } else if (mt == Mod::MOD_ARMS) {
        if (ver == Mod::MOD_V1) {
            return new
                  Mod("ARMS V1", Mod::MOD_ARMS, Mod::MOD_V1, 5000,
                      "REPLACEMENT\nMETAL LEGS.\nSERVO ASSISTED\nHYDRAULICS\nALLOW THE\nCARRYING OF\nHEAVIER ITEMS.",
                      41, 45);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("ARMS V2", Mod::MOD_ARMS, Mod::MOD_V2, 25000,
                      "PLASTEEL ARMS.\nLIGHTER AND\nTOUGHER THAN\nMETAL ARMS.\nENHANCED\nHYDRAULIC\nSYSTEM AND\nPRESSURE\nCONTROL.",
                      42, 46);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("ARMS V3", Mod::MOD_ARMS, Mod::MOD_V3, 50000,
                      "CYBERMESH ARMS.\nPLASTEEL CORE\nWITH SYNTHETIC\nMUSCLE FIBRE.\nEXCELLENT\nTACTILE\nCONTROL AND\nWEIGHT LOADING.",
                      43, 47);
        }
    } else if (mt == Mod::MOD_CHEST) {
        if (ver == Mod::MOD_V1) {
            return new
                  Mod("CHEST V1", Mod::MOD_CHEST, Mod::MOD_V1, 8000,
                      "METAL CHEST\nCAGE. ALL\nINTERNAL\nORGANS ARE\nSHIELDED BY\nHEAVY METAL\nCASING.",
                      48, 51);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("CHEST V2", Mod::MOD_CHEST, Mod::MOD_V2, 40000,
                      "PLASTEEL CHEST\nCAGE. LIGHTER\nAND TOUGHER\nTHAN THE METAL\nVERSION WITH\nBETTER\nMOBILITY.",
                      49, 52);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("CHEST V3", Mod::MOD_CHEST, Mod::MOD_V3, 80000,
                      "CYBERMESH\nCHEST CAGE\nWITH POWER\nSHIELDING. ALL\nINTERNAL\nORGANS ARE\nSHIELDED BY A\nLOW POWER\nFIELD\nGENERATOR.",
                      50, 53);
        }
    } else if (mt == Mod::MOD_HEART) {
        if (ver == Mod::MOD_V1) {
            return new
                  Mod("HEART V1", Mod::MOD_HEART, Mod::MOD_V1, 4500,
                      "HEART\nSTIMULATOR.\nMONITORS AND\nMAINTAINS THE\nHEART RATE OF\nTHE INDIVIDUAL.",
                      54);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("HEART V2", Mod::MOD_HEART, Mod::MOD_V2, 22500,
                      "HEART\nACCELERATOR\nAND MONITOR.\nHEART RATE IS\nALMOST DOUBLED\nALLOWING THE\nQUICKER\nDISTRIBUTION\nOF HORMONES.",
                      55);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("HEART V3", Mod::MOD_HEART, Mod::MOD_V3, 45000,
                      "CYBERNETIC\nHEART. POWERFUL\nPUMP WITH IN\nBUILT SYSTEM\nANALYSIS AND\nOVERIDE\nFACILITIES.\nTRIPLES OUTPUT\nOF A STANDARD\nHUMAN HEART.",
                      56);
        }
    } else if (mt == Mod::MOD_EYES) {
        if (ver == Mod::MOD_V1) {
            return new
                  Mod("EYES V1", Mod::MOD_EYES, Mod::MOD_V1, 4500,
                      "VISION\nENHANCER WITH\nLIMITED ZOOM\nFACILITY.\nALLOWS NEAR\nPERFECT SIGHT\nEVEN AT NIGHT.",
                      60);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("EYES V2", Mod::MOD_EYES, Mod::MOD_V2, 22500,
                      "ENHANCED MODEL\nOF THE V1\nINCORPORATING\nA TARGET\nAQUISITION\nFACILITY AND\nRANGE FINDER.",
                      61);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("EYES V3", Mod::MOD_EYES, Mod::MOD_V3, 45000,
                      "LATEST\nNEUROCYBERNETIC\nIMPLANTS. GIVES\nPERFECT NIGHT\nVISION WITH\nTHE AID OF\nTARGETERS\nRANGE FINDERS\nAND THREAT\nIDENTIFIER.",
                      62);
        }
    } else if (mt == Mod::MOD_BRAIN) {
        if (ver == Mod::MOD_V1) {
            return new
                  Mod("BRAIN V1", Mod::MOD_BRAIN, Mod::MOD_V1, 4500,
                      "NEURAL\nENHANCER THAT\nSPEEDS UP\nBRAIN STEM\nRESPONSE TIME.",
                      57);
        } else if (ver == Mod::MOD_V2) {
            return new
                  Mod("BRAIN V2", Mod::MOD_BRAIN, Mod::MOD_V2, 22500,
                      "NEURO CNS\nDEVICE THAT\nINTERCEPTS\nMOST LOW LEVEL\nFUNCTIONS\nLEAVING THE\nBRAIN FREE TO\nDEAL WITH HIGH\nEND DESICIONS.",
                      58);
        } else if (ver == Mod::MOD_V3) {
            return new
                  Mod("BRAIN V3", Mod::MOD_BRAIN, Mod::MOD_V3, 45000, 
                      "CEREBRAL\nMULTIPLIER\nCONNECTED TO A\nCOMPUNET\nGIVING ACCESS\nTO A MASS OF\nTACTICAL INFO.\nADAPTED FROM\nTHE MILITARY\nVERSION.",
                      59);
        }
    }

    return NULL;
}

bool ModManager::saveToFile(PortableFile &file) {
    file.write32(mods_.size());

    for(unsigned int i=0; i<mods_.size(); i++) {
        Mod *pMod = mods_.get(i);
        file.write32(pMod->getType());
        file.write32(pMod->getVersion());
    }
    return true;
}

bool ModManager::loadFromFile(PortableFile &infile, const format_version& v) {

    int nbMods = infile.read32();

    for (int i=0;i<nbMods; i++) {
        int type = infile.read32();
        Mod::EModType mt = Mod::Unknown;
        Mod::EModVersion mv = Mod::MOD_V1;
        switch (type) {
            case 0: mt = Mod::MOD_LEGS;break;
            case 1: mt = Mod::MOD_ARMS;break;
            case 2: mt = Mod::MOD_CHEST;break;
            case 3: mt = Mod::MOD_HEART;break;
            case 4: mt = Mod::MOD_EYES;break;
            case 5: mt = Mod::MOD_BRAIN;break;
            default: mt = Mod::Unknown;
        }

        int ver = infile.read32();
        switch (ver) {
            case 0: mv = Mod::MOD_V1;break;
            case 1: mv = Mod::MOD_V2;break;
            case 2: mv = Mod::MOD_V3;break;
            default: mv = Mod::MOD_V1;
        }
        // enable weapon
        enableMod(mt, mv);
    }
    return true;
}
