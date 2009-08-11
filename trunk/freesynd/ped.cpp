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
#include <math.h>

Ped::Ped()
{
    memset(stand_anims_, 0, sizeof(stand_anims_));
    memset(walk_anims_, 0, sizeof(walk_anims_));
    memset(stand_fire_anims_, 0, sizeof(stand_fire_anims_));
    memset(walk_fire_anims_, 0, sizeof(walk_fire_anims_));
}

bool Ped::drawStandFrame(int x, int y, int dir, int frame, WeaponIndex weapon)
{
    if (weapon == MedKit)
        weapon = Unarmed;

    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_anims_[weapon] + dir, frame, x, y);
}

bool Ped::drawWalkFrame(int x, int y, int dir, int frame, WeaponIndex weapon)
{
    if (weapon == MedKit)
        weapon = Unarmed;
    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_anims_[weapon] + dir, frame, x, y);
}

bool Ped::drawStandFireFrame(int x, int y, int dir, int frame,
        WeaponIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_fire_anims_[weapon] + dir, frame, x, y);
}

bool Ped::drawWalkFireFrame(int x, int y, int dir, int frame,
        WeaponIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_fire_anims_[weapon] + dir, frame, x, y);
}

bool Ped::lastStandFireFrame(int dir, int frame, WeaponIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(
            stand_fire_anims_[weapon] + dir, frame);
}

bool Ped::lastWalkFireFrame(int dir, int frame, WeaponIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(walk_fire_anims_[weapon] + dir, frame);
}

bool Ped::drawDieFrame(int x, int y, int frame)
{
    return g_App.gameSprites().drawFrame(die_anim_, frame, x, y);
}

int Ped::lastDieFrame()
{
    return g_App.gameSprites().lastFrame(die_anim_);
}

void Ped::drawDeadFrame(int x, int y, int frame)
{
    if (frame > g_App.gameSprites().lastFrame(dead_anim_))
        frame = g_App.gameSprites().lastFrame(dead_anim_);
    g_App.gameSprites().drawFrame(dead_anim_, frame, x, y);
}

void Ped::drawHitFrame(int x, int y, int dir, int frame)
{
    if (frame > g_App.gameSprites().lastFrame(hit_anim_ + dir / 2))
        frame = g_App.gameSprites().lastFrame(hit_anim_ + dir / 2);
    g_App.gameSprites().drawFrame(hit_anim_ + dir / 2, frame, x, y);
}

int Ped::lastHitFrame(int dir)
{
    return g_App.gameSprites().lastFrame(hit_anim_ + dir / 2);
}

void Ped::drawPickupFrame(int x, int y, int frame)
{
    g_App.gameSprites().drawFrame(pickup_anim_, frame, x, y);
}

int Ped::lastPickupFrame()
{
    return g_App.gameSprites().lastFrame(pickup_anim_);
}

