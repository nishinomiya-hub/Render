#pragma once
#include <glad/glad.h>
#include <string>
#include <stb_image.h>
#include <iostream>

class Texture {
public:
    Texture(const std::string& path);
    void Bind(unsigned int slot = 0) const;
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
};
