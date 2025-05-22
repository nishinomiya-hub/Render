#pragma once

#include <glad/glad.h>

namespace Renderer {
    void Init();
    void BeginFrame();
    void EndFrame();
    void Shutdown();
}