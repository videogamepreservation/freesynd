/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#ifndef WEAPON_HOLDER_H
#define WEAPON_HOLDER_H

#include <vector>

#include "weapon.h"

/*!
 * A weapon holder is an object that can hold weapons.
 * Creation/destruction of weapons holds by the WeaponHolder is
 * not his responsibility.
 */
class WeaponHolder {
public:
    struct WeaponSelectCriteria {
        union {
            //! weapon index from weapons_ in mission_
            uint32 indx;
            //! use only this weapon for attack
            WeaponInstance *wi;
            //! use only this type of weapon
            Weapon::WeaponType wpn_type;
            //! use weapon that inflicts this type of damage
            //! MapObject::DamageType
            uint32 dmg_type;
        } criteria;

        enum CriteriaType {
            kCritNotSet = 0,             // No criteria is set
            kCritIndex = 1,             // indx
            kCritPointer = 2,           // wi
            kCritWeaponType = 3,        // wpn_type
            kCritDamageStrict = 4,      // type == dmg_type
            kCritDamageNonStrict = 5,   // type & dmg_type != 0
            kCritPlayerSelection = 6    // Manage selection from weapon selector
        };
        //! Union descriptor
        CriteriaType desc;
        bool use_ranks;
        /*!
         * When in kCritPlayerSelection mode and a medikit was selected for an agent,
         * if this field is true, all selected agent will use a medikit.
         */
        bool apply_to_all;
    };

public:
    //! This constat indicates that there is no weapon selected.
    static const int kNoWeaponSelected;
    /*! Defines the maximum number of weapons an agent can carry.*/
    static const uint8 kMaxHoldedWeapons;

    WeaponHolder();
    virtual ~WeaponHolder() {}

    int numWeapons() { return weapons_.size(); }

    WeaponInstance *weapon(int n) {
        assert(n < (int) weapons_.size());
        return weapons_[n];
    }

    void addWeapon(WeaponInstance *w) {
        assert(w);
        assert(weapons_.size() < kMaxHoldedWeapons);
        weapons_.push_back(w);
    }

    //! Removes the weapon from the inventory at the given index.
    WeaponInstance *removeWeapon(uint8 n);

    //! Removes the given weapon from the inventory.
    void removeWeaponInstance(WeaponInstance *w);

    //! Selects the weapon at given index in the inventory
    void selectWeapon(uint8 n);
    //! Deselects a selected weapon if any
    void deselectWeapon();

    //! Returns the currently used weapon or null if no weapon is used
    WeaponInstance *selectedWeapon() {
        return selected_weapon_ >= 0
                && selected_weapon_ < (int) weapons_.size()
            ? weapons_[selected_weapon_] : NULL;
    }

    //! Selects a weapon based on the given criteria
    bool selectRequiredWeapon(WeaponSelectCriteria *pw_to_use = NULL);
    //! Called when a weapon has no ammo to select another one
    void selectNextWeapon();

protected:
    /*!
     * Called when a weapon has been deselected.
     * \param wi The deselected weapon
     */
    virtual void handleWeaponDeselected(WeaponInstance * wi) {}
    /*!
     * Called when a weapon has been selected.
     * \param wi The selected weapon
     */
    virtual void handleWeaponSelected(WeaponInstance * wi) {}
    //! Updates the prefered weapon criteria based on current selection
    void updtPreferedWeapon();
protected:
    /*!
     * The list of weapons carried by the holder.
     */
    std::vector<WeaponInstance *> weapons_;
    /*!
     * The currently selected weapon inside the inventory.
     */
    int selected_weapon_;
    /*!
     * On automatic weapon selection, weapon will be selected upon
     * this criteria.
     */
    WeaponSelectCriteria prefered_weapon_;
};

#endif
