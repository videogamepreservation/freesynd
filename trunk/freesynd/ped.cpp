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
        }else {
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
                    setDestinationP(mission ,in_vehicle_->tileX(),
                        in_vehicle_->tileY(), in_vehicle_->tileZ(),
                        in_vehicle_->offX(), in_vehicle_->offY(), 0, 320);
                else {
                    if(dest_path_.back().tileX() != in_vehicle_->tileX()
                        || dest_path_.back().tileY() != in_vehicle_->tileY()
                        || dest_path_.back().tileZ() != in_vehicle_->tileZ()
                        || dest_path_.back().offX() != in_vehicle_->offX()
                        || dest_path_.back().offY() != in_vehicle_->offY())
                        setDestinationP(mission ,in_vehicle_->tileX(),
                            in_vehicle_->tileY(), in_vehicle_->tileZ(),
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
                    setDestinationP(mission, pickup_weapon_->tileX(),
                        pickup_weapon_->tileY(), pickup_weapon_->tileZ(),
                        pickup_weapon_->offX(), pickup_weapon_->offY(), 0, 320);
                else {
                    if(dest_path_.back().tileX() != pickup_weapon_->tileX()
                        || dest_path_.back().tileY() != pickup_weapon_->tileY()
                        || dest_path_.back().tileZ() != pickup_weapon_->tileZ()
                        || dest_path_.back().offX() != pickup_weapon_->offX()
                        || dest_path_.back().offY() != pickup_weapon_->offY())
                        setDestinationP(mission, pickup_weapon_->tileX(),
                            pickup_weapon_->tileY(), pickup_weapon_->tileZ(),
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

void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                     int ox, int oy, int oz, int new_speed) {
    // NOTE: this is a "flood" algorithm, it expands until it reaches other's
    // flood point, then it removes unrelated points
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
    unsigned short blvl = 0, tlvl = 0, nlvl;
    std::vector <toSetDesc> bv;
    std::vector <toSetDesc> tv;
    std::set <unsigned short> lv;
    toSetDesc sadd;
    floodPointDesc *pfdp;
    pfdp = &(mdpmirror[tile_x_ + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    pfdp->t |= m_fdBasePoint;
    sadd.coords.x = tile_x_;
    sadd.coords.y = tile_y_;
    sadd.coords.z = tile_z_;
    sadd.p = pfdp;
    bv.push_back(sadd);
    pfdp = &(mdpmirror[x + y * m->mmax_x_ + z * m->mmax_m_xy]);
    pfdp->t |= (m_fdTargetPoint | m_fdConstant);
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
        nlvl = blvl + 1;
        for (unsigned short i = bn[blvl].indxs; i < mindx; i++) {
            toSetDesc bref = bv[i];
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
        nlvl = tlvl + 1;
        for (unsigned short i = tn[tlvl].indxs; i < mindx; i++) {
            toSetDesc bref = tv[i];
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y + 1;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    sadd.coords.x = bref.coords.x - 1;
                    sadd.coords.y = bref.coords.y;
                    sadd.coords.z = bref.coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    sadd.coords.x = bref.coords.x;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lv.insert(i);
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = bref.coords.x + 1;
                    sadd.coords.y = bref.coords.y - 1;
                    sadd.coords.z = bref.coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0) {
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.p->lvl = nlvl;
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
    printf("bv %i, tv %i, lv %i\n", bv.size(), tv.size(), lv.size());
    printf("btime %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);

    if (!nodeset && lv.size() == 0) {
        free(mdpmirror);
        return;
    }
    if (blvl == bn.size())
        blvl--;
    if (tlvl == tn.size())
        tlvl--;
    if (lt == m_fdBasePoint) {
        if (nodeset) {
            unsigned short indx = bn[blvl].indxs;
            unsigned short n = bn[blvl].n;
            for (unsigned short i = 0; i < n; i++) {
                std::vector <toSetDesc>::iterator it = bv.begin() + indx;
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                bv.erase(it);
            }
            bn.pop_back();
        }
        blvl--;
    } else {
        if (nodeset) {
            unsigned short indx = tn[tlvl].indxs;
            unsigned short n = tn[tlvl].n;
            for (unsigned short i = 0; i < n; i++) {
                std::vector <toSetDesc>::iterator it = tv.begin() + indx;
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                tv.erase(it);
            }
            tn.pop_back();
        }
        tlvl--;
    }

    if (blvl != 0) {
        unsigned short indx = bn[blvl].indxs;
        unsigned short n = bn[blvl].n;
        for (unsigned short i = 0; i < n; i++) {
            std::vector <toSetDesc>::iterator it = bv.begin() + indx;
            if ((it->p->t & m_fdLink) == 0) {
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                bv.erase(it);
            } else {
                indx++;
            }
        }
        bn[blvl].n = bv.size() - bn[blvl].indxs;
    }

    if (tlvl != 0) {
        unsigned short indx = tn[tlvl].indxs;
        unsigned short n = tn[tlvl].n;
        for (unsigned short i = 0; i < n; i++) {
            std::vector <toSetDesc>::iterator it = tv.begin() + indx;
            if ((it->p->t & m_fdLink) == 0) {
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                tv.erase(it);
            } else {
                indx++;
            }
        }
        tn[tlvl].n = tv.size() - tn[tlvl].indxs;
    }
    printf("bv %i, tv %i\n", bv.size(), tv.size());

    if (blvl > 1) {
        blvl--;
        unsigned short indx = bn[blvl].indxs + bn[blvl].n;
        indx--;
        do {
            std::vector <toSetDesc>::iterator it = bv.begin() + indx;
            bool remv = true;
            if (it->p->dirh != 0) {
                if ((it->p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (it->p->dirl != 0) {
                if ((it->p->dirl & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (it->p->dirm != 0) {
                if ((it->p->dirm & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x02) == 0x02) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x20) == 0x20) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x80) == 0x80) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (remv) {
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                //bv.erase(it);
            }
            indx--;
        } while(indx != 0);
    }
    if (tlvl > 1) {
        tlvl--;
        unsigned short indx = tn[tlvl].indxs + tn[tlvl].n;
        indx--;
        do {
            std::vector <toSetDesc>::iterator it = tv.begin() + indx;
            bool remv = true;
            if (it->p->dirh != 0) {
                if ((it->p->dirh & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirh & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z + 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (it->p->dirl != 0) {
                if ((it->p->dirl & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirl & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z - 1;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (it->p->dirm != 0) {
                if ((it->p->dirm & 0x01) == 0x01) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x02) == 0x02) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x04) == 0x04) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x08) == 0x08) {
                    sadd.coords.x = it->coords.x + 1;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x10) == 0x10) {
                    sadd.coords.x = it->coords.x;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x20) == 0x20) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y - 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x40) == 0x40) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
                if ((it->p->dirm & 0x80) == 0x80) {
                    sadd.coords.x = it->coords.x - 1;
                    sadd.coords.y = it->coords.y + 1;
                    sadd.coords.z = it->coords.z;
                    sadd.p = &(mdpmirror[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy]);
                    if ((it->p->lvl + 1) == sadd.p->lvl)
                        remv = false;
                }
            }
            if (remv) {
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                //tv.erase(it);
            }
            indx--;
        } while(indx != 0);
    }
    printf("bv %i, tv %i\n", bv.size(), tv.size());
    bn.clear();
    tn.clear();
    bv.clear();
    tv.clear();
    toDefineXYZ ctile;
    ctile.x = tile_x_;
    ctile.y = tile_y_;
    ctile.z = tile_z_;
    unsigned char ct = m_fdBasePoint;
    bool tnr = true, np = true;
    do {
        toDefineXYZ toadd;
        char dist = 5;
        pfdp = &(mdpmirror[ctile.x + ctile.y * m->mmax_x_
                    + ctile.z * m->mmax_m_xy]);
        if (pfdp->dirh != 0) {
            if ((pfdp->dirh & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        if (pfdp->dirl != 0) {
            if ((pfdp->dirl & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirl & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
            }
            if ((pfdp->dirl & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirl & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        if (pfdp->dirm != 0) {
            if ((pfdp->dirm & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 1;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x02) == 0x02) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 1;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x08) == 0x08) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 1;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x20) == 0x20) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 1;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x80) == 0x80) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
                        char cmpdist = 2;
                        if (cmpdist < dist) {
                            toadd = sadd.coords;
                            dist = cmpdist;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    ct = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                    dist = 0;np = false;
                    toadd = sadd.coords;
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        dest_path_.push_back(PathNode(toadd.x, toadd.y, toadd.z));
        ctile = toadd;
        printf("x %i, y %i, z %i\n",toadd.x,toadd.y,toadd.z);
    } while (tnr);

    if(dest_path_.size() != 0) {
        dest_path_.back().setOffX(ox);
        dest_path_.back().setOffY(oy);
        dest_path_.back().setOffZ(oz);
    }

    free(mdpmirror);
    if(dest_path_.size() != 0)
        speed_ = new_speed;
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
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
