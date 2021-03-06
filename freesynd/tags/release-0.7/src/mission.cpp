/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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
#include <string.h>
#include <assert.h>
#include <string>

#include "gfx/screen.h"
#include "app.h"
#include "model/objectivedesc.h"
#include "utils/log.h"

Mission::Mission()
{
    status_ = RUNNING;
    memset(&level_data_, 0, sizeof(level_data_));
    memset(minimap_overlay_, 0, 128*128);
    
    mtsurfaces_ = NULL;
    mdpoints_ = NULL;
    mdpoints_cp_ = NULL;
    i_map_id_ = 0;
    p_map_ = NULL;
    min_x_= 0;
    min_y_ = 0;
    max_x_ = 0;
    max_y_ = 0;
    cur_objective_ = 0;
    p_minimap_ = NULL;
    players_group_id_ = 1;
    p_squad_ = new Squad();
}

Mission::~Mission()
{
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        delete vehicles_[i];
    for (unsigned int i = 0; i < peds_.size(); i++)
        delete peds_[i];
    for (unsigned int i = 0; i < weapons_.size(); i++)
        delete weapons_[i];
    for (unsigned int i = 0; i < sfx_objects_.size(); i++)
        delete sfx_objects_[i];
    for (unsigned int i = 0; i < prj_shots_.size(); i++)
        delete prj_shots_[i];
    for (unsigned int i = 0; i < statics_.size(); i++)
        delete statics_[i];
    for (unsigned int i = 0; i < objectives_.size(); i++)
        delete objectives_[i];
    clrSurfaces();

    if (p_minimap_) {
        delete p_minimap_;
    }

    if (p_squad_) {
        delete p_squad_;
    }

    listeners_.clear();
}

#define copydata(x, y) memcpy(&level_data_.x, levelData + y, sizeof(level_data_.x))

void Mission::objectiveMsg(std::string& msg) {
    if (cur_objective_ < objectives_.size()) {
        msg = objectives_[cur_objective_]->msg;
    } else {
        msg = "";
    }
}

bool Mission::loadLevel(uint8 * levelData)
{
    copydata(u01, 0);
    copydata(map, 6);
    copydata(offset_ref, 32774);
    copydata(people, 32776);
    copydata(cars, 56328);
    copydata(statics, 59016);
    copydata(weapons, 71016);
    copydata(sfx, 89448);
    copydata(scenarios, 97128);
    copydata(u09, 113512);
    copydata(mapinfos, 113960);
    copydata(objectives, 113974);
    copydata(u11, 114058);

    i_map_id_ = READ_LE_UINT16(level_data_.mapinfos.map);
#ifdef _DEBUG
    printf("map to load %X\n", i_map_id_);
#endif
    min_x_ = READ_LE_UINT16(level_data_.mapinfos.min_x) / 2;
    min_y_ = READ_LE_UINT16(level_data_.mapinfos.min_y) / 2;
    max_x_ = READ_LE_UINT16(level_data_.mapinfos.max_x) / 2;
    max_y_ = READ_LE_UINT16(level_data_.mapinfos.max_y) / 2;

    vehicles_.clear();

    // NOTE: Original objects data is based on offsets, but our objetcs are different
    // in size and are not in a single memory block, because of this indexes
    // in our data "arrays" are mirrored for object's position within original
    // array

    // indexes within vehicle array
    uint16 vindx[64];
    // indexes within peds array
    uint16 pindx[256];
    // contains indexes for driver's vehicle
    uint16 driverindx[256];
    uint16 windx[512];
    memset(vindx, 0xFF, 2*64);
    memset(pindx, 0xFF, 2*256);
    memset(driverindx, 0xFF, 2*256);
    memset(windx, 0xFF, 2*512);

#if 0
    // for hacking vehicles data
    char nameSv[256];
    sprintf(nameSv, "vehicles%02X.hex", i_map_id_);
    FILE *staticsFv = fopen(nameSv,"wb");
    if (staticsFv) {
        fwrite(level_data_.cars, 1, 42*64, staticsFv);
        fclose(staticsFv);
    }

#endif

    for (uint8 i = 0; i < 64; i++) {
        LEVELDATA_CARS & car = level_data_.cars[i];
        // car.sub_type 0x09 - train
        if (car.type == 0x0)
            continue;
        VehicleInstance *v =
            g_App.vehicles().loadInstance((uint8 *) & car, i_map_id_);
        if (v) {
            vindx[i] = vehicles_.size();
            vehicles_.push_back(v);
            if (car.offset_of_driver != 0 && ((car.offset_of_driver - 2) / 92 + 2) * 92
                == car.offset_of_driver) {
                driverindx[(car.offset_of_driver - 2) / 92] = vindx[i];
            }
        }
    }

    peds_.clear();

#if 0
    // for hacking peds data
    char nameSp[256];
    sprintf(nameSp, "peds%02X.hex", i_map_id_);
    FILE *staticsFp = fopen(nameSp,"wb");
    if (staticsFp) {
        fwrite(level_data_.people, 1, 256*92, staticsFp);
        fclose(staticsFp);
    }
#endif
#ifdef _DEBUG
    std::map <uint32, std::string> obj_ids;
    // NOTE: not very useful way of remembering "Who is who"
    obj_ids[0] = "Undefined";
    obj_ids[players_group_id_] = "Players Agents or Persuaded";
    obj_ids[2] = "Enemy Agents";
    obj_ids[3] = "Enemy Guards";
    obj_ids[4] = "Policemen";
    obj_ids[5] = "Civilians";
#endif
    ModOwner mods_enemy;
    // enemies get top version of mods
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_LEGS));
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_ARMS));
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_CHEST));
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_HEART));
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_EYES));
    mods_enemy.addMod(g_App.mods().getHighestVersion(Mod::MOD_BRAIN));
    
    for (uint16 i = 0; i < 256; i++) {
        //if (i == 40)
            //i = 40;
        LEVELDATA_PEOPLE & pedref = level_data_.people[i];
        if(pedref.type == 0x0 || pedref.desc == 0x0D || pedref.desc == 0x0C)
            continue;
        PedInstance *p =
            g_App.peds().loadInstance((uint8 *) & pedref, i_map_id_);
        if (p) {
#ifdef _DEBUG
            p->setDebugID(i);
#endif
            if (pedref.desc == 0x05) {
                if (driverindx[i] != 0xFFFF) {
                    p->putInVehicle(vehicles_[driverindx[i]],
                        PedInstance::pa_smUsingCar);
                    VehicleInstance *v = vehicles_[driverindx[i]];
                    assert(v);
                    if (v->hasDriver()) {
                        PedInstance *curr_driver = v->getDriver();
                        curr_driver->leaveVehicle();
                        curr_driver->putInVehicle(v, PedInstance::pa_smInCar);
                        v->setDriver(p);
                        v->setDriver(curr_driver);
                    } else
                        v->setDriver(p);
                } else {
                    uint16 vin = READ_LE_UINT16(pedref.offset_of_vehicle);
                    if (vin != 0) {
                        vin = (vin - 0x5C02) / 42; // 42 vehicle data size
                        vin = vindx[vin];
                        if (vin != 0xFFFF) {
                            VehicleInstance *v = vehicles_[vin];
                            v->setDriver(p);
                            if (v->isDriver(p))
                                p->putInVehicle(v, PedInstance::pa_smUsingCar);
                            else
                                p->putInVehicle(v, PedInstance::pa_smInCar);
                        }
                    }
                }
            }
            pindx[i] = peds_.size();
            peds_.push_back(p);
            if (i < 4) {
                p->setObjGroupID(players_group_id_);
                p->setObjGroupDef(PedInstance::og_dmAgent);
                p->addEnemyGroupDef(2);
                p->addEnemyGroupDef(3);
                p->setHostileDesc(PedInstance::pd_smArmed);
                p->setSightRange(7 * 256);
                // not in all missions our agents health is 16, this fixes it
                p->setHealth(16);
                p->setStartHealth(16);
                p->setBaseSpeed(256);
            } else if (i > 7) {
                unsigned int mt = p->getMainType();
                p->setObjGroupDef(mt);
                if (mt == PedInstance::og_dmAgent) {
                    p->setObjGroupID(2);
                    p->addEnemyGroupDef(1);
                    p->setBaseSpeed(256);
                    *((ModOwner *)p) = mods_enemy;
                } else if (mt == PedInstance::og_dmGuard) {
                    p->setObjGroupID(3);
                    p->addEnemyGroupDef(1);
                    p->setBaseSpeed(192);
                } else if (mt == PedInstance::og_dmPolice) {
                    p->setObjGroupID(4);
                    p->setHostileDesc(PedInstance::pd_smArmed);
                    p->setBaseSpeed(160);
                } else {
                    p->setObjGroupID(5);
                    // civilians and criminals
                    p->setBaseSpeed(128);
                }
                p->setSightRange(7 * 256);
                // TODO: set scenarios
                
                uint16 offset_start = READ_LE_UINT16(pedref.offset_scenario_start);
                uint16 offset_nxt = offset_start;
                VehicleInstance *v = p->inVehicle();
                bool not_in_vehicle = true;
                if (v)
                    not_in_vehicle = false;
                if (offset_start)
                    p->dropActQ();
//#define SHOW_SCENARIOS_DEBUG
#ifdef SHOW_SCENARIOS_DEBUG
                printf("=====\n");
#endif
                PedInstance::actionQueueGroupType as;
                as.group_desc = PedInstance::gd_mStandWalk;
                as.origin_desc = 1;
                // TODO : kenya map timeout scenario somewhere
                while (offset_nxt) {
                    // sc.type
                    // 1, 8 - walking
                    // 2 - vehicle
                    // 7, 9(repeat) - end marker
                    // 10(loop?)?5(has offset <?ped?>)?
                    // 11 - protected target reached destination(kenya)
                    LEVELDATA_SCENARIOS sc = level_data_.scenarios[offset_nxt / 8];
#ifdef SHOW_SCENARIOS_DEBUG
                    printf("sc.type = %i, nxt = %i\n", sc.type, offset_nxt / 8);
#endif
                    offset_nxt = READ_LE_UINT16(sc.next);
                    assert(offset_nxt != offset_start);

                    if (sc.tilex != 0 && sc.tiley != 0) {
                        PathNode pn(sc.tilex >> 1, sc.tiley >> 1, sc.tilez,
                            (sc.tilex & 0x01) << 7, (sc.tiley & 0x01) << 7);
                        if (sc.type == 0x08) {
                            p->createActQWait(as, 2000);
                            // no need for exclusive wait
                            as.actions.back().group_desc = PedInstance::gd_mStandWalk;
                        }
                        if (v)
                            p->createActQUsingCar(as, &pn, v);
                        else
                            p->createActQWalking(as, &pn, NULL, p->getDir(), 0, true);
                            //p->createActQWalking(as, &pn, NULL, -1);
                        if (!not_in_vehicle && offset_nxt == 0)
                            p->createActQResetActionQueue(as);
                    } else if (sc.type == 2) {
                        if (not_in_vehicle) {
                            uint16 bindx = READ_LE_UINT16(sc.offset_object);
                            // TODO: test all maps for objects other then vehicle
                            assert(bindx >= 0x5C02 && bindx < 0x6682);
                            bindx -= 0x5C02;
                            bindx /= 42;
                            if (vindx[bindx] != 0xFFFF) {
                                v = vehicles_[vindx[bindx]];
                                p->createActQGetInCar(as, v);
                            }
                        } else {
                            PathNode pn(v->tileX(), v->tileY(), v->tileZ(),
                                v->offX(), v->offY());
                            p->createActQUsingCar(as, &pn, v);
                        }
                    } else if (sc.type == 9) {
                        p->createActQResetActionQueue(as);
                    } else if (sc.type == 10) {
                        p->createActQWait(as, 5000);
                        if (offset_nxt == 0)
                            p->createActQResetActionQueue(as);
                    }
                }
                if (as.actions.size() != 0) {
                    as.main_act = as.actions.size() - 1;
                    p->addActQToQueue(as);
                }
#ifdef SHOW_SCENARIOS_DEBUG
                printf("+++++\n");
#endif
            } else if (i > 3 && i < 8) {
                p->setMap(-1);
                p->setHealth(-1);
                p->setIsIgnored(true);
                p->setStateMasks(PedInstance::pa_smUnavailable);
                p->setAgentIs(PedInstance::Agent_Non_Active);
            }
        }
    }

