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
#include <assert.h>

#include "missionmanager.h"
#include "app.h"
#include "utils/file.h"
#include "utils/log.h"
#include "resources.h"
#include "core/gamecontroller.h"
#include "core/missionbriefing.h"
#include "model/objectivedesc.h"
#include "vehicle.h"
#include "mission.h"
#include "core/squad.h"
#include "pedmanager.h"

class LoadMissionException : public std::exception
{
public:
    LoadMissionException( const char * Msg )
    {
        this->msg = Msg;
    }
 
    virtual ~LoadMissionException() throw()
    {
 
    }
 
    virtual const char * what() const throw()
    {
        return this->msg.c_str();
    }
 
private:
    std::string msg;
};

MissionManager::MissionManager()
{
}

/*!
 * Loads the briefing for the given mission id.
 * \param n Mission id
 * \return NULL if mission could not be loaded
 */
MissionBriefing *MissionManager::loadBriefing(int n) {
    char tmp[100];
    // Briefing file depends on the current language
    switch(g_Ctx.currLanguage()) {
        case AppContext::ENGLISH:
            sprintf(tmp, MISSION_PATTERN_EN, n);
            break;
        case AppContext::FRENCH:
            sprintf(tmp, MISSION_PATTERN_FR, n);
            break;
        case AppContext::ITALIAN:
            sprintf(tmp, MISSION_PATTERN_IT, n);
            break;
        case AppContext::GERMAN:
            sprintf(tmp, MISSION_PATTERN_GE, n);
            break;
    }
    int size;
    uint8 *data = File::loadOriginalFile(tmp, size);
    if (data == NULL) {
        return NULL;
    }

    // Create Briefing
    MissionBriefing *p_mb = new MissionBriefing();
    // Loads briefing text
    if (!p_mb->loadBriefing(data, size)) {
        delete[] data;
        delete p_mb;
        return NULL;
    }
    delete[] data;

    // Loads the mission to get the minimap
    LevelData::LevelDataAll level_data;
    if (load_level_data(n, level_data)) {
        uint16 map_id = READ_LE_UINT16(level_data.mapinfos.map);
        Map *p_map = g_App.maps().loadMap(map_id);
        if (p_map == NULL) {
            delete p_mb;
            return NULL;
        }
        p_mb->init_minimap(p_map, level_data);
    }

    return p_mb;
}

#define copydata(x, y) memcpy(&level_data.x, data + y, sizeof(level_data.x))

/*!
 * Loads a mission.
 * \param n Mission id.
 * \return NULL if Mission could not be loaded.
 */
Mission *MissionManager::loadMission(int n)
{
    LOG(Log::k_FLG_IO, "MissionManager", "loadMission()", ("loading mission %i", n));

    // Initialize LevelData structure from data read in file
    LevelData::LevelDataAll level_data;
    if (load_level_data(n, level_data)) {

        Mission *m = create_mission(level_data);

        if (m) {
            Map *p_map = g_App.maps().loadMap(m->mapId());
            if (p_map == NULL) {
                delete m;
                return NULL;
            }
            m->set_map(p_map);
            m->setSurfaces();
        }

        return m;
    }

    return NULL;
}

/*!
 * Creates a Mission object from the LevelDataAll structure and fills the overlay for the
 * briefing minimap.
 */
bool MissionManager::load_level_data(int n, LevelData::LevelDataAll &level_data) {
    char tmp[100];
    int size;
    
    sprintf(tmp, GAME_PATTERN, n);
    uint8 *data = File::loadOriginalFile(tmp, size);
    if (data == NULL) {
        return false;
    }

#if 1
    hackMissions(n, data);
#endif

    // Initialize LevelData structure from data read in file
    memset(&level_data, 0, sizeof(level_data));
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

    return true;
}

/*!
 *
 */
