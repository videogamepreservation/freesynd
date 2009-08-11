/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
    mods_.reserve(18);
}

ModManager::~ModManager()
{
    for (unsigned int i = 0; i < mods_.size(); i++)
        delete mods_[i];
}

void ModManager::loadMods()
{
    mods_.
        push_back(new
                  Mod("LEGS V1", 6000,
                      "REPLACEMENT\nMETAL LEGS.\nSERVO ASSISTED\nHYDRAULICS\nALLOW QUICKER\nMOVEMENT.",
                      5, 33, 37));
    mods_.
        push_back(new
                  Mod("ARMS V1", 5000,
                      "REPLACEMENT\nMETAL LEGS.\nSERVO ASSISTED\nHYDRAULICS\nALLOW THE\nCARRYING OF\nHEAVIER ITEMS.",
                      4, 41, 45));
    mods_.
        push_back(new
                  Mod("CHEST V1", 8000,
                      "METAL CHEST\nCAGE. ALL\nINTERNAL\nORGANS ARE\nSHIELDED BY\nHEAVY METAL\nCASING.",
                      3, 48, 51));
    mods_.
        push_back(new
                  Mod("HEART V1", 4500,
                      "HEART\nSTIMULATOR.\nMONITORS AND\nMAINTAINS THE\nHEART RATE OF\nTHE INDIVIDUAL.",
                      2, 54));
    mods_.
        push_back(new
                  Mod("EYES V1", 4500,
                      "VISION\nENHANCER WITH\nLIMITED ZOOM\nFACILITY.\nALLOWS NEAR\nPERFECT SIGHT\nEVEN AT NIGHT.",
                      1, 60));
    mods_.
        push_back(new
                  Mod("BRAIN V1", 4500,
                      "NEURAL\nENHANCER THAT\nSPEEDS UP\nBRAIN STEM\nRESPONSE TIME.",
                      0, 57));

    mods_.
        push_back(new
                  Mod("LEGS V2", 30000,
                      "PLASTEEL LEGS.\nLIGHTER AND\nTOUGHER THAN\nMETAL LEGS.\nENHANCED\nHYDRAULIC\nSYSTEM AND\nBALANCE\nCONTROL.",
                      5, 34, 38));
    mods_.
        push_back(new
                  Mod("ARMS V2", 25000,
                      "PLASTEEL ARMS.\nLIGHTER AND\nTOUGHER THAN\nMETAL ARMS.\nENHANCED\nHYDRAULIC\nSYSTEM AND\nPRESSURE\nCONTROL.",
                      4, 42, 46));
    mods_.
        push_back(new
                  Mod("CHEST V2", 40000,
                      "PLASTEEL CHEST\nCAGE. LIGHTER\nAND TOUGHER\nTHAN THE METAL\nVERSION WITH\nBETTER\nMOBILITY.",
                      3, 49, 52));
    mods_.
        push_back(new
                  Mod("HEART V2", 22500,
                      "HEART\nACCELERATOR\nAND MONITOR.\nHEART RATE IS\nALMOST DOUBLED\nALLOWING THE\nQUICKER\nDISTRIBUTION\nOF HORMONES.",
                      2, 55));
    mods_.
        push_back(new
                  Mod("EYES V2", 22500,
                      "ENHANCED MODEL\nOF THE V1\nINCORPORATING\nA TARGET\nAQUISITION\nFACILITY AND\nRANGE FINDER.",
                      1, 61));
    mods_.
        push_back(new
                  Mod("BRAIN V2", 22500,
                      "NEURO CNS\nDEVICE THAT\nINTERCEPTS\nMOST LOW LEVEL\nFUNCTIONS\nLEAVING THE\nBRAIN FREE TO\nDEAL WITH HIGH\nEND DESICIONS.",
                      0, 58));

    mods_.
        push_back(new
                  Mod("LEGS V3", 60000,
                      "CYBERMESH LEGS.\nPLASTEEL CORE\nWITH SYNTHETIC\nMUSCLE FIBRE.\nSUPERB\nRESPONSE AND\nBALANCE\nCOUPLED WITH\nHIGH SPEED.",
                      5, 35, 39));
    mods_.
        push_back(new
                  Mod("ARMS V3", 50000,
                      "CYBERMESH ARMS.\nPLASTEEL CORE\nWITH SYNTHETIC\nMUSCLE FIBRE.\nEXCELLENT\nTACTILE\nCONTROL AND\nWEIGHT LOADING.",
                      4, 43, 47));
    mods_.
        push_back(new
                  Mod("CHEST V3", 80000,
                      "CYBERMESH\nCHEST CAGE\nWITH POWER\nSHIELDING. ALL\nINTERNAL\nORGANS ARE\nSHIELDED BY A\nLOW POWER\nFIELD\nGENERATOR.",
                      3, 50, 53));
    mods_.
        push_back(new
                  Mod("HEART V3", 45000,
                      "CYBERNETIC\nHEART. POWERFUL\nPUMP WITH IN\nBUILT SYSTEM\nANALYSIS AND\nOVERIDE\nFACILITIES.\nTRIPLES OUTPUT\nOF A STANDARD\nHUMAN HEART.",
                      2, 56));
    mods_.
        push_back(new
                  Mod("EYES V3", 45000,
                      "LATEST\nNEUROCYBERNETIC\nIMPLANTS. GIVES\nPERFECT NIGHT\nVISION WITH\nTHE AID OF\nTARGETERS\nRANGE FINDERS\nAND THREAT\nIDENTIFIER.",
                      1, 62));
    mods_.
        push_back(new
                  Mod("BRAIN V3", 45000,
                      "CEREBRAL\nMULTIPLIER\nCONNECTED TO A\nCOMPUNET\nGIVING ACCESS\nTO A MASS OF\nTACTICAL INFO.\nADAPTED FROM\nTHE MILITARY\nVERSION.",
                      0, 59));
}