#if 0
    // for hacking map data
    char nameSmap[256];
    sprintf(nameSmap, "map%02X.hex", i_map_id_);
    FILE *staticsFmap = fopen(nameSmap,"wb");
    if (staticsFmap) {
        fwrite(level_data_.map.objs, 1, 32768, staticsFmap);
        fclose(staticsFmap);
    }

#endif
    // NOTE: this part transcodes original map overlay for minimap into
    // our representation, in original agent our/enemy is defined by ped offset
    // or by weapon offset - if weapon has owner we look into type/index of
    // owner to define our/enemy state; original map overlay is 16384x2
    // array(container), only using map size we can correctly use our
    // minimap_overlay_; our agent = 1, enemy agent = 2, tile doesn't have
    // ped = 0
    for (uint32 i = 0; i < (128*128); i++) {
        uint32 pin = READ_LE_UINT16(level_data_.map.objs + i * 2);
        if (pin >= 0x0002 && pin < 0x5C02) {
            if (pin >= 0x0002 && pin < 0x02e2) {
                minimap_overlay_[i] = 1;
            } else {
                pin = pindx[(pin - 2) / 92];
                if (pin != 0xFFFF && peds_[pin]->getMainType()
                    == PedInstance::m_tpAgent) {
                    minimap_overlay_[i] = 2;
                }
            }
        } else if (pin >= 0x9562 && pin < 0xDD62) {
            pin = (pin - 0x9562) / 36; // 36 = weapon data size
            LEVELDATA_WEAPONS & wref = level_data_.weapons[pin];
            if (wref.desc == 0x05) {
                pin = READ_LE_UINT16(wref.offset_owner);
                if (pin != 0) {
                    pin = (pin - 2) / 92; // 92 = ped data size
                    if (pin > 7) {
                        if (pindx[pin] != 0xFFFF
                            && peds_[pindx[pin]]->getMainType()
                            == PedInstance::m_tpAgent)
                        {
                            minimap_overlay_[i] = 2;
                        }
                    } else {
                        minimap_overlay_[i] = 1;
                    }
                }
            }
        }
    }

#if 0
    // for hacking statics data
    char nameSs[256];
    sprintf(nameSs, "statics%02X.hex", i_map_id_);
    FILE *staticsFs = fopen(nameSs,"wb");
    if (staticsFs) {
        fwrite(level_data_.statics, 1, 12000, staticsFs);
        fclose(staticsFs);
    }
#endif

    statics_.clear();
    for (uint16 i = 0; i < 400; i++) {
        LEVELDATA_STATICS & sref = level_data_.statics[i];
        if(sref.desc == 0)
            continue;
        Static *s = Static::loadInstance((uint8 *) & sref, i_map_id_);
        if (s)
            statics_.push_back(s);
    }

#if 0
    // for hacking weapons data
    char nameSw[256];
    sprintf(nameSw, "weapons%02X.hex", i_map_id_);
    FILE *staticsFw = fopen(nameSw,"wb");
    if (staticsFw) {
        fwrite(level_data_.weapons, 1, 36*512, staticsFw);
        fclose(staticsFw);
    }
#endif
    weapons_.clear();
    for (uint16 i = 0; i < 512; i++) {
        LEVELDATA_WEAPONS & wref = level_data_.weapons[i];
        if(wref.desc == 0)
            continue;
        WeaponInstance *w = createWeaponInstance((uint8 *) & wref);
        if (w) {
            if (wref.desc == 0x05) {
                uint16 offset_owner = READ_LE_UINT16(wref.offset_owner);
                if (offset_owner != 0) {
                    offset_owner = (offset_owner - 2) / 92; // 92 = ped data size
                    if (offset_owner > 7 && pindx[offset_owner] != 0xFFFF) {
                        // TODO: correct weapons for enemy agents
                        peds_[pindx[offset_owner]]->addWeapon(w);
                        w->setOwner(peds_[pindx[offset_owner]]);
                        w->setIsIgnored(true);
                        windx[i] = weapons_.size();
                        weapons_.push_back(w);
                    } else {
                        delete w;
                    }
                } else {
                    delete w;
                }
            } else {
                w->setMap(i_map_id_);
                w->setOwner(NULL);
                windx[i] = weapons_.size();
                weapons_.push_back(w);
            }
        }
    }

#if 0
    // for hacking objectives data
    char nameSo[256];
    sprintf(nameSo, "obj%02X.hex", i_map_id_);
    FILE *staticsFo = fopen(nameSo,"wb");
    if (staticsFo) {
        fwrite(level_data_.objectives, 1, 140, staticsFo);
        fclose(staticsFo);
    }