bool PedInstance::animate(int elapsed, Mission * mission)
{
    bool updated = false;
    Ped::WeaponIndex weapon_idx =
        selectedWeapon()? selectedWeapon()->index() : Ped::Unarmed;

    if (weapon_idx == Ped::MedKit && selectedWeapon()->ammoRemaining()
            && health_ < start_health_) {
        health_ = start_health_;
        selectedWeapon()->setAmmoRemaining(0);
        return true;
    }

    if (health_ <= 0) {
        target_ = 0;
        target_x_ = target_y_ = -1;
        speed_ = 0;
        clearDestination();
        while (numWeapons())
            dropWeapon(0);
    }
    else if (isHostile()) {
        // find a weapon with ammo
        for (int i = 0; selectedWeapon() == 0 && i < numWeapons(); i++)
            if (weapon(i)->ammoRemaining() > 0)
                selected_weapon_ = i;

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
                        setDestination(mission->ped(i)->tileX(),
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

            for (int i = 0; i < mission->numWeapons(); i++)
                if (mission->weapon(i)->map() != -1
                        && mission->weapon(i)->ammoRemaining() > 0) {
                    float d = distanceTo(mission->weapon(i));

                    if (d < closest) {
                        closest = d;
                        closest_w = mission->weapon(i);
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
        else
            setDestination(in_vehicle_->tileX(), in_vehicle_->tileY(), 0,
                    in_vehicle_->offX(), in_vehicle_->offY(), 320);
    }

    if (pickup_weapon_ && pickup_weapon_->map() == -1) {
        pickup_weapon_ = 0;
        clearDestination();
        speed_ = 0;
    }

    if (pickup_weapon_) {
        if (samePosition(pickup_weapon_)) {
            if (frame_ > ped_->lastPickupFrame() + 1)
                frame_ = 0;

            if (frame_ == ped_->lastPickupFrame() + 1) {
                weapons_.push_back(pickup_weapon_);
                pickup_weapon_->setMap(-1);
                pickup_weapon_ = 0;
                return true;
            }
        }
        else
            setDestination(pickup_weapon_->tileX(), pickup_weapon_->tileY(), 0,
                    pickup_weapon_->offX(), pickup_weapon_->offY(), 320);
    }

    if (putdown_weapon_) {
        if (frame_ > ped_->lastPickupFrame() + 1)
            frame_ = 0;

        if (frame_ == ped_->lastPickupFrame() + 1) {
            WeaponInstance *w = putdown_weapon_;
            w->setMap(map());
            w->setTileX(tile_x_);
            w->setTileY(tile_y_);
            w->setTileZ(tile_z_);
            w->setOffX(off_x_);
            w->setOffY(off_y_);
            putdown_weapon_ = 0;
            return true;
        }
    }

    updated = ShootableMovableMapObject::animate(elapsed);

    if (target_) {
        if (inRange(target_)) {
            target_x_ = target_->screenX();
            target_y_ = target_->screenY();
        }
        else {
            stopFiring();
        }
    }

    if (target_x_ != -1 && target_y_ != -1
            && firing_ == PedInstance::Firing_Not) {
        int stx = screenX() + 30;
        int sty = screenY() - 4;

        int fuzz = 16;

        if (target_x_ - stx < -fuzz) {
            if (target_y_ - sty < -fuzz) {
                dir_ = 6;
            }
            else if (target_y_ - sty > fuzz) {
                dir_ = 0;
            }
            else {
                dir_ = 7;
            }
        }
        else if (target_x_ - stx > fuzz) {
            if (target_y_ - sty < -fuzz) {
                dir_ = 4;
            }
            else if (target_y_ - sty > fuzz) {
                dir_ = 2;
            }
            else {
                dir_ = 3;
            }
        }
        else {
            if (target_y_ - sty < -fuzz) {
                dir_ = 5;
            }
            else if (target_y_ - sty > fuzz) {
                dir_ = 1;
            }
            else {
                // shoot yourself?
            }
        }

        firing_ = PedInstance::Firing_Fire;
        updated = true;

        if (selectedWeapon()->sound())
            selectedWeapon()->sound()->play();
    }

    if (weapon_idx == Ped::Unarmed || weapon_idx == Ped::MedKit)
        firing_ = PedInstance::Firing_Not;

    if (firing_ != PedInstance::Firing_Not || updated || health_ <= 0
            || receive_damage_ || pickup_weapon_ || putdown_weapon_) {
        MapObject::animate(elapsed);
        if (receive_damage_ && frame_ == ped_->lastHitFrame(dir_) + 1) {
            health_ -= receive_damage_;

            if (health_ < 0)
                health_ = 0;

            receive_damage_ = 0;
            frame_ = 0;
        }

        if (health_ == 0 && frame_ == ped_->lastDieFrame() + 1) {
            health_ = -1;
            frame_ = 0;
        }

        if (firing_ == PedInstance::Firing_Fire
                || firing_ == PedInstance::Firing_Stop) {
            bool lastFrame;

            if (speed_)
                lastFrame = ped_->lastWalkFireFrame(dir_, frame_, weapon_idx);
            else
                lastFrame = ped_->lastStandFireFrame(dir_, frame_, weapon_idx);

            if (lastFrame) {
                if (target_ && target_->health() > 0)
                    target_->inflictDamage(hit_damage_ + 1);

                if (selectedWeapon()->ammoRemaining() > 0)
                    selectedWeapon()->setAmmoRemaining(
                            selectedWeapon()->ammoRemaining() - 1);

                if (selectedWeapon()->ammoRemaining() == 0)
                    selectNextWeapon();

                // last frame, still firing, reload
                if (firing_ == PedInstance::Firing_Fire)
                    firing_ = PedInstance::Firing_Reload;

                if (firing_ == PedInstance::Firing_Stop) {
                    firing_ = PedInstance::Firing_Not;
                    target_ = 0;
                    target_x_ = target_y_ = -1;
                }
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

void PedInstance::stopFiring()
{
    if (firing_ != PedInstance::Firing_Not)
        firing_ = PedInstance::Firing_Stop;
}

PedInstance *Ped::createInstance(int map)
{
    return new PedInstance(this, map);
}

void PedInstance::kill()
{
    dead_ = true;
}

bool isOnScreen(int scrollX, int scrollY, int x, int y)
{
    return x >= scrollX && y >= scrollY
            && x < scrollX + GAME_SCREEN_WIDTH - 129
            && y < scrollY + GAME_SCREEN_HEIGHT;
}

bool getOnScreen(int scrollX, int scrollY, int &x, int &y, int tx, int ty)
{
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

void PedInstance::showPath(int scrollX, int scrollY)
{
    int px = screenX();
    int py = screenY();

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
ped_(ped), firing_(PedInstance::Firing_Not), target_(NULL), target_x_(-1),
target_y_(-1), hit_damage_(0), receive_damage_(0), sight_range_(0),
is_hostile_(false), reload_count_(0), selected_weapon_(-1),
pickup_weapon_(0), putdown_weapon_(0), in_vehicle_(0)
{
}

void PedInstance::draw(int x, int y, int scrollX, int scrollY)
{
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

    if (receive_damage_)
        ped_->drawHitFrame(x, y, dir_, frame_);
    else if (health_ == 0)
        ped_->drawDieFrame(x, y, frame_);
    else if (health_ == -1)
        ped_->drawDeadFrame(x, y, frame_);
    else if (pickup_weapon_ && dest_path_.empty())
        ped_->drawPickupFrame(x, y, frame_);
    else if (putdown_weapon_)
        ped_->drawPickupFrame(x, y, frame_);
    else if (firing_ == PedInstance::Firing_Not
             || firing_ == PedInstance::Firing_Reload) {
        bool lastFrame;

        if (speed_)
            lastFrame =
                ped_->drawWalkFrame(x, y, dir_, frame_, weapon_idx);
        else
            lastFrame =
                ped_->drawStandFrame(x, y, dir_, frame_, weapon_idx);

        if (lastFrame) {
            if (firing_ == PedInstance::Firing_Reload) {
                reload_count_++;
                int required = 1;

                if (weapon_idx == Ped::Pistol
                        || weapon_idx == Ped::Shotgun)
                    required = 50;

                if (reload_count_ >= required) {
                    firing_ = PedInstance::Firing_Fire;
                    reload_count_ = 0;
                }
            }
        }
    }
    else {
        bool lastFrame;

        if (speed_)
            lastFrame = ped_->drawWalkFireFrame(x, y, dir_, frame_, weapon_idx);
        else
            lastFrame =
                ped_->drawStandFireFrame(x, y, dir_, frame_, weapon_idx);

        if (lastFrame) {
            // draw the impact, TODO: this doesn't work if the shooter is off screen.
            g_App.gameSprites().drawSpriteXYZ(18 * 40 + 9 + firing_ - 3,
                                              target_x_ - scrollX + 129 +
                                              (target_ ? 15 : 0),
                                              target_y_ - scrollY -
                                              (target_ ? 15 : 0), 0);
        }
    }
}

bool PedInstance::inRange(ShootableMapObject * t)
{
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

bool PedInstance::inSightRange(MapObject *t)
{
    float d =
        sqrt((float) (t->tileX() - tileX()) * (t->tileX() - tileX()) +
             (t->tileY() - tileY()) * (t->tileY() - tileY()) +
             (t->tileZ() - tileZ()) * (t->tileZ() - tileZ()));

    return d < sight_range_;
}

void PedInstance::selectNextWeapon()
{
    int nextWeapon = -1;

    Weapon *selectedWeapon = (Weapon *) weapon(selected_weapon_);

    if (selectedWeapon)
        for (int i = numWeapons() - 1; i >=0 && nextWeapon == -1; i--)
            if (i != selected_weapon_
                    && *((Weapon *) weapon(i)) == *selectedWeapon
                    && weapon(i)->ammoRemaining())
                nextWeapon = i;

    if (nextWeapon == -1)
        selectBestWeapon();
    else
        selected_weapon_ = nextWeapon;
}

void PedInstance::selectBestWeapon()
{
    int bestWeapon = -1;
    int bestWeaponRank = -1;

    for (int i = numWeapons() - 1; i >=0; i--)
        if (weapon(i)->ammoRemaining() && weapon(i)->rank() > bestWeaponRank) {
            bestWeapon = i;
            bestWeaponRank = weapon(i)->rank();
        }

    selected_weapon_ = bestWeapon;
}

void PedInstance::dropWeapon(int n)
{
    assert(n >= 0 && n < (int) weapons_.size());

    if (selected_weapon_ == n)
        selected_weapon_ = -1;

    WeaponInstance *w = weapons_[n];
    std::vector < WeaponInstance * >::iterator it;
    for (it = weapons_.begin(); *it != w; it++);
    assert(*it == w);
    weapons_.erase(it);

    putdown_weapon_ = w;
}

void PedInstance::pickupWeapon(WeaponInstance * w)
{
    assert(w->map() == map());
    if (weapons_.size() >= 8)
        return;

    pickup_weapon_ = w;
    frame_ = 0;
}

void PedInstance::putInVehicle(VehicleInstance * v)
{
    assert(map_ != -1);

    in_vehicle_ = v;
}

void PedInstance::leaveVehicle()
{
    assert(map_ == -1 && in_vehicle_);

    map_ = in_vehicle_->map();
    in_vehicle_ = 0;
}

int PedInstance::map()
{
    if (map_ == -1) {
        assert(in_vehicle_);
        return in_vehicle_->map();
    }

    return map_;
}

bool PedInstance::walkable(int x, int y, int z)
{
    Map *m = g_App.maps().map(map());
    if (m->stairsAt(x, y, z) || m->stairsAt(x, y, z + 1))
        return true;
    return m->tileAt(x, y, z) > 5 && m->tileAt(x, y, z + 1) <= 5;
}
