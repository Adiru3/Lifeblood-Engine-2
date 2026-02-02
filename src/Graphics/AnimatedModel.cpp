#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Graphics/AnimatedModel.h"
#include <iostream>

#define TINYGLTF_IMPLEMENTATION

// Define these to prevent TinyGLTF from creating its own STB implementation,
// because Texture.cpp already provides STB_IMAGE_IMPLEMENTATION.
#define TINYGLTF_NO_STB_IMAGE 
#define TINYGLTF_NO_STB_IMAGE_WRITE 

// However, we DO need STB Image Write for TinyGLTF if we want to save (which we might not use yet).
// But for now, to fix linking, we rely on external symbols.
// Texture.cpp provides stbi_load (read).
// If we need Write, we should define STB_IMAGE_WRITE_IMPLEMENTATION in ONE place.
// Let's define it here, but NOT STB_IMAGE_IMPLEMENTATION.
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#include "tiny_gltf.h" 

AnimatedModel::AnimatedModel(const std::string& path) {
    LoadGLTF(path);
}

AnimatedModel::~AnimatedModel() {
    // Cleanup GL buffers
}

void AnimatedModel::LoadGLTF(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    
    // Load GLTF
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path); // or LoadBinaryFromFile
    
    if (!warn.empty()) std::cout << "GLTF Warn: " << warn << std::endl;
    if (!err.empty()) std::cout << "GLTF Err: " << err << std::endl;
    
    if (!ret) {
        std::cout << "Failed to load GLTF: " << path << std::endl;
        return;
    }
    
    // Parse Nodes, Skins, Animations...
    // (Complex parsing logic omitted for brevity in this "Don't Compile" step)
    // We would fill 'vertices' with bone weights here.
    
    std::cout << "Loaded Animated Model: " << path << std::endl;
    
    // Upload to GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexSkin), &vertices[0], GL_STATIC_DRAW);
    
    // Pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkin), (void*)0);
    // Norm
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkin), (void*)offsetof(VertexSkin, normal));
    // BoneIDs
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexSkin), (void*)offsetof(VertexSkin, boneIDs));
    // Weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSkin), (void*)offsetof(VertexSkin, weights));
    
    glBindVertexArray(0);
}

void AnimatedModel::Update(float dt) {
    // Animate Bones
    // m_BoneMatrices = ...;
}

void AnimatedModel::Draw(Shader* shader) {
    shader->Use();
    // Send Bones
    for (int i = 0; i < m_BoneMatrices.size(); ++i) {
        // shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", m_BoneMatrices[i]);
    }
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);
}
