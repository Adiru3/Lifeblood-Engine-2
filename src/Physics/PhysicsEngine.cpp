#include "Physics/PhysicsEngine.h"
#include "Game/Player.h"
#include <cmath>
#include <iostream>

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

    // Apply Gravity
    if (!player->onGround) {
        player->velocity.z -= GRAVITY * dt;
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
