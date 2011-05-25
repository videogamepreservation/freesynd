/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#ifndef MISSION_H
#define MISSION_H

#include "common.h"
#include "mapobject.h"
#include "pathsurfaces.h"
#include <string>
#include <vector>
#include <set>

class VehicleInstance;
class PedInstance;
class WeaponInstance;
class Static;
class MapHelper;

/*!
 * A structure that holds mission statistics.
 */
typedef struct {
    /*! How many agents participated in the mission. */
    int agents;
    /*! How many time did the mission last. */
    int mission_duration;
    /*! How many opposing agents where captured.*/
    int agentCaptured;
    /*! How many opposing agents where killed.*/
    int enemyKilled;
    /*! How many criminal where killed.*/
    int criminalKilled;
    /*! How many civilian where killed.*/
    int civilKilled;
    /*! How many policemen where killed.*/
    int policeKilled;
    /*! How many guards where killed.*/
    int guardKilled;
    /*! How many people where convinced.*/
    int convinced;
    /*! How many times did agents shoot.*/
    int nbOfShots;
    /*! How many times did agents hit.*/
    int nbOfHits;
} MissionStats;

/*!
 * Contains information read from original mission data file.
 */
class Mission : public MapHelper {
public:
#define MAX_INFO_ENHANCE_LVL    10
    /*!
     * List of all possible mission status. 
     */
    enum Status {
        RUNNING = 0,
        ABORTED = 1,
        FAILED = 2,
        COMPLETED = 3
    };

    Mission();
    virtual ~Mission();
    bool loadMission(uint8 *missData, int size);
    bool loadLevel(uint8 *levelData);
    bool loadMap();

    int map() { return map_; }

    int mapWidth();
    int mapHeight();
    int startX();
    int startY();

    int minX() { return min_x_; }
    int minY() { return min_y_; }
    int maxX() { return max_x_; }
    int maxY() { return max_y_; }

    int minScreenX();
    int minScreenY();
    int maxScreenX();
    int maxScreenY();
    void drawMap(int scrollx, int scrolly);

    //--- MapHelper stuff
    virtual void drawAt(int tilex, int tiley, int tilez, int x, int y,
            int scrollX, int scrollY);
    virtual void createFastKeys(int tilex, int tiley,
        int maxtilex, int maxtiley);
    //---

    const char *briefing() { return briefing_.c_str(); }
    void objectiveMsg(const char ** msg);

    int infoCost(unsigned char lvl) {
        assert(lvl < MAX_INFO_ENHANCE_LVL);
        return info_costs_[lvl];
    }

    int enhanceCost(unsigned char lvl) {
        assert(lvl < MAX_INFO_ENHANCE_LVL);
        return enhance_costs_[lvl];
    }

    int numPeds() { return (int) peds_.size(); }
    PedInstance *ped(int i) { return peds_[i]; }
    int numVehicles() { return (int) vehicles_.size(); }
    VehicleInstance *vehicle(int i) { return vehicles_[i]; }

    int numWeapons() { return (int) weapons_.size(); }
    WeaponInstance *weapon(int i) { return weapons_[i]; }
    int numStatics() { return (int) statics_.size(); }
    Static *statics(int i) { return statics_[i]; }
    /*! Return the mission statistics. */
    MissionStats *getStatistics() { return &stats_; }

    void start();
    //! Forces mission status
    void setStatus(Status status) { status_ = status; }
    //! Returns mission status
    Status getStatus() { return status_; }
    //! Check if objectives are completed or failed
    void checkObjectives();
    //! Returns true if mission status is failed
    bool failed() { return status_ == FAILED; }
    //! Returns true if mission status is completed
    bool completed() { return status_ == COMPLETED; }
    void end();

