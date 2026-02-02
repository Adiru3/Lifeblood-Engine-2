#pragma once
#include "Math/Vec3.h"
#include "Game/Weapon.h"
#include <vector>

class Player {
public:
    Vec3 position;
    Vec3 velocity;
    Vec3 viewAngles; // x=pitch, y=yaw
    
    bool onGround;
    bool wantsToJump;
    
    // Input state
    float forwardMove; // +1 to -1
    float rightMove;   // +1 to -1

    // Gameplay
    int health;
    float speedModifier; // For HE grenade slow
    int currentWeaponIndex;
    std::vector<Weapon> inventory;
    bool isScoped = false;
    bool isCrouched = false;
    float currentFOV = 110.0f;

    Player();
    
    Vec3 GetWishDir(); 
    void AddWeapon(WeaponType type);
};
