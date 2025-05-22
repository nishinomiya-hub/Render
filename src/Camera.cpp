#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(2.5f),
      MouseSensitivity(0.1f),
      Position(position),
      WorldUp(up),Yaw(yaw),Pitch(pitch) , OrthoSize(5.0f),
      FOV(45.0f), AspectRatio(16.0f/9.0f), NearClip(0.1f), FarClip(100.0f)
{
    updateCameraVectors();
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(
        glm::radians(FOV),
        AspectRatio,
        NearClip,
        FarClip
    );
}

// 设置投影参数的方法
void Camera::SetPerspective(float fov, float aspectRatio, float nearClip, float farClip) {
    FOV = fov;
    AspectRatio = aspectRatio;
    NearClip = nearClip;
    FarClip = farClip;
}
glm::mat4 Camera::GetViewMatrix()  const{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::SetOrthographic(float size, float aspectRatio, float nearClip, float farClip)
{
    OrthoSize = size;
    AspectRatio = aspectRatio;
    NearClip = nearClip;
    FarClip = farClip;
}

glm::mat4 Camera::GetOrthoMatrix() const
{
    const float left = -OrthoSize * AspectRatio;
    const float right = OrthoSize * AspectRatio;
    const float bottom = -OrthoSize;
    const float top = OrthoSize;
    
    return glm::ortho(
        left, right,    // X轴范围
        bottom, top,    // Y轴范围
        NearClip,       // 近裁剪面
        FarClip        // 远裁剪面
    );
}

void Camera::ProcessArcballRotate(float dx, float dy)
{
            // 计算旋转角度
        const float angleX = dx * MouseSensitivity;
        const float angleY = dy * MouseSensitivity;

        // 绕目标点旋转
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), 
                                   glm::radians(angleY), 
                                   Right);
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(angleX),
                                   WorldUp);

        // 更新相机位置
        glm::vec4 newPos = rotX * rotY * glm::vec4(Position, 1.0f);
        Position = glm::vec3(newPos);

        // 更新方向向量
        updateCameraVectors();

}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;
    if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}