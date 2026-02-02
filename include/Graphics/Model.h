#pragma once
#include <vector>
#include <string>
#include "Math/Vec3.h"
#include "Graphics/Shader.h"

struct Vertex {
    Vec3 Position;
    Vec3 Normal;
    Vec3 TexCoords;
};

class Model {
public:
    Model(const std::string& path);
    void Draw(Shader* shader);

private:
    unsigned int VAO, VBO;
    std::vector<Vertex> vertices;
    void SetupMesh();
    void LoadModel(const std::string& path);
};
