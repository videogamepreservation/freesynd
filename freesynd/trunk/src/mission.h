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

#include <string>
#include <vector>
#include <set>

#include "common.h"
#include "mapobject.h"
#include "weapon.h"
#include "map.h"
#include "model/leveldata.h"
#include "core/squad.h"
#include "core/gameevent.h"

class VehicleInstance;
class PedInstance;
class Agent;
class ObjectiveDesc;

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
    /*!
     * List of all possible mission status. 
     */
    enum Status {
        RUNNING = 0,
        ABORTED = 1,
        FAILED = 2,
        COMPLETED = 3
    };

    Mission(const LevelData::MapInfos & map_infos);
    virtual ~Mission();

    /*!
     * Sets the given map for the mission.
     * If p_map is not null, creates a minimap from it.
     * \param p_map The map to set.
     */
    void set_map(Map *p_map);

    /*!
     * Returns the map id used for the mission.
     */
    int map() { return i_map_id_; }
    uint16 mapId() { return i_map_id_; }
    /*!
     * Returns the map used for the mission.
     */
    Map * get_map() {return p_map_; }

    int mapWidth();
    int mapHeight();

    int minX() { return min_x_; }
    int minY() { return min_y_; }
    int maxX() { return max_x_; }
    int maxY() { return max_y_; }

    void drawMap(int scrollx, int scrolly);

    //--- MapHelper stuff
    virtual void drawAt(int tilex, int tiley, int tilez, int x, int y);
    virtual void createFastKeys(int tilex, int tiley,
        int maxtilex, int maxtiley);
    //---

    void objectiveMsg(std::string& msg);

    int numPeds() { return (int) peds_.size(); }
    PedInstance *ped(int i) { return peds_[i]; }
    void addPed(PedInstance *p) { peds_.push_back(p); }

    int numVehicles() { return (int) vehicles_.size(); }
    VehicleInstance *vehicle(int i) { return vehicles_[i]; }
    void addVehicle(VehicleInstance *pVehicle) { vehicles_.push_back(pVehicle); }

    int numWeapons() { return (int) weapons_.size(); }
    WeaponInstance *weapon(int i) { return weapons_[i]; }
    void addWeapon(WeaponInstance *w);

    int numStatics() { return (int) statics_.size(); }
    Static *statics(int i) { return statics_[i]; }
    void addStatic(Static *pStatic) { statics_.push_back(pStatic); }

    int numSfxObjects() { return (int) sfx_objects_.size(); }
    SFXObject *sfxObjects(int i) { return sfx_objects_[i]; }

    int numPrjShots() { return (int) prj_shots_.size(); }
    ProjectileShot *prjShots(int i) { return prj_shots_[i]; }

    void addSfxObject(SFXObject *so) {
        sfx_objects_.push_back(so);
    }
    void delSfxObject(int i) {
        delete sfx_objects_[i];
        sfx_objects_.erase((sfx_objects_.begin() + i));
    }

    void addPrjShot(ProjectileShot *prj) {
        prj_shots_.push_back(prj);
    }
    void delPrjShot(int i) {
        delete prj_shots_[i];
        prj_shots_.erase((prj_shots_.begin() + i));
    }

    /*! Return the mission statistics. */
    MissionStats *getStatistics() { return &stats_; }
    void incStatisticsShots(int shots) { stats_.nbOfShots += shots; }
    void incStatisticsHits() { stats_.nbOfHits++; }
    void incStatisticsHits(int hits) { stats_.nbOfHits += hits; }

    void start();
    //! Returns mission status
    Status getStatus() { return status_; }
    //! Adds an objective for the mission
    void addObjective(ObjectiveDesc *pObjective) { objectives_.push_back(pObjective); }
    //! Check if objectives are completed or failed
    void checkObjectives();
    //! Ends mission with the given status
    void endWithStatus(Status status);
    //! Returns true if mission status is failed
    bool failed() { return status_ == FAILED; }
    //! Returns true if mission status is completed
    bool completed() { return status_ == COMPLETED; }
    void addWeaponsFromPedToAgent(PedInstance *p, Agent *pAg);
    void end();

    MapObject * findAt(int tilex, int tiley, int tilez,
        MapObject::MajorTypeEnum *majorT, int *searchIndex, bool only);
    bool setSurfaces();
    void clrSurfaces();
    bool getWalkable(MapTilePoint &mtp);
    bool getWalkableClosestByZ(MapTilePoint &mtp);
    bool getShootableTile(int &x, int &y, int &z, int &ox, int &oy, int &oz);
    bool isTileSolid(int x, int y, int z, int ox, int oy, int oz);

    void adjXYZ(int &x, int &y, int &z);

    void blockerExists(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
        double *dist, MapObject** blockerObj);
    uint8 inRangeCPos(toDefineXYZ * cp, ShootableMapObject ** t,
        PathNode * pn = NULL, bool setBlocker = false,
        bool checkTileOnly = false, double maxr = -1.0, double * distTo = NULL);
    void getInRangeAll(toDefineXYZ * cp, std::vector<ShootableMapObject *> & targets,
        uint8 mask, bool checkTileOnly = true, double maxr = -1.0);
    uint32 playersGroupID() { return players_group_id_;}

    // map-tile surfaces
    // x + y * mmax_x_ + z * mmax_m_xy
    surfaceDesc *mtsurfaces_;
    // map-directions points
    floodPointDesc *mdpoints_;
    // for copy in pathfinding
    floodPointDesc *mdpoints_cp_;
    // initialized in set_map, used for in-class calculations
    // map maximum x,y,z values
    int mmax_x_, mmax_y_, mmax_z_;
    // initialized in setSurfaces, used for in-class calculations
    int mmax_m_all, mmax_m_xy;

    MiniMap * getMiniMap() { return p_minimap_; }
    /*!
     * Returns the current squad.
     */
    Squad * getSquad() { return p_squad_; }

