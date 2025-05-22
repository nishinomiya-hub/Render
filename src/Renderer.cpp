#include "Renderer.h"

void Renderer::Init() {
    // 深度测试
    glEnable(GL_DEPTH_TEST);

    // 默认禁用混合和背面剔除（按需启用）
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 多边形偏移（用于防止 Z-Fighting）
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    // 混合函数（透明渲染时启用）
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Renderer::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
    // 可以加入后期处理
}

void Renderer::Shutdown() {}
