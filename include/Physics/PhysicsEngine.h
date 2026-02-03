#pragma once
#include "Math/Vec3.h"

class Player; // Forward declaration

class PhysicsEngine {
public:
    static void Init(const std::string& configPath);
    static void PMove(Player* player, float dt);
    static void ApplyImpulse(Player* player, const Vec3& impulse);
    
    // Configurable Physics Constants
    static float STOP_SPEED;
    static float DUCK_SCALE;
    static float GRAVITY;
    static float ACCELERATE;
    static float AIR_ACCELERATE; 
    static float FRICTION;
    static float MAX_SPEED;
    static float JUMP_POWER;

private:
    static void Friction(Player* player, float dt);
    static void Accelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt);
    static void AirAccelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt);
    static void AirMove(Player* player, float dt);
    static void WalkMove(Player* player, float dt);
    static void CheckGround(Player* player); 
};
