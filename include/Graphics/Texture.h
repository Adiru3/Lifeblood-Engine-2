#pragma once
#include <string>
#include <glad/glad.h>

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    Texture(const std::string& path);
    ~Texture();

    void Bind(int unit = 0);
};
