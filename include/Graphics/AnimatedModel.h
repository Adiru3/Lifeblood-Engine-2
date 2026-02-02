#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include "Graphics/Shader.h"
#include "Math/Mat4.h"

// Replaced GLM with native types

#define MAX_BONES 100

struct BoneInfo {
    int id;
    Mat4 offset;
};

struct VertexSkin {
    float position[3];
    float normal[3];
    float texCoords[2];
    int boneIDs[4];
    float weights[4];
};

class AnimatedModel {
public:
    AnimatedModel(const std::string& path);
    ~AnimatedModel();

    void Update(float dt);
    void Draw(Shader* shader);

private:
    void LoadGLTF(const std::string& path);
    
    unsigned int VAO, VBO, EBO;
    std::vector<VertexSkin> vertices;
    std::vector<unsigned int> indices;
    
    std::vector<Mat4> m_BoneMatrices;
    // TinyGLTF structs would be here for data storage
};