#endif
    // 0x01 offset of ped
    // 0x02 offset of ped
    // 0x03 offset of ped, next objective 0x00 + coord, nxt 0x00 + offset of ped
    // second objective is where ped should go, third ped that should reach it
    // also can be without those data or can have offset of ped + coord
    // 0x05 offset of weapon
    // 0x0E offset of vehicle
    // 0x0F offset of vehicle
    // 0x10 coordinates
    // looks like that objectives even if they are defined where not fully
    // defined(or are correct), indonesia has one objective but has 2
    // 0x0 objectives with peds offset + coords, rockies mission
    // has 0x0e objective + 0x01 but offsets are wrong as in original
    // gameplay only 1 persuade + evacuate present, in description
    // 0x0e + 2 x 0x01 + 0x0f, because of this careful loading required
    // max 5(6 read) objectives
    std::vector <PedInstance *> peds_evacuate;
    for (uint8 i = 0; i < 6; i++) {
        //bool isset = false;
        ObjectiveDesc *objd = NULL;

        LEVELDATA_OBJECTIVES & obj = level_data_.objectives[i];
        unsigned int bindx = READ_LE_UINT16(obj.offset), cindx = 0;
        // TODO: checking is implemented for correct offset, because
        // in game data objective description is not correctly defined
        // some offsets are wrong, objective type is missing somewhere
        // check this, also 0x03 is not fully implemented
        // Also for some objectives there should be "small" actions defined
        // inside ped data, in 1 lvl when agents are close to target
        // ped goes to car and moves to location, if reached mission is
        // failed, similar actions are in many missions(scenarios defines this)

        switch (READ_LE_UINT16(obj.type)) {
            case 0x00:
            {
                int x = READ_LE_INT16(obj.mapposx);
                if (x != 0) {
/*                    objd = new LocationObjective(objv_ReachLocation,
                       READ_LE_INT16(obj.mapposx),
                       READ_LE_INT16(obj.mapposy),
                       READ_LE_INT16(obj.mapposz));
                    //objd->pos_xyz.x = READ_LE_INT16(obj.mapposx);
                    //objd->pos_xyz.y = READ_LE_INT16(obj.mapposy);
                    //objd->pos_xyz.z = READ_LE_INT16(obj.mapposz);
                    objd->msg = "";
                    objd->condition = 32;
                    assert(objectives_.size() != 0);
                    ObjectiveDesc *pLastObj = objectives_.back();
                    pLastObj->condition |= 1;
                    pLastObj->subobjindx = objectives_.size();
                    objd->indx_grpid.targetindx = pLastObj->indx_grpid.targetindx;
                    objd->targettype = pLastObj->targettype;
                    isset = true;*/
                    //isset = true;
                }
            }
                break;
            case 0x01:
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && pindx[cindx] != 0xFFFF) {
                        PedInstance *p = peds_[pindx[cindx]];
                        objd = new ObjPersuade(p);
                        // Adds the ped to the list of peds to evacuate
                        peds_evacuate.push_back(p);
                    } else
                        printf("0x01 incorrect offset");
                } else
                    printf("0x01 type not matched");

                //isset = true;
                break;
            case 0x02: // Assassinate a civilian
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && pindx[cindx] != 0xFFFF) {
                        objd = new ObjAssassinate(peds_[pindx[cindx]]);
                    } else
                        printf("0x02 incorrect offset");
                } else
                    printf("0x02 type not matched");
                break;
            case 0x03:
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && pindx[cindx] != 0xFFFF) {
                        objd = new ObjProtect(peds_[pindx[cindx]]);
                    } else
                        printf("0x03 incorrect offset");
                } else
                    printf("0x03 type not matched");
                break;
            case 0x05:
                if (bindx >= 0x9562 && bindx < 0xDD62) {
                    bindx -= 0x9562;
                    cindx = bindx / 36;
                    if ((cindx * 36) == bindx && windx[cindx] != 0xFFFF) {
                        objd = new ObjTakeWeapon(weapons_[windx[cindx]]);
                    } else
                        printf("0x05 incorrect offset");
                } else
                    printf("0x05 type not matched %X", bindx);

                break;
            case 0x0A:
                objd = new ObjEliminate(PedInstance::og_dmPolice);
                break;
            case 0x0B:
                objd = new ObjEliminate(PedInstance::og_dmAgent);
                break;
            case 0x0E:
                if (bindx >= 0x5C02 && bindx < 0x6682) {
                    bindx -= 0x5C02;
                    cindx = bindx / 42;
                    if ((cindx * 42) == bindx && vindx[cindx] != 0xFFFF) {
                        objd = new ObjDestroyVehicle(vehicles_[vindx[cindx]]);
                    } else
                        printf("0x0E incorrect offset");
                } else
                    printf("0x0E type not matched");

                break;
            case 0x0F:
                if (bindx >= 0x5C02 && bindx < 0x6682) {
                    bindx -= 0x5C02;
                    cindx = bindx / 42;
                    if ((cindx * 42) == bindx && vindx[cindx] != 0xFFFF) {
                        objd = new ObjUseVehicle(vehicles_[vindx[cindx]]);
                        // TODO Do we have to add the vehicle to the list of object to evacuate?
                    } else
                        printf("0x0F incorrect offset");
                } else
                    printf("0x0F type not matched");
                break;
            case 0x10:
                // realworld coordinates, not tile based
                objd = new ObjEvacuate(READ_LE_INT16(obj.mapposx),
                    READ_LE_INT16(obj.mapposy),
                    READ_LE_INT16(obj.mapposz),
                    peds_evacuate);

                break;
#ifdef _DEBUG
            default:
                FSERR(Log::k_FLG_GAME, "Mission", "loadLevel", ("Unknown objective %X\n", READ_LE_UINT16(obj.type)));
                break;
#endif
        }

        // An objective has been created, adds it to the list
        // of objectives
        if (objd != NULL) {
            objectives_.push_back(objd);
        } else {
            // TODO add a better error handling
            break;
        }
    }

    // Add a special Objective to end a mission
    objectives_.push_back(new ObjSucceed());
    peds_evacuate.clear();

#if 0
    // for hacking scenarios data
    char nameSss[256];
    sprintf(nameSss, "scenarios%02X.hex", i_map_id_);
    FILE *staticsFss = fopen(nameSss,"wb");
    if (staticsFss) {
        fwrite(level_data_.scenarios, 1, 2048 * 8, staticsFss);
        fclose(staticsFss);
    }
#endif
#ifdef SHOW_SCENARIOS_DEBUG
    for (uint16 i = 1; i < 2047; i++) {
        LEVELDATA_SCENARIOS & scenario = level_data_.scenarios[i];
        if (scenario.type == 0)
            break;
        else {
            printf("num %i\n", i);
            printf("next %i\n", READ_LE_INT16(scenario.next) / 8);
            printf("object offset %X\n", READ_LE_INT16(scenario.offset_object));
            printf("x = %i, y = %i, z = %i\n",
                   scenario.tilex >> 1,
                   scenario.tiley >> 1,
                   scenario.tilez);
            printf("type = %i\n\n", scenario.type);
        }
    }
#endif

    // adding visual markers(arrow + 1,2,3,4) above our agents
    // availiable/selected on screen
    addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
    addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
    addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
    addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
    addSfxObject(new SFXObject(i_map_id_, SFXObject::sfxt_AgentFirst));
    addSfxObject(new SFXObject(i_map_id_, SFXObject::sfxt_AgentSecond));
    addSfxObject(new SFXObject(i_map_id_, SFXObject::sfxt_AgentThird));
    addSfxObject(new SFXObject(i_map_id_, SFXObject::sfxt_AgentFourth));

    return true;
}

/*!
 * Sets the given map for the mission.
 * Creates a minimap from it.
 * \param p_map The map to set.
 */
void Mission::set_map(Map *p_map) {
    if (p_map) {
        p_map_ = p_map;
        p_map_->mapDimensions(&mmax_x_, &mmax_y_, &mmax_z_);

        if (p_minimap_) {
            delete p_minimap_;
        }
        p_minimap_ = new MiniMap(p_map_);
    }
}

int Mission::mapWidth()
{
    return p_map_->width();
}

int Mission::mapHeight()
{
    return p_map_->height();
}

int Mission::startX()
{
    int x =
        p_map_->tileToScreenX(peds_[0]->tileX(),
        peds_[0]->tileY(), mmax_z_ - 1, 0,
                                   0);
    x -= (GAME_SCREEN_WIDTH - 129) / 2;
    if (x < 0)
        x = 0;
    return x;
}

int Mission::startY()
{
    int y =
        p_map_->tileToScreenY(peds_[0]->tileX(),
                                   peds_[0]->tileY(), mmax_z_ - 1, 0,
                                   0);
    y -= GAME_SCREEN_HEIGHT / 2;
    if (y < 0)
        y = 0;
    return y;
}

int Mission::minScreenX()
{
    return p_map_->tileToScreenX(min_x_, min_y_, 0, 0, 0);
}

int Mission::minScreenY()
{
    return p_map_->tileToScreenY(min_x_, min_y_, 0, 0, 0);
}

int Mission::maxScreenX()
{
    return p_map_->tileToScreenX(max_x_, max_y_, 0, 0, 0);
}

int Mission::maxScreenY()
{
    return p_map_->tileToScreenY(max_x_, max_y_, 0, 0, 0);
}

int fastKey(int tx, int ty, int tz)
{
    return tx | (ty << 8) | (tz << 16);
}

int fastKey(MapObject * m)
{
    return fastKey(m->tileX(), m->tileY(), m->tileZ());
}

