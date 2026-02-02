#pragma once
#include <vector>
#include <glad/glad.h>
#include "Math/Vec3.h"
#include "Graphics/Shader.h"

struct Particle {
    Vec3 position;
    Vec3 velocity;
    Vec3 color;
    float life;      // Remaining life
    float startLife; // Total life duration
    float size;
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void Init();
    void Update(float dt);
    void Draw(Shader* shader, const Vec3& cameraPos, const Vec3& cameraUp, const Vec3& cameraRight);
    
    void Emit(const Vec3& pos, const Vec3& vel, const Vec3& color, float life, float size, int count, float spread);

private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
};