void MissionManager::hackMissions(int n, uint8 *data) {
    // Kenya map, adding additional walking points to scenarios
    if (n == 40) {
        uint8 *scen_start = data + 97128 + 8 * 86;
        // coord offsets changing for next point
        scen_start[4] = ((scen_start[4] & 0xFE) | 1);
        scen_start[5] = (scen_start[5] & 0xFE);

        scen_start = data + 97128 + 8 * 87;
        WRITE_LE_UINT16(scen_start, 90 * 8);
        scen_start = data + 97128 + 8 * 90;
        WRITE_LE_UINT16(scen_start, 91 *8);
        scen_start += 4;
        // coords x = 72,ox = 128, y = 32, oy = 128, z = 2
        scen_start[0] = (72 << 1) | 1;
        scen_start[1] = (32 << 1) | 1;
        // type 1 = reach location
        scen_start[3] = 1;

        scen_start += 4;
        WRITE_LE_UINT16(scen_start, 92 *8);
        scen_start += 4;
        // coords x = 61,ox = 0, y = 32, oy = 0, z = 2
        scen_start[0] = (62 << 1) | 1;
        scen_start[1] = (57 << 1) | 1;
        // type 1 = reach location
        scen_start[3] = 1;

        scen_start += 4;
        WRITE_LE_UINT16(scen_start, 88 *8);
        scen_start += 4;
        // coords x = 42,ox = 0, y = 59, oy = 0, z = 2
        scen_start[0] = (42 << 1) | 1;
        scen_start[1] = (59 << 1);
        // type 1 = reach location
        scen_start[3] = 1;
    }
}

void MissionManager::exportMissionData(LevelData::LevelDataAll &level_data, Mission *pMission) {

#if 0
    // for hacking vehicles data
    char nameSv[256];
    sprintf(nameSv, "vehicles%02X.hex", pMission->mapId());
    FILE *staticsFv = fopen(nameSv,"wb");
    if (staticsFv) {
        fwrite(level_data.cars, 1, 42*64, staticsFv);
        fclose(staticsFv);
    }
#endif

#if 0
    // for hacking map data
    char nameSmap[256];
    sprintf(nameSmap, "map%02X.hex", pMission->mapId());
    FILE *staticsFmap = fopen(nameSmap,"wb");
    if (staticsFmap) {
        fwrite(level_data.map.objs, 1, 32768, staticsFmap);
        fclose(staticsFmap);
    }

#endif
    
#if 0
    // for hacking statics data
    char nameSs[256];
    sprintf(nameSs, "statics%02X.hex", pMission->mapId());
    FILE *staticsFs = fopen(nameSs,"wb");
    if (staticsFs) {
        fwrite(level_data.statics, 1, 12000, staticsFs);
        fclose(staticsFs);
    }
#endif

#if 0
    // for hacking weapons data
    char nameSw[256];
    sprintf(nameSw, "weapons%02X.hex", pMission->mapId());
    FILE *staticsFw = fopen(nameSw,"wb");
    if (staticsFw) {
        fwrite(level_data.weapons, 1, 36*512, staticsFw);
        fclose(staticsFw);
    }
#endif

#if 0
    // for hacking objectives data
    char nameSo[256];
    sprintf(nameSo, "obj%02X.hex", pMission->mapId());
    FILE *staticsFo = fopen(nameSo,"wb");
    if (staticsFo) {
        fwrite(level_data.objectives, 1, 140, staticsFo);
        fclose(staticsFo);
    }
#endif

#if 0
    // for hacking scenarios data
    char nameSss[256];
    sprintf(nameSss, "scenarios%02X.hex", pMission->mapId());
    FILE *staticsFss = fopen(nameSss,"wb");
    if (staticsFss) {
        fwrite(level_data.scenarios, 1, 2048 * 8, staticsFss);
        fclose(staticsFss);
    }
#endif
}

/*!
 * Creates a Mission object from the LevelDataAll structure.
 */
