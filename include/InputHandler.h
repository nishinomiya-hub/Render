#pragma once

#include "SceneNode.h"
#include "Camera.h"
#include "GLFW/glfw3.h"



class InputHandler {
public:
    enum class RotateMode {
        NONE,
        OBJECT,
        CAMERA_ARCBALL
    };
    InputHandler(GLFWwindow* window, Camera& camera, SceneNode& target);
    void ProcessKeyboardInput(float deltaTime); // 新增键盘处理方法

    void Update(float deltaTime);
    void SetRotationTarget(SceneNode* newTarget);
private:
    // 鼠标回调相关
    static void MounseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    // 旋转逻辑
    void StartRotate(double x, double y);
    void ProcessRotate(double x, double y);
    void EndRotate();

    // 窗口和相机引用
    GLFWwindow* m_Window;
    Camera& m_Camera;
    SceneNode& m_Target;
    
    struct {
        bool leftPressed = false;
        bool rotating = false;
        glm::dvec2 startPos;
        glm::dvec2 currentPos;
    } m_MouseState;
    
    RotateMode m_RotateMode = RotateMode::OBJECT;
    float m_RotateSpeed = 0.5f; // 旋转速度系数
};