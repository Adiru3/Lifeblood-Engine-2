#pragma once
#include "Math/Vec3.h"

class Player; // Forward declaration

class PhysicsEngine {
public:
    static void PMove(Player* player, float dt);
    static void ApplyImpulse(Player* player, const Vec3& impulse);

private:
    // Quake Physics Constants
    static constexpr float STOP_SPEED = 100.0f;
    static constexpr float DUCK_SCALE = 0.25f;
    static constexpr float GRAVITY = 800.0f;
    static constexpr float ACCELERATE = 10.0f;
    static constexpr float AIR_ACCELERATE = 10.0f; // Balanced Air Control
    static constexpr float FRICTION = 6.0f;
    static constexpr float MAX_SPEED = 320.0f; // Standard Quake Speed
    static constexpr float JUMP_POWER = 270.0f;

    static void Friction(Player* player, float dt);
    static void Accelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt);
    static void AirAccelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt);
    static void AirMove(Player* player, float dt);
    static void WalkMove(Player* player, float dt);
    static void CheckGround(Player* player); // Simple ground check
};