protected:
    bool sWalkable(char thisTile, char upperTile);
    bool isSurface(char thisTile);
    bool isStairs(char thisTile);

    //! If the current objective has generated an event -> dispatch it
    void handleObjectiveEvent(GameEvent & evt, ObjectiveDesc *pObj);
    //! Sends an event to all listeners
    void fireGameEvent(GameEvent &evt);
protected:
    
    // eventually all this level data will be replaced by objects like this:
    std::vector<VehicleInstance *> vehicles_;
    std::vector<PedInstance *> peds_;
    std::vector<WeaponInstance *> weapons_;
    std::vector<Static *> statics_;
    std::vector<SFXObject *> sfx_objects_;
    std::vector<ProjectileShot *> prj_shots_;
    std::vector<VehicleInstance *> cache_vehicles_;
    std::vector<PedInstance *> cache_peds_;
    std::vector<WeaponInstance *> cache_weapons_;
    std::vector<Static *> cache_statics_;
    std::vector<SFXObject *> cache_sfx_objects_;

    std::vector <ObjectiveDesc *> objectives_;
    //std::vector <ObjectiveDesc> sub_objectives_;
    uint16 cur_objective_;
    /*!
     * Mission status. 
     * By default, a mission is running but it can be
     * aborted if user escapes the mission, failed if
     * missions objectives are not fullfilled or completed.
     */
    Status status_;

    int min_x_, min_y_, max_x_, max_y_;
    /*!
     * The id of the map for that mission.
     */
    uint16 i_map_id_;
    /*!
     * A pointer to the map.
     */
    Map *p_map_;

    std::set<int> fast_vehicle_cache_, fast_ped_cache_, fast_weapon_cache_,
            fast_statics_cache_, fast_sfx_objects_cache_;
    /*! Statistics : time, shots, ...*/
    MissionStats stats_;
    // minimap in colours, map z = 0 tiles transformed based on
    // walkdata->minimap_colours_ in function createMinimap
    MiniMap *p_minimap_;
   
    uint32 players_group_id_;
    /*!
     * The squad selected for the mission. It contains only active agents.
     */
    Squad *p_squad_;
};

#endif