void Mission::createFastKeys(int tilex, int tiley, int maxtilex, int maxtiley) {

    if (tilex < 0)
        tilex = 0;
    if (tiley < 0)
        tiley = 0;
    if (maxtilex >= mmax_x_)
        maxtilex = mmax_x_;
    if (maxtiley >= mmax_y_)
        maxtiley = mmax_y_;

    cache_vehicles_.clear();
    cache_peds_.clear();
    cache_weapons_.clear();
    cache_statics_.clear();
    cache_sfx_objects_.clear();

    fast_vehicle_cache_.clear();
    fast_ped_cache_.clear();
    fast_weapon_cache_.clear();
    fast_statics_cache_.clear();
    fast_sfx_objects_cache_.clear();

    // updating position for visual markers
    for (unsigned int i = 0; i < 4; i++) {
        PedInstance *p = peds_[i];
        if (p->agentIs() == PedInstance::Agent_Active && p->health() > 0) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                //sfx_objects_[i]->setPosition(p->tileX(), p->tileY(), p->tileZ(),
                    //p->offX(), p->offY(), p->offZ() + 320);
                sfx_objects_[i + 4]->setPosition(p->tileX(), p->tileY(),
                    p->tileZ(), p->offX() - 16, p->offY(), p->offZ() + 256);
            }
        } else {
            //sfx_objects_[i]->setMap(-1);
            sfx_objects_[i + 4]->setMap(-1);
        }
    }
        

    // vehicles
    for (unsigned int i = 0; i < vehicles_.size(); i++) {
        VehicleInstance *v = vehicles_[i];
        if (v->tileX() >= tilex && v->tileX() < maxtilex
            && v->tileY() >= tiley && v->tileY() < maxtiley)
        {
            // NOTE: a trick to make vehicles be drawn correctly z+1
            fast_vehicle_cache_.insert(fastKey(v->tileX(),
                v->tileY(), v->tileZ() + 1));
            cache_vehicles_.push_back(v);
        }
    }

    // peds
    for (unsigned int i = 0; i < 4; i++) {
        PedInstance *p = peds_[i];
        if (p->agentIs() == PedInstance::Agent_Active && p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }
    for (unsigned int i = 8; i < peds_.size(); i++) {
        PedInstance *p = peds_[i];
        if (p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }

    // weapons
    for (unsigned int i = 0; i < weapons_.size(); i++) {
        WeaponInstance *w = weapons_[i];
        if (w->map() != -1 && w->tileX() >= tilex && w->tileX() < maxtilex
            && w->tileY() >= tiley && w->tileY() < maxtiley)
        {
            fast_weapon_cache_.insert(fastKey(w));
            cache_weapons_.push_back(w);
        }
    }

    // statics
    for (unsigned int i = 0; i < statics_.size(); i++) {
        Static *s = statics_[i];
        if (s->tileX() >= tilex && s->tileX() < maxtilex
            && s->tileY() >= tiley && s->tileY() < maxtiley)
        {
            fast_statics_cache_.insert(fastKey(s));
            cache_statics_.push_back(s);
        }
    }

    // sfx objects
    for (unsigned int i = 0; i < sfx_objects_.size(); i++) {
        SFXObject *so = sfx_objects_[i];
        if (so->map() != -1 && so->tileX() >= tilex && so->tileX() < maxtilex
            && so->tileY() >= tiley && so->tileY() < maxtiley)
        {
            fast_sfx_objects_cache_.insert(fastKey(so));
            cache_sfx_objects_.push_back(so);
        }
    }
}

void Mission::drawMap(int scrollx, int scrolly)
{
    p_map_->draw(scrollx, scrolly, this);
}

void Mission::drawAt(int tilex, int tiley, int tilez, int x, int y)
{
    int key = fastKey(tilex, tiley, tilez);

    if (fast_vehicle_cache_.find(key) != fast_vehicle_cache_.end()) {
        // draw vehicles
        for (unsigned int i = 0; i < cache_vehicles_.size(); i++)
            if (cache_vehicles_[i]->tileX() == tilex
                && cache_vehicles_[i]->tileY() == tiley
                // NOTE: a trick to make vehicles be drawn correctly z+1
                && (cache_vehicles_[i]->tileZ() + 1) == tilez)
                cache_vehicles_[i]->draw(x, y);
    }

    if (fast_ped_cache_.find(key) != fast_ped_cache_.end()) {
        // draw peds
        for (unsigned int i = 0; i < cache_peds_.size(); i++)
            if (cache_peds_[i]->tileX() == tilex
                && cache_peds_[i]->tileY() == tiley
                && cache_peds_[i]->tileZ() == tilez) {
                cache_peds_[i]->draw(x, y);
#if 0
                g_Screen.drawLine(x - TILE_WIDTH / 2, y,
                                  x + TILE_WIDTH / 2, y, 11);
                g_Screen.drawLine(x + TILE_WIDTH / 2, y,
                                  x + TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  11);
                g_Screen.drawLine(x + TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  x - TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  11);
                g_Screen.drawLine(x - TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  x - TILE_WIDTH / 2, y, 11);
#endif

            }
    }

    if (fast_weapon_cache_.find(key) != fast_weapon_cache_.end()) {
        // draw weapons
        for (unsigned int i = 0; i < cache_weapons_.size(); i++)
            if (cache_weapons_[i]->map() != -1 && cache_weapons_[i]->tileX() == tilex
                && cache_weapons_[i]->tileY() == tiley
                && cache_weapons_[i]->tileZ() == tilez) {
                cache_weapons_[i]->draw(x, y);
            }
    }

    if (fast_statics_cache_.find(key) != fast_statics_cache_.end()) {
        // draw statics
        for (unsigned int i = 0; i < cache_statics_.size(); i++)
            if (cache_statics_[i]->tileX() == tilex
                && cache_statics_[i]->tileY() == tiley
                && cache_statics_[i]->tileZ() == tilez) {
                cache_statics_[i]->draw(x, y);
            }
    }

    if (fast_sfx_objects_cache_.find(key) != fast_sfx_objects_cache_.end()) {
        // draw sfx objects
        for (unsigned int i = 0; i < cache_sfx_objects_.size(); i++)
            if (cache_sfx_objects_[i]->tileX() == tilex
                && cache_sfx_objects_[i]->tileY() == tiley
                && cache_sfx_objects_[i]->tileZ() == tilez) {
                cache_sfx_objects_[i]->draw(x, y);
            }
    }
}

void Mission::start()
{
    LOG(Log::k_FLG_GAME, "Mission", "start()", ("Start mission"));
    // Reset mission statistics
    stats_.agents = 0;
    stats_.mission_duration = 0;
    stats_.agentCaptured = 0;
    stats_.enemyKilled = 0;
    stats_.criminalKilled = 0;
    stats_.civilKilled = 0;
    stats_.policeKilled = 0;
    stats_.guardKilled = 0;
    stats_.convinced = 0;
    stats_.nbOfShots = 0;
    stats_.nbOfHits = 0;
    // Clear squad
    p_squad_->clear();
    cur_objective_ = 0;

    for (int i = 0; i < 4; i++) {
        PedInstance *p = peds_[i];
        Agent *pAg = g_Session.agents().squadMember(i);
        if (pAg && pAg->isActive()) {
            stats_.agents += 1;
            p->setHealth(pAg->health() *
                            peds_[i]->health() / 255);
            while (pAg->numWeapons()) {
                WeaponInstance *wi = pAg->removeWeapon(0);
                weapons_.push_back(wi);
                p->addWeapon(wi);
                wi->setOwner(p);
                wi->setIsIgnored(true);
            }
            p->setAgentIs(PedInstance::Agent_Active);
            *((ModOwner *)p) = *((ModOwner *)pAg);
            // adds the agent to the mission squad
            p_squad_->setMember(i, p);
        } else {
            p->setHealth(-1);
            p->setAgentIs(PedInstance::Agent_Non_Active);
            p->setIsIgnored(true);
            p->setStateMasks(PedInstance::pa_smUnavailable);
        }
    }    
}

/*! 
 * Checks if objectives are completed or failed and updates
 * mission status.
 */
void Mission::checkObjectives() {
    // checking agents, if all are dead -> mission failed
    if (p_squad_->isAllDead()) {
        status_ = FAILED;
        return;
    }

    // We only check the current objective
    if (cur_objective_ < objectives_.size()) {
        ObjectiveDesc * pObj = objectives_[cur_objective_];

        // If it's the first time the objective is checked,
        // declares it started
        if (pObj->status == kNotStarted) {
            LOG(Log::k_FLG_GAME, "Mission", "checkObjectives()", ("Start objective : %d", cur_objective_));
            // An objective has just started, warn all listeners
            GameEvent evt = pObj->start();
            handleObjectiveEvent(evt, pObj);
        }

        // Checks if the objective is completed
        GameEvent evt = pObj->evaluate(this);
        // the objective may have generated an event
        if (evt.type_ != GameEvent::kNone) {
            // so dispatch it
            handleObjectiveEvent(evt, pObj);
        }

        if (pObj->isTerminated()) {
            if (pObj->status == kFailed) {
                status_ = FAILED;
            } else {
                // Objective is completed -> go to next one
                cur_objective_++;
            }
        }
    }
}

void Mission::handleObjectiveEvent(GameEvent & evt, ObjectiveDesc *pObj) {
    switch(evt.type_) {
    case GameEvent::kObjTargetSet:
        {
        MapObject *pObject = static_cast<MapObject *>(evt.pCtxt_);
        p_minimap_->setTarget(pObject);
        }
        break;
    case GameEvent::kObjEvacuate:
        {
        ObjEvacuate * pObjEvac = static_cast<ObjEvacuate *>(pObj);
        p_minimap_->setEvacuationPoint(pObjEvac->posXYZ());
        }
        break;
    case GameEvent::kObjTargetCleared:
        p_minimap_->clearTarget();
        break;
    case GameEvent::kObjSucceed:
        // Special event to know a mission is completed
        status_ = COMPLETED;
        break;
    default:
        break;
    }
    fireGameEvent(evt);
}

/*! 
 * Adds a listener to the list of listeners of mission events.
 * \param pListener The listener
 */
void Mission::addListener(GameEventListener *pListener) {
    if (pListener) {
        listeners_.push_back(pListener);
    }
}

/*! 
 * Removes the listener from the list of listeners of mission events.
 * \param pListener The listener
 */
void Mission::removeListener(GameEventListener *pListener) {
    for (std::list < GameEventListener * >::iterator it = listeners_.begin();
         it != listeners_.end(); it++) {
             if (pListener == *it) {
                 listeners_.erase(it);
                 return;
             }
    }
}

/*! 
 * Removes all listeners from the list of listeners of mission events.
 */
void Mission::removeListeners() {
    listeners_.clear();
}

/*! 
 * Sends the event to all listeners of mission events.
 * \param evt The event to send
 */
void Mission::fireGameEvent(GameEvent &evt) {
    if (evt.type_ != GameEvent::kNone) {
        for (std::list < GameEventListener * >::iterator it = listeners_.begin();
                            it != listeners_.end(); it++) {
            (*it)->handleGameEvent(evt);
        }
    }
}

void Mission::addWeaponsFromPedToAgent(PedInstance* p, Agent* pAg)
{
    while (p->numWeapons()) {
        WeaponInstance *wi = p->removeWeapon(0);
        std::vector < WeaponInstance * >::iterator it =
            weapons_.begin();
        while (it != weapons_.end() && *it != wi)
            it++;
        assert(it != weapons_.end());
        weapons_.erase(it);
        wi->deactivate();
        // auto-reload for pistol
        if (wi->getMainType() == Weapon::Pistol)
            wi->setAmmoRemaining(wi->ammo());
        wi->resetWeaponUsedTime();
        pAg->addWeapon(wi);
    }
}


void Mission::end()
{
    LOG(Log::k_FLG_GAME, "Mission", "end()", ("End mission"));
    for (unsigned int i = 8; i < peds_.size(); i++) {
        PedInstance *p = peds_[i];
        if (p->health() <= 0) {
            switch (p->getMainType()) {
                case PedInstance::m_tpAgent:
                    stats_.enemyKilled++;
                    break;
                case PedInstance::m_tpCriminal:
                    stats_.criminalKilled++;
                    break;
                case PedInstance::m_tpPedestrian:
                    stats_.civilKilled++;
                    break;
                case PedInstance::m_tpGuard:
                    stats_.guardKilled++;
                    break;
                case PedInstance::m_tpPolice:
                    stats_.policeKilled++;
                    break;
            }
        } else if (p->isPersuaded()) {
            if (p->objGroupDef() == PedInstance::og_dmAgent) {
                stats_.agentCaptured++;
                if (completed()) {
                    Agent *pAg = g_Session.agents().createAgent(false);
                    if (pAg) {
                        addWeaponsFromPedToAgent(p, pAg);
                        *((ModOwner *)pAg) = *((ModOwner *)p);
                    }
                }
            } else
                stats_.convinced++;
        }
    }

    // synch ped agents with agent from cryo chamber
    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        PedInstance *p_pedAgent = p_squad_->member(i);
        if (p_pedAgent) {
            Agent *pAg = g_Session.agents().squadMember(i);
            if (p_pedAgent->health() <= 0) {
                // an agent died -> remove him from cryo
                p_pedAgent->destroyAllWeapons();
                g_Session.agents().destroyAgentSlot(i);
            } else {
                // synch only weapons
                addWeaponsFromPedToAgent(p_pedAgent, pAg);
            }
        }
    }

    // reset squad
    p_squad_->clear();
}