    void addWeapon(WeaponInstance *w);
    MapObject * findAt(int tilex, int tiley, int tilez,
        MapObject::MajorTypeEnum *majorT, int *searchIndex, bool only);
    bool setSurfaces();
    void clrSurfaces();
    bool getWalkable(int &x, int &y, int &z, int &ox, int &oy);
    void adjXYZ(int &x, int &y, int &z);
    void blockerExists(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
        double dist, MapObject** blockerObj);

    typedef struct {
        uint8 unkn10[6];
    } LEVELDATA_UNKN01;         // total: 8 bytes

    typedef struct {// was not able to verify correctness
        // 128*128 offsets size 2 bytes(LE)
        uint8 objs[32768];
    } LEVELDATA_MAP;            // total: 32768 bytes

    // This structure contains all people in the game, including agents
    // This struct size is 92.
    // most 2 byte variable have little-endianess byte order(LE)
    typedef struct {
        // (LE data)'offset + 32774' gives the offset in this
        // file of the next object
        uint8 offset_next[2];
        // (LE data)'offset + 32774' gives the offset in this
        // file of the previous object (sometimes weapon, or
        // the next target for example ???)
        uint8 offset_prev[2];
        // [0] - offset, [1] - tile
        uint8 mapposx[2];
        uint8 mapposy[2];
        // to convert to our system use as uint16(LE)
        // tile = (uint16)/128, offz =(uint16)%128 or offz = mapposz[0] & 0x1F
        uint8 mapposz[2];
        //0x04 - ped on map
        //0x05 - ped not on map(driving)
        //0x0D - they are not visible/present on original map(on water located), purpose?
        //0x0C - located level above possible walking surface, purpose?
        //0x0D and 0x0C are excluded from being loaded
        uint8 desc;
        // 0x0 - standing, 0x10 - walking, 0x11 - dead
        uint8 state;
        uint8 unkn3[2];         // nothing changes when this changes
        uint8 index_base_anim[2];  //index in (HSTA-0.ANI)
        uint8 index_current_frame[2];   //index in (HFRA-0.ANI)
        uint8 index_current_anim[2]; // index in (HSTA-0.ANI)
        uint8 health[2];
        uint8 offset_last_enemy[2];
        //0x01 ped; 0x02 vehicle; 0x04 weapon;
        //0x05 object; allow to display a target, a pickup, and for minimap
        uint8 type;
        uint8 status; // this can be sub type(?)
        //from 0xF0 to 0x10 : south = 0
        //from 0x10 to 0x30 : south-east = 1
        //from 0x30 to 0x50 : east = 2
        //from 0x50 to 0x70 : east-north = 3
        //from 0x70 to 0x90 : north = 4
        //from 0x90 to 0xB0 : north-west = 5
        //from 0xB0 to 0xD0 : west = 6
        //from 0xD0 to 0xF0 : west-south = 7
        // surface is mapped not to 360 degrees/surface, but 256 degrees/surface
        uint8 orientation;
        uint8 unkn4;
        // when 01 pedestrian, 02 agent, 04 police, 08 guard, 16 criminal
        uint8 type_ped;
        uint8 unkn5[3];
        uint8 offset_of_persuader[2];
        uint8 unkn6[2];
        uint8 offset_of_vehicle[2];
        uint8 offset_scenario_1[2];
        uint8 offset_scenario_2[2];
        uint8 unkn7[2];
        uint8 offset_of_vehicle_2[2]; // ??
        uint8 goto_mapposx[2];
        uint8 goto_mapposy[2];
        uint8 goto_mapposz[2];
        uint8 unkn8[6];
        uint8 offset_equipment[2];
        //bitmask, 0b - gender, 1-2b - leg, 3-4b - arm, 5-6b - chest
        // 7-8b - heart, 9-10b - eye, 11-12b - brain, 13-15b - unknown
        uint8 mods_info[2];
        uint8 unkn9[6];
        uint8 offset_cur_weapon[2];
        // IPA levels: white bar level,set level,exhaused level and forced level
        uint8 unkn10;
        uint8 adrena_amount;
        uint8 adrena_dependency;
        uint8 adrena_effect;
        uint8 unkn11;
        uint8 inteli_amount;
        uint8 inteli_dependency;
        uint8 inteli_effect;
        uint8 unkn12;
        uint8 percep_amount;
        uint8 percep_dependency;
        uint8 percep_effect;
        uint8 unkn13;
        uint8 unkn14[9];
    } LEVELDATA_PEOPLE;         // total: 92 bytes

