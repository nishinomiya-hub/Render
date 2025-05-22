#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "InputHandler.h"
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"




int main() {
    // 初始化 GLFW 和窗口
    glfwInit();
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL Renderer", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Renderer::Init();
    Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");
    Scene scene;
    auto path = std::filesystem::u8path("res/model/changli/鸣潮 小长离1.04.fbx");
    scene.AddModel(path.string(), glm::vec3(0, 0, 0));
    auto targetNode = scene.GetNode(0);
    if (!targetNode) {
        std::cerr << "Failed to get scene node!" << std::endl;
        return -1;
    }
    Camera camera;

    camera.SetPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    //Mesh cubeMesh = CreateCubeMesh();

    // 创建输入处理器（自动注册回调）
    InputHandler inputHandler(window, camera, *targetNode);

    // 时间管理
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 计算时间差
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入（键盘和鼠标）
        inputHandler.ProcessKeyboardInput(deltaTime);
        

        Renderer::BeginFrame();

        scene.Render(camera,shader); // 场景渲染



        // ImGui UI 渲染
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Control Panel");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        Renderer::EndFrame();
        glfwSwapBuffers(window);
    }

    Renderer::Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