void Mission::addWeapon(WeaponInstance * w)
{
    w->setMap(p_map_->id());
    for (unsigned int i = 0; i < weapons_.size(); i++)
        if (weapons_[i] == w)
            return;
    weapons_.push_back(w);
}

MapObject * Mission::findAt(int tilex, int tiley, int tilez,
                            MapObject::MajorTypeEnum *majorT, int *searchIndex,
                            bool only)
{
    switch(*majorT) {
        case MapObject::mjt_Ped:
            for (unsigned int i = *searchIndex; i < peds_.size(); i++)
                if ((!peds_[i]->isIgnored()) && peds_[i]->tileX() == tilex
                    && peds_[i]->tileY() == tiley
                    && peds_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Ped;
                    return peds_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Weapon:
            for (unsigned int i = *searchIndex; i < weapons_.size(); i++)
                if ((!weapons_[i]->isIgnored()) && weapons_[i]->tileX() == tilex
                    && weapons_[i]->tileY() == tiley
                    && weapons_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Weapon;
                    return weapons_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Static:
            for (unsigned int i = *searchIndex; i < statics_.size(); i++)
                if (statics_[i]->tileX() == tilex
                    && statics_[i]->tileY() == tiley
                    && statics_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Static;
                    return statics_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Vehicle:
            for (unsigned int i = *searchIndex; i < vehicles_.size(); i++)
                if (vehicles_[i]->tileX() == tilex
                    && vehicles_[i]->tileY() == tiley
                    && vehicles_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Vehicle;
                    return vehicles_[i];
                }
            break;
        default:
            printf("undefined majortype %i", *majorT);
            break;
    }
    return NULL;
}

// Surface walkable
bool Mission::sWalkable(char thisTile, char upperTile) {

    return (
            // checking surface
            (((thisTile >= 0x05 && thisTile <= 0x09) ||
            thisTile == 0x0B || (thisTile >= 0x0D && thisTile <= 0x0F)
            || (thisTile == 0x11 || thisTile == 0x12)))
            // or checking stairs
            || ((thisTile > 0x00 && thisTile < 0x05))
        ) && (upperTile == 0x00 || upperTile == 0x10);
}

bool Mission::isSurface(char thisTile) {
    return (thisTile >= 0x05 && thisTile <= 0x09) ||
        thisTile == 0x0B || (thisTile >= 0x0D && thisTile <= 0x0F)
        || (thisTile == 0x11 || thisTile == 0x12);
}

bool Mission::isStairs(char thisTile) {
    return thisTile > 0x00 && thisTile < 0x05;
}

bool Mission::setSurfaces() {

    // Description: creates map of walkable surfaces, also
    // defines directions where movement is possible

    // NOTE: tiles walkdata type 0x0D are quiet special, and they
    // are not handled correctly, these correction and andjustings
    // can create additional speed drain, as such I didn't
    // implemented them as needed. To make it possible a patch
    // required to walkdata and a lot of changes which I don't
    // want to do.
    // 0x10 appear above walking tile where train stops
    clrSurfaces();
    mmax_m_all = mmax_x_ * mmax_y_ * mmax_z_;
    mtsurfaces_ = (surfaceDesc *)malloc(mmax_m_all * sizeof(surfaceDesc));
    mdpoints_ = (floodPointDesc *)malloc(mmax_m_all * sizeof(floodPointDesc));
    mdpoints_cp_ = (floodPointDesc *)malloc(mmax_m_all * sizeof(floodPointDesc));
    if(mtsurfaces_ == NULL || mdpoints_ == NULL || mdpoints_cp_ == NULL) {
        clrSurfaces();
        printf("ERROR: memory allocation failed in Mission::setSurfaces");
        return false;
    }
    mmax_m_xy = mmax_x_ * mmax_y_;
    memset((void *)mtsurfaces_, 0, mmax_m_all * sizeof(surfaceDesc));
    memset((void *)mdpoints_, 0, mmax_m_all * sizeof(floodPointDesc));
    for (int ix = 0; ix < mmax_x_; ix++) {
        for (int iy = 0; iy < mmax_y_; iy++) {
            for (int iz = 0; iz < mmax_z_; iz++) {
                mtsurfaces_[ix + iy * mmax_x_ + iz * mmax_m_xy].twd =
                    g_App.walkdata_p_[p_map_->tileAt(ix, iy, iz)];
            }
        }
    }

    // to make surfaces where large doors are located walkable
    for (std::vector<Static *>::iterator it = statics_.begin();
        it != statics_.end(); it++)
    {
        Static *s = *it;
        if (s->getMainType() == Static::smt_LargeDoor) {
            int indx = s->tileX() + s->tileY() * mmax_x_
                + s->tileZ() * mmax_m_xy;
            mtsurfaces_[indx].twd = 0x00;
            if (s->getSubType() == 0) {
                if (indx - 1 >= 0)
                    mtsurfaces_[indx - 1].twd = 0x00;
                if (indx + 1 < mmax_m_all)
                    mtsurfaces_[indx + 1].twd = 0x00;
            } else if (s->getSubType() == 2) {
                if (indx - mmax_x_ >= 0)
                    mtsurfaces_[indx - mmax_x_].twd = 0x00;
                if (indx + mmax_x_ < mmax_m_all)
                    mtsurfaces_[indx + mmax_x_].twd = 0x00;
            }
        }
    }

    //printf("surface data size %i\n", sizeof(surfaceDesc) * mmax_m_all);
    //printf("flood data size %i\n", sizeof(floodPointDesc) * mmax_m_all);

    for (unsigned int i = 0; i < peds_.size(); i++) {
        PedInstance *p = peds_[i];
        int x = p->tileX();
        int y = p->tileY();
        int z = p->tileZ();
        if (z >= mmax_z_ || z < 0 || p->health() <= 0) {
            // TODO : check on all maps those peds correct position
            p->setTileZ(mmax_z_ - 1);
            continue;
        }
        if (mdpoints_[x + y * mmax_x_ + z * mmax_m_xy].t == m_fdNotDefined) {
            toDefineXYZ stodef;
            std::vector<toDefineXYZ> vtodefine;
            mdpoints_[x + y * mmax_x_ + z * mmax_m_xy].t = m_fdDefReq;
            stodef.x = x;
            stodef.y = y * mmax_x_;
            stodef.z = z * mmax_m_xy;
            vtodefine.push_back(stodef);
            do {
                stodef = vtodefine.back();
                vtodefine.pop_back();
                x = stodef.x;
                y = stodef.y;
                z = stodef.z;
                //if (x == 50 && y / mmax_x_ == 27 && z / mmax_m_xy == 2)
                    //x = 50;
                uint8 this_s = mtsurfaces_[x + y + z].twd;
                uint8 upper_s = 0;
                floodPointDesc *cfp = &(mdpoints_[x + y + z]);
                int zm = z - mmax_m_xy;
                // if current is 0x00 or 0x10 tile we will use lower tile
                // to define it
                if (this_s == 0x00 || this_s == 0x10) {
                    if (zm < 0) {
                        cfp->t = m_fdNonWalkable;
                        continue;
                    }
                    z = zm;
                    zm -= mmax_m_xy;
                    upper_s = this_s;
                    this_s = mtsurfaces_[x + y + z].twd;
                    if (!sWalkable(this_s, upper_s))
                        continue;
                } else if (this_s == 0x11 || this_s == 0x12) {
                    int zp_tmp = z + mmax_m_xy;
                    if (zp_tmp < mmax_m_all) {
                        // we are defining tile above current
                        cfp = &(mdpoints_[x + y + zp_tmp]);
                    } else
                        cfp->t = m_fdNonWalkable;
                }
                int xm = x - 1;
                int ym = y - mmax_x_;
                int xp = x + 1;
                int yp = y + mmax_x_;
                int zp = z + mmax_m_xy;
                floodPointDesc *nxtfp;
                if (zp < mmax_m_all) {
                    upper_s = mtsurfaces_[x + y + zp].twd;
                    if(!sWalkable(this_s, upper_s)) {
                        cfp->t = m_fdNonWalkable;
                        continue;
                    }
                } else {
                    cfp->t = m_fdNonWalkable;
                    continue;
                }
                unsigned char sdirm = 0x00;
                unsigned char sdirh = 0x00;
                unsigned char sdirl = 0x00;
                unsigned char sdirmr = 0x00;

                switch (this_s) {
                    case 0x00:
                        cfp->t = m_fdNonWalkable;
                        break;
                    case 0x01:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x01) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zm]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (ym >= 0) {
                            nxtfp = &(mdpoints_[x + ym + zp]);
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x01 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s, mtsurfaces_[
                                    x + ym + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x01) {
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x01) {
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x02:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x02) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (yp < mmax_m_xy) {
                            nxtfp = &(mdpoints_[x + yp + zp]);
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if(isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x02 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s,  mtsurfaces_[
                                    x + yp + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x02) {
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x02) {
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x03:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x03) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            nxtfp = &(mdpoints_[xp + y + zp]);
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x03 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s,
                                    mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x03) {
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (sWalkable(this_s, upper_s)) { 
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x03) {
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x04:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x04) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (xm >= 0) {
                            nxtfp = &(mdpoints_[xm + y + zp]);
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x04 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s, mtsurfaces_[
                                    xm + y + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x04) {
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x04) {
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x05:
                    case 0x06:
                    case 0x07:
                    case 0x08:
                    case 0x09:
                    case 0x0B:
                    case 0x0D:
                    case 0x0E:
                    case 0x0F:
                        cfp->t = m_fdWalkable;
                        if (!((this_s > 0x05 && this_s < 0x0A) || this_s == 0x0B
                            || this_s == 0x0F))
                        {
                            cfp->t |= m_fdSafeWalk;
                        }
                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x20 | 0x40 | 0x80);
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x20 | 0x80);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x03)
                                {
                                    sdirl |= 0x40;
                                }
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x20 | 0x80);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02 || upper_s == 0x04
                                    || upper_s == 0x12)) {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xm + y + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x12)
                                            sdirh |= 0x40;
                                        else
                                            sdirm |= 0x40;
                                        nxtfp = &(mdpoints_[xm + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x20 | 0x80);

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04 | 0x08);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03 || upper_s == 0x11))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x11)
                                            sdirh |= 0x04;
                                        else
                                            sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if(ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x08 | 0x10 | 0x20);
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x08 | 0x20);
                                if (this_s == 0x02 || this_s == 0x03 || this_s == 0x04){
                                    sdirl |= 0x10;
                                }
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x08 | 0x20);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x03
                                    || upper_s == 0x04 || upper_s == 0x11))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + ym + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x11)
                                            sdirh |= 0x10;
                                        else
                                            sdirm |= 0x10;
                                        nxtfp = &(mdpoints_[x + ym + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x08 | 0x20);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x80 | 0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04 || upper_s == 0x12))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x12)
                                            sdirh |= 0x01;
                                        else
                                            sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges

                        if (xm >= 0) {
                            if (ym >= 0 && (sdirm & 0x20) != 0) {
                                nxtfp = &(mdpoints_[xm + ym + zp]);
                                this_s = mtsurfaces_[xm + ym + z].twd;
                                upper_s = mtsurfaces_[xm + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x20);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x80) != 0) {
                                nxtfp = &(mdpoints_[xm + yp + zp]);
                                this_s = mtsurfaces_[xm + yp + z].twd;
                                upper_s = mtsurfaces_[xm + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x80);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            if (ym >= 0 && (sdirm & 0x08) != 0) {
                                nxtfp = &(mdpoints_[xp + ym + zp]);
                                this_s = mtsurfaces_[xp + ym + z].twd;
                                upper_s = mtsurfaces_[xp + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x08);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x0A:
                    case 0x0C:
                    case 0x10:
                        cfp->t = m_fdNonWalkable;
                        break;
                    case 0x11:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x04) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xm + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x01) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + ym + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x02) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + yp + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04 | 0x08);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02 || this_s == 0x04){
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if(ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x08 | 0x10);
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x08 | 0x20);
                                if (this_s == 0x02 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x10;
                                }
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x08 | 0x20);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x03 || upper_s == 0x04)) {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + ym + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x10;
                                        nxtfp = &(mdpoints_[x + ym + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x08);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges
                        if (xp < mmax_x_) {
                            if (ym >= 0 && (sdirm & 0x08) != 0) {
                                nxtfp = &(mdpoints_[xp + ym + zp]);
                                this_s = mtsurfaces_[xp + ym + z].twd;
                                upper_s = mtsurfaces_[xp + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x08);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x12:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x01) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + ym + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x04) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xm + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x03) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xp + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        }

                        if (xm >=0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x40 | 0x80);
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x20 | 0x80);
                                if (this_s == 0x01 || this_s == 0x02 || this_s == 0x03){
                                    sdirl |= 0x40;
                                }
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x20 | 0x80);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xm + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x40;
                                        nxtfp = &(mdpoints_[xm + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x20 | 0x80);

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x80 | 0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges
                        if (yp < mmax_m_xy) {
                            if (xm >= 0 && (sdirm & 0x80) != 0) {
                                nxtfp = &(mdpoints_[xm + yp + zp]);
                                this_s = mtsurfaces_[xm + yp + z].twd;
                                upper_s = mtsurfaces_[xm + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x80);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                            if (xp < mmax_x_ && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }

                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                }
            } while (vtodefine.size());
        }
    }