    // This structure contains all cars in the game, including destroyed
    // This struct size is 42.
    typedef struct {
        uint8 offset_next[2];
        uint8 offset_prev[2];
        uint8 mapposx[2];
        uint8 mapposy[2];
        uint8 mapposz[2];
        // 0x04 on map(visible)
        // 0x05 not on map(hidden)
        uint8 desc;
        uint8 unkn1;
        uint8 unkn2;
        uint8 unkn3;
        uint8 index_base_anim[2];
        uint8 index_current_frame[2];
        uint8 index_current_anim[2];
        uint8 health[2];
        uint8 offset_last_enemy[2];
        uint8 type; // 0x02
        uint8 sub_type;
        uint8 orientation;
        uint8 unkn4;
        uint8 offset_of_driver; //driver
        uint8 unkn5[11];
        uint8 speed;
        uint8 unkn6;
    } LEVELDATA_CARS;           // total: 42 bytes

    typedef struct {
        uint8 offset_next[2];
        uint8 offset_prev[2];
        uint8 mapposx[2];
        uint8 mapposy[2];
        uint8 mapposz[2];
        // 0x04 - on map
        // 0x06 - on map, but why not 0x04?
        // 0x07 - on map(not sure), but why not 0x04?
        uint8 desc;
        uint8 unkn11;
        uint8 unkn12;
        uint8 unkn13;
        uint8 index_base_anim[2];
        uint8 index_current_frame[2];
        uint8 index_current_anim[2];
        uint8 unkn20[4];
        uint8 type;
        // more info mapobject.cpp
        uint8 sub_type;
        uint8 orientation;
        uint8 unkn27[3];
    } LEVELDATA_STATICS;        // total: 30 bytes

    typedef struct {
        uint8 offset_next[2];
        uint8 offset_prev[2];
        uint8 mapposx[2];;
        uint8 mapposy[2];;
        uint8 mapposz[2];;
        // 0x04 on map
        // 0x05 not on map(in inventory)
        uint8 desc;
        uint8 unkn2;
        uint8 unkn3;
        uint8 unkn4;
        uint8 index_base_anim[2];
        uint8 index_current_frame[2];
        uint8 index_current_anim[2];
        uint8 unkn5[4];
        // look peds for more info
        uint8 type; // 0x04
        // 0x01 persuadertron; 0x02 pistol; 0x03 gauss gun; 0x04 shotgun;
        // 0x05 uzi; 0x06 minigun; 0x07 laser; 0x08 flamer; 0x09 long range;
        // 0x0A scanner; 0x0B medikit; 0x0C time bomb; 0x0D access card;
        // 0x0E invalid; 0x0F invalid; 0x10 invalid; 0x11 energy shield;
        uint8 sub_type;
        uint8 unkn6[2];
        uint8 offset_next_inventory[2];
        uint8 offset_prev_inventory[2];
        uint8 offset_owner[2];
        uint8 unkn7[2];
    } LEVELDATA_WEAPONS;         // total: 36 bytes

    typedef struct {
        uint8 offset_next[2];
        uint8 offset_prev[2];
        uint8 mapposx[2];
        uint8 mapposy[2];
        uint8 mapposz[2];
        uint8 unkn1[4];
        uint8 index_base_anim[2];
        uint8 index_current_frame[2];
        uint8 index_current_anim[2];
        uint8 unkn2[8];
        uint8 offset_owner[2];
    } LEVELDATA_SFX;       // total: 30 bytes

