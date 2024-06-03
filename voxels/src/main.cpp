#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/shader.h"
#include "Graphics/texture.h"

#include "IO/keyboard.h"
#include "IO/mouse.h"
#include "IO/joystick.h"

#include "Graphics/camera.h"

#include "WorldGeneration/chunckGenerator.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void showFPS(GLFWwindow* pWindow);

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float fps_currentTime = 0.0f;
float fps_lastTime = 0.0f;
int nbFrames = 0;

// Screen size
int Width = 1200, Height = 800;

// Game objects
Camera camera((float)Width, (float)Height, 90.0f);
float camSpeed = 10.0f;

bool freezeMovement = false;

int main(){
    int _render_dist = 8;
    std::cout << "High render distance will impact memory usage" << std::endl;
    std::cout << "Render dist (4-16) = ";
    std::cin >> _render_dist;
    if (_render_dist > 16)
        _render_dist = 16;
    if (_render_dist < 4)
        _render_dist = 4;
    std::cout << "Render dist = " << _render_dist << std::endl;
    std::cout << "Tab - pause; w,a,s,d - move; shift - down; space - up; e,r - adjust camera speed" << std::endl;

    Shader* chunckShader;

    // OPENGL INIT

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Voxel demo", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, Width, Height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, Keyboard::keyCallback);
    
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendColor(0.0f, 0.0f, 0.0f, 0.75f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // SHADERS =======================================
    chunckShader = new Shader("assets/shaders/vertexShaders/vertex_chunck.glsl", 
                              "assets/shaders/fragmentShaders/fragment_chunck.glsl",
                              "assets/shaders/geometryShaders/geometry_chunck.glsl");

    // block colors
    glm::vec3 block_colors[256]{ glm::vec3(1.0f, 1.0f, 1.0f) };
    block_colors[1] = glm::vec3(0.25f, 0.25f, 1.0f); // WATER
    block_colors[2] = glm::vec3(1.0f, 1.0f, 1.0f); // SOLID

    chunckShader->activate();
    for (unsigned int i = 0; i < 256; i++) {
        chunckShader->set3Float(("colors[" + std::to_string(i) + "]"), block_colors[i]);
    }

    // face culling
    glEnable(GL_CULL_FACE);

    Mouse::centerCursor(true);
    camera.cameraSetPos(glm::vec3(0.0f, 0.0f, 0.0f));

    ChunckGenerator::init(_render_dist, 0.25f);
    ChunckGenerator::updatePos(camera.cameraPos());

    // MAIN LOOP =======================
    while (!glfwWindowShouldClose(window)){
        // Time
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        //background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // test objects
        showFPS(window);

        // change generator position
        ChunckGenerator::updatePos(camera.cameraPos());

        // Draw models
        bool newPerspective = camera.NeedToGetNewPerspective();

        // draw chuncks
        ChunckGenerator::Draw(chunckShader, &camera, newPerspective);
        ChunckGenerator::update();

        //buffers swap (next frame)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    //delete buffers
    ChunckGenerator::delAll();

    glfwTerminate();
    return 0;
} 

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Width = width;
    Height = height;

    camera.setScreenSize((float)Width, (float)Height);
}

void processInput(GLFWwindow* window){
    if (Keyboard::key(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }

    if (Keyboard::keyWentDown(GLFW_KEY_TAB)) {
        if (!freezeMovement) Mouse::centerCursor(false);
        else Mouse::centerCursor(true);
        freezeMovement = !freezeMovement;
    }
    if (freezeMovement) return;

    // camera movement
    float cameraSpeed = camSpeed * deltaTime;
    float cameraSensitivity = 10.0f * deltaTime;

    float x = 0.0f, z = 0.0f;
    float yaw = ((float)Mouse::getMouseDX()) * cameraSensitivity, pitch = ((float)Mouse::getMouseDY()) * cameraSensitivity;

    if (Keyboard::keyWentDown(GLFW_KEY_E)) {
        camSpeed += 1.0f;
        if (camSpeed > 25.0f) camSpeed = 25.0f;
    }
    if (Keyboard::keyWentDown(GLFW_KEY_R)) {
        camSpeed -= 1.0f;
        if (camSpeed < 1.0f) camSpeed = 1.0f;
    }

    if (Keyboard::key(GLFW_KEY_W)) {
        z = cameraSpeed;
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        z = -cameraSpeed;
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        x = cameraSpeed;
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        x = -cameraSpeed;
    }
    if (Keyboard::key(GLFW_KEY_SPACE)) {
        camera.moveUp(cameraSpeed);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        camera.moveUp(-cameraSpeed);
    }

    camera.cameraRotate(yaw, pitch);
    camera.cameraMove(z, 0, x);
}

// just testing
void showFPS(GLFWwindow* pWindow)
{
    double fps_currentTime = glfwGetTime();
    double delta = fps_currentTime - fps_lastTime;
    nbFrames++;
    if (delta >= 1.0) { // If last cout was more than 1 sec ago

        double _fps = double(nbFrames) / delta;

        std::stringstream ss;
        ss << "Hello OpenGl" << " [" << _fps << " FPS]";

        glfwSetWindowTitle(pWindow, ss.str().c_str());

        nbFrames = 0;
        fps_lastTime = (float)fps_currentTime;
    }
}