#if 0
    unsigned int cw = 0;
    for (int iz = 0; iz < mmax_z_; iz++) {
        for (int iy = 0; iy < mmax_y_; iy++) {
            for (int ix = 0; ix < mmax_x_; ix++) {
                floodPointDesc *cfpp = &(mdpoints_[ix + iy * mmax_x_ + iz * mmax_m_xy]);

                if ((cfpp->t & m_fdWalkable) == m_fdWalkable)
                    cw++;
            }
        }
    }

    printf("flood walkables %i\n", cw);
#endif
    return true;
}

void Mission::clrSurfaces() {

    if(mtsurfaces_ != NULL) {
        free(mtsurfaces_);
        mtsurfaces_ = NULL;
    }
    if(mdpoints_ != NULL) {
        free(mdpoints_);
        mdpoints_ = NULL;
    }
    if(mdpoints_cp_ != NULL) {
        free(mdpoints_cp_);
        mdpoints_cp_ = NULL;
    }
}

/*!
 * Uses isometric coordinates transformation to find walkable tile,
 * starting from top.
 * \param mtp
 * \return True if walkable tile found
 */
bool Mission::getWalkable(MapTilePoint &mtp) {
    bool gotit = false;
    int bx, by, box, boy;
    int bz = mmax_z_;
    unsigned int cindx;
    unsigned char twd;
    do {
        bz--;
        // using lowered Z, at start postion is at top of tile not at bottom
        bx = mtp.tx * 256 + mtp.ox + 128 * (bz - 1);
        box = bx % 256;
        bx = bx / 256;
        by = mtp.ty * 256 + mtp.oy + 128 * (bz - 1);
        boy = by % 256;
        by = by / 256;
        if (bz >= mmax_z_ || bx >= mmax_x_ || by >= mmax_y_)
            continue;
        if (bz < 0 || bx < 0 || by < 0)
            break;
        cindx = bx + by * mmax_x_ + bz * mmax_m_xy;
        if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable) {
            twd = mtsurfaces_[cindx].twd;
            int dx = 0;
            int dy = 0;
            switch (twd) {
                case 0x01:
                    dy = ((boy + 128) * 2) / 3;
                    dx = box + 128 - dy / 2;
                    if (dx < 256) {
                        gotit = true;
                        box = dx;
                        boy = dy;
                    } else {
                        if ((bx + 1) < mmax_x_) {
                            cindx = (bx + 1) + by * mmax_x_ + bz * mmax_m_xy;
                            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                && mtsurfaces_[cindx].twd == 0x01)
                            {
                                gotit = true;
                                bx++;
                                box = dx - 256;
                                boy = dy;
                            }
                        }
                    }
                    break;
                case 0x02:
                    if (boy < 128) {
                        dy = boy * 2;
                        dx = box + boy;
                        if (dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((bx + 1) < mmax_x_) {
                                cindx = (bx + 1) + by * mmax_x_ + bz * mmax_m_xy;
                                if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                    && mtsurfaces_[cindx].twd == 0x02)
                                {
                                    gotit = true;
                                    bx++;
                                    box = dx - 256;
                                    boy = dy;
                                }
                            }
                        }
                    } else {
                        // TODO : add check 0x01?
                    }
                    break;
                case 0x03:
                    if (box < 128) {
                        dx = box * 2;
                        dy = box + boy;
                        if (dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((by + 1) < mmax_y_) {
                                cindx = bx + (by + 1) * mmax_x_ + bz * mmax_m_xy;
                                if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                    && mtsurfaces_[cindx].twd == 0x03)
                                {
                                    gotit = true;
                                    by++;
                                    box = dx;
                                    boy = dy - 256;
                                }
                            }
                        }
                    } else {
                        // TODO : 0x04?
                    }
                    break;
                case 0x04:
                    dx = ((box + 128) * 2) / 3;
                    dy = boy + 128 - dx / 2;
                    if (dy < 256) {
                        gotit = true;
                        box = dx;
                        boy = dy;
                    } else {
                        if ((by + 1) < mmax_y_) {
                            cindx = bx + (by + 1) * mmax_x_ + bz * mmax_m_xy;
                            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                && mtsurfaces_[cindx].twd == 0x04)
                            {
                                gotit = true;
                                by++;
                                box = dx;
                                boy = dy - 256;
                            }
                        }
                    }
                    break;
                default:
                    gotit = true;
                    // TODO: 0x01 0x02 0x03 0x04?
                break;
            }
        } else if (bz - 1 >= 0) {
            cindx = bx + by * mmax_x_ + (bz - 1) * mmax_m_xy;
            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable) {
                twd = mtsurfaces_[cindx].twd;
                int dx = 0;
                int dy = 0;
                switch (twd) {
                    case 0x01:
                        dy = (boy * 2) / 3;
                        dx = box - dy / 2;
                        if (dx >= 0) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x02:
                        dy = (boy - 128) * 2;
                        dx = (box + dy / 2) - 128;
                        if (dy >= 0 && dx >= 0 && dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x03:
                        dx = (box - 128) * 2;
                        dy = (boy + dx / 2) - 128;
                        if (dx >= 0 && dy >= 0 && dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x04:
                        dx = (box * 2) / 3;
                        dy = boy - dx / 2;
                        if (dy >= 0) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    default:
                        break;
                }
                
            }
        }
    } while (bz != 0 && !gotit);
    if (gotit) {
        mtp.tx = bx;
        mtp.ty = by;
        mtp.tz = bz;
        mtp.ox = box;
        mtp.oy = boy;
    }
    return gotit;
}

