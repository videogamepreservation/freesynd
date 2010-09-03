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

#include "common.h"
#include "app.h"
#include "pathsurfaces.h"
#include <math.h>

#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif
Ped::Ped() {
    memset(stand_anims_, 0, sizeof(stand_anims_));
    memset(walk_anims_, 0, sizeof(walk_anims_));
    memset(stand_fire_anims_, 0, sizeof(stand_fire_anims_));
    memset(walk_fire_anims_, 0, sizeof(walk_fire_anims_));
}

bool Ped::drawStandFrame(int x, int y, int dir, int frame, WeaponIndex weapon) {
    if (weapon == MedKit)
        weapon = Unarmed;

    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_anims_[weapon] + dir, frame, x, y);
}

bool Ped::drawWalkFrame(int x, int y, int dir, int frame, WeaponIndex weapon) {
    if (weapon == MedKit)
        weapon = Unarmed;

    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastStandFrame(int dir, int frame, WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(
            stand_anims_[weapon] + dir, frame);
}

int Ped::lastWalkFrame(int dir, int frame, WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(walk_anims_[weapon] + dir, frame);
}


bool Ped::drawStandFireFrame(int x, int y, int dir, int frame,
        WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_fire_anims_[weapon] + dir, frame, x, y);
}

bool Ped::drawWalkFireFrame(int x, int y, int dir, int frame,
        WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_fire_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastStandFireFrame(int dir, int frame, WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(
            stand_fire_anims_[weapon] + dir, frame);
}

int Ped::lastWalkFireFrame(int dir, int frame, WeaponIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(walk_fire_anims_[weapon] + dir, frame);
}

bool Ped::drawDieFrame(int x, int y, int frame) {
    return g_App.gameSprites().drawFrame(die_anim_, frame, x, y);
}

int Ped::lastDieFrame() {
    return g_App.gameSprites().lastFrame(die_anim_);
}

void Ped::drawDeadFrame(int x, int y, int frame) {
    if (frame > g_App.gameSprites().lastFrame(dead_anim_))
        frame = g_App.gameSprites().lastFrame(dead_anim_);

    g_App.gameSprites().drawFrame(dead_anim_, frame, x, y);
}

void Ped::drawHitFrame(int x, int y, int dir, int frame) {
    if (frame > g_App.gameSprites().lastFrame(hit_anim_ + dir / 2))
        frame = g_App.gameSprites().lastFrame(hit_anim_ + dir / 2);

    g_App.gameSprites().drawFrame(hit_anim_ + dir / 2, frame, x, y);
}

int Ped::lastHitFrame(int dir) {
    return g_App.gameSprites().lastFrame(hit_anim_ + dir / 2);
}

void Ped::drawPickupFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(pickup_anim_, frame, x, y);
}

int Ped::lastPickupFrame() {
    return g_App.gameSprites().lastFrame(pickup_anim_);
}


bool PedInstance::animate(int elapsed, Mission *mission) {
    bool updated = false;
    Ped::WeaponIndex weapon_idx =
        selectedWeapon()? selectedWeapon()->index() : Ped::Unarmed;

    if (is_an_agent_ == PedInstance::Agent_Non_Active)
        return true;

    if (weapon_idx == Ped::MedKit && selectedWeapon()->ammoRemaining()
            && health_ < start_health_) {
        health_ = start_health_;
        selectedWeapon()->setAmmoRemaining(0);
        return true;
    }

    if (health_ < 0) {
        if (numWeapons())
            dropAllWeapons();

    }
    else if (isHostile()) {
        // find a weapon with ammo
        for (int i = 0; selectedWeapon() == 0 && i < numWeapons(); i++) {
            if (weapon(i)->ammoRemaining() > 0)
                selected_weapon_ = i;
        }

        target_ = 0;
        if (selectedWeapon()) {
            // for the moment, assume peds can only be hostile towards the agents
            for (int i = 0; i < 4; i++)
                if (mission->ped(i)->health() > 0
                        && inRange(mission->ped(i))) {
                    if (mission->ped(i)->inVehicle())
                        setTarget(mission->ped(i)->inVehicle());
                    else
                        setTarget(mission->ped(i));

                    clearDestination();
                    speed_ = 0;
                }

            for (int i = 0; target() == 0 && i < 4; i++)
                if (mission->ped(i)->health() > 0
                        && inSightRange(mission->ped(i))) {
                    if (dest_path_.size() == 0)
                        setDestinationP(mission, mission->ped(i)->tileX(),
                                       mission->ped(i)->tileY(),
                                       mission->ped(i)->tileZ());
                    break;
                }
        }
        else {
            if (pickup_weapon_ && pickup_weapon_->map() == -1)
                pickup_weapon_ = 0;

            // find the closest weapon with ammo
            float closest = 9999;
            WeaponInstance *closest_w = 0;

            for (int i = 0; i < mission->numWeapons(); i++) {
                if (mission->weapon(i)->map() != -1
                        && mission->weapon(i)->ammoRemaining() > 0) {
                    float d = distanceTo(mission->weapon(i));

                    if (d < closest) {
                        closest = d;
                        closest_w = mission->weapon(i);
                    }
                }
            }

            if (pickup_weapon_ != closest_w && closest_w) {
                if (weapons_.size() > 7)
                    dropWeapon(0);

                pickupWeapon(closest_w);
            }
        }

        if (target_ == 0)
            stopFiring();
    }

    if (in_vehicle_) {
        if (map_ == -1) {
            tile_x_ = in_vehicle_->tileX();
            tile_y_ = in_vehicle_->tileY();
            tile_z_ = in_vehicle_->tileZ();
            off_x_ = in_vehicle_->offX();
            off_y_ = in_vehicle_->offY();
        }
        else if (samePosition(in_vehicle_)) {
            map_ = -1;
            return true;
        }
        else {
            if(health_ > 0) {
                if(dest_path_.empty())
                    setDestinationP(mission ,in_vehicle_->tileX(), in_vehicle_->tileY(), 0,
                        in_vehicle_->offX(), in_vehicle_->offY(), 0, 320);
                else {
                    if(dest_path_.back().tileX() != in_vehicle_->tileX()
                        || dest_path_.back().tileY() != in_vehicle_->tileY()
                        || dest_path_.back().tileZ() != in_vehicle_->tileZ()
                        || dest_path_.back().offX() != in_vehicle_->offX()
                        || dest_path_.back().offY() != in_vehicle_->offY()
                        /*|| dest_path_.back().offZ() != in_vehicle_->offZ()*/)
                        setDestinationP(mission ,in_vehicle_->tileX(), in_vehicle_->tileY(), 0,
                            in_vehicle_->offX(), in_vehicle_->offY(), 0, 320);
                }
            }
        }
    }

    if (pickup_weapon_ && pickup_weapon_->map() == -1) {
        pickup_weapon_ = 0;
        clearDestination();
        speed_ = 0;
    }
    if( draw_timeout_ > 11 ) {
        if(getDrawnAnim() == PedInstance::PickupAnim
                || getDrawnAnim() == PedInstance::PutdownAnim) {
            if(speed_) {
                setDrawnAnim(PedInstance::WalkAnim);
            }else
                setDrawnAnim(PedInstance::StandAnim);
        }

        if(getDrawnAnim() == PedInstance::HitAnim) {
            if(speed_) {
                setDrawnAnim(PedInstance::WalkAnim);
            }else
                setDrawnAnim(PedInstance::StandAnim);
        }
        draw_timeout_ = 0;
        return true;
    } else {
        if(getDrawnAnim() == PedInstance::PickupAnim
                || getDrawnAnim() == PedInstance::PutdownAnim) {
            if(draw_timeout_ % 4 == 0)
                frame_ = ped_->lastPickupFrame() + 3;
            draw_timeout_++;
        }
        if(getDrawnAnim() == PedInstance::HitAnim)
            draw_timeout_++;
    }
    if (pickup_weapon_) {
        if (samePosition(pickup_weapon_)) {
            {
                weapons_.push_back(pickup_weapon_);
                pickup_weapon_->setMap(-1);
                pickup_weapon_ = 0;
                frame_ = ped_->lastPickupFrame();
                setDrawnAnim(PedInstance::PickupAnim);
                draw_timeout_ = 1;
                return true;
            }
        }
        else{
            if(health_ > 0) {
                if(dest_path_.empty())
                    setDestinationP(mission, pickup_weapon_->tileX(), pickup_weapon_->tileY(), 0,
                        pickup_weapon_->offX(), pickup_weapon_->offY(), 0, 320);
                else {
                    if(dest_path_.back().tileX() != pickup_weapon_->tileX()
                        || dest_path_.back().tileY() != pickup_weapon_->tileY()
                        || dest_path_.back().tileZ() != pickup_weapon_->tileZ()
                        || dest_path_.back().offX() != pickup_weapon_->offX()
                        || dest_path_.back().offY() != pickup_weapon_->offY()
                        || dest_path_.back().offZ() != pickup_weapon_->offZ())
                        setDestinationP(mission, pickup_weapon_->tileX(), pickup_weapon_->tileY(), 0,
                            pickup_weapon_->offX(), pickup_weapon_->offY(), 0, 320);
                }
            }
        }
    }

    if (putdown_weapon_) {
         {
            WeaponInstance *w = putdown_weapon_;
            w->setMap(map());
            w->setTileX(tile_x_);
            w->setTileY(tile_y_);
            w->setTileZ(tile_z_);
            w->setOffX(off_x_);
            w->setOffY(off_y_);
            w->setOffZ(off_z_);
            putdown_weapon_ = 0;
            frame_ = ped_->lastPickupFrame();
            setDrawnAnim(PedInstance::PutdownAnim);
            draw_timeout_ = 1;
            if(speed() != 0){
                clearDestination();
                setSpeed(0);
            }
            return true;
        }
    }

    updated = movementP(elapsed);

    if (target_) {
        if (inRange(target_)) {
            if(target_->health() > 0) {
                target_x_ = target_->screenX();
                target_y_ = target_->screenY();
            } else {
                target_ = NULL;
                target_x_ = -1;
                target_y_ = -1;
            }
        }
        else {
            stopFiring();
        }
    }

    if (target_x_ != -1 && target_y_ != -1
            && firing_ == PedInstance::Firing_Not
            && (selectedWeapon()
            && selectedWeapon()->ammoRemaining())) {
        int stx = screenX() + 30;
        int sty = screenY() - 4;

        int fuzz = 16;

        if (target_x_ - stx < -fuzz) {
            if (target_y_ - sty < -fuzz)
                dir_ = 6;
            else if (target_y_ - sty > fuzz)
                dir_ = 0;
            else
                dir_ = 7;
        }
        else if (target_x_ - stx > fuzz) {
            if (target_y_ - sty < -fuzz)
                dir_ = 4;
            else if (target_y_ - sty > fuzz)
                dir_ = 2;
            else
                dir_ = 3;
        }
        else {
            if (target_y_ - sty < -fuzz)
                dir_ = 5;
            else if (target_y_ - sty > fuzz)
                dir_ = 1;
            else {
                // shoot yourself?
            }
        }

        setHitDamage(selectedWeapon()->shot());
        firing_ = PedInstance::Firing_Fire;
        updated = true;

        selectedWeapon()->playSound();
    }

    if (weapon_idx == Ped::Unarmed || weapon_idx == Ped::MedKit)
        firing_ = PedInstance::Firing_Not;

    if ((firing_ == PedInstance::Firing_Reload
            || firing_ == PedInstance::Firing_Not)
            && health_ > 0 && draw_timeout_ == 0) {
        if(speed_){
            setDrawnAnim(PedInstance::WalkAnim);
        }
        else
            setDrawnAnim(PedInstance::StandAnim);

        if (selectedWeapon())
            if (selectedWeapon()->ammoRemaining() == 0)
                selectNextWeapon();
    }
    if (firing_ == PedInstance::Firing_Reload) {
        reload_count_++;
        int required = 1;

        if (weapon_idx == Ped::Pistol
                || weapon_idx == Ped::Shotgun)
            required = 50;

        if (reload_count_ >= required) {
            firing_ = PedInstance::Firing_Not;
            reload_count_ = 0;
        }
    }

    if (firing_ != PedInstance::Firing_Not || updated || health_ <= 0
            || receive_damage_ || pickup_weapon_ || putdown_weapon_) {
        MapObject::animate(elapsed);
        if (receive_damage_) {
            health_ -= receive_damage_;

            if (health_ <= 0){
                health_ = 0;
                target_ = 0;
                target_x_ = target_y_ = -1;
                speed_ = 0;
                clearDestination();
                setDrawnAnim(PedInstance::DieAnim);
            }else{
                setDrawnAnim(PedInstance::HitAnim);
            }

            receive_damage_ = 0;
            frame_ = 0;
            draw_timeout_ = 1;
        }else
            if (health_ == 0) {
                health_ = -1;
                frame_ = 0;
                setDrawnAnim(PedInstance::DeadAnim);
                draw_timeout_ = 0;
            }

        if ((firing_ == PedInstance::Firing_Fire
                || firing_ == PedInstance::Firing_Stop)
                && health_ > 0) {

            draw_timeout_ = 0;
            if (speed_){
                setDrawnAnim(PedInstance::WalkFireAnim);
            }
            else
                setDrawnAnim(PedInstance::StandFireAnim);

            if (target_ && target_->health() > 0 && hit_damage_){
                target_->inflictDamage(hit_damage_);
                hit_damage_ = 0;
            }

            if (selectedWeapon()->ammoRemaining() > 0) {
                selectedWeapon()->setAmmoRemaining(
                        selectedWeapon()->ammoRemaining() - 1);
            }

            // last frame, still firing, reload
            if (firing_ == PedInstance::Firing_Fire)
                firing_ = PedInstance::Firing_Reload;

            if (firing_ == PedInstance::Firing_Stop) {
                firing_ = PedInstance::Firing_Not;
                target_ = 0;
                target_x_ = target_y_ = -1;
            }
        }
        return true;
    }

    if (frame_ != 0) {
        frame_ = 0;
        return true;
    }

    return false;
}