    typedef struct {// was not able to verify correctness
        uint8 next[2];
        uint8 offset_object[2];
        // tile_x_ = (tilex < < 7) / 256
        // off_x_ = (tilex < < 7) % 256
        // tile_y_ = (tiley < < 7) / 256
        // off_y_ = (tiley < < 7) % 256
        // tile_z_ = tilez
        uint8 tilex;
        uint8 tiley;
        uint8 tilez;
        uint8 type;
    } LEVELDATA_SCENARIOS;         // total: 8 bytes

    typedef struct {
        uint8 unkn10[448];
    } LEVELDATA_UNKN09;         // total: 448 bytes

    typedef struct {
        uint8 map[2];
        uint8 min_x[2];
        uint8 min_y[2];
        uint8 max_x[2];
        uint8 max_y[2];
        //status flag is set to 1 if the mission has been successfully completed
        uint8 status;
        uint8 unkn1[3];
    } LEVELDATA_MAPINFOS;        // total: 14 bytes

    typedef struct {
        // only max 5 objectives are non-zero, we will read 6
        // 0x00 action for non-agent(?) ;0x01 persuade; 0x02 assassinate;
        // 0x03 protect; 0x05 equipment aquisition; 0x0a combat sweep (police);
        // 0x0b combat sweep; 0x0e destroy vehicle; 0x0f use vehicle;
        // 0x10 evacuate
        // more info in mission.cpp : loadLevel()
        uint8 type[2];
        // 'offset + 32774' gives the offset in this file of the first objective
        uint8 offset[2];
        uint8 mapposx[2];
        uint8 mapposy[2];
        uint8 mapposz[2];
        //If “protect”, the next objective are the goals and their type is zero.
        //The list finish with zero and the offset of the protected item ?
        //The status flag is set to 1 if the objective has to be completed
        uint8 status;
        uint8 unkn1[3];
    } LEVELDATA_OBJECTIVES;         // total: 14 bytes

    typedef struct {
        uint8 unkn10[1952];
    } LEVELDATA_UNKN11;         // total: 1896 bytes

    typedef struct {
        /*      0 */ LEVELDATA_UNKN01 u01;
        /*      6 */ LEVELDATA_MAP map;
        /*  32774 0x0*/ uint8 offset_ref[2];
        /*  32776 0x02*/ LEVELDATA_PEOPLE people[256];
        /*  56328 0x5C02*/ LEVELDATA_CARS cars[64];
        /*  59016 0x6682*/ LEVELDATA_STATICS statics[400];
        /*  71016 0x9562*/ LEVELDATA_WEAPONS weapons[512];
        /*  89448 0xDD62*/ LEVELDATA_SFX sfx[256];
        /*  97128 0xFB62*/ LEVELDATA_SCENARIOS scenarios[2048];
        /* 113512 */ LEVELDATA_UNKN09 u09;
        /* 113960 */ LEVELDATA_MAPINFOS mapinfos;
        /* 113974 */ LEVELDATA_OBJECTIVES objectives[6];
        /* 114058 */ LEVELDATA_UNKN11 u11;
    } LEVELDATA;

    // map-tile surfaces
    surfaceDesc *mtsurfaces_;
    // map-directions points
    floodPointDesc *mdpoints_;
    // for copy in pathfinding
    floodPointDesc *mdpoints_cp_;
    // initialized in createMinimap, used for in-class calculations
    int mmax_x_, mmax_y_, mmax_z_;
    // initialized in setSurfaces, used for in-class calculations
    int mmax_m_all, mmax_m_xy;

    void createMinimap();
    unsigned char getMinimapColour(int x, int y);
    // 0 - not present, 1 - our agent, 2 - enemy agent
    unsigned char getMinimapOverlay(int x, int y) {
        return minimap_overlay_[x + y * mmax_x_];
    }

