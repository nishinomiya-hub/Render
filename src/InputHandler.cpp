#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow* window, Camera& camera,SceneNode& target)
    : m_Window(window), m_Camera(camera),m_Target(target)
{
    // 设置鼠标回调
    glfwSetMouseButtonCallback(window, MounseButtonCallBack);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetWindowUserPointer(window, this);
    
    // 可选：禁用光标
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::ProcessKeyboardInput(float deltaTime) {
    // 相机移动控制
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    // 其他全局控制（示例：退出键）
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);
}

void InputHandler::Update(float deltaTime)
{
}

void InputHandler::SetRotationTarget(SceneNode *newTarget)
{
}

void InputHandler::MounseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
{
    auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            handler->StartRotate(x, y);
        } else if (action == GLFW_RELEASE) {
            handler->EndRotate();
        }
    }
}

// 保持原有鼠标回调实现
void InputHandler::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    
    if (handler->m_MouseState.leftPressed) {
        handler->ProcessRotate(xpos, ypos);
    }
}

void InputHandler::StartRotate(double x, double y)
{
    m_MouseState.leftPressed = true;
    m_MouseState.rotating = true;
    m_MouseState.startPos = glm::dvec2(x, y);
    m_MouseState.currentPos = m_MouseState.startPos;
}

// 处理旋转
void InputHandler::ProcessRotate(double x, double y) {
    if (!m_MouseState.rotating) return;

    // 计算偏移量
    glm::dvec2 offset = glm::dvec2(x, y) - m_MouseState.currentPos;
    m_MouseState.currentPos = glm::dvec2(x, y);

    // 转换为旋转角度
    const float dx = static_cast<float>(offset.x) * m_RotateSpeed;
    const float dy = static_cast<float>(offset.y) * m_RotateSpeed;

    const float yawRad = glm::radians(dx);
    const float pitchRad = glm::radians(dy);

    // 创建四元数
    glm::quat yawRot = glm::angleAxis(yawRad, glm::vec3(0, 1, 0));
    glm::quat pitchRot = glm::angleAxis(pitchRad, glm::vec3(1, 0, 0));

    switch (m_RotateMode) {
    case RotateMode::OBJECT:
        // 物体绕自身轴旋转
        m_Target.Rotation = yawRot * pitchRot * m_Target.Rotation;
        break;
        
    case RotateMode::CAMERA_ARCBALL:
        // 相机弧球旋转（绕目标物体）
        m_Camera.ProcessArcballRotate(dx, dy);
        break;
    }
}

// 结束旋转
void InputHandler::EndRotate() {
    m_MouseState.leftPressed = false;
    m_MouseState.rotating = false;
}