void PedInstance::stopFiring() {
    if (firing_ != PedInstance::Firing_Not)
        firing_ = PedInstance::Firing_Stop;
}

PedInstance *Ped::createInstance(int map) {
    return new PedInstance(this, map);
}

void PedInstance::kill() {
    dead_ = true;
}

bool isOnScreen(int scrollX, int scrollY, int x, int y) {
    return x >= scrollX && y >= scrollY
            && x < scrollX + GAME_SCREEN_WIDTH - 129
            && y < scrollY + GAME_SCREEN_HEIGHT;
}

bool getOnScreen(int scrollX, int scrollY, int &x, int &y, int tx, int ty) {
    bool off = false;

    // get x, y on screen
    while (!isOnScreen(scrollX, scrollY, x, y)) {
        if (abs(tx - x) != 0)
            x += (tx - x) / abs(tx - x);

        if (abs(ty - y) != 0)
            y += (ty - y) / abs(ty - y);

        off = true;
    }

    return off;
}

void PedInstance::showPath(int scrollX, int scrollY) {
    int px = screenX();
    int py = screenY();

    if (is_an_agent_ == PedInstance::Agent_Non_Active)
        return;

    for (std::list<PathNode>::iterator it = dest_path_.begin();
            it != dest_path_.end(); it++) {
        PathNode & d = *it;
        int x =
            g_App.maps().tileToScreenX(map(), d.tileX(), d.tileY(),
                                       d.tileZ(), d.offX(), d.offY());
        int y =
            g_App.maps().tileToScreenY(map(), d.tileX(), d.tileY(),
                                       d.tileZ(), d.offX(), d.offY());

        int ox = x;
        int oy = y;
        if (isOnScreen(scrollX, scrollY, x, y))
            getOnScreen(scrollX, scrollY, px, py, x, y);
        else if (isOnScreen(scrollX, scrollY, px, py))
            getOnScreen(scrollX, scrollY, x, y, px, py);
        else {
            px = x;
            py = y;
            continue;
        }

        int cl = 11;
        g_Screen.drawLine(px - scrollX + 129, py - scrollY,
                x - scrollX + 129, y - scrollY, cl);
        g_Screen.drawLine(px - scrollX + 129 - 1, py - scrollY,
                x - scrollX + 129 - 1, y - scrollY, cl);
        g_Screen.drawLine(px - scrollX + 129, py - scrollY - 1,
                x - scrollX + 129, y - scrollY - 1, cl);
        g_Screen.drawLine(px - scrollX + 129 - 1, py - scrollY - 1,
                x - scrollX + 129 - 1, y - scrollY - 1, cl);

        px = ox;
        py = oy;
    }
}

PedInstance::PedInstance(Ped *ped, int m) : ShootableMovableMapObject(m),
draw_timeout_(0), ped_(ped), firing_(PedInstance::Firing_Not),
drawn_anim_(PedInstance::StandAnim), target_(NULL), target_x_(-1),
target_y_(-1), hit_damage_(0), receive_damage_(0), sight_range_(0),
is_hostile_(false), reload_count_(0), selected_weapon_(-1),
pickup_weapon_(0), putdown_weapon_(0), in_vehicle_(0),
is_an_agent_(PedInstance::Not_Agent) {
    hold_on_.wayFree = 0;
}

void PedInstance::draw(int x, int y, int scrollX, int scrollY) {

    if (is_an_agent_ == PedInstance::Agent_Non_Active)
        return;

    Ped::WeaponIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Ped::Unarmed;
    addOffs(x, y);

    // ensure on map
    if (x < 129 || y < 0 || map_ == -1)
        return;

    if (selectedWeapon() == 0) {
        firing_ = PedInstance::Firing_Not;
        target_ = 0;
        target_x_ = target_y_ = -1;
    }

    switch(getDrawnAnim()){
        case PedInstance::HitAnim:
            ped_->drawHitFrame(x, y, dir_, frame_);
            break;
        case PedInstance::DieAnim:
            ped_->drawDieFrame(x, y, frame_);
            break;
        case PedInstance::DeadAnim:
            ped_->drawDeadFrame(x, y, frame_);
            break;
        case PedInstance::PickupAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::PutdownAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::WalkAnim:
            ped_->drawWalkFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::StandAnim:
            ped_->drawStandFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::WalkFireAnim:
            ped_->drawWalkFireFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::StandFireAnim:
            ped_->drawStandFireFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::NoAnimation:
            printf("hmm NoAnimation\n");
            break;
    }
    /*
    {
        if (lastFrame) {
            // draw the impact, TODO: this doesn't work if the shooter is off screen.
            g_App.gameSprites().drawSpriteXYZ(18 * 40 + 9 + firing_ - 3,
                                              target_x_ - scrollX + 129 +
                                              (target_ ? 15 : 0),
                                              target_y_ - scrollY -
                                              (target_ ? 15 : 0), 0);
        }
    }*/
}

void PedInstance::drawSelectorAnim(int x, int y) {

    if (is_an_agent_ == PedInstance::Agent_Non_Active)
        return;

    Ped::WeaponIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Ped::Unarmed;

    switch(getDrawnAnim()){
        case PedInstance::HitAnim:
            ped_->drawHitFrame(x, y, dir_, frame_);
            break;
        case PedInstance::DieAnim:
            ped_->drawDieFrame(x, y, frame_);
            break;
        case PedInstance::DeadAnim:
            ped_->drawDeadFrame(x, y, frame_);
            break;
        case PedInstance::PickupAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::PutdownAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::WalkAnim:
            ped_->drawWalkFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::StandAnim:
            ped_->drawStandFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::WalkFireAnim:
            ped_->drawWalkFireFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::StandFireAnim:
            ped_->drawStandFireFrame(x, y, dir_, frame_, weapon_idx);
            break;
        case PedInstance::NoAnimation:
            printf("hmm NoAnimation\n");
            break;
    }
}

bool PedInstance::inRange(ShootableMapObject *t) {
    if (selectedWeapon() == 0)
        return false;

    int maxr;
    // TODO: this should be in Weapon
    switch (selectedWeapon()->index()) {
    case Ped::Pistol:
        maxr = 5;
        break;
    case Ped::Minigun:
        maxr = 5;
        break;
    case Ped::Flamer:
        maxr = 5;
        break;
    case Ped::LongRange:
        maxr = 15;
        break;
    case Ped::Uzi:
        maxr = 5;
        break;
    case Ped::Laser:
        maxr = 50;
        break;
    case Ped::Gauss:
        maxr = 15;
        break;
    case Ped::Shotgun:
        maxr = 2;
        break;
    default:
        maxr = 0;
        break;
    }

    float d = distanceTo(t);

    if (d >= maxr)
        return false;

    // check for obstacles.
    Map *m = g_App.maps().map(map());

    float sx = (float) tileX();
    float sy = (float) tileY();
    int lvl = tileZ() + 1;

    //printf("%i %i %i -> %i %i %i\n", tileX(), tileY(), tileZ(), t->tileX(), t->tileY(), t->tileZ());

    while (fabs(sx - t->tileX()) > 1.0f || fabs(sy - t->tileY()) > 1.0f) {
        //printf("at %i %i %i is %i\n", (int)sx, (int)sy, lvl, map->tileAt((int)sx, (int)sy, lvl));
        if (m->tileAt((int) sx, (int) sy, lvl) > 5)
            return false;
        sx += (t->tileX() - tileX()) / d;
        sy += (t->tileY() - tileY()) / d;
    }

    return true;
}

bool PedInstance::inSightRange(MapObject *t) {
    float d =
        sqrt((float) (t->tileX() - tileX()) * (t->tileX() - tileX()) +
             (t->tileY() - tileY()) * (t->tileY() - tileY()) +
             (t->tileZ() - tileZ()) * (t->tileZ() - tileZ()));

    return d < sight_range_;
}

void PedInstance::selectNextWeapon() {
    int nextWeapon = -1;

    Weapon *curSelectedWeapon = (Weapon *) weapon(selected_weapon_);

    if (curSelectedWeapon)
        for (int i = numWeapons() - 1; i >=0 && nextWeapon == -1; i--)
            if (i != selected_weapon_
                    && weapon(i)->rank() == curSelectedWeapon->rank()
                    && weapon(i)->ammoRemaining())
                nextWeapon = i;

    if (nextWeapon == -1)
        selectBestWeapon();
    else
        selected_weapon_ = nextWeapon;
}

void PedInstance::selectBestWeapon() {
    int bestWeapon = -1;
    int bestWeaponRank = -1;

    for (int i = numWeapons() - 1; i >=0; i--)
        if (weapon(i)->ammoRemaining() && weapon(i)->rank() > bestWeaponRank) {
            bestWeapon = i;
            bestWeaponRank = weapon(i)->rank();
        }

    if(bestWeapon != -1)
        selected_weapon_ = bestWeapon;
}

void PedInstance::dropWeapon(int n) {
    assert(n >= 0 && n < (int) weapons_.size());

    if (selected_weapon_ == n)
        selected_weapon_ = -1;

    WeaponInstance *w = weapons_[n];
    std::vector < WeaponInstance * >::iterator it;

    for (it = weapons_.begin(); *it != w; it++) {}

    assert(*it == w);
    weapons_.erase(it);

    putdown_weapon_ = w;
}

void PedInstance::dropAllWeapons() {

    selected_weapon_ = -1;

    std::vector < WeaponInstance * >::iterator it;
    int n = 0;

    for (it = weapons_.begin(); it != weapons_.end(); it++) {
        WeaponInstance *w = weapons_[n];
        w->setMap(map());
        w->setTileX(tile_x_);
        w->setTileY(tile_y_);
        w->setTileZ(tile_z_);
        w->setOffX(off_x_);
        w->setOffY(off_y_);
        w->setOffZ(off_z_);
    }

    while(weapons_.size())
        weapons_.pop_back();

}

void PedInstance::pickupWeapon(WeaponInstance * w) {
    assert(w->map() == map());

    if (weapons_.size() >= 8)
        return;

    pickup_weapon_ = w;
    frame_ = 0;
}

bool PedInstance::wePickupWeapon() {
    return pickup_weapon_ != 0;
}

void PedInstance::putInVehicle(VehicleInstance * v) {
    assert(map_ != -1);
    in_vehicle_ = v;
}

void PedInstance::leaveVehicle() {
    assert(map_ == -1 && in_vehicle_);
    map_ = in_vehicle_->map();

    // TODO: only if driver exits this must occur
    in_vehicle_->clearDestination();
    in_vehicle_->setSpeed(0);
    in_vehicle_ = 0;
}

int PedInstance::map() {
    if (map_ == -1) {
        assert(in_vehicle_);
        return in_vehicle_->map();
    }

    return map_;
}

bool PedInstance::walkable(int x, int y, int z) {
    Map *m = g_App.maps().map(map());

    if (m->stairsAt(x, y, z) || m->stairsAt(x, y, z + 1))
        return true;

    uint8 thisTile = g_App.walkdata_[m->tileAt(x, y, z)];
    uint8 upperTile = g_App.walkdata_[m->tileAt(x, y, z + 1)];
    //printf(" %i : %i : %i\n",thisTile,z,upperTile);
    return thisTile != 0x0C && thisTile != 0x10 && thisTile != 0x0
        && thisTile != upperTile
        && ((thisTile == 0x05 || thisTile == 0x0D)
            ? upperTile == 0x0 : true);
}

PedInstance::AnimationDrawn PedInstance::getDrawnAnim() {
    return drawn_anim_;
}

void PedInstance::setDrawnAnim(PedInstance::AnimationDrawn drawn_anim) {
    drawn_anim_ = drawn_anim;
}

int PedInstance::getDistance(int x1, int y1, int z1,
                                int x2, int y2, int z2) {
    // It is not quiet correct, but int's are better then float + sqrt
    return (x2 - x1) * (x2 - x1)+ (y2 - y1) * (y2 - y1)
        + (z2 - z1) * (z2 - z1);
}

int PedInstance::calcDistance(unsigned short lvl, int x, int y, int z,
             std::vector <linkDesc> ::iterator par) {
    int dist = 0;
    while(lvl != 0) {
        dist += getDistance(x, y, z,
            par->j.x, par->j.y, par->j.z);
        x = par->j.x;
        y = par->j.y;
        z = par->j.z;
        par = (std::vector <linkDesc> ::iterator)(par->p);
        lvl--;
    }
    dist += getDistance(x, y, z,
        tile_x_, tile_y_, tile_z_);
    return dist;
}

void PedInstance::markBadNodes(unsigned short lvl, unsigned short clvl,
                    int n, int atindex, std::vector <linkDesc> ** lvls) {
    std::vector <linkDesc> * curlvl = lvls[clvl];

    if (curlvl->size() != 0) {
        int indxend = atindex + n;
        for (int i = atindex; i < indxend; i++) {
            linkDesc &childnode = (*curlvl)[i];
            if (childnode.n != 0) {
                markBadNodes(lvl, clvl + 1, childnode.n,
                    childnode.atindex, lvls);
            }
            childnode.bad = true;
            childnode.destr = false;
        }
    }
}

void PedInstance::setLvlNode(std::vector <linkDesc> ::iterator it,
    std::vector <linkDesc> ** lvls, std::vector <reachedDesc> * preached,
    unsigned short lvlnum, unsigned char nt, int itstart, bool setreached,
    unsigned int pid) {

    int sz = preached->size();
    if (itstart == -1)
        return;

    reachedDesc &rpr = (*preached)[itstart];
    unsigned int id = rpr.j.pj->id;
    do{
        reachedDesc &rpr = (*preached)[itstart];
        bool toadd = true;
        if (rpr.j.pj->id == id) {
             if (rpr.atlevel != 0xFFFF) {
                std::vector <linkDesc> * slvl = lvls[rpr.atlevel];
                linkDesc &getit = (*slvl)[rpr.atindex];
                int dist = it->dist + getDistance (rpr.j.x,
                    rpr.j.y, rpr.j.z, it->j.x, it->j.y, it->j.z);
                if (rpr.dist > dist) {
                    getit.bad = true;
                    getit.destr = false;
                    if (rpr.atlevel != lvlnum && getit.n != 0){
                        markBadNodes (lvlnum, rpr.atlevel + 1, getit.n,
                            getit.atindex, lvls);
                    }
                    linkDesc ladd;
                    rpr.atlevel = lvlnum;
                    rpr.atindex = lvls[lvlnum]->size();
                    rpr.dist = dist;
                    ladd.dist = dist;
                    ladd.bad = false;
                    ladd.j = rpr.j;
                    ladd.n = 0;
                    ladd.nt = nt;
                    if (lvlnum > 0) {
                        ladd.p = it;
                        it->n++;
                        if(it->atindex == -1)
                            it->atindex = lvls[lvlnum]->size();
                    }
                    ladd.pid = pid;
                    ladd.destr = setreached;
                    ladd.atindex = -1;
                    lvls[lvlnum]->push_back(ladd);
                }
                toadd = false;
            }
            if (toadd) {
                rpr.atlevel = lvlnum;
                rpr.atindex = lvls[lvlnum]->size();
                rpr.dist = it->dist + getDistance (rpr.j.x,
                    rpr.j.y, rpr.j.z, it->j.x, it->j.y, it->j.z);
                linkDesc ladd;
                ladd.j = rpr.j;
                ladd.dist = rpr.dist;
                ladd.bad = false;
                ladd.n = 0;
                ladd.nt = nt;
                if (lvlnum > 0) {
                    ladd.p = it;
                    it->n++;
                    if(it->atindex == -1)
                        it->atindex = lvls[lvlnum]->size();
                }
                ladd.pid = pid;
                ladd.destr = setreached;
                ladd.atindex = -1;
                lvls[lvlnum]->push_back(ladd);
            }
        } else
            break;
    } while((++itstart) < sz);
}

//#if 0
void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                     int ox, int oy, int oz, int new_speed) {
    // NOTE: Although this implementation uses single junction type at once
    // I think it is possible to implement use of two junctions at once
    // like surface + stairs or stairs + surface. How efficient this
    // can be I don't know. But exclusion of doubles maybe will produce faster
    // results, or better. And distance can be calculated not in such primitive
    // way, but calculating real distance for in-surface real path not like
    // here - simple line length.
    m->adjXYZ(x, y, z);
    dest_path_.clear();
    setSpeed(0);
    printf("target x : %i; y : %i; z : %i = = ox :%i, oy :%i, oz :%i\n",
        x, y, z, ox, oy, oz);

    surfaceDesc *targetd = &(m->mtsurfaces_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    //if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0)
        //return;

    int old_z = tile_z_;
    int old_oz = off_z_;
    tile_z_ += (off_z_ == 0 ? 0 : 1);
    off_z_ = 0;
    surfaceDesc *based = &(m->mtsurfaces_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    unsigned int starttime = SDL_GetTicks();
    printf("stime %i.%i\n", starttime/1000, starttime%1000);
    printf("base %i, bt %i, target %i, tt %i\n",based->id, based->t,targetd->id,targetd->t);
    printf("btwd %i, ttwd %i\n",based->twd, targetd->twd);

    printf("base pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
        tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0) {
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if(based->t == m_sdNonwalkable) {
        printf("Movement from nonwalkable postion\n");
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if (in_vehicle_) {
        if(in_vehicle_->tileX() != x
            || in_vehicle_->tileY() != y
            || in_vehicle_->tileZ() != z
            || in_vehicle_->offX() != ox
            || in_vehicle_->offY() != oy
            || in_vehicle_->offZ() != oz)
        in_vehicle_ = 0;
    }
    if (pickup_weapon_) {
        if(pickup_weapon_->tileX() != x
            || pickup_weapon_->tileY() != y
            || pickup_weapon_->tileZ() != z
            || pickup_weapon_->offX() != ox
            || pickup_weapon_->offY() != oy
            || pickup_weapon_->offZ() != oz)
        pickup_weapon_ = 0;
    }

    if (tile_x_ == x && tile_y_ == y && tile_z_ == z) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy, oz));
        speed_ = new_speed;
        return;
    }

    if ((based->t & m_sdSurface) == m_sdSurface
        && (targetd->t & m_sdSurface) == m_sdSurface) {
        if (targetd->id == based->id) {
            getPathAtSurfaceP(m, &dest_path_, x, y, z, ox, oy, oz);
            speed_ = new_speed;
            tile_z_ = old_z;
            off_z_ = old_oz;
            //for (std::list<PathNode>::iterator it = dest_path_.begin();
            //    it != dest_path_.end(); it++) {
            //    printf("sf+++ t %i, x %i, y %i, z %i +++\n",
            //        m->mtsurfaces_[it->tileX() + it->tileY() * m->mmax_x_ + it->tileZ() * m->mmax_m_xy].t,
            //        it->tileX(), it->tileY(), it->tileZ());
            //}
            return;
        }
    } else if ((based->t & m_sdStairs) == m_sdStairs
        && (targetd->t & m_sdStairs) == m_sdStairs) {
        if (targetd->id == based->id) {
            getPathAtStairsP(m, &dest_path_, x, y, z, ox, oy, oz);
            speed_ = new_speed;
            tile_z_ = old_z;
            off_z_ = old_oz;
            //for (std::list<PathNode>::iterator it = dest_path_.begin();
            //    it != dest_path_.end(); it++) {
            //    printf("st+++ t %i, x %i, y %i, z %i +++\n",
            //        m->mtsurfaces_[it->tileX() + it->tileY() * m->mmax_x_ + it->tileZ() * m->mmax_m_xy].t,
            //        it->tileX(), it->tileY(), it->tileZ());
            //}
            return;
        }
    }
    //return;
    unsigned short lvlnum = 0;

    std::vector <linkDesc> * lvls[MAX_LVLS_PATH];
    memset(lvls, NULL, sizeof(std::vector <linkDesc> *) * MAX_LVLS_PATH);
    for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
        lvls[i] = new std::vector <linkDesc>;
    }

    std::vector <linkDesc> * parlvl = NULL;
    std::vector <reachedDesc> sfcreached;
    for (std::vector <junctionDesc> ::iterator itfill = m->sfcjunctions_.begin();
        itfill != m->sfcjunctions_.end(); itfill++) {
        reachedDesc rd;
        rd.j = *itfill;
        rd.atindex = 0;
        rd.atlevel = 0xFFFF;
        rd.dist = 0;
        sfcreached.push_back(rd);
    }

    std::vector <reachedDesc> strreached;
    for (std::vector <junctionDesc> ::iterator itfill = m->strjunctions_.begin();
        itfill != m->strjunctions_.end(); itfill++) {
        reachedDesc rd;
        rd.j = *itfill;
        rd.atindex = 0;
        rd.atlevel = 0xFFFF;
        rd.dist = 0;
        strreached.push_back(rd);
    }

    std::vector <linkDesc> tofill;
    if(tofill.empty()) {
        linkDesc toadd;
        toadd.dist = 0;
        toadd.j.x = tile_x_;
        toadd.j.y = tile_y_;
        toadd.j.z = tile_z_;
        tofill.push_back(toadd);
    }

    unsigned char targettype = targetd->t & (m_sdStairs | m_sdSurface);
    bool isreached = targetd->id == based->id
        && targettype == (based->t & (m_sdStairs | m_sdSurface));

    switch(based->t) {
        case (m_sdStairs | m_sdJunction):
        case m_sdStairs:
            setLvlNode(tofill.begin(), lvls, &strreached,
                lvlnum, m_sdSurface, m->stritstarts_[based->id], isreached, 0);
            break;
        case (m_sdSurface | m_sdJunction):
        case m_sdSurface:
            setLvlNode(tofill.begin(), lvls, &sfcreached,
                lvlnum, m_sdStairs, m->sfcitstarts_[based->id], isreached, 0);
            break;
    }
    // this variable limits finding best possible, but increases speed
    bool destr = false;
    bool onemore = true;
//~~~~~~~~~~~~~~~~~~~~~~~~~~
checkloop___label:
//~~~~~~~~~~~~~~~~~~~~~~~~~~
    {
        printf("lvl processed %i, time %i.%i, nodes %i\n", lvlnum,
            (SDL_GetTicks() - starttime)/1000, (SDL_GetTicks() - starttime)%1000,
            lvls[lvlnum]->size());
        if (lvls[lvlnum]->size() == 0 || lvlnum > (MAX_LVLS_PATH - 2))
            goto exitloop___label;

        if (destr) {
            if (!onemore)
                goto exitloop___label;
            else
                onemore = false;
        }

        parlvl = lvls[lvlnum];
        lvlnum++;
        std::vector <linkDesc> ::iterator itend = parlvl->end();
        for (std::vector <linkDesc> ::iterator it = parlvl->begin();
            it != itend; it++) {
            if (!it->bad && !it->destr) {
                if (it->nt == m_sdStairs) {

                    int x = it->j.x;
                    int y = it->j.y * m->mmax_x_;
                    int z = it->j.z * m->mmax_m_xy;
                    int xm = x - 1;
                    int ym = y - m->mmax_x_;
                    int xp = x + 1;
                    int yp = y + m->mmax_x_;
                    surfaceDesc * csf = it->j.pj;
                    unsigned int idc = 0;
                    if (csf->idjh != 0) {
                        int zp = z + m->mmax_m_xy;
                        if ( (csf->idjh & 0x000000FF) == 0x000000F0) {
                            idc = m->mtsurfaces_[x + yp + zp].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjh & 0x0000F200) == 0x0000F200) {
                            idc = m->mtsurfaces_[xp + y + zp].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjh & 0x00F40000) == 0x00F40000) {
                            idc = m->mtsurfaces_[x + ym + zp].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjh & 0xF6000000) == 0xF6000000) {
                            idc = m->mtsurfaces_[xm + y + zp].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                    }
                    if (csf->idjl != 0) {
                        if ( (csf->idjl & 0x000000FF) == 0x000000F0) {
                            idc = m->mtsurfaces_[x + yp + z].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface, 
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjl & 0x0000F200) == 0x0000F200) {
                            idc = m->mtsurfaces_[xp + y + z].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjl & 0x00F40000) == 0x00F40000) {
                            idc = m->mtsurfaces_[x + ym + z].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                        if ( (csf->idjl & 0xF6000000) == 0xF6000000) {
                            idc = m->mtsurfaces_[xm + y + z].id;
                            if (idc != it->pid) {
                                isreached = targetd->id == idc
                                    && targettype == m_sdStairs;
                                destr = destr || isreached;
                                setLvlNode(it, lvls, &strreached, lvlnum, m_sdSurface,
                                    m->stritstarts_[idc], isreached, it->j.pj->id);
                            }
                        }
                    }
                } else if(it->nt == m_sdSurface) {
                    if (it->j.pj->idjh != 0 && it->j.pj->idjh != it->pid) {
                        isreached = targetd->id == it->j.pj->idjh
                            && targettype == m_sdSurface;
                        destr = destr || isreached;
                        setLvlNode(it, lvls, &sfcreached, lvlnum, m_sdStairs,
                            m->sfcitstarts_[it->j.pj->idjh], isreached,
                            it->j.pj->id);
                    }
                    if (it->j.pj->idjl != 0 && it->j.pj->idjl != it->pid) {
                        isreached = targetd->id == it->j.pj->idjl
                            && targettype == m_sdSurface;
                        destr = destr || isreached;
                        setLvlNode(it, lvls, &sfcreached, lvlnum, m_sdStairs,
                            m->sfcitstarts_[it->j.pj->idjl], isreached,
                            it->j.pj->id);
                    }
                }
            }
        }
    }goto checkloop___label;
//~~~~~~~~~~~~~~~~~~~~~~~~~~
exitloop___label:
//~~~~~~~~~~~~~~~~~~~~~~~~~~
    strreached.clear();
    sfcreached.clear();
    tofill.clear();
    if (lvls[0]->size() == 0) {
        for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
            delete lvls[i];
        }
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    printf("time %i.%i\n", (SDL_GetTicks() - starttime)/1000,
        (SDL_GetTicks() - starttime)%1000);
    
    std::vector <linkDesc> ::iterator reachedit;
    int dist = -1;
    unsigned short clvl;
    do {
        parlvl = lvls[lvlnum];
        for (std::vector <linkDesc> ::iterator it = parlvl->begin();
            it != parlvl->end(); it++) {
            if (it->destr) {
                if (dist == -1) {
                    reachedit = it;
                    dist = it->dist + getDistance (it->j.x, it->j.y, it->j.z,
                        x, y, z);
                    clvl = lvlnum;
                } else {
                    int cmpdist;
                    cmpdist = it->dist + getDistance (it->j.x, it->j.y, it->j.z,
                        x, y, z);
                    if (cmpdist < dist) {
                        dist = cmpdist;
                        reachedit = it;
                        clvl = lvlnum;
                    }
                }
            }
        }
        lvlnum--;
    } while(lvlnum != 0);
    printf("best dest in %i.%i\n", (SDL_GetTicks() - starttime)/1000,
        (SDL_GetTicks() - starttime)%1000);
    printf("dist %i\n", (unsigned int)dist);
    
    if (dist != -1) {
        clvl++;
        do {
            clvl--;
            printf("x %i, y %i, z%i, t %i\n",reachedit->j.x,
                reachedit->j.y, reachedit->j.z, reachedit->j.pj->t);
            tofill.push_back(*reachedit);
            reachedit = reachedit->p;
        }while(clvl != 0);
    } else {
        for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
            delete lvls[i];
        }
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    std::list <junctionDesc> prmj;
    for (std::vector <linkDesc>::iterator it = tofill.begin();
        it != tofill.end(); it++) {
        junctionDesc toadd;

        std::vector <linkDesc>::iterator fit = it + 1;
        if (it->j.pj->t == (m_sdSurface | m_sdJunction)) {
            int x = it->j.x;
            int y = it->j.y * m->mmax_x_;
            int z = it->j.z * m->mmax_m_xy;
            int xm = x - 1;
            int ym = y - m->mmax_x_;
            int xp = x + 1;
            int yp = y + m->mmax_x_;
            int zp = z + m->mmax_m_xy;
            surfaceDesc * csf = it->j.pj;
            int z0 = -1;
            int z2 = -1;
            int z4 = -1;
            int z6 = -1;
            surfaceDesc *csf0, *csf2, *csf4, *csf6;
            unsigned int fastxyz0, fastxyz2, fastxyz4, fastxyz6;
            if (csf->idjh != 0) {
                if ( (csf->idjh & 0x000000FF) == 0x000000F0) {
                    csf0 = &(m->mtsurfaces_[x + yp + zp]);
                    z0 = it->j.z + 1;
                    fastxyz0 = it->j.x
                        | ((it->j.y + 1) << 8) | (z0 << 16);
                }
                if ( (csf->idjh & 0x0000F200) == 0x0000F200) {
                    csf2 = &(m->mtsurfaces_[xp + y + zp]);
                    z2 = it->j.z + 1;
                    fastxyz2 = (it->j.x + 1)
                        | (it->j.y << 8) | (z2 << 16);
                }
                if ( (csf->idjh & 0x00F40000) == 0x00F40000) {
                    csf4 = &(m->mtsurfaces_[x + ym + zp]);
                    z4 = it->j.z + 1;
                    fastxyz4 = it->j.x
                        | ((it->j.y - 1) << 8) | (z4 << 16);
                }
                if ( (csf->idjh & 0xF6000000) == 0xF6000000) {
                    csf6 = &(m->mtsurfaces_[xm + y + zp]);
                    z6 = it->j.z + 1;
                    fastxyz6 = (it->j.x - 1)
                        | (it->j.y << 8) | (z6 << 16);
                }
            }
            if (csf->idjl != 0) {
                if ( (csf->idjl & 0x000000FF) == 0x000000F0) {
                    csf0 = &(m->mtsurfaces_[x + yp + z]);
                    z0 = it->j.z;
                    fastxyz0 = it->j.x
                        | ((it->j.y + 1) << 8) | (z0 << 16);
                }
                if ( (csf->idjl & 0x0000F200) == 0x0000F200) {
                    csf2 = &(m->mtsurfaces_[xp + y + z]);
                    z2 = it->j.z;
                    fastxyz2 = (it->j.x + 1)
                        | (it->j.y << 8) | (z2 << 16);
                }
                if ( (csf->idjl & 0x00F40000) == 0x00F40000) {
                    csf4 = &(m->mtsurfaces_[x + ym + z]);
                    z4 = it->j.z;
                    fastxyz4 = it->j.x
                        | ((it->j.y - 1) << 8) | (z4 << 16);
                }
                if ( (csf->idjl & 0xF6000000) == 0xF6000000) {
                    csf6 = &(m->mtsurfaces_[xm + y + z]);
                    z6 = it->j.z;
                    fastxyz6 = (it->j.x - 1)
                        | (it->j.y << 8) | (z6 << 16);
                }
            }
            if (prmj.size() != 0) {
                if (prmj.front().pj->t != it->j.pj->t) {
                    if (z0 != -1) {
                        if (csf0->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyz0) {
                            toadd.x = it->j.x;
                            toadd.y = it->j.y + 1;
                            toadd.z = z0;
                            toadd.pj = csf0;
                            toadd.fastxyz = fastxyz0;
                            prmj.push_front(toadd);
                        }
                    }
                    if (z2 != -1) {
                        if (csf2->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyz2) {
                            toadd.x = it->j.x + 1;
                            toadd.y = it->j.y;
                            toadd.z = z2;
                            toadd.pj = csf2;
                            toadd.fastxyz = fastxyz2;
                            prmj.push_front(toadd);
                        }
                    }
                    if (z4 != -1) {
                        if (csf4->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyz4) {
                            toadd.x = it->j.x;
                            toadd.y = it->j.y - 1;
                            toadd.z = z4;
                            toadd.pj = csf4;
                            toadd.fastxyz = fastxyz4;
                            prmj.push_front(toadd);
                        }
                    }
                    if (z6 != -1) {
                        if (csf6->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyz6) {
                            toadd.x = it->j.x - 1;
                            toadd.y = it->j.y;
                            toadd.z = z6;
                            toadd.pj = csf6;
                            toadd.fastxyz = fastxyz6;
                            prmj.push_front(toadd);
                        }
                    }
                    prmj.push_front(it->j);
                } else {
                    prmj.push_front(it->j);
                }
            } else {
                prmj.push_front(it->j);
            }
            if (fit == tofill.end())
                break;
            if (z0 != -1) {
                if (csf0->id == fit->j.pj->id
                    && fit->j.fastxyz != fastxyz0) {
                    toadd.x = it->j.x;
                    toadd.y = it->j.y + 1;
                    toadd.z = z0;
                    toadd.pj = csf0;
                    toadd.fastxyz = fastxyz0;
                    prmj.push_front(toadd);
                }
            }
            if (z2 != -1) {
                if (csf2->id == fit->j.pj->id
                    && fit->j.fastxyz != fastxyz2) {
                    toadd.x = it->j.x + 1;
                    toadd.y = it->j.y;
                    toadd.z = z2;
                    toadd.pj = csf2;
                    toadd.fastxyz = fastxyz2;
                    prmj.push_front(toadd);
                }
            }
            if (z4 != -1) {
                if (csf4->id == fit->j.pj->id
                    && fit->j.fastxyz != fastxyz4) {
                    toadd.x = it->j.x;
                    toadd.y = it->j.y - 1;
                    toadd.z = z4;
                    toadd.pj = csf4;
                    toadd.fastxyz = fastxyz4;
                    prmj.push_front(toadd);
                }
            }
            if (z6 != -1) {
                if (csf6->id == fit->j.pj->id
                    && fit->j.fastxyz != fastxyz6) {
                    toadd.x = it->j.x - 1;
                    toadd.y = it->j.y;
                    toadd.z = z6;
                    toadd.pj = csf6;
                    toadd.fastxyz = fastxyz6;
                    prmj.push_front(toadd);
                }
            }
        } else if (it->j.pj->t == (m_sdStairs | m_sdJunction)) {
            int xl = -1;
            int yl = -1;
            int zl = -1;
            int xh = -1;
            int yh = -1;
            int zh = -1;
            switch (it->j.pj->twd) {
                case 0x01:
                    if (it->j.pj->idjl != 0) {
                        xl = it->j.x;
                        yl = it->j.y + 1;
                        zl = it->j.z - 1;
                    }
                    if (it->j.pj->idjh != 0) {
                        xh = it->j.x;
                        yh = it->j.y - 1;
                        zh = it->j.z;
                    }
                    break;
                case 0x02:
                    if (it->j.pj->idjl != 0) {
                        xl = it->j.x;
                        yl = it->j.y - 1;
                        zl = it->j.z - 1;
                    }
                    if (it->j.pj->idjh != 0) {
                        xh = it->j.x;
                        yh = it->j.y + 1;
                        zh = it->j.z;
                    }
                    break;
                case 0x03:
                    if (it->j.pj->idjl != 0) {
                        xl = it->j.x - 1;
                        yl = it->j.y;
                        zl = it->j.z - 1;
                    }
                    if (it->j.pj->idjh != 0) {
                        xh = it->j.x + 1;
                        yh = it->j.y;
                        zh = it->j.z;
                    }
                    break;
                case 0x04:
                    if (it->j.pj->idjl != 0) {
                        xl = it->j.x + 1;
                        yl = it->j.y;
                        zl = it->j.z - 1;
                    }
                    if (it->j.pj->idjh != 0) {
                        xh = it->j.x - 1;
                        yh = it->j.y;
                        zh = it->j.z;
                    }
                    break;
            }

            surfaceDesc *cstl;
            surfaceDesc *csth;
            int fastxyzl, fastxyzh;
            if (xl != -1) {
                cstl = &(m->mtsurfaces_[xl
                    + yl * m->mmax_x_ + zl * m->mmax_m_xy]);
                fastxyzl = xl | (yl << 8) | (zl << 16);
            }
            if (xh != -1) {
                csth = &(m->mtsurfaces_[xh
                    + yh * m->mmax_x_ + zh * m->mmax_m_xy]);
                fastxyzh = xh | (yh << 8) | (zh << 16);
            }
            if (prmj.size() != 0) {
                if (prmj.front().pj->t != it->j.pj->t) {
                    if (xl != -1) {
                        if (cstl->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyzl) {
                            toadd.x = xl;
                            toadd.y = yl;
                            toadd.z = zl;
                            toadd.pj = cstl;
                            toadd.fastxyz = fastxyzl;
                            prmj.push_front(toadd);
                        }
                    }
                    if (xh != -1) {
                        if (csth->id == prmj.front().pj->id
                            && prmj.front().fastxyz != fastxyzh) {
                            toadd.x = xh;
                            toadd.y = yh;
                            toadd.z = zh;
                            toadd.pj = csth;
                            toadd.fastxyz = fastxyzh;
                            prmj.push_front(toadd);
                        }
                    }
                    prmj.push_front(it->j);
                } else {
                    prmj.push_front(it->j);
                }
            } else {
                prmj.push_front(it->j);
            }
            if (fit == tofill.end())
                break;
            if (xl != -1 && it->j.pj->idjl == fit->j.pj->id) {
                if (fit->j.fastxyz != fastxyzl) {
                    toadd.x = xl;
                    toadd.y = yl;
                    toadd.z = zl;
                    toadd.pj = cstl;
                    toadd.fastxyz = fastxyzl;
                    prmj.push_front(toadd);
                }
            }
            if (xh != -1 && it->j.pj->idjh == fit->j.pj->id) {
                if (fit->j.fastxyz != fastxyzh) {
                    toadd.x = xh;
                    toadd.y = yh;
                    toadd.z = zh;
                    toadd.pj = csth;
                    toadd.fastxyz = fastxyzh;
                    prmj.push_front(toadd);
                }
            }
        }
    }

    for (std::list <junctionDesc>::iterator it = prmj.begin();
        it != prmj.end();it++) {
            printf("^^^ t %i, x %i, y %i, z %i, id %i ^^^\n",it->pj->t, it->x, it->y, it->z, it->pj->id);
    }
    printf("linked in %i.%i\n", (SDL_GetTicks() - starttime)/1000,
        (SDL_GetTicks() - starttime)%1000);

    std::list<PathNode> pathchunk;
    int old_x = tile_x_;
    int old_y = tile_y_;
    int old_ox = off_x_;
    int old_oy = off_y_;
    for (std::list <junctionDesc>::iterator it = prmj.begin();
        it != prmj.end(); it++) {
        std::list <junctionDesc>::iterator fit = it;
        fit++;
        if (it == prmj.begin()) {
            if ((based->t & m_sdSurface) == m_sdSurface) {
                if (it->x != tile_x_ || it->y != tile_y_
                    || it->z != tile_z_) {
                    getPathAtSurfaceP(m, &pathchunk, it->x, it->y, it->z);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                    pathchunk.clear();
                    tile_x_ = it->x;
                    tile_y_ = it->y;
                    tile_z_ = it->z;
                    off_x_ = 128;
                    off_y_ = 128;
                    off_z_ = 0;
                    based = &(m->mtsurfaces_[tile_x_ +
                        tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                } else {
                    it = fit;
                    if (it != prmj.end()) {
                        tile_x_ = it->x;
                        tile_y_ = it->y;
                        tile_z_ = it->z;
                        off_x_ = 128;
                        off_y_ = 128;
                        off_z_ = 0;
                        based = &(m->mtsurfaces_[tile_x_ +
                            tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        fit++;
                    }
                }
            } else if ((based->t & m_sdStairs) == m_sdStairs) {
                if (it->x != tile_x_ || it->y != tile_y_
                    || it->z != tile_z_) {
                    getPathAtStairsP(m, &pathchunk, it->x, it->y, it->z);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                    pathchunk.clear();
                    tile_x_ = it->x;
                    tile_y_ = it->y;
                    tile_z_ = it->z;
                    off_x_ = 128;
                    off_y_ = 128;
                    off_z_ = 0;
                    based = &(m->mtsurfaces_[tile_x_ +
                        tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                } else {
                    it = fit;
                    if (it != prmj.end()) {
                        tile_x_ = it->x;
                        tile_y_ = it->y;
                        tile_z_ = it->z;
                        off_x_ = 128;
                        off_y_ = 128;
                        off_z_ = 0;
                        based = &(m->mtsurfaces_[tile_x_ +
                            tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        fit++;
                    }
                }
            }
        } else {
            if ((based->t & m_sdSurface) == m_sdSurface) {
                if ((it->pj->t & m_sdSurface) == m_sdSurface) {
                    dest_path_.push_back(PathNode(tile_x_, tile_y_,
                        tile_z_, off_x_, off_y_, off_z_));
                    getPathAtSurfaceP(m, &pathchunk, it->x, it->y, it->z);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                    pathchunk.clear();
                    tile_x_ = it->x;
                    tile_y_ = it->y;
                    tile_z_ = it->z;
                    off_x_ = 128;
                    off_y_ = 128;
                    off_z_ = 0;
                    based = &(m->mtsurfaces_[tile_x_ +
                        tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                } else {
                    if (dest_path_.empty()) {
                        dest_path_.push_back(PathNode(tile_x_, tile_y_,
                            tile_z_, off_x_, off_y_, off_z_));
                        tile_x_ = it->x;
                        tile_y_ = it->y;
                        tile_z_ = it->z;
                        off_x_ = 128;
                        off_y_ = 128;
                        off_z_ = 0;
                        based = &(m->mtsurfaces_[tile_x_ +
                            tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        it = fit;
                        if (it != prmj.end()) {
                            fit++;
                        }
                    } else {
                        if (it != prmj.end()) {
                            PathNode &dref = dest_path_.back();
                            if (dref.tileX() != tile_x_ || dref.tileY() != tile_y_
                                || dref.tileZ() != tile_z_) {
                                dest_path_.push_back(PathNode(tile_x_, tile_y_,
                                    tile_z_, off_x_, off_y_, off_z_));
                            }
                            tile_x_ = it->x;
                            tile_y_ = it->y;
                            tile_z_ = it->z;
                            off_x_ = 128;
                            off_y_ = 128;
                            off_z_ = 0;
                            based = &(m->mtsurfaces_[tile_x_ +
                                tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        }
                    }
                }
            } else if ((based->t & m_sdStairs) == m_sdStairs) {
                if ((it->pj->t & m_sdStairs) == m_sdStairs) {
                    dest_path_.push_back(PathNode(tile_x_, tile_y_,
                        tile_z_, off_x_, off_y_, off_z_));
                    getPathAtStairsP(m, &pathchunk, it->x, it->y, it->z);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                    pathchunk.clear();
                    tile_x_ = it->x;
                    tile_y_ = it->y;
                    tile_z_ = it->z;
                    off_x_ = 128;
                    off_y_ = 128;
                    off_z_ = 0;
                    based = &(m->mtsurfaces_[tile_x_ +
                        tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                } else {
                    if (dest_path_.empty()) {
                        dest_path_.push_back(PathNode(tile_x_, tile_y_,
                            tile_z_, off_x_, off_y_, off_z_));
                        tile_x_ = it->x;
                        tile_y_ = it->y;
                        tile_z_ = it->z;
                        off_x_ = 128;
                        off_y_ = 128;
                        off_z_ = 0;
                        based = &(m->mtsurfaces_[tile_x_ +
                            tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        it = fit;
                        if (it != prmj.end()) {
                            fit++;
                        }
                    } else {
                        if (it != prmj.end()) {
                            PathNode &dref = dest_path_.back();
                            if (dref.tileX() != tile_x_ || dref.tileY() != tile_y_
                                || dref.tileZ() != tile_z_) {
                                dest_path_.push_back(PathNode(tile_x_, tile_y_,
                                    tile_z_, off_x_, off_y_, off_z_));
                            }
                            tile_x_ = it->x;
                            tile_y_ = it->y;
                            tile_z_ = it->z;
                            off_x_ = 128;
                            off_y_ = 128;
                            off_z_ = 0;
                            based = &(m->mtsurfaces_[tile_x_ +
                                tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                        }
                    }
                }
            }
        }
        if (fit == prmj.end()) {
            if (dest_path_.size() != 0) {
                PathNode &dref = dest_path_.back();
                if (it != prmj.end()) {
                    if (dref.tileX() != it->x || dref.tileY() != it->y
                        || dref.tileZ() != it->z) {
                        dest_path_.push_back(PathNode(it->x, it->y,
                            it->z));
                    }
                    tile_x_ = it->x;
                    tile_y_ = it->y;
                    tile_z_ = it->z;
                    off_x_ = 128;
                    off_y_ = 128;
                    off_z_ = 0;
                    based = &(m->mtsurfaces_[tile_x_ +
                        tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
                }
                if (tile_x_ != x || tile_y_ != y
                    || tile_z_ != z) {
                    if ((targetd->t & m_sdStairs) == m_sdStairs)
                        getPathAtStairsP(m, &pathchunk, x, y, z, ox, oy, oz);
                    if ((targetd->t & m_sdSurface) == m_sdSurface)
                        getPathAtSurfaceP(m, &pathchunk, x, y, z, ox, oy, oz);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                } else {
                    dest_path_.push_back(PathNode(x, y, z, ox, oy, oz));
                }
            } else {
                if (tile_x_ != x || tile_y_ != y || tile_z_ != z) {
                    if ((targetd->t & m_sdStairs) == m_sdStairs)
                        getPathAtStairsP(m, &pathchunk, x, y, z, ox, oy, oz);
                    if ((targetd->t & m_sdSurface) == m_sdSurface)
                        getPathAtSurfaceP(m, &pathchunk, x, y, z, ox, oy, oz);
                    for (std::list<PathNode>::iterator sit = pathchunk.begin();
                        sit != pathchunk.end(); sit++)
                        dest_path_.push_back(*sit);
                } else
                    dest_path_.push_back(PathNode(x, y, z, ox, oy, oz));
            }
        }
        if(it == prmj.end())
            break;
    }

    for (std::list <PathNode>::iterator it = dest_path_.begin();
        it != dest_path_.end();it++) {
            printf("---t %i, x %i, y %i, z %i ---\n",
                m->mtsurfaces_[it->tileX() + it->tileY() * m->mmax_x_ + it->tileZ() * m->mmax_m_xy].t,
                it->tileX(), it->tileY(), it->tileZ());
    }
    if (dest_path_.size() != 0)
        speed_ = new_speed;
    tile_x_ = old_x;
    tile_y_ = old_y;
    tile_z_ = old_z;
    off_x_ = old_ox;
    off_y_ = old_oy;
    off_z_ = old_oz;
    for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
        delete lvls[i];
    }
    printf("path set in %i.%i\n", (SDL_GetTicks() - starttime)/1000,
        (SDL_GetTicks() - starttime)%1000);
}
//#endif
#if 0
// this algorithm can reach a corner and stop, as such it will
// not produce path, also path if reached is not always the shortest one
void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                     int ox, int oy, int oz, int new_speed) {
    m->adjXYZ(x, y, z);
    dest_path_.clear();
    setSpeed(0);
    printf("target x : %i; y : %i; z : %i = = ox :%i, oy :%i, oz :%i\n",
        x, y, z, ox, oy, oz);

    surfaceDesc *targetd = &(m->mtsurfaces_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    //if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0)
        //return;

    int old_z = tile_z_;
    int old_oz = off_z_;
    tile_z_ += (off_z_ == 0 ? 0 : 1);
    off_z_ = 0;
    surfaceDesc *based = &(m->mtsurfaces_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);

    printf("base %i, bt %i, target %i, tt %i\n",based->id, based->t,targetd->id,targetd->t);
    printf("btwd %i, ttwd %i\n",based->twd, targetd->twd);
    printf("base pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
        tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);

    //printf("tt dir %x, idh %x, idl %x\n", targetd->dir, targetd->idjh, targetd->idjl);
    //return;
    if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0) {
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if(based->t == m_sdNonwalkable) {
        printf("Movement from nonwalkable postion\n");
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if (in_vehicle_) {
        if(in_vehicle_->tileX() != x
            || in_vehicle_->tileY() != y
            || in_vehicle_->tileZ() != z
            || in_vehicle_->offX() != ox
            || in_vehicle_->offY() != oy
            || in_vehicle_->offZ() != oz)
        in_vehicle_ = 0;
    }
    if (pickup_weapon_) {
        if(pickup_weapon_->tileX() != x
            || pickup_weapon_->tileY() != y
            || pickup_weapon_->tileZ() != z
            || pickup_weapon_->offX() != ox
            || pickup_weapon_->offY() != oy
            || pickup_weapon_->offZ() != oz)
        pickup_weapon_ = 0;
    }

    if (tile_x_ == x && tile_y_ == y && tile_z_ == z) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy, oz));
        speed_ = new_speed;
        return;
    }

    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;
    printf("time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);

    PathNode closest;
    float closest_dist = 100000;

    open.insert(PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_,off_z_));
    int watchDog = 100000;
    while (!open.empty()) {
        watchDog--;
        float dist = 10000;
        PathNode p;
        std::set < PathNode >::iterator pit;
        for (std::set < PathNode >::iterator it = open.begin();
             it != open.end(); it++) {
            float d =
                sqrt((float) (x - it->tileX()) * (x - it->tileX()) +
                     (y - it->tileY()) * (y - it->tileY())
                     + (z - it->tileZ()) * (z - it->tileZ()));
            if (d < dist) {
                dist = d;
                p = *it;
                pit = it;
            }
        }
        if (dist < closest_dist) {
            closest = p;
            closest_dist = dist;
        }
        //printf("found best dist %f in %i nodes\n", dist, open.size());
        open.erase(pit);
        closed.insert(p);

        if ((p.tileX() == x && p.tileY() == y && p.tileZ() == z)
            || watchDog < 0) {
            if (watchDog < 0) {
                p = closest;
                dest_path_.push_front(PathNode
                           (p.tileX(), p.tileY(), p.tileZ(), ox, oy, oz));
            } else
                dest_path_.push_front(PathNode(x, y, z, ox, oy, oz));
            while (parent.find(p) != parent.end()) {
                p = parent[p];
                if (p.tileX() == tile_x_ && p.tileY() == tile_y_
                    && p.tileZ() == tile_z_)
                    break;
                dest_path_.push_front(p);
            }
            break;
        }

        std::list < PathNode > neighbours;
        surfaceDesc *csp = &(m->mtsurfaces_[p.tileX() + p.tileY() * m->mmax_x_
            + p.tileZ() * m->mmax_m_xy]);

        if ((csp->t & m_sdSurface) == m_sdSurface) {
            if ((csp->dir & 0x0000000F) == 0x00000000)
                neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                       p.tileZ()));
            if ((csp->dir & 0x000000F0) == 0x00000010)
                neighbours.push_back(PathNode (p.tileX() + 1, p.tileY() + 1,
                       p.tileZ()));
            if ((csp->dir & 0x00000F00) == 0x00000200)
                neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                       p.tileZ()));
            if ((csp->dir & 0x0000F000) == 0x00003000)
                neighbours.push_back(PathNode (p.tileX() + 1, p.tileY() - 1,
                       p.tileZ()));
            if ((csp->dir & 0x000F0000) == 0x00040000)
                neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                       p.tileZ()));
            if ((csp->dir & 0x00F00000) == 0x00500000)
                neighbours.push_back(PathNode (p.tileX() - 1, p.tileY() - 1,
                       p.tileZ()));
            if ((csp->dir & 0x0F000000) == 0x06000000)
                neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                       p.tileZ()));
            if ((csp->dir & 0xF0000000) == 0x70000000)
                neighbours.push_back(PathNode (p.tileX() - 1, p.tileY() + 1,
                       p.tileZ()));
            if (csp->idjh != 0) {
                if ( (csp->idjh & 0x000000FF) == 0x000000F0) {
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                           p.tileZ() + 1));
                }
                if ( (csp->idjh & 0x0000F200) == 0x0000F200) {
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                           p.tileZ() + 1));
                }
                if ( (csp->idjh & 0x00F40000) == 0x00F40000) {
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                           p.tileZ() + 1));
                }
                if ( (csp->idjh & 0xF6000000) == 0xF6000000) {
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                           p.tileZ() + 1));
                }
            }
            if (csp->idjl != 0) {
                if ( (csp->idjl & 0x000000FF) == 0x000000F0) {
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                           p.tileZ()));
                }
                if ( (csp->idjl & 0x0000F200) == 0x0000F200) {
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                           p.tileZ()));
                }
                if ( (csp->idjl & 0x00F40000) == 0x00F40000) {
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                           p.tileZ()));
                }
                if ( (csp->idjl & 0xF6000000) == 0xF6000000) {
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                           p.tileZ()));
                }
            }
        } else if ((csp->t & m_sdStairs) == m_sdStairs) {
            switch (csp->twd) {
                case 0x01:
                    if ((csp->dir & 0x000000FF) == 0x000000F0)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                            p.tileZ() - 1));
                    if ((csp->dir & 0x0000FF00) == 0x0000F200)
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                            p.tileZ()));
                    if ((csp->dir & 0x00FF0000) == 0x00F40000)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                            p.tileZ() + 1));
                    if ((csp->dir & 0xFF000000) == 0xF6000000)
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                            p.tileZ()));
                    if (csp->idjl != 0) {
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                               p.tileZ() - 1));
                    }
                    if (csp->idjh != 0) {
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                               p.tileZ()));
                    }
                    break;
                case 0x02:
                    if ((csp->dir & 0x000000FF) == 0x000000F0)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                            p.tileZ() + 1));
                    if ((csp->dir & 0x0000FF00) == 0x0000F200)
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                            p.tileZ()));
                    if ((csp->dir & 0x00FF0000) == 0x00F40000)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                            p.tileZ() - 1));
                    if ((csp->dir & 0xFF000000) == 0xF6000000)
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                            p.tileZ()));
                    if (csp->idjl != 0) {
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                               p.tileZ() - 1));
                    }
                    if (csp->idjh != 0) {
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                               p.tileZ()));
                    }
                    break;
                case 0x03:
                    if ((csp->dir & 0x000000FF) == 0x000000F0)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                            p.tileZ()));
                    if ((csp->dir & 0x0000FF00) == 0x0000F200)
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                            p.tileZ() + 1));
                    if ((csp->dir & 0x00FF0000) == 0x00F40000)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                            p.tileZ()));
                    if ((csp->dir & 0xFF000000) == 0xF6000000)
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                            p.tileZ() - 1));
                    if (csp->idjl != 0) {
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                               p.tileZ() - 1));
                    }
                    if (csp->idjh != 0) {
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                               p.tileZ()));
                    }
                    break;
                case 0x04:
                    if ((csp->dir & 0x000000FF) == 0x000000F0)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                            p.tileZ()));
                    if ((csp->dir & 0x0000FF00) == 0x0000F200)
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                            p.tileZ() - 1));
                    if ((csp->dir & 0x00FF0000) == 0x00F40000)
                        neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                            p.tileZ()));
                    if ((csp->dir & 0xFF000000) == 0xF6000000)
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                            p.tileZ() + 1));
                    if (csp->idjl != 0) {
                        neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                               p.tileZ() - 1));
                    }
                    if (csp->idjh != 0) {
                        neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                               p.tileZ()));
                    }
                    break;
            }
        }

        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }
    if(dest_path_.size() != 0)
        speed_ = new_speed;
    printf("end time %i.%i, iterations %i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000, watchDog);
}
#endif

#if 0
void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                     int ox, int oy, int oz, int new_speed) {
    m->adjXYZ(x, y, z);
    dest_path_.clear();
    setSpeed(0);
    printf("target x : %i; y : %i; z : %i = = ox :%i, oy :%i, oz :%i\n",
        x, y, z, ox, oy, oz);

    surfaceDesc *targetd = &(m->mtsurfaces_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    //if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0)
        //return;

    int old_z = tile_z_;
    int old_oz = off_z_;
    tile_z_ += (off_z_ == 0 ? 0 : 1);
    off_z_ = 0;
    surfaceDesc *based = &(m->mtsurfaces_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);

    printf("base %i, bt %i, target %i, tt %i\n",based->id, based->t,targetd->id,targetd->t);
    printf("btwd %i, ttwd %i\n",based->twd, targetd->twd);
    printf("base pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
        tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);

    //printf("tt dir %x, idh %x, idl %x\n", targetd->dir, targetd->idjh, targetd->idjl);
    //return;
    if(targetd->t == m_sdNonwalkable || map_ == -1 || health_ <= 0) {
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if(based->t == m_sdNonwalkable) {
        printf("Movement from nonwalkable postion\n");
        tile_z_ = old_z;
        off_z_ = old_oz;
        return;
    }

    if (in_vehicle_) {
        if(in_vehicle_->tileX() != x
            || in_vehicle_->tileY() != y
            || in_vehicle_->tileZ() != z
            || in_vehicle_->offX() != ox
            || in_vehicle_->offY() != oy
            || in_vehicle_->offZ() != oz)
        in_vehicle_ = 0;
    }
    if (pickup_weapon_) {
        if(pickup_weapon_->tileX() != x
            || pickup_weapon_->tileY() != y
            || pickup_weapon_->tileZ() != z
            || pickup_weapon_->offX() != ox
            || pickup_weapon_->offY() != oy
            || pickup_weapon_->offZ() != oz)
        pickup_weapon_ = 0;
    }

    if (tile_x_ == x && tile_y_ == y && tile_z_ == z) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy, oz));
        speed_ = new_speed;
        return;
    }
    printf("time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
    floodPointDesc *mdpmirror = NULL;
    mdpmirror = (floodPointDesc *)malloc(m->mmax_m_all * sizeof(floodPointDesc));
    if (mdpmirror == NULL) {
        printf("not enough memory: setDestinationP\n");
        return;
    }
    memcpy(mdpmirror, m->mdpoints_, m->mmax_m_all * sizeof(floodPointDesc));

    unsigned char lt;
    unsigned short blvl = 0, tlvl = 0;
    std::vector <toSetDesc> bv;
    std::vector <toSetDesc> tv;
    std::set <unsigned short> lv;
    toSetDesc sadd;
    floodPointDesc *pfdp;
    pfdp = &(m->mdpoints_[tile_x_ + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    pfdp->t |= (m_fdBasePoint | m_fdLink | m_fdConstant);
    sadd.coords.x = tile_x_;
    sadd.coords.y = tile_y_;
    sadd.coords.z = tile_z_;
    sadd.p = pfdp;
    bv.push_back(sadd);
    pfdp = &(m->mdpoints_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);
    pfdp->t |= (m_fdTargetPoint | m_fdLink | m_fdConstant);
    sadd.coords.x = x;
    sadd.coords.y = y;
    sadd.coords.z = z;
    sadd.p = pfdp;
    tv.push_back(sadd);
    lvlNodesDesc ladd;
    ladd.indxs = 0;
    ladd.n = 1;
    std::vector <lvlNodesDesc> bn;
    std::vector <lvlNodesDesc> tn;
    bn.push_back(ladd);
    tn.push_back(ladd);
    bool nodeset;

    do {
        unsigned short mindx = bn[blvl].indxs + bn[blvl].n;
        for (unsigned short i = bn[blvl].indxs; i < mindx; i++) {
            toSetDesc bref = bv[i];
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
            }
        }
        ladd.indxs = mindx;
        ladd.n = bv.size() - mindx;
        if (ladd.n > 0) {
            nodeset = true;
            bn.push_back(ladd);
            blvl++;
        } else
            nodeset = false;
        if (!(nodeset && lv.size() == 0))
            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        mindx = tn[tlvl].indxs + tn[tlvl].n;
        for (unsigned short i = tn[tlvl].indxs; i < mindx; i++) {
            toSetDesc bref = tv[i];
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(m->mdpoints_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
            }
        }
        ladd.indxs = mindx;
        ladd.n = tv.size() - mindx;
        if (ladd.n > 0) {
            nodeset = true;
            tn.push_back(ladd);
            tlvl++;
        } else
            nodeset = false;
    } while (nodeset && lv.size() == 0);
    printf("bv %i, tv %i\n", bv.size(), tv.size());


    free(mdpmirror);
    if(dest_path_.size() != 0)
        speed_ = new_speed;
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
}
#endif

void PedInstance::getPathAtStairsP(Mission *m, std::list<PathNode> *new_path,
                                    int x, int y, int z, int ox, int oy, int oz)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    PathNode closest;
    float closest_dist = 100000;

    open.insert(PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_,off_z_));
    int watchDog = 2000;
    while (!open.empty()) {
        watchDog--;
        float dist = 100000;
        PathNode p;
        std::set < PathNode >::iterator pit;
        for (std::set < PathNode >::iterator it = open.begin();
             it != open.end(); it++) {
            float d =
                sqrt((float) (x - it->tileX()) * (x - it->tileX()) +
                     (y - it->tileY()) * (y - it->tileY()));
            if (d < dist) {
                dist = d;
                p = *it;
                pit = it;
            }
        }
        if (dist < closest_dist) {
            closest = p;
            closest_dist = dist;
        }
        //printf("found best dist %f in %i nodes\n", dist, open.size());
        open.erase(pit);
        closed.insert(p);

        if ((p.tileX() == x && p.tileY() == y && p.tileZ() == z)
            || watchDog < 0) {
            if (watchDog < 0) {
                p = closest;
                new_path->push_front(PathNode
                           (p.tileX(), p.tileY(), p.tileZ(), ox, oy, oz));
            } else
                new_path->push_front(PathNode(x, y, z, ox, oy, oz));
            while (parent.find(p) != parent.end()) {
                p = parent[p];
                if (p.tileX() == tile_x_ && p.tileY() == tile_y_
                    && p.tileZ() == tile_z_)
                    break;
                new_path->push_front(p);
            }
            break;
        }

        std::list < PathNode > neighbours;
        surfaceDesc *cst = &(m->mtsurfaces_[p.tileX() + p.tileY() * m->mmax_x_
            + p.tileZ() * m->mmax_m_xy]);
        switch (cst->twd) {
            case 0x01:
                if ((cst->dir & 0x000000FF) == 0x000000F0)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                        p.tileZ() - 1));
                if ((cst->dir & 0x0000FF00) == 0x0000F200)
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                        p.tileZ()));
                if ((cst->dir & 0x00FF0000) == 0x00F40000)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                        p.tileZ() + 1));
                if ((cst->dir & 0xFF000000) == 0xF6000000)
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                        p.tileZ()));
                break;
            case 0x02:
                if ((cst->dir & 0x000000FF) == 0x000000F0)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                        p.tileZ() + 1));
                if ((cst->dir & 0x0000FF00) == 0x0000F200)
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                        p.tileZ()));
                if ((cst->dir & 0x00FF0000) == 0x00F40000)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                        p.tileZ() - 1));
                if ((cst->dir & 0xFF000000) == 0xF6000000)
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                        p.tileZ()));
                break;
            case 0x03:
                if ((cst->dir & 0x000000FF) == 0x000000F0)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                        p.tileZ()));
                if ((cst->dir & 0x0000FF00) == 0x0000F200)
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                        p.tileZ() + 1));
                if ((cst->dir & 0x00FF0000) == 0x00F40000)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                        p.tileZ()));
                if ((cst->dir & 0xFF000000) == 0xF6000000)
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                        p.tileZ() - 1));
                break;
            case 0x04:
                if ((cst->dir & 0x000000FF) == 0x000000F0)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                        p.tileZ()));
                if ((cst->dir & 0x0000FF00) == 0x0000F200)
                    neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                        p.tileZ() - 1));
                if ((cst->dir & 0x00FF0000) == 0x00F40000)
                    neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                        p.tileZ()));
                if ((cst->dir & 0xFF000000) == 0xF6000000)
                    neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                        p.tileZ() + 1));
                break;
        }

        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }
}

void PedInstance::getPathAtSurfaceP(Mission *m, std::list<PathNode> *new_path,
                                    int x, int y, int z, int ox, int oy, int oz)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    PathNode closest;
    float closest_dist = 100000;

    open.insert(PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_,off_z_));
    int watchDog = 2000;
    while (!open.empty()) {
        watchDog--;
        float dist = 100000;
        PathNode p;
        std::set < PathNode >::iterator pit;
        for (std::set < PathNode >::iterator it = open.begin();
             it != open.end(); it++) {
            float d =
                sqrt((float) (x - it->tileX()) * (x - it->tileX()) +
                     (y - it->tileY()) * (y - it->tileY()));
            if (d < dist) {
                dist = d;
                p = *it;
                pit = it;
            }
        }
        if (dist < closest_dist) {
            closest = p;
            closest_dist = dist;
        }
        //printf("found best dist %f in %i nodes\n", dist, open.size());
        open.erase(pit);
        closed.insert(p);

        if ((p.tileX() == x && p.tileY() == y && p.tileZ() == z)
            || watchDog < 0) {
            if (watchDog < 0) {
                p = closest;
                new_path->push_front(PathNode
                       (p.tileX(), p.tileY(), p.tileZ(), ox, oy, oz));
            } else
                new_path->push_front(PathNode(x, y, z, ox, oy, oz));
            while (parent.find(p) != parent.end()) {
                p = parent[p];
                if (p.tileX() == tile_x_ && p.tileY() == tile_y_
                    && p.tileZ() == tile_z_)
                    break;
                new_path->push_front(p);
            }
            break;
        }

        std::list < PathNode > neighbours;
        surfaceDesc *csf = &(m->mtsurfaces_[p.tileX() + p.tileY() * m->mmax_x_
            + p.tileZ() * m->mmax_m_xy]);

        if ((csf->dir & 0x0000000F) == 0x00000000)
            neighbours.push_back(PathNode (p.tileX(), p.tileY() + 1,
                   p.tileZ()));
        if ((csf->dir & 0x000000F0) == 0x00000010)
            neighbours.push_back(PathNode (p.tileX() + 1, p.tileY() + 1,
                   p.tileZ()));
        if ((csf->dir & 0x00000F00) == 0x00000200)
            neighbours.push_back(PathNode (p.tileX() + 1, p.tileY(),
                   p.tileZ()));
        if ((csf->dir & 0x0000F000) == 0x00003000)
            neighbours.push_back(PathNode (p.tileX() + 1, p.tileY() - 1,
                   p.tileZ()));
        if ((csf->dir & 0x000F0000) == 0x00040000)
            neighbours.push_back(PathNode (p.tileX(), p.tileY() - 1,
                   p.tileZ()));
        if ((csf->dir & 0x00F00000) == 0x00500000)
            neighbours.push_back(PathNode (p.tileX() - 1, p.tileY() - 1,
                   p.tileZ()));
        if ((csf->dir & 0x0F000000) == 0x06000000)
            neighbours.push_back(PathNode (p.tileX() - 1, p.tileY(),
                   p.tileZ()));
        if ((csf->dir & 0xF0000000) == 0x70000000)
            neighbours.push_back(PathNode (p.tileX() - 1, p.tileY() + 1,
                   p.tileZ()));


        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }
}
void PedInstance::addDestinationP(Mission *m, int x, int y, int z,
                                  int ox, int oy, int new_speed) {
    // TODO: adding destination maybe difficult because of
    // action currently in execution (pickup/putdown weapon, etc.)
    //z = tile_z_;
    dest_path_.push_back(PathNode(x, y, z, ox, oy));
    speed_ = new_speed;
}

bool PedInstance::movementP(int elapsed)
{
    bool updated = false;

    if (!dest_path_.empty()) {
        int nxtTileX = dest_path_.front().tileX();
        int nxtTileY = dest_path_.front().tileY();
        int nxtTileZ = dest_path_.front().tileZ();
        if (hold_on_.wayFree == 1) {
            if (hold_on_.tilex == nxtTileX && hold_on_.tiley == nxtTileY
                && hold_on_.tilez == nxtTileZ)
                return updated;
            else
                hold_on_.wayFree = 0;
        } else if (hold_on_.wayFree == 2) {
            if (hold_on_.xadj || hold_on_.yadj) {
                if(abs(hold_on_.tilex - nxtTileX) <= hold_on_.xadj
                    && abs(hold_on_.tiley - nxtTileY) <= hold_on_.yadj
                    && hold_on_.tilez == nxtTileZ) {
                    dest_path_.clear();
                    speed_ = 0;
                    return updated;
                } else
                    hold_on_.wayFree = 0;
            } else {
                if (hold_on_.tilex == nxtTileX && hold_on_.tiley == nxtTileY
                    && hold_on_.tilez == nxtTileZ) {
                    dest_path_.clear();
                    speed_ = 0;
                    return updated;
                } else
                    hold_on_.wayFree = 0;
            }
        }
        int adx =
             nxtTileX * 256 + dest_path_.front().offX();
        int ady =
             nxtTileY * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;

        if (abs(adx - atx) < 16 && abs(ady - aty) < 16) {
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_z_ = nxtTileZ;
            tile_y_ = nxtTileY;
            tile_x_ = nxtTileX;
            dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;
            updated = true;
        } else {

            // TODO: something better?
            int fuzz = 16;
            if (ady < (aty - fuzz)) {
                if (adx < (atx - fuzz))
                    dir_ = 5;
                else if (adx > (atx + fuzz))
                    dir_ = 3;
                else if (adx < (atx + fuzz))
                    dir_ = 4;
            } else if (abs(ady - aty) < fuzz) {
                if (adx < (atx - fuzz))
                    dir_ = 6;
                else if (adx > (atx + fuzz))
                    dir_ = 2;
            } else if (abs(ady - aty) > fuzz) {
                if (adx < (atx - fuzz))
                    dir_ = 7;
                else if (adx > (atx + fuzz))
                    dir_ = 1;
                else if (adx < (atx + fuzz))
                    dir_ = 0;
            }

            int dx = 0, dy = 0;
            int d =
                (int) sqrt((float) (adx - atx) * (adx - atx) +
                           (ady - aty) * (ady - aty));

            if (abs(adx - atx) > 0)
                dx = (adx - atx) * (speed_ * elapsed / 1000) / d;
            if (abs(ady - aty) > 0)
                dy = (ady - aty) * (speed_ * elapsed / 1000) / d;

            if (abs(dx) > abs(adx - atx))
                dx = (adx - atx);
            if (abs(dy) > abs(ady - aty))
                dy = (ady - aty);

            if (updatePlacement(off_x_ + dx, off_y_ + dy)) {
                ;
            } else {
                // TODO: avoid obstacles.
                speed_ = 0;
            }
            if(nxtTileX == tile_x_ && nxtTileY == tile_y_)
                tile_z_ = nxtTileZ;
            if(nxtTileX == tile_x_ && nxtTileY == tile_y_
                && nxtTileZ == tile_z_ 
                && dest_path_.front().offX() == off_x_
                && dest_path_.front().offY() == off_y_
                && dest_path_.front().offZ() == off_z_)
                dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;

            updated = true;
        }
    } else if (speed_) {
        printf("Running at speed %i, destination unknown\n", speed_);
        speed_ = 0;
    }

    return updated;
}
