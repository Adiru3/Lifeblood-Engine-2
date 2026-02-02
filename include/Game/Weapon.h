#pragma once
#include <string>
#include <vector>

enum WeaponType {
    WEAPON_KNIFE,
    WEAPON_DEAGLE,
    WEAPON_SHOTGUN,
    WEAPON_KALASH,  // AK
    WEAPON_SCOUT,
    WEAPON_HE,
    WEAPON_FLASH
};

struct Weapon {
    WeaponType type;
    std::string name;
    int damage;
    float fireRate;
    float nextFireTime;
    int ammo;
    
    // Stats for specific logic
    bool isAutomatic;
    int pellets; // For shotgun
};

class WeaponSystem {
public:
    static std::vector<Weapon> GetAllWeapons() {
        std::vector<Weapon> list;
        // 0: Knife
        list.push_back({WEAPON_KNIFE, "Knife", 25, 0.5f, 0, 0, false, 1});
        // 1: Deagle
        list.push_back({WEAPON_DEAGLE, "Deagle", 45, 0.3f, 0, 7, false, 1});
        // 2: Shotgun
        list.push_back({WEAPON_SHOTGUN, "Shotgun", 15, 0.9f, 0, 8, false, 6}); // 15 dmg * 6 pellets
        // 3: Kalash
        list.push_back({WEAPON_KALASH, "AK-47", 32, 0.1f, 0, 30, true, 1});
        // 4: Scout
        list.push_back({WEAPON_SCOUT, "Scout", 75, 1.2f, 0, 10, false, 1});
        // 5: HE
        list.push_back({WEAPON_HE, "HE Grenade", 90, 1.0f, 0, 1, false, 1});
        // 6: Flash
        list.push_back({WEAPON_FLASH, "Flashbang", 0, 1.0f, 0, 2, false, 1});
        
        return list;
    }
};
