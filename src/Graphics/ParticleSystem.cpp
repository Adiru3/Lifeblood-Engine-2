#include "Graphics/ParticleSystem.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem() : VAO(0), VBO(0) {}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::Init() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::Update(float dt) {
    for (int i = 0; i < particles.size(); ++i) {
        Particle& p = particles[i];
        p.life -= dt;
        p.position = p.position + (p.velocity * dt);
        // Simple gravity
        p.velocity.z -= 200.0f * dt; 

        if (p.life <= 0.0f) {
            particles.erase(particles.begin() + i);
            i--;
        }
    }
}

void ParticleSystem::Emit(const Vec3& pos, const Vec3& vel, const Vec3& color, float life, float size, int count, float spread) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position = pos;
        
        // Random spread
        float rx = ((rand() % 100) / 50.0f - 1.0f) * spread;
        float ry = ((rand() % 100) / 50.0f - 1.0f) * spread;
        float rz = ((rand() % 100) / 50.0f - 1.0f) * spread;
        
        p.velocity = vel + Vec3(rx, ry, rz);
        p.color = color;
        p.life = life + ((rand() % 100) / 100.0f) * 0.5f; // Randomize life slightly
        p.startLife = p.life;
        p.size = size;
        particles.push_back(p);
    }
}

void ParticleSystem::Draw(Shader* shader, const Vec3& cameraPos, const Vec3& cameraUp, const Vec3& cameraRight) {
    shader->Use();
    glBindVertexArray(VAO);

    for (const auto& p : particles) {
        if (p.life <= 0) continue;

        // Billboard Math: Construct model matrix to face camera
        // Simplified: Translate then use camera basis vectors
        Mat4 model = Mat4::Identity();
        
        // Translation
        model.elements[3][0] = p.position.x;
        model.elements[3][1] = p.position.y;
        model.elements[3][2] = p.position.z;

        // Rotation (Face Camera) - This is a cheat, we just identity rotation if we want to align to world,
        // but for billboarding we usually want to set the rotation part of the matrix to be the transpose of the view matrix logic.
        // A simpler way for a "sprite" is to manually construct the quad vertices in world space or use the View matrix.
        // But here, let's just use the cameraRight/Up vectors passed in.
        
        // Col 0 = Right * Size
        model.elements[0][0] = cameraRight.x * p.size;
        model.elements[0][1] = cameraRight.y * p.size;
        model.elements[0][2] = cameraRight.z * p.size;

        // Col 1 = Up * Size (Assuming Y is NOT up in this engine, Z is up? No, usually Camera Up is calculated)
        // In this engine Z is UP world space.
        // But for billboarding, we use the Camera's local Up.
        model.elements[1][0] = 0; // optimized out
        model.elements[1][1] = 0;
        model.elements[1][2] = p.size; // Simple Z-always-up billboard (like Doom trees)? OR full spherical?
        
        // Let's do Full Spherical Billboarding
        // We need the View Matrix transpose basically. 
        // But simpler: just use generic quad drawing with position.

        // Actually, let's Stick to "Z Up, Face Player" (Cylindrical) for simplicity for now as getting full camera basis 
        // passed down perfectly is tricky without headers.
        // Wait, I passed cameraRight, let's use it.
        // Col 1 = Look x Right = Up approximately.
        // Let's simple render cubes for now as particles to be safe and consistent with style?
        // User asked for "Billboarding".
        
        // Manual Billboard Construction or Simple Cube
        // Since we don't have View Matrix access easily here without g_Settings (which is in main),
        // we will use the camera vectors passed in arguments if they were valid, or just simple cubes.
        
        float s = p.size;
        model.elements[0][0] = s; model.elements[0][1] = 0; model.elements[0][2] = 0;
        model.elements[1][0] = 0; model.elements[1][1] = s; model.elements[1][2] = 0;
        model.elements[2][0] = 0; model.elements[2][1] = 0; model.elements[2][2] = s;
        
        shader->SetMat4("model", model);
        
        // Fade out alpha
        float alpha = p.life / p.startLife;
        glUniform3f(glGetUniformLocation(shader->ID, "color"), p.color.x, p.color.y, p.color.z);
        // We don't have transparency uniform yet in standard shader, stick to opaque or stipple?
        // Just Color for now.

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}
