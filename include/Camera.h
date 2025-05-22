#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    // 相机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float FOV;           // 视野角度
    float AspectRatio;    // 宽高比
    float NearClip;       // 近裁剪面
    float FarClip;        // 远裁剪面

    // 新增方法
    glm::mat4 GetProjectionMatrix() const;
    void SetPerspective(float fov, float aspectRatio, float nearClip, float farClip);

    // 欧拉角
    float Yaw;
    float Pitch;

    // 移动参数
    float MovementSpeed;
    float MouseSensitivity;

    // 构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    float OrthoSize;  // 正交投影半高尺寸（单位：世界空间单位）

    void SetOrthographic(float size, float aspectRatio, float nearClip, float farClip);
    glm::mat4 GetOrthoMatrix() const;

    void ProcessArcballRotate(float dx, float dy);
private:
    void updateCameraVectors();
};