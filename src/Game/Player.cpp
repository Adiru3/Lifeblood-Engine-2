#include "Game/Player.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Player::Player() {
    position = Vec3(0, 0, 50); 
    velocity = Vec3(0, 0, 0);
    viewAngles = Vec3(0, 0, 0);
    onGround = false;
    wantsToJump = false;
    forwardMove = 0;
    rightMove = 0;
    health = 100;
    speedModifier = 1.0f;
    
    // Default loadout: Knife
    auto allWeapons = WeaponSystem::GetAllWeapons();
    inventory.push_back(allWeapons[0]); 
    currentWeaponIndex = 0;
}

Vec3 Player::GetWishDir() {
    float yawRad = viewAngles.y * (M_PI / 180.0f);
    
    Vec3 forward;
    forward.x = cos(yawRad);
    forward.y = sin(yawRad);
    forward.z = 0;

    Vec3 right;
    right.x = sin(yawRad);
    right.y = -cos(yawRad); 
    right.z = 0;

    Vec3 dir = (forward * forwardMove) + (right * rightMove);
    return dir.Normalized();
}

void Player::AddWeapon(WeaponType type) {
    auto allWeapons = WeaponSystem::GetAllWeapons();
    for (const auto& w : inventory) {
        if (w.type == type) return; // Already have it
    }
    // Find weapon definition
    for (const auto& w : allWeapons) {
        if (w.type == type) {
            inventory.push_back(w);
            return;
        }
    }
}
