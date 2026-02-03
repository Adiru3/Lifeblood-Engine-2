#include "Physics/PhysicsEngine.h"
#include "Game/Player.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Instantiate Statics
float PhysicsEngine::STOP_SPEED = 100.0f;
float PhysicsEngine::DUCK_SCALE = 0.25f;
float PhysicsEngine::GRAVITY = 800.0f;
float PhysicsEngine::ACCELERATE = 10.0f;
float PhysicsEngine::AIR_ACCELERATE = 10.0f;
float PhysicsEngine::FRICTION = 6.0f;
float PhysicsEngine::MAX_SPEED = 320.0f;
float PhysicsEngine::JUMP_POWER = 270.0f;

void PhysicsEngine::Init(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << configPath << ", using defaults." << std::endl;
        return;
    }
    
    std::cout << "Loading Physics Config..." << std::endl;
    std::string line;
    while(std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key;
        if(std::getline(ss, key, '=')) {
            float val; ss >> val;
            if (key == "gravity") GRAVITY = val;
            else if (key == "stop_speed") STOP_SPEED = val;
            else if (key == "duck_scale") DUCK_SCALE = val;
            else if (key == "accelerate") ACCELERATE = val;
            else if (key == "air_accelerate") AIR_ACCELERATE = val;
            else if (key == "friction") FRICTION = val;
            else if (key == "max_speed") MAX_SPEED = val;
            else if (key == "jump_power") JUMP_POWER = val;
        }
    }
}

void PhysicsEngine::ApplyImpulse(Player* player, const Vec3& impulse) {
    player->velocity = player->velocity + impulse;
    player->onGround = false;
}

void PhysicsEngine::PMove(Player* player, float dt) {
    if (player->health <= 0) return;

    // Check Ground interaction
    CheckGround(player);

    if (player->wantsToJump && player->onGround) {
        player->velocity.z = JUMP_POWER;
        player->onGround = false;
        player->wantsToJump = false; // Auto-hop handled by holding space elsewhere or here? 
        // Quake style usually allows holding space implies next frame jump. 
        // For simple implementation, we just set z velocity.
    }

    // Apply Friction
    if (player->onGround) {
        Friction(player, dt);
    }

    // Get Wish Direction
    Vec3 wishDir = player->GetWishDir();
    float wishSpeed = (player->forwardMove != 0 || player->rightMove != 0) ? MAX_SPEED : 0;
    
    // Crouch / Slide Logic
    if (player->isCrouched) {
        // If moving fast on ground, SLIDE!
        float speed = player->velocity.Length();
        if (speed > 250.0f && player->onGround) {
             // Low friction slide
             wishSpeed = 0; // Control lock during slide? Or just reduced control
        } else {
             // Normal Crouch walk
             wishSpeed = MAX_SPEED * 0.4f;
        }
    }

    // Accelerate
    if (player->onGround) {
        if (player->isCrouched && player->velocity.Length() > 250.0f) {
            // Apply very low friction for slide
            Friction(player, dt * 0.1f); 
        } else {
            Friction(player, dt);
            Accelerate(player, wishDir, wishSpeed, ACCELERATE, dt);
        }
    } else {
        AirAccelerate(player, wishDir, wishSpeed, AIR_ACCELERATE, dt);
    }

    // Speed Cap (Hard limit)
    if (player->velocity.Length() > 2000.0f) {
        player->velocity = player->velocity.Normalized() * 2000.0f;
    }

    // Apply Gravity
    if (!player->onGround) {
        player->velocity.z -= GRAVITY * dt;
    }

    // Ceiling Check (Sky Limit)
    if (player->position.z > 3000.0f) {
        player->position.z = 3000.0f;
        if (player->velocity.z > 0) player->velocity.z = 0;
    }

    // Integrate Position
    player->position = player->position + (player->velocity * dt);

    // Simple Floor Collision
    if (player->position.z < 0) {
        player->position.z = 0;
        player->velocity.z = 0;
        player->onGround = true;
    }
}

void PhysicsEngine::Friction(Player* player, float dt) {
    float speed = player->velocity.Length();
    if (speed < 0.1f) return;

    float drop = 0;
    
    // Quake Friction
    float control = speed < STOP_SPEED ? STOP_SPEED : speed;
    drop += control * FRICTION * dt;

    float newSpeed = speed - drop;
    if (newSpeed < 0) newSpeed = 0;
    
    if (speed > 0) {
        newSpeed /= speed;
    }
    
    player->velocity = player->velocity * newSpeed;
}

void PhysicsEngine::Accelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt) {
    float currentSpeed = Vec3::Dot(player->velocity, wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    if (addSpeed <= 0) return;

    float accelSpeed = accel * wishSpeed * dt;
    if (accelSpeed > addSpeed) accelSpeed = addSpeed;

    player->velocity = player->velocity + (wishDir * accelSpeed);
}

void PhysicsEngine::AirAccelerate(Player* player, const Vec3& wishDir, float wishSpeed, float accel, float dt) {
    float currentSpeed = Vec3::Dot(player->velocity, wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    if (addSpeed <= 0) return;

    float accelSpeed = accel * wishSpeed * dt;
    if (accelSpeed > addSpeed) accelSpeed = addSpeed;

    player->velocity = player->velocity + (wishDir * accelSpeed);
}

void PhysicsEngine::AirMove(Player* player, float dt) {
    // Already handled in PMove logic structure
}

void PhysicsEngine::WalkMove(Player* player, float dt) {
    // Already handled in PMove logic structure
}

void PhysicsEngine::CheckGround(Player* player) {
    if (player->position.z <= 0.1f && player->velocity.z <= 180.0f) {
        player->onGround = true;
    } else {
        player->onGround = false;
    }
}