Mission * MissionManager::create_mission(LevelData::LevelDataAll &level_data) {
    Mission *p_mission = new Mission(level_data.mapinfos);

    // Init indexes
    DataIndex di;
    memset(di.vindx, 0xFF, 2*64);
    memset(di.pindx, 0xFF, 2*256);
    memset(di.driverindx, 0xFF, 2*256);
    memset(di.windx, 0xFF, 2*512);

    try {
        createVehicles(level_data, di, p_mission);
    
        createPeds(level_data, di, p_mission);

        for (uint16 i = 0; i < 400; i++) {
            LevelData::Statics & sref = level_data.statics[i];
            if(sref.desc == 0)
                continue;
            Static *s = Static::loadInstance((uint8 *) & sref, p_mission->mapId());
            if (s) {
                p_mission->addStatic(s);
            }
        }
    
        createWeapons(level_data, di, p_mission);
    
        createObjectives(level_data, di, p_mission);

#ifdef SHOW_SCENARIOS_DEBUG
    for (uint16 i = 1; i < 2047; i++) {
        LevelData::Scenarios & scenario = level_data.scenarios[i];
        if (scenario.type == 0)
            break;
        else {
            printf("num %i\n", i);
            printf("next %i\n", READ_LE_INT16(scenario.next) / 8);
            printf("object offset 0x%X\n", READ_LE_INT16(scenario.offset_object));
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
        p_mission->addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
        p_mission->addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
        p_mission->addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
        p_mission->addSfxObject(new SFXObject(-1, SFXObject::sfxt_SelArrow));
        p_mission->addSfxObject(new SFXObject(p_mission->mapId(), SFXObject::sfxt_AgentFirst));
        p_mission->addSfxObject(new SFXObject(p_mission->mapId(), SFXObject::sfxt_AgentSecond));
        p_mission->addSfxObject(new SFXObject(p_mission->mapId(), SFXObject::sfxt_AgentThird));
        p_mission->addSfxObject(new SFXObject(p_mission->mapId(), SFXObject::sfxt_AgentFourth));

        return p_mission;

    } catch (const LoadMissionException & ex) {
        FSERR(Log::k_FLG_GAME, "Mission", "loadMission", ("Failed to load mission %s\n", ex.what()));
        if (p_mission) {
            delete p_mission;
        }
        return NULL;
    }
}

void MissionManager::createWeapons(const LevelData::LevelDataAll &level_data, DataIndex &di, Mission *pMission) {
    for (uint16 i = 0; i < 512; i++) {
        const LevelData::Weapons & wref = level_data.weapons[i];
        if(wref.desc == 0)
            continue;
        WeaponInstance *w = create_weapon_instance(wref);
        if (w) {
            if (wref.desc == 0x05) {
                uint16 offset_owner = READ_LE_UINT16(wref.offset_owner);
                if (offset_owner != 0) {
                    offset_owner = (offset_owner - 2) / 92; // 92 = ped data size
                    if (offset_owner > 7 && di.pindx[offset_owner] != 0xFFFF) {
                        // TODO: still there is a problem of weapons setup
                        // some police officers can have more then 1 weapon
                        // others none (pacific Rim)
                        pMission->ped(di.pindx[offset_owner])->addWeapon(w);
                        w->setOwner(pMission->ped(di.pindx[offset_owner]));
                        di.windx[i] = pMission->numWeapons();
                        pMission->addWeapon(w);
                    } else {
                        delete w;
                    }
                } else {
                    delete w;
                }
            } else {
                w->setMap(pMission->mapId());
                w->setOwner(NULL);
                di.windx[i] = pMission->numWeapons();
                pMission->addWeapon(w);
            }
        }
    }
}

WeaponInstance * MissionManager::create_weapon_instance(const LevelData::Weapons &gamdata) {
    Weapon::WeaponType wType = Weapon::Unknown;

    switch (gamdata.sub_type) {
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

    Weapon *pWeapon = g_gameCtrl.weapons().getWeapon(wType);
    if (pWeapon) {
        WeaponInstance *wi = WeaponInstance::createInstance(pWeapon);
        wi->setPosition(gamdata.mapposx[1], gamdata.mapposy[1],
            READ_LE_UINT16(gamdata.mapposz) >> 7, gamdata.mapposx[0],
            gamdata.mapposy[0], gamdata.mapposz[0] & 0x7F);
        return wi;
    }

    return NULL;
}


void MissionManager::createVehicles(const LevelData::LevelDataAll &level_data, DataIndex &di, Mission *pMission) {
    for (uint8 i = 0; i < 64; i++) {
        const LevelData::Cars & car = level_data.cars[i];
        // car.sub_type 0x09 - train
        if (car.type == 0x0)
            continue;
        VehicleInstance *v =
            createVehicleInstance(car, pMission->mapId());
        if (v) {
            di.vindx[i] = pMission->numVehicles();
            pMission->addVehicle(v);
            if (car.offset_of_driver != 0 && ((car.offset_of_driver - 2) / 92 + 2) * 92
                == car.offset_of_driver)
            {
                di.driverindx[(car.offset_of_driver - 2) / 92] = di.vindx[i];
            }
        }
    }
}
/*!
 *
 */
VehicleInstance * MissionManager::createVehicleInstance(const LevelData::Cars &gamdata, uint16 map)
{
    // TODO: check all maps
    // TODO: train, join somehow

    int hp = READ_LE_INT16(gamdata.health);
    int dir = gamdata.orientation >> 5;

    VehicleAnimation *vehicleanim = new VehicleAnimation();
    int cur_anim = READ_LE_UINT16(gamdata.index_current_anim) - dir;
    //setVehicleBaseAnim(vehicleanim, cur_anim);
    vehicleanim->set_base_anims(cur_anim);
    VehicleInstance *vehicle_new = new VehicleInstance(vehicleanim, map);
    vehicle_new->setHealth(hp);
    vehicle_new->setStartHealth(hp);
    vehicle_new->setType(gamdata.sub_type);
    switch (gamdata.sub_type) {
        case 0x01:
            // large armored
            break;
        case 0x04:
            // large armored damaged
            // it is actually base animation and they have 8 directions
            //setVehicleBaseAnim(vehicleanim, cur_anim - 12 + (dir >> 1));
            vehicleanim->set_base_anims(cur_anim - 12 + (dir >> 1));
            vehicle_new->setStartHealth(0);
            vehicle_new->setHealth(-1);
            vehicle_new->setIsIgnored(true);
            vehicleanim->set_animation_type(VehicleAnimation::kBurntAnim);
            break;
        case 0x05:
            // train head
        case 0x09:
            // train body
            break;
        case 0x0D:
            // grey vehicle
            break;
        case 0x11:
            // firefighters vehicle
            break;
        case 0x1C:
            // small armored vehicle
            break;
        case 0x24:
            // police vehicle
            break;
        case 0x28:
            // medical vehicle
            break;
#if _DEBUG
        default:
            printf("uknown vehicle type %02X , %02X, %X\n", gamdata.sub_type,
                gamdata.orientation,
                READ_LE_UINT16(gamdata.index_current_frame));
            printf("x = %i, xoff = %i, ", gamdata.mapposx[1],
                gamdata.mapposx[0]);
            printf("y = %i, yoff = %i, ", gamdata.mapposy[1],
                gamdata.mapposy[0]);
            printf("z = %i, zoff = %i\n", gamdata.mapposz[1],
                gamdata.mapposz[0]);
            break;
#endif
    }
    int z = READ_LE_UINT16(gamdata.mapposz) >> 7;

    // TODO: the size should be adjusted on orientation/direction change
    // and it should be different per vehicle type
    vehicle_new->setSizeX(256);
    vehicle_new->setSizeY(256);
    vehicle_new->setSizeZ(192);

    int oz = gamdata.mapposz[0] & 0x7F;
    vehicle_new->setPosition(gamdata.mapposx[1], gamdata.mapposy[1],
                            z, gamdata.mapposx[0],
                            gamdata.mapposy[0], oz);
    vehicle_new->setDirection(gamdata.orientation);

    return vehicle_new;
}

void MissionManager::createPeds(const LevelData::LevelDataAll &level_data, DataIndex &di, Mission *pMission) {

#if 0
    // for hacking peds data
    char nameSp[256];
    sprintf(nameSp, "peds%02X.hex", p_mission->mapId());
    FILE *staticsFp = fopen(nameSp,"wb");
    if (staticsFp) {
        fwrite(level_data.people, 1, 256*92, staticsFp);
        fclose(staticsFp);
    }
#endif
#ifdef _DEBUG
    std::map <uint32, std::string> obj_ids;
    // NOTE: not very useful way of remembering "Who is who"
    obj_ids[0] = "Undefined";
    obj_ids[pMission->playersGroupID()] = "Players Agents or Persuaded";
    obj_ids[2] = "Enemy Agents";
    obj_ids[3] = "Enemy Guards";
    obj_ids[4] = "Policemen";
    obj_ids[5] = "Civilians";
#endif
    ModOwner mods_enemy;
    // enemies get top version of mods
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_LEGS));
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_ARMS));
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_CHEST));
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_HEART));
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_EYES));
    mods_enemy.addMod(g_gameCtrl.mods().getHighestVersion(Mod::MOD_BRAIN));
    
    PedManager peds;
    for (uint16 i = 0; i < 256; i++) {
        const LevelData::People & pedref = level_data.people[i];
        
        PedInstance *p =
            peds.loadInstance(pedref, i, pMission->mapId());
        if (p) {
            if (pedref.location == LevelData::kPeopleLocInVehicle) {
                uint16 vid = 0xFFFF;  // Id of the vehicle
                bool setDriver = false;  // Tells if ped should be the driver
                if (di.driverindx[i] != 0xFFFF) {
                    // Current ped is the driver
                    vid = di.driverindx[i];
                    setDriver = true;
                } else {
                    // Current ped is just a passenger
                    uint16 vin = READ_LE_UINT16(pedref.offset_of_vehicle);
                    if (vin != 0) {
                        vin = (vin - 0x5C02) / 42; // 42 vehicle data size
                        vid = di.vindx[vin];
                    }
                }

                if(vid == 0xFFFF) {
                    throw LoadMissionException("Vehicle not found");
                }
                Vehicle *pVehicle = pMission->vehicle(vid);

                pVehicle->addPassenger(p);
                if (setDriver) {
                    VehicleInstance *pCar = dynamic_cast<VehicleInstance *>(pVehicle);
                    pCar->forceSetDriver(p);
                }
            }
            di.pindx[i] = pMission->numPeds();
            pMission->addPed(p);

            if (i < AgentManager::kMaxSlot) {
                // We're loading one of our agents
                Agent *pAg = g_gameCtrl.agents().squadMember(i);
                p->initAsAgent(pAg, pMission->playersGroupID());
                // adds all agent's weapons to the mission weapons
                for (int wi=0; wi<p->numWeapons(); wi++) {
                    pMission->addWeapon(p->weapon(wi));
                }
                // adds the agent to the mission squad
                pMission->getSquad()->setMember(i, p);
            } else {
                unsigned int mt = p->type();
                p->setObjGroupDef(mt);
                if (mt == PedInstance::og_dmAgent) {
                    p->setObjGroupID(2);
                    p->addEnemyGroupDef(1);
                    p->setBaseSpeed(256);
                    *((ModOwner *)p) = mods_enemy;
                    p->setTimeBeforeCheck(400);
                    p->setBaseModAcc(0.5);
                    p->setPersuasionPoints(32);
                } else if (mt == PedInstance::og_dmGuard) {
                    p->setObjGroupID(3);
                    p->addEnemyGroupDef(1);
                    p->setBaseSpeed(192);
                    p->setTimeBeforeCheck(300);
                    p->setBaseModAcc(0.45);
                    p->setPersuasionPoints(4);
                } else if (mt == PedInstance::og_dmPolice) {
                    p->setObjGroupID(4);
                    p->setHostileDesc(PedInstance::pd_smArmed);
                    p->setBaseSpeed(160);
                    p->setTimeBeforeCheck(400);
                    p->setBaseModAcc(0.4);
                    p->setPersuasionPoints(8);
                } else if (mt == PedInstance::og_dmCivilian) {
                    p->setObjGroupID(5);
                    p->addEnemyGroupDef(6);
                    p->setHostileDesc(PedInstance::pd_smArmed);
                    p->setBaseSpeed(128);
                    p->setTimeBeforeCheck(600);
                    p->setBaseModAcc(0.2);
                    p->setPersuasionPoints(1);
                } else if (mt == PedInstance::og_dmCriminal) {
                    p->setObjGroupID(6);
                    p->setBaseSpeed(128);
                    p->setTimeBeforeCheck(500);
                    p->setBaseModAcc(0.2);
                    p->setPersuasionPoints(1);
                }
                p->setSightRange(7 * 256);
                // TODO: set scenarios
                
                uint16 offset_start = READ_LE_UINT16(pedref.offset_scenario_start);
                uint16 offset_nxt = offset_start;
                Vehicle *v = p->inVehicle();
                bool not_in_vehicle = true;
                if (v)
                    not_in_vehicle = false;
                if (offset_start)
                    p->dropActQ();
//#define SHOW_SCENARIOS_DEBUG
#ifdef SHOW_SCENARIOS_DEBUG
                printf("=====\n");
#endif
                if (p->getDebugID() == 17)
                    printf("Scenario for %d\n", i);

                PedInstance::actionQueueGroupType as;
                as.group_desc = PedInstance::gd_mStandWalk;
                as.origin_desc = fs_actions::kOrigScript;
                // this field will hold the index of a potential trigger
                int32 has_trigger = -1;

                while (offset_nxt) {
                    // sc.type
                    // 1 - walking/driving to pos, x,y defined
                    // 2 - vehicle to use and goto
                    // 3?(south africa)
                    // 5?(kenya)
                    // 6 (kenya) - ped offset when in vehicle, and? (TODO)
                    // 7 - assasinate target escaped, mission failed (TODO properly)
                    // 8 - walking to pos, triggers on our agents in range, x,y defined
                    // 9 - repeat from start, actually this might be end of script
                    // 10 - train stops and waits
                    // 11 - protected target reached destination(kenya) (TODO properly)
                    LevelData::Scenarios sc = level_data.scenarios[offset_nxt / 8];
#ifdef SHOW_SCENARIOS_DEBUG
                    printf("id = %i, sc.type = %i, nxt = %i\n", i, sc.type, offset_nxt / 8);
#endif
                    offset_nxt = READ_LE_UINT16(sc.next);
                    assert(offset_nxt != offset_start);

                    if (sc.tilex != 0 && sc.tiley != 0) {
                        // This scenario defines something that uses a location
                        PathNode pn(sc.tilex >> 1, sc.tiley >> 1, sc.tilez,
                            (sc.tilex & 0x01) << 7, (sc.tiley & 0x01) << 7);
                        if (sc.type == LevelData::kScenarioTypeTrigger) {
                            p->createActQTrigger(as, &pn, 6 * 256);
                            has_trigger = as.actions.size();
                            // no need for exclusive wait
                            p->createActQWait(as, 3000);
                            as.actions.back().group_desc = PedInstance::gd_mStandWalk;
                        }
                        if (v)
                            p->createActQUsingCar(as, &pn, v);
                        else
                            p->createActQWalking(as, &pn, NULL, p->direction(), 0, true);
                        //p->createActQWalking(as, &pn, NULL, -1);
                        if ((!not_in_vehicle) && offset_nxt == 0)
                            p->createActQResetActionQueue(as);
                    } else if (sc.type == 2) {
                        if (not_in_vehicle) {
                            uint16 bindx = READ_LE_UINT16(sc.offset_object);
                            // TODO: test all maps for objects other then vehicle
                            assert(bindx >= 0x5C02 && bindx < 0x6682);
                            bindx -= 0x5C02;
                            bindx /= 42;
                            if (di.vindx[bindx] != 0xFFFF) {
                                v = pMission->vehicle(di.vindx[bindx]);
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
                        // train will wait
                        p->createActQWait(as, 10000);
                        // resetting movement for train
                        if (offset_nxt == 0) {
                            p->createActQResetActionQueue(as);
                        }
                    }
#if 0
#ifdef _DEBUG
                    switch (sc.type) {
                        case 1:
                        case 2:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                            break;
                        default:
                            printf("Bingo\n");
                    }
#endif
#endif
                }
                if (as.actions.size() != 0) {
                    if (has_trigger != -1) {
                        // A trigger was defined, so all actions after it
                        // are paused until the trigger is set off.
                        p->pauseAllInActG(as, (uint32)has_trigger);
                        as.main_act = (uint32)has_trigger - 1;
                    } else
                        as.main_act = as.actions.size() - 1;
                    p->addActQToQueue(as);
                }
#ifdef SHOW_SCENARIOS_DEBUG
                printf("+++++\n");
#endif
            }
        }
    }
}

void MissionManager::createObjectives(const LevelData::LevelDataAll &level_data,
                                        DataIndex &di, Mission *pMission) {
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
        ObjectiveDesc *objd = NULL;

        const LevelData::Objectives & obj = level_data.objectives[i];
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
                    */
                }
            }
                break;
            case 0x01:
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && di.pindx[cindx] != 0xFFFF) {
                        PedInstance *p = pMission->ped(di.pindx[cindx]);
                        objd = new ObjPersuade(p);
                        p->setRcvDamageDef(MapObject::ddmg_PedPanicImmune);
                        // Adds the ped to the list of peds to evacuate
                        peds_evacuate.push_back(p);
                    } else
                        printf("0x01 incorrect offset");
                } else
                    printf("0x01 type not matched");
                break;
            case 0x02: // Assassinate a civilian
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && di.pindx[cindx] != 0xFFFF) {
                        PedInstance *p = pMission->ped(di.pindx[cindx]);
                        p->setRcvDamageDef(MapObject::ddmg_PedPanicImmune);
                        objd = new ObjAssassinate(p);
                    } else
                        printf("0x02 incorrect offset");
                } else
                    printf("0x02 type not matched");
                break;
            case 0x03:
                if (bindx > 0 && bindx < 0x5C02) {
                    cindx = (bindx - 2) / 92;
                    if ((cindx * 92 + 2) == bindx && di.pindx[cindx] != 0xFFFF) {
                        PedInstance *p = pMission->ped(di.pindx[cindx]);
                        p->setRcvDamageDef(MapObject::ddmg_PedPanicImmune);
                        objd = new ObjProtect(p);
                    } else
                        printf("0x03 incorrect offset");
                } else
                    printf("0x03 type not matched");
                break;
            case 0x05:
                if (bindx >= 0x9562 && bindx < 0xDD62) {
                    bindx -= 0x9562;
                    cindx = bindx / 36;
                    if ((cindx * 36) == bindx && di.windx[cindx] != 0xFFFF) {
                        objd = new ObjTakeWeapon(pMission->weapon(di.windx[cindx]));
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
                    if ((cindx * 42) == bindx && di.vindx[cindx] != 0xFFFF) {
                        objd = new ObjDestroyVehicle(pMission->vehicle(di.vindx[cindx]));
                    } else
                        printf("0x0E incorrect offset");
                } else
                    printf("0x0E type not matched");

                break;
            case 0x0F:
                if (bindx >= 0x5C02 && bindx < 0x6682) {
                    bindx -= 0x5C02;
                    cindx = bindx / 42;
                    if ((cindx * 42) == bindx && di.vindx[cindx] != 0xFFFF) {
                        objd = new ObjUseVehicle(pMission->vehicle(di.vindx[cindx]));
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
            pMission->addObjective(objd);
        } else {
            // TODO add a better error handling
            break;
        }
    }

    // Clear temp list
    peds_evacuate.clear();
}