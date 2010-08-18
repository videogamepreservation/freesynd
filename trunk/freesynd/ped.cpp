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

double PedInstance::getDistance(int x1, int y1, int z1,
                                int x2, int y2, int z2, Mission *m) {

    return sqrt(pow((double)(x2 - x1), 2) + pow((double)(y2 - y1), 2)
        + pow((double)(z2 - z1), 2));
}

double PedInstance::calcDistance(unsigned short lvl, toDefineXYZ posxyz,
             std::vector <linkDesc> ::iterator par) {
    double dist = 0;
    while(lvl != 0) {
        dist += getDistance(posxyz.x, posxyz.y, posxyz.z,
            par->j.x, par->j.y, par->j.z, NULL);
        posxyz.x = par->j.x;
        posxyz.y = par->j.y;
        posxyz.z = par->j.z;
        par = (std::vector <linkDesc> ::iterator)(par->p);
        lvl--;
    }
    dist += getDistance(posxyz.x, posxyz.y, posxyz.z,
        tile_x_, tile_y_, tile_z_, NULL);
    return dist;
}

void PedInstance::markBadNodes(unsigned short lvl, unsigned short clvl,
                            toDefineXYZ posxyz, std::vector <linkDesc> ** lvls) {
    std::vector <linkDesc> * curlvl = lvls[clvl];
    bool found = true;
    while (curlvl != NULL && found) {
        found = false;
        for (std::vector <linkDesc> ::iterator it = curlvl->begin();
            it != curlvl->end(); it++) {
            if (!it->bad) {
                if (it->pcoord.x == posxyz.x && it->pcoord.y == posxyz.y
                    && it->pcoord.z == posxyz.z) {
                    if (it->n != 0) {
                        toDefineXYZ curxyz;
                        curxyz.x = it->j.x;
                        curxyz.y = it->j.y;
                        curxyz.z = it->j.z;
                        markBadNodes(lvl, clvl + 1, curxyz, lvls);
                    }
                    it->bad = true;
                    it->destr = false;
                    found = true;
                }
            }
        }
        clvl++;
        if (clvl > lvl)
            break;
        curlvl = lvls[clvl];
    }
}

bool PedInstance::setLvlNode(std::vector <junctionDesc> * pjunctions,
    std::vector <linkDesc> ::iterator it, std::vector <linkDesc> ** lvls,
    std::vector <reachedDesc> * preached, unsigned short lvlnum,
    unsigned char nt, unsigned int id, bool setreached) {

    bool gotbad = false;
    for (std::vector <junctionDesc> ::iterator stit = pjunctions->begin();
        stit != pjunctions->end(); stit++) {
        bool toadd = true;
        if (stit->pj->id == id) {
            for (std::vector <reachedDesc> ::iterator stitr = preached->begin();
                stitr != preached->end(); stitr++) {
                if (stit->x == stitr->j.x && stit->y == stitr->j.y
                    && stit->z == stitr->j.z) {
                    toDefineXYZ curxyz;
                    curxyz.x = stit->x;
                    curxyz.y = stit->y;
                    curxyz.z = stit->z;
                    double dist = calcDistance (lvlnum, curxyz, it);
                    std::vector <linkDesc> * slvl = lvls[stitr->atlevel];
                    double gdist = 0;
                    std::vector <linkDesc> ::iterator par;
                    for (std::vector <linkDesc> ::iterator getit = slvl->begin();
                        getit != slvl->end(); getit++) {
                        if (getit->j.x == stit->x && getit->j.y == stit->y
                            && getit->j.z == stit->z && !getit->bad) {
                            par = (std::vector <linkDesc> ::iterator)(getit->p);
                            gdist = calcDistance (stitr->atlevel, curxyz, par);
                            if (gdist > dist) {
                                getit->bad = true;
                                gotbad = true;
                                if (getit->destr)
                                    getit->destr = false;
                                if (stitr->atlevel != lvlnum && getit->n != 0){
                                    markBadNodes (lvlnum, stitr->atlevel, curxyz, lvls);
                                }
                                linkDesc ladd;
                                stitr->atlevel = lvlnum;
                                ladd.bad = false;
                                ladd.j = *stit;
                                ladd.n = 0;
                                ladd.nt = nt;
                                if (lvlnum > 0) {
                                    ladd.p = it;
                                    ladd.pcoord.x = it->j.x;
                                    ladd.pcoord.y = it->j.y;
                                    ladd.pcoord.z = it->j.z;
                                    it->n++;
                                }
                                ladd.destr = setreached;
                                lvls[lvlnum]->push_back(ladd);
                            }
                            toadd = false;
                            break;
                        }
                    }
                }
            }
            if (toadd) {
                linkDesc ladd;
                ladd.bad = false;
                ladd.j = *stit;
                ladd.n = 0;
                ladd.nt = nt;
                if (lvlnum > 0) {
                    ladd.p = it;
                    ladd.pcoord.x = it->j.x;
                    ladd.pcoord.y = it->j.y;
                    ladd.pcoord.z = it->j.z;
                    it->n++;
                }
                ladd.destr = setreached;
                lvls[lvlnum]->push_back(ladd);
                reachedDesc rd;
                rd.atlevel = lvlnum;
                rd.j = ladd.j;
                preached->push_back(rd);
            }
        }
    }
    return gotbad;
}

void PedInstance::setDestinationPNew(Mission *m, int x, int y, int z,
                                     int ox, int oy, int oz, int new_speed) {
    surfaceDesc *targetd = &(m->mtsurfaces_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    if(targetd->t == m_sdNonwalkable)
        return;

    surfaceDesc *based = &(m->mtsurfaces_[tile_x_ + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    unsigned short lvlnum = 0;
    bool found =  false;
    std::vector <linkDesc> * lvls[MAX_LVLS_PATH];
    memset(lvls, NULL, sizeof(std::vector <linkDesc> *) * MAX_LVLS_PATH);
    for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
        lvls[i] = new std::vector <linkDesc>;
    }
    std::vector <linkDesc> * parlvl = NULL;
    std::vector <reachedDesc> sfcreached;
    std::vector <reachedDesc> strreached;
    std::vector <linkDesc> fornull;
    unsigned char targettype = targetd->t & (m_sdStairs | m_sdSurface);
    bool isreached = targetd->id == based->id
        && targettype == (based->t & (m_sdStairs | m_sdSurface));

    switch(based->t) {
        case (m_sdStairs | m_sdJunction):
        case m_sdStairs:
            // iterator here does nothing, as there is no NULL iterator
            if(setLvlNode(&(m->strjunctions_), fornull.end(),
                lvls, &strreached, lvlnum, m_sdSurface, based->id, isreached)) {
            }
            break;
        case (m_sdSurface | m_sdJunction):
        case m_sdSurface:
            // iterator here does nothing, as there is no NULL iterator
            if(setLvlNode(&(m->sfcjunctions_), fornull.end(),
                lvls, &sfcreached, lvlnum, m_sdStairs, based->id, isreached)) {
            }
            break;
    }

    do {
        printf("lvl processed %i\n", lvlnum);
        if (lvls[lvlnum]->size() == 0)
            break;

        if (lvlnum > (MAX_LVLS_PATH - 2))
            break;

        parlvl = lvls[lvlnum];
        lvlnum++;
        for (std::vector <linkDesc> ::iterator it = parlvl->begin();
            it != parlvl->end(); it++) {
            if (!it->bad && !it->destr) {
                if (it->nt == m_sdStairs) {

                    int x = it->j.x;
                    int y = it->j.y * m->mmax_x_;
                    int z = it->j.z * m->mmax_m_xy;
                    int xm = x - 1;
                    int ym = y - m->mmax_x_;
                    int xp = x + 1;
                    int yp = y + m->mmax_x_;
                    int zp = z + m->mmax_m_xy;
                    surfaceDesc * csf = it->j.pj;
                    if (csf->idjh != 0) {
                        if ( (csf->idjh & 0x000000FF) == 0x000000F0) {
                            isreached = targetd->id == m->mtsurfaces_[x + yp + zp].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[x + yp + zp].id, isreached)) {
                            }
                        }
                        if ( (csf->idjh & 0x0000F200) == 0x0000F200) {
                            isreached = targetd->id == m->mtsurfaces_[xp + y + zp].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[xp + y + zp].id, isreached)) {
                            }
                        }
                        if ( (csf->idjh & 0x00F40000) == 0x00F40000) {
                            isreached = targetd->id == m->mtsurfaces_[x + ym + zp].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[x + ym + zp].id, isreached)) {
                            }
                        }
                        if ( (csf->idjh & 0xF6000000) == 0xF6000000) {
                            isreached = targetd->id == m->mtsurfaces_[xm + y + zp].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[xm + y + zp].id, isreached)) {
                            }
                        }
                    }
                    if (csf->idjl != 0) {
                        if ( (csf->idjl & 0x000000FF) == 0x000000F0) {
                            isreached = targetd->id == m->mtsurfaces_[x + yp + z].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface, 
                                m->mtsurfaces_[x + yp + z].id, isreached)) {
                            }
                        }
                        if ( (csf->idjl & 0x0000F200) == 0x0000F200) {
                            isreached = targetd->id == m->mtsurfaces_[xp + y + z].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[xp + y + z].id, isreached)) {
                            }
                        }
                        if ( (csf->idjl & 0x00F40000) == 0x00F40000) {
                            isreached = targetd->id == m->mtsurfaces_[x + ym + z].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[x + ym + z].id, isreached)) {
                            }
                        }
                        if ( (csf->idjl & 0xF6000000) == 0xF6000000) {
                            isreached = targetd->id == m->mtsurfaces_[xm + y + z].id
                                && targettype == m_sdStairs;
                            if (setLvlNode(&(m->strjunctions_), it, lvls,
                                &strreached, lvlnum, m_sdSurface,
                                m->mtsurfaces_[xm + y + z].id, isreached)) {
                            }
                        }
                    }
                } else if(it->nt == m_sdSurface) {
                    if (it->j.pj->idjh != 0) {
                        isreached = targetd->id == it->j.pj->idjh
                            && targettype == m_sdSurface;
                        if (setLvlNode(&(m->sfcjunctions_), it, lvls,
                            &sfcreached, lvlnum, m_sdStairs,
                            it->j.pj->idjh, isreached)) {
                        }
                    }
                    if (it->j.pj->idjl != 0) {
                        isreached = targetd->id == it->j.pj->idjl
                            && targettype == m_sdSurface;
                        if (setLvlNode(&(m->sfcjunctions_), it, lvls,
                            &sfcreached, lvlnum, m_sdStairs,
                            it->j.pj->idjl, isreached)) {
                        }
                    }
                }
            }
        }
    }while(1);

    if (lvls[0]->size() == 0) {
        for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
            lvls[i] = new std::vector <linkDesc>;
        }
        return;
    }

    std::vector <linkDesc> ::iterator reachedit;
    double dist = 0;
    double cmpdist;
    toDefineXYZ posxyz;
    unsigned short clvl;
    do {
        parlvl = lvls[lvlnum];
        for (std::vector <linkDesc> ::iterator it = parlvl->begin();
            it != parlvl->end(); it++) {
            if (it->destr) {
                if (dist == 0) {
                    reachedit = it;
                    posxyz.x = it->j.x;
                    posxyz.y = it->j.y;
                    posxyz.z = it->j.z;
                    dist = calcDistance (lvlnum, posxyz, it->p);
                    dist += getDistance (posxyz.x, posxyz.y, posxyz.z,
                        x, y, z, NULL);
                    clvl = lvlnum;
                } else {
                    posxyz.x = it->j.x;
                    posxyz.y = it->j.y;
                    posxyz.z = it->j.z;
                    cmpdist = calcDistance (lvlnum, posxyz, it->p);
                    cmpdist += getDistance (posxyz.x, posxyz.y, posxyz.z,
                        x, y, z, NULL);
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
    printf("dist %i\n", (unsigned int)dist);
    if (dist != 0) {
        clvl++;
        do {
            clvl--;
            printf("x %i, y %i, z%i\n",reachedit->j.x, reachedit->j.y, reachedit->j.z);
            reachedit = reachedit->p;
        }while(clvl != 0);
    }

    for (unsigned short i = 0; i < MAX_LVLS_PATH; i++) {
        lvls[i] = new std::vector <linkDesc>;
    }
}

void PedInstance::setDestinationP(Mission *m, int x, int y, int z, int ox,
                                       int oy, int oz, int new_speed)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    z = tile_z_;

    dest_path_.clear();
    setSpeed(0);
    /*
    printf("before x : %i; y : %i; z : %i = = ox :%i, oy :%i, oz :%i\n",
        x, y, z, ox, oy, oz);
    int nx = x * 256 + ox + 256 / 3;
    x = nx / 256;
    ox = nx % 256;
    int ny = y * 256 + oy + 256 / 3;
    y = ny / 256;
    oy = ny % 256;
    */
    printf("x : %i; y : %i; z : %i = = ox :%i, oy :%i, oz :%i\n",
        x, y, z, ox, oy, oz);

    if (map_ == -1 || health_ <= 0
        || !(walkable(x, y, z)))
        return;

    //setDestinationPNew(m, x, y, z + 1, ox, oy, oz, new_speed);
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

    if (!walkable(tile_x_, tile_y_, tile_z_)) {
        float dBest = 100000, dCur;
        int xBest,yBest;
        // we got somewhere we shouldn't, we need to find somewhere that is walkable
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        for (int j = 0; j < 5; j++)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                    }
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        if(dBest == 100000)
            return;
        else {
            tile_x_ = xBest;
            tile_y_ = yBest;
        }
    }

    PathNode closest;
    float closest_dist = 100000;

    open.insert(PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_));
    int watchDog = 3000;
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
                pit = it;       // it cannot be const_iterator because of this assign
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
                dest_path_.
                    push_front(PathNode
                               (p.tileX(), p.tileY(), p.tileZ(), ox, oy));
            } else
                dest_path_.push_front(PathNode(x, y, z, ox, oy));
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
        if (p.tileX() > 0) {
            neighbours.
                push_back(PathNode(p.tileX() - 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (walkable(p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (walkable(p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileX() < g_App.maps().map(map())->maxX()) {
            neighbours.
                push_back(PathNode(p.tileX() + 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (walkable(p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (walkable(p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileY() > 0)
            neighbours.
                push_back(PathNode(p.tileX(), p.tileY() - 1, p.tileZ()));
        if (p.tileY() < g_App.maps().map(map())->maxY())
            neighbours.
                push_back(PathNode(p.tileX(), p.tileY() + 1, p.tileZ()));

        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (walkable(it->tileX(), it->tileY(), it->tileZ())
                && open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }

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
        printf("Running speed %i, destination unknown\n", speed_);
        speed_ = 0;
    }

    return updated;
}
