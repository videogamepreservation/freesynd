#include "model/objectivedesc.h"
#include "ped.h"
#include "app.h"

ObjPersuade::ObjPersuade(MapObject * pMapObject) : TargetObjective(pMapObject) {
    msg = g_App.menus().getMessage("GOAL_PERSUADE");
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjPersuade::selfEvaluate(GameEvent &evt, Mission *pMission) {
    PedInstance *p = static_cast<PedInstance *>(p_target_);
    if (p->health() <= 0)
    {
        // Target is dead -> mission is failed
        endObjective(evt, false);
    } else if (p->isPersuaded())
    {
        endObjective(evt, true);
    }
}

ObjAssassinate::ObjAssassinate(MapObject * pMapObject) : TargetObjective(pMapObject) {
    msg = g_App.menus().getMessage("GOAL_ASSASSINATE");
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjAssassinate::selfEvaluate(GameEvent &evt, Mission *pMission) {
    PedInstance *p = static_cast<PedInstance *>(p_target_);
    if (p->health() <= 0)
    {
        // Target is dead -> objective is completed
        endObjective(evt, true);
    } else {
        int x = p->tileX();
        int y = p->tileY();
        // target might be off visible area (escaped) -> failed
        if (p->inVehicle() && (x < (pMission->minX() >> 1)
            || x > pMission->maxX() + ((pMission->mmax_x_ - pMission->maxX()) >> 1)
            || y < (pMission->minY() >> 1)
            || y > pMission->maxY() + ((pMission->mmax_y_ - pMission->maxY()) >> 1)))
        {
            endObjective(evt, false);
        }
    }
}

/*!
 * Constructeur.
 * \param
 */
ObjDestroyVehicle::ObjDestroyVehicle(MapObject * pVehicle) : TargetObjective(pVehicle) {
    msg = g_App.menus().getMessage("GOAL_DESTROY_VEHICLE");
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjDestroyVehicle::selfEvaluate(GameEvent &evt, Mission *pMission) {
    VehicleInstance *pVehicle = static_cast<VehicleInstance *>(p_target_);

    if (pVehicle->health() <= 0) {
        endObjective(evt, true);
    }
}

/*!
 * Constructeur.
 * \param
 */
ObjTakeWeapon::ObjTakeWeapon(MapObject * pWeapon) : TargetObjective(pWeapon) {
    msg = g_App.menus().getMessage("GOAL_TAKE_WEAPON");
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjTakeWeapon::selfEvaluate(GameEvent &evt, Mission *pMission) {
    WeaponInstance *pWeapon = static_cast<WeaponInstance *>(p_target_);

    if (pWeapon->health() <= 0) {
        endObjective(evt, false);
    } else {
        ShootableMapObject *owner = pWeapon->getOwner();
        if (owner && owner->majorType() == MapObject::mjt_Ped
            && ((PedInstance *)owner)->isOurAgent())
        {
            endObjective(evt, true);
        }
    }
}

ObjEliminate::ObjEliminate(PedInstance::objGroupDefMasks subtype) : 
        ObjectiveDesc() {
    if (subtype == PedInstance::og_dmAgent) {
        msg = g_App.menus().getMessage("GOAL_ELIMINATE_AGENTS");
        targetsubtype = subtype;
        indx_grpid.grpid = 2;
    } else if (subtype == PedInstance::og_dmPolice) {
        msg = g_App.menus().getMessage("GOAL_ELIMINATE_POLICE");
        targetsubtype = subtype;
        indx_grpid.grpid = 4;
    } else {
        targetsubtype = PedInstance::og_dmUndefined;
        indx_grpid.grpid = 0;
    }
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjEliminate::selfEvaluate(GameEvent &evt, Mission *pMission) {
    for (int i=0; i<pMission->numPeds(); i++) {
        PedInstance *pPed = pMission->ped(i);

        if(pPed->objGroupDef() == targetsubtype
            // we can persuade them, will be
            // counted as eliminating for now
            && pPed->objGroupID() == indx_grpid.grpid
            && pPed->health() > 0)
        {
            break;
        }
    }
}

ObjEvacuate::ObjEvacuate(int x, int y, int z, std::vector <PedInstance *> &lstOfPeds) : 
        LocationObjective(x, y, z) {
    msg = g_App.menus().getMessage("GOAL_EVACUATE");
    // Copy all peds in the local list
    for (std::vector<PedInstance *>::iterator it_p = lstOfPeds.begin();
            it_p != lstOfPeds.end(); it_p++)
    {
        pedsToEvacuate.push_back(*it_p);
    }
}

/*!
 * Evaluate the objective.
 * \param evt
 * \param pMission
 */
void ObjEvacuate::selfEvaluate(GameEvent &evt, Mission *pMission) {
    // evacuating people
    for (std::vector<PedInstance *>::iterator it_p
        = pedsToEvacuate.begin();
        it_p != pedsToEvacuate.end(); it_p++)
    {
        if ((*it_p)->health() < 0) {
            // One of the peds is dead, objective is failed
            status = kFailed;
            return;
        }

        if ((*it_p)->distanceToPosXYZ(&pos_xyz) > 512) {
            // one of the peds is not yet in the evacuation perimeter
            return;
        }
    }

    for (size_t indx = AgentManager::kSlot1; indx < AgentManager::kMaxSlot; indx++) {
        PedInstance *pAgent = pMission->getSquad()->member(indx);
        if (pAgent) {
            if (pAgent->distanceToPosXYZ(&pos_xyz) > 512) {
                // one of the peds is not yet in the evacuation perimeter
                return;
            }
        }
    }

    // If we're here, then all peds are in the circle -> objective is completed
    status = kCompleted;
    evt.type_ = GameEvent::kObjTargetCleared;
}