bool Mission::getWalkableClosestByZ(MapTilePoint &mtp) {
    // NOTE: using z from mtp as start to find closest z
    int inc_z = mtp.tz;
    int dec_z = mtp.tz;
    bool found = false;

    do {
        if (inc_z < mmax_z_) {
            if ((mdpoints_[mtp.tx + mtp.ty * mmax_x_
                + inc_z * mmax_m_xy].t & m_fdWalkable) == m_fdWalkable)
            {
                mtp.tz = inc_z;
                found = true;
                break;
            }
            inc_z++;
        }
        if (dec_z >= 0) {
            if ((mdpoints_[mtp.tx + mtp.ty * mmax_x_
                + dec_z * mmax_m_xy].t & m_fdWalkable) == m_fdWalkable)
            {
                mtp.tz = dec_z;
                found = true;
                break;
            }
            dec_z--;
        }
    } while (inc_z < mmax_z_ || dec_z >= 0);

    return found;
}

void Mission::adjXYZ(int &x, int &y, int &z) {
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (z < 0 || z >= mmax_z_)
        z = 0;
    if (x >= mmax_x_)
        x = mmax_x_ - 1;
    if (y >= mmax_y_)
        y = mmax_y_ - 1;
}

WeaponInstance *Mission::createWeaponInstance(uint8 * data)
{
    Mission::LEVELDATA_WEAPONS * gamdata =
        (Mission::LEVELDATA_WEAPONS *) data;
    Weapon::WeaponType wType = Weapon::Unknown;

    switch (gamdata->sub_type) {
        case 0x01:
            wType = Weapon::Persuadatron;
            break;
        case 0x02:
            wType = Weapon::Pistol;
            break;
        case 0x03:
            wType = Weapon::GaussGun;
            break;
        case 0x04:
            wType = Weapon::Shotgun;
            break;
        case 0x05:
            wType = Weapon::Uzi;
            break;
        case 0x06:
            wType = Weapon::Minigun;
            break;
        case 0x07:
            wType = Weapon::Laser;
            break;
        case 0x08:
            wType = Weapon::Flamer;
            break;
        case 0x09:
            wType = Weapon::LongRange;
            break;
        case 0x0A:
            wType = Weapon::Scanner;
            break;
        case 0x0B:
            wType = Weapon::MediKit;
            break;
        case 0x0C:
            wType = Weapon::TimeBomb;
            break;
        case 0x0D:
            wType = Weapon::AccessCard;
            break;
        case 0x11:
            wType = Weapon::EnergyShield;
            break;
        default:
            wType = Weapon::Unknown;
            break;
    }

    Weapon *pWeapon = g_App.weapons().getWeapon(wType);
    if (pWeapon) {
        WeaponInstance *wi = pWeapon->createInstance();
        wi->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
            READ_LE_UINT16(gamdata->mapposz) >> 7, gamdata->mapposx[0],
            gamdata->mapposy[0], gamdata->mapposz[0] & 0x7F);
        return wi;
    }

    return NULL;
}