    unsigned int getMaxInfoLvl() {return max_info_lvl_; }
    unsigned int getMaxEnhanceLvl() {return max_enhance_lvl_; }

protected:
    bool sWalkable(char thisTile, char upperTile);
    bool isSurface(char thisTile);
    bool isStairs(char thisTile);

    WeaponInstance *createWeaponInstance(uint8 *data);

protected:
    LEVELDATA level_data_;

    // eventually all this level data will be replaced by objects like this:
    std::vector<VehicleInstance *> vehicles_;
    std::vector<PedInstance *> peds_;
    std::vector<WeaponInstance *> weapons_;
    std::vector<Static *> statics_;
    std::vector<SFXObject *> sfx_objects_;
    std::vector<VehicleInstance *> cache_vehicles_;
    std::vector<PedInstance *> cache_peds_;
    std::vector<WeaponInstance *> cache_weapons_;
    std::vector<Static *> cache_statics_;
    std::vector<SFXObject *> cache_sfx_objects_;

    typedef enum {
        objv_None,
        // Setup control over object where possible to lose this control
        objv_AquireControl,
        // Leave control over object where possible to lose this control
        // NOTE: reserved in case we will need it
        objv_LoseControl,
        // Protect object at all cost
        objv_Protect,
        // Obtain inventory object
        // NOTE: maybe in future we have not only weapons, costumes? drones?
        objv_GetObject,
        // Object of defined subtype (of type) should be destroyed
        // defined by indx
        objv_DestroyObject,
        // Use of object untill condition is met
        objv_UseObject,
        // Leave object
        // NOTE: reserved in case we will need it
        objv_LeaveObject,
        // Objects should be at defined location
        objv_ReachLocation,
        // Objective for defined object(s), has sub-objective
        // NOTE: this can be used to set objective(s) for single ped or group
        objv_ExecuteObjective,
        // Should wait some time
        // NOTE: reserved in case we will need it
        objv_Wait
    }ObjectiveType;

    typedef struct {
        // type of objective
        ObjectiveType type;
        // 0 - not defined, 1 - ped, 2 - weapon, 3 - static, 4 - vehicle
        MapObject::MajorTypeEnum targettype;
        // 0 - not defined, 1 - our agent, 2 - enemy agent, 3 - guards
        // 4 - police, 5 - civilians
        uint8 targetsubtype;
        // index within vector of data
        uint16 targetindx;
        // 0 - not defined, 1b - has sub objective, 2b - refers to all objects
        // of subtype, 3b - completed, 4b - failed, 5b - check previous
        // objecives for fail
        uint32 condition;
        // indx for sub objective
        uint16 subobjindx;
        // tile position
        uint8 posxt;
        uint8 posyt;
        uint8 poszt;
        // offset position
        uint8 posxo;
        uint8 posyo;
        uint8 poszo;
        // This message should be setup during objective definition
        const char* msg;
        uint16 nxtobjindx;
    }ObjectiveDesc;

    std::vector <ObjectiveDesc> objectives_;
    std::vector <ObjectiveDesc> sub_objectives_;
    uint16 cur_objective_;
    /*!
     * Mission status. 
     * By default, a mission is running but it can be
     * aborted if user escapes the mission, failed if
     * missions objectives are not fullfilled or completed.
     */
    Status status_;

    int info_costs_[10];
    unsigned char max_info_lvl_;
    int enhance_costs_[10];
    unsigned char max_enhance_lvl_;
    std::string briefing_;
    int map_, min_x_, min_y_, max_x_, max_y_;

    std::set<int> fast_vehicle_cache_, fast_ped_cache_, fast_weapon_cache_,
            fast_statics_cache_;
    /*! Statistics : time, shots, ...*/
    MissionStats stats_;
    // minimap in colours, map z = 0 tiles transformed based on
    // walkdata->minimap_colours_ in function createMinimap
    unsigned char *minimap_c_;
    unsigned char minimap_overlay_[128*128];
};

#endif