/*!
* This function looks for blockers - statics, vehicles, peds, weapons
*/
void Mission::blockerExists(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
                            double *dist, MapObject** blockerObj)
{
    // TODO: calculating closest blocker first? (start point can be closer though)
    double inc_xyz[3];
    inc_xyz[0] = (endXYZ->x - startXYZ->x) / (*dist);
    inc_xyz[1] = (endXYZ->y - startXYZ->y) / (*dist);
    inc_xyz[2] = (endXYZ->z - startXYZ->z) / (*dist);
    toDefineXYZ copyStartXYZ = *startXYZ;
    toDefineXYZ copyEndXYZ = *endXYZ;
    toDefineXYZ blockStartXYZ;
    toDefineXYZ blockEndXYZ;
    double closest = -1;

    for (unsigned int i = 0; i < statics_.size(); i++) {
        MapObject * s_blocker = statics_[i];
        if (s_blocker->isIgnored())
            continue;
        if (s_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = s_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < vehicles_.size(); i++) {
        MapObject * v_blocker = vehicles_[i];
        if (v_blocker->isIgnored())
            continue;
        if (v_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = v_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < peds_.size(); i++) {
        MapObject * p_blocker = peds_[i];
        if (p_blocker->isIgnored())
            continue;
        if (p_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = p_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < weapons_.size(); i++) {
        MapObject * w_blocker = weapons_[i];
        if (w_blocker->isIgnored())
            continue;
        if (w_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = w_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    if (*blockerObj != NULL) {
        *startXYZ = blockStartXYZ;
        *endXYZ = blockEndXYZ;
        *dist = closest;
    }
}

/*!
* \return mask where bits are:
* 0b - target in range(1); 1b - blocker is object, "t" is set(2)
* 2b - blocker object, "pn" is set(4), 3b - reachable point set,
* 4b - blocker tile, "pn" is set(16), 5b - out of visible reach(32)
* NOTE: only if "pn" or "t" are not null, variables are set
*/
uint8 Mission::inRangeCPos(toDefineXYZ * cp, ShootableMapObject ** t,
    PathNode * pn, bool setBlocker, bool checkTileOnly, double maxr,
    double * distTo)
{
    // TODO: some objects mid point is higher then map z
    assert(maxr >= 0);

    int cx = (*cp).x;
    int cy = (*cp).y;
    int cz = (*cp).z;
    if (cz > (mmax_z_ - 1) * 128)
        return 32;
    int tx = 0;
    int ty = 0;
    int tz = 0;
    if (t && *t) {
        tx = (*t)->tileX() * 256 + (*t)->offX();
        ty = (*t)->tileY() * 256 + (*t)->offY();
        tz = (*t)->tileZ() * 128 + (*t)->offZ() + ((*t)->sizeZ() >> 1);
    } else {
        tx = pn->tileX() * 256 + pn->offX();
        ty = pn->tileY() * 256 + pn->offY();
        tz = pn->tileZ() * 128 + pn->offZ();
    }
    if (tz > (mmax_z_ - 1) * 128)
        return 32;

    double d = 0;
    d = sqrt((double)((tx - cx) * (tx - cx) + (ty - cy) * (ty - cy)
        + (tz - cz) * (tz - cz)));
    uint8 block_mask = 1;

    if (distTo)
        *distTo = d;
    if (d == 0)
        return block_mask;

    double sx = (double) cx;
    double sy = (double) cy;
    double sz = (double) cz;

    if (d >= maxr) {
        block_mask = 0;
        if (pn == NULL)
            return block_mask;
        if (t && *t) {
            tz -= 128;
            *t = NULL;
        }
        double dist_k = (double)maxr / d;
        tx = cx + (int)((tx - cx) * dist_k);
        ty = cy + (int)((ty - cy) * dist_k);
        tz = cz + (int)((tz - cz) * dist_k);
        block_mask = 8;
        if (setBlocker) {
            pn->setTileXYZ(tx / 256, ty / 256, tz / 128);
            pn->setOffXYZ(tx % 256, ty % 256, tz % 128);
        }
    }

    // NOTE: these values are less then 1, if they are incremented time
    // required to check range will be shorter less precise check, if
    // decremented longer more precise. Increment is (n * 8)
    double inc_x = ((tx - cx) * 8) / d;
    double inc_y = ((ty - cy) * 8) / d;
    double inc_z = ((tz - cz) * 8) / d;

    int oldx = cx / 256;
    int oldy = cy / 256;
    int oldz = cz / 128;
    double dist_close = d;

    while (dist_close > 16.0f) {
        int nx = (int)sx / 256;
        int ny = (int)sy / 256;
        int nz = (int)sz / 128;
        unsigned char twd = mtsurfaces_[nx + ny * mmax_x_
            + nz * mmax_m_xy].twd;
        if (oldx != nx || oldy != ny || oldz != nz
            || (twd >= 0x01 && twd <= 0x04))
        {
            if (!(twd == 0x00 || twd == 0x0C || twd == 0x10)) {
                bool is_blocked = false;
                int offz = (int)sz % 128;
                switch (twd) {
                    case 0x01:
                        if (offz <= (127 - (((int)sy % 256) >> 1)))
                            is_blocked = true;
                        break;
                    case 0x02:
                        if (offz <= (((int)sy % 256) >> 1))
                            is_blocked = true;
                        break;
                    case 0x03:
                        if (offz <= (((int)sx % 256) >> 1))
                            is_blocked = true;
                        break;
                    case 0x04:
                        if (offz <= (127 - (((int)sx % 256) >> 1)))
                            is_blocked = true;
                        break;
                    default:
                        is_blocked = true;
                }
                if (is_blocked) {
                    if (block_mask == 1)
                        block_mask = 16;
                    else
                        block_mask |= 16;
                    if (setBlocker) {
                        if (pn) {
                            sx -= inc_x;
                            sy -= inc_y;
                            sz -= inc_z;
                            pn->setTileXYZ((int)sx / 256, (int)sy / 256,
                                (int)sz / 128);
                            pn->setOffXYZ((int)sx % 256, (int)sy % 256,
                                (int)sz % 128);
                        }
                        break;
                    }
                    break;
                }
            }
            oldx = nx;
            oldy = ny;
            oldz = nz;
        }
        sx += inc_x;
        sy += inc_y;
        sz += inc_z;
        double dsx = sx - (double)tx;
        double dsy = sy - (double)ty;
        double dsz = sz - (double)tz;
        dist_close = sqrt(dsx * dsx + dsy * dsy + dsz * dsz);
    }
    if (checkTileOnly)
        return block_mask;

    toDefineXYZ startXYZ = {cx, cy, cz};
    toDefineXYZ endXYZ = {tx, ty, tz};
    MapObject *blockerObj = NULL;

    double dist_blocker = d;
    blockerExists(&startXYZ, &endXYZ, &dist_blocker, &blockerObj);

    if (blockerObj) {
        bool blockerObj_is_closer = false;
        if (block_mask != 1 && ((t && *t != blockerObj) || pn)) {
            int dcx = cx - (int)sx;
            int dcy = cy - (int)sy;
            int dcz = cz - (int)sz;
            if (dist_blocker
                    < sqrt((double)(dcx * dcx + dcy * dcy + dcz * dcz)))
            {
                blockerObj_is_closer = true;
            }
        }
        if (block_mask == 1 && !blockerObj_is_closer) {
            block_mask = 0;
            blockerObj_is_closer = true;
        }
        if (setBlocker) {
            if (pn && blockerObj_is_closer) {
                pn->setTileXYZ(startXYZ.x / 256, startXYZ.y / 256,
                    startXYZ.z / 128);
                pn->setOffXYZ(startXYZ.x % 256, startXYZ.y % 256,
                    startXYZ.z % 128);
                block_mask |= 4;
            }
            if (t) {
                if (blockerObj_is_closer) {
                    *t = (ShootableMapObject *)blockerObj;
                    block_mask |= 2;
                } else
                    *t = NULL;
            }
        } else if (blockerObj_is_closer) {
            if (t && *t) {
                if (*t != blockerObj)
                    block_mask |= 6;
                else
                    block_mask = 1;
            } else
                block_mask |= 6;
        }
    } else {
        if (setBlocker) {
            if (block_mask != 1 && t)
                *t = NULL;
        }
    }

    return block_mask;
}

void Mission::getInRangeAll(toDefineXYZ * cp,
   std::vector<ShootableMapObject *> & targets, uint8 mask,
   bool checkTileOnly, double maxr)
{
    if (mask & MapObject::mjt_Ped) {
        for (int i = 0; i < numPeds(); i++) {
            ShootableMapObject *p = ped(i);
            if (!p->isIgnored())
                if (inRangeCPos(cp, &p, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(p);
                }
        }
    }
    if (mask & MapObject::mjt_Static) {
        for (int i = 0; i < numStatics(); i++) {
            ShootableMapObject *st = statics(i);
            if (!st->isIgnored())
                if (inRangeCPos(cp, &st, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(st);
                }
        }
    }
    if (mask & MapObject::mjt_Vehicle) {
        for (int i = 0; i < numVehicles(); i++) {
            ShootableMapObject *v = vehicle(i);
            if (!v->isIgnored())
                if (inRangeCPos(cp, &v, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(v);
                }
        }
    }
    if (mask & MapObject::mjt_Weapon) {
        for (int i = 0; i < numWeapons(); i++) {
            ShootableMapObject *w = weapon(i);
            if (!w->isIgnored())
                if (inRangeCPos(cp, &w, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(w);
                }
        }
    }
}

bool Mission::getShootableTile(int &x, int &y, int &z, int &ox, int &oy,
    int &oz)
{
    bool gotit = false;
    int bx, by, box, boy;
    int bz = mmax_z_;
    unsigned char twd;
    unsigned int cindx;
    do {
        bz--;
        int bzm = bz - 1;
        bx = x * 256 + ox + 128 * bzm;
        box = bx % 256;
        bx = bx / 256;
        by = y * 256 + oy + 128 * bzm;
        boy = by % 256;
        by = by / 256;
        if (bz >= mmax_z_ || bx >= mmax_x_ || by >= mmax_y_)
            continue;
        if (bz < 0 || bx < 0 || by < 0)
            break;
        twd = mtsurfaces_[bx + by * mmax_x_ + bzm * mmax_m_xy].twd;
        int dx = 0;
        int dy = 0;
        switch (twd) {
            case 0x01:
                dy = (boy * 2) / 3;
                dx = box - dy / 2;
                if (dx >= 0) {
                    gotit = true;
                    box = dx;
                    boy = dy;
                } else {
                    if ((bx - 1) >= 0) {
                        cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                        if (mtsurfaces_[cindx].twd == 0x01) {
                            gotit = true;
                            bx--;
                            box = dx + 256;
                            boy = dy;
                        }
                    }
                }
                break;
            case 0x02:
                dy = (boy - 128) * 2;
                dx = (box + dy / 2) - 128;
                if (dy >= 0) {
                    if (dx >= 0) {
                        if (dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((bx + 1) < mmax_x_) {
                                cindx = (bx + 1) + by * mmax_x_ + bzm * mmax_m_xy;
                                if (mtsurfaces_[cindx].twd == 0x02) {
                                    gotit = true;
                                    bx++;
                                    box = dx - 256;
                                    boy = dy;
                                }
                            }
                        }
                    } else {
                        if ((bx - 1) >= 0) {
                            cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                            if (mtsurfaces_[cindx].twd == 0x02) {
                                gotit = true;
                                bx--;
                                box = dx + 256;
                                boy = dy;
                            }
                        }
                    }
                }
                break;
            case 0x03:
                dx = (box - 128) * 2;
                dy = (boy + dx / 2) - 128;
                if (dx >= 0) {
                    if (dy >= 0) {
                        if (dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((by + 1) < mmax_y_) {
                                cindx = bx + (by + 1) * mmax_x_ + bzm * mmax_m_xy;
                                if (mtsurfaces_[cindx].twd == 0x03) {
                                    gotit = true;
                                    by++;
                                    box = dx;
                                    boy = dy - 256;
                                }
                            }
                        }
                    } else {
                        if ((by - 1) >= 0) {
                            cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                            if (mtsurfaces_[cindx].twd == 0x03) {
                                gotit = true;
                                by--;
                                box = dx;
                                boy = dy + 256;
                            }
                        }
                    }
                }
                break;
            case 0x04:
                dx = (box * 2) / 3;
                dy = boy - dx / 2;
                if (dy >= 0) {
                    gotit = true;
                    box = dx;
                    boy = dy;
                } else {
                    if ((by - 1) >= 0) {
                        cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                        if (mtsurfaces_[cindx].twd == 0x04) {
                            gotit = true;
                            by--;
                            box = dx;
                            boy = dy + 256;
                        }
                    }
                }
                break;
            default:
                if (!(twd == 0x00 || twd == 0x0C || twd == 0x10))
                    gotit = true;
            break;
        }
       if (!gotit) {
            if (box < 128 && (bx - 1) >= 0) {
                cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                twd = mtsurfaces_[cindx].twd;
                if (twd == 0x01) {
                    dy = (boy * 2) / 3;
                    dx = (box + 256) - dy / 2;
                    if (dx < 256) {
                        bx--;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                } else if (twd == 0x04) {
                    dx = ((box + 256) * 2) / 3;
                    dy = boy - dx / 2;
                    if (dy >= 0) {
                        bx--;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                }
            }
            if (!gotit && boy < 128 && (by - 1) >= 0) {
                cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                twd = mtsurfaces_[cindx].twd;
                if (twd == 0x01) {
                    dy = ((boy + 256) * 2) / 3;
                    dx = box - dy / 2;
                    if (dx >= 0) {
                        by--;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                } else if (twd == 0x04) {
                    dx = (box * 2) / 3;
                    dy = (boy + 256) - dx / 2;
                    if (dy < 256) {
                        by--;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                }
                if(!gotit && box < 128 && (bx - 1) >= 0) {
                    cindx--;
                    int dx2 = 0;
                    int dy2 = 0;
                    twd = mtsurfaces_[cindx].twd;
                    if (twd == 0x01) {
                        dy2 = ((boy + 256) * 2) / 3;
                        dx2 = (box + 256) - dy2 / 2;
                        if (dx2 < 256 && dy2 < 256) {
                            bx--;
                            by--;
                            gotit = true;
                            box = dx2;
                            boy = dy2;
                        }
                    } else if (twd == 0x04) {
                        dx2 = ((box + 256) * 2) / 3;
                        dy2 = (boy + 256) - dx2 / 2;
                        if (dx2 < 256 && dy2 < 256) {
                            bx--;
                            by--;
                            gotit = true;
                            box = dx2;
                            boy = dy2;
                        }
                    }
                }
            }
       }
    } while (bz != 0 && !gotit);
    if (gotit) {
        twd = mtsurfaces_[bx + by * mmax_x_ + (bz - 1) * mmax_m_xy].twd;
        switch (twd) {
            case 0x01:
                oz = 127 - (boy >> 1);
                bz--;
                break;
            case 0x02:
                oz = boy >> 1;
                bz--;
                break;
            case 0x03:
                oz = box >> 1;
                bz--;
                break;
            case 0x04:
                oz = 127 - (box >> 1);
                bz--;
                break;
            default:
                oz = 0;
        }
        x = bx;
        y = by;
        z = bz;
        ox = box;
        oy = boy;
        assert(bz >= 0);
    }
    return gotit;
}

bool Mission::isTileSolid(int x, int y, int z, int ox, int oy, int oz) {
    bool solid = true;
    uint8 twd = mtsurfaces_[x + y * mmax_x_ + z * mmax_m_xy].twd;
    switch (twd) {
        case 0x00:
        case 0x0C:
        case 0x10:
            solid = false;
            break;
        case 0x01:
            if (oz > (127 - (oy >> 1)))
                solid = false;
            break;
        case 0x02:
            if (oz > (oy >> 1))
                solid = false;
            break;
        case 0x03:
            if (oz > (ox >> 1))
                solid = false;
            break;
        case 0x04:
            if (oz > (127 - (ox >> 1)))
                solid = false;
            break;
        default:
            break;
    }

    return solid;
}
