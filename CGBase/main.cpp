/**

 * @author Dusan Janosevic
 * @copyright Copyright (c) 2022
 *
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "shader.hpp"
#include "model.hpp" //Klasa za ucitavanje modela
#include "renderable.hpp" //Klasa za bafere
#include "cubebuffer.hpp"
#include "pyramidbuffer.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "stb_image.h"


const int WindowWidth = 1920;
const int WindowHeight = 1080;
const std::string WindowTitle = "Chadd scene";
const float TargetFPS = 144.0f;
const float TargetFrameTime = 1.0f / TargetFPS;

struct Input {
    bool MoveLeft;
    bool MoveRight;
    bool MoveUp;
    bool MoveDown;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
    bool ShouldRotate;
    bool MoveRug;
    bool Grow;
    bool Shrink;
};

struct EngineState {
    Input* mInput;
    Camera* mCamera;
    float mDT;
};

const std::vector<int> MinFilterValues = {
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

float mScalingFactor;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) //Aktivira se pri mijenjanju velicine prozora
{
    glViewport(0, 0, width, height);
}

static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;

    bool IsPressed = action == GLFW_PRESS;
    if (IsPressed) {
        switch (key) {
            case GLFW_KEY_R: UserInput->MoveRug = !UserInput->MoveRug; break;
            case GLFW_KEY_C: UserInput->ShouldRotate = !UserInput->ShouldRotate; break;
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        }
    }
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
        case GLFW_KEY_A: UserInput->LookLeft = IsDown; break;
        case GLFW_KEY_D: UserInput->LookRight = IsDown; break;
        case GLFW_KEY_W: UserInput->LookDown = IsDown; break;
        case GLFW_KEY_S: UserInput->LookUp = IsDown; break;
        case GLFW_KEY_SPACE: UserInput->Grow = IsDown; break;
        case GLFW_KEY_LEFT_CONTROL: UserInput->Shrink = IsDown; break;

        case GLFW_KEY_RIGHT: UserInput->MoveLeft = IsDown; break;
        case GLFW_KEY_LEFT: UserInput->MoveRight = IsDown; break;
        case GLFW_KEY_UP: UserInput->MoveUp = IsDown; break;
        case GLFW_KEY_DOWN: UserInput->MoveDown = IsDown; break;
    }
}

static void
ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

static void
HandleInput(EngineState* state, GLFWwindow* Window) {
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;
        if (UserInput->LookLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
        if (UserInput->LookRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
        if (UserInput->LookUp) FPSCamera->Move(0.0f, -1.0f, state->mDT);
        if (UserInput->LookDown) FPSCamera->Move(0.0f, 1.0f, state->mDT);
        if (UserInput->Grow) FPSCamera->Grow();
        if (UserInput->Shrink) FPSCamera->Shrink();
    float speed = 3.0f;
    float mouseSpeed = 0.015f;
    double  xpos, ypos;
    int currWidth;
    int currHeight;
    glfwGetWindowSize(Window, &currHeight, &currWidth);
    glfwGetCursorPos(Window, &xpos, &ypos);
    double horizontalAngle = mouseSpeed * float(currWidth / 2 - xpos);
    double verticalAngle = mouseSpeed * float(currHeight / 2 - ypos);
    FPSCamera->SetPitch(verticalAngle);
    FPSCamera->SetYaw(-horizontalAngle);
    FPSCamera->updateVectors();
}


static void
ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    mScalingFactor += yoffset*0.1;
}

int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(ErrorCallback);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetScrollCallback(Window, ScrollCallback);

    Shader LightShader("shaders/basic.vert", "shaders/basic.frag");
    Shader BasicShader("shaders/phong.vert", "shaders/phong.frag");
    glUseProgram(BasicShader.GetId());
    BasicShader.SetUniform3f("uDirLight.Direction", glm::vec3(1.0f, -1.0f, 1.0f));

    BasicShader.SetUniform3f("uDirLight.Ka", glm::vec3(0.1275f, 0.1275f, 0.1275f));
    BasicShader.SetUniform3f("uDirLight.Kd", glm::vec3(0.4284f, 0.4284f, 0.4284f));
    BasicShader.SetUniform3f("uDirLight.Ks", glm::vec3(0.271906f, 0.271906f, 0.271906f));

    //BasicShader.SetUniform3f("uDirLight.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
    //BasicShader.SetUniform3f("uDirLight.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
    //BasicShader.SetUniform3f("uDirLight.Ks", glm::vec3(0.0f, 0.0f, 0.0f));


    BasicShader.SetUniform1f("uPointLight1.Kc", 1.0f);
    BasicShader.SetUniform1f("uPointLight1.Kl", 0.7);
    BasicShader.SetUniform1f("uPointLight1.Kq", 1.8);

    BasicShader.SetUniform1f("uPointLight2.Kc", 1.0f);
    BasicShader.SetUniform1f("uPointLight2.Kl", 0.7);
    BasicShader.SetUniform1f("uPointLight2.Kq", 1.8);

    BasicShader.SetUniform1f("uPointLight3.Kc", 1.0f);
    BasicShader.SetUniform1f("uPointLight3.Kl", 0.7);
    BasicShader.SetUniform1f("uPointLight3.Kq", 1.8);

    BasicShader.SetUniform1f("uPointLight4.Kc", 1.0f);
    BasicShader.SetUniform1f("uPointLight4.Kl", 0.7);
    BasicShader.SetUniform1f("uPointLight4.Kq", 1.8);

    BasicShader.SetUniform3f("uSpotlight.Ka", glm::vec3(0.7f, 0.1f, 0.1f));
    BasicShader.SetUniform3f("uSpotlight.Kd", glm::vec3(0.7f, 0.1f, 0.1f));
    BasicShader.SetUniform3f("uSpotlight.Ks", glm::vec3(0.7f, 0.1f, 0.1f));

    //BasicShader.SetUniform3f("uSpotlight.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
    //BasicShader.SetUniform3f("uSpotlight.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
    //BasicShader.SetUniform3f("uSpotlight.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

    BasicShader.SetUniform1f("uSpotlight.Kc", 1.0f);
    BasicShader.SetUniform1f("uSpotlight.Kl", 0.092f);
    BasicShader.SetUniform1f("uSpotlight.Kq", 0.032f);

    BasicShader.SetUniform1f("uSpotlight.InnerCutOff", glm::cos(glm::radians(13.5f)));
    BasicShader.SetUniform1f("uSpotlight.OuterCutOff", glm::cos(glm::radians(17.5f)));

    BasicShader.SetUniform1i("uMaterial.Kd", 0);
    BasicShader.SetUniform1i("uMaterial.Ks", 1);
    BasicShader.SetUniform1f("uMaterial.Shininess", 128.0f);
    glUseProgram(0);

    Model Star("res/star/star.obj");
    if (!Star.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }

    Model Bee("res/bee/bee.obj");
    if (!Bee.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }

    Model Goku("res/goku/Goku.obj");
    if (!Goku.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }

    Model Dragon("res/dragon/dragon.obj");
    if (!Dragon.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }


    unsigned BrickTexture = Texture::LoadImageToTexture("textures/brick.png");
    glBindTexture(GL_TEXTURE_2D, BrickTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned BrickSmallTexture = Texture::LoadImageToTexture("textures/brickSmall.png");
    glBindTexture(GL_TEXTURE_2D, BrickSmallTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned ClothTexture = Texture::LoadImageToTexture("textures/cloth.jpg");
    glBindTexture(GL_TEXTURE_2D, ClothTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned SandTexture = Texture::LoadImageToTexture("textures/sand.jpg");
    glBindTexture(GL_TEXTURE_2D, SandTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned MoonTexture = Texture::LoadImageToTexture("textures/moon.jpg");
    glBindTexture(GL_TEXTURE_2D, MoonTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned TreeTexture = Texture::LoadImageToTexture("textures/tree.jpg");
    glBindTexture(GL_TEXTURE_2D, TreeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned LeafTexture = Texture::LoadImageToTexture("textures/leaf.jpg");
    glBindTexture(GL_TEXTURE_2D, LeafTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned WhiteTexture = Texture::LoadImageToTexture("textures/white.png");
    unsigned BlackDotsTexture = Texture::LoadImageToTexture("textures/blackWithDots.jpg");
    unsigned BlackTexture = Texture::LoadImageToTexture("textures/black.jpg");

    CubeBuffer cubeBuffer;
    Renderable cube(cubeBuffer.GetVertices(), cubeBuffer.GetVertexCount(), cubeBuffer.GetIndices(), cubeBuffer.GetIndicesCount());
    PyramidBuffer pyramidBuffer;
    Renderable pyramid(pyramidBuffer.GetVertices(), pyramidBuffer.GetVertexCount(), pyramidBuffer.GetIndices(), pyramidBuffer.GetIndicesCount());

    glm::mat4 m(1.0f);
    
    float angle = 0;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.05, 0.1, 0.2, 1.0);

    float FrameStartTime = glfwGetTime();
    float FrameEndTime = glfwGetTime();
    float dt = FrameEndTime - FrameStartTime;
    float RugXPosition = -0.6;
    float RugZPosition = -0.3;
    float Distance = 2.5f;

    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;
    BasicShader.SetUniform1i("ourTexture", 1);
    LightShader.SetUniform1i("ourTexture", 1);
    glfwSetWindowUserPointer(Window, &State);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwWindowHint(GLFW_DECORATED, false);
    UserInput.ShouldRotate = false;
    UserInput.MoveRug = true;

    glm::mat4 v = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
    glm::mat4 p = glm::perspective(glm::radians(90.0f), (float)WindowWidth / WindowHeight, 0.1f, 100.0f);
    while (!glfwWindowShouldClose(Window)) {
        v = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
        HandleInput(&State, Window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (UserInput.MoveDown)
        {
            RugZPosition = RugZPosition - 0.05;
        }
        if (UserInput.MoveUp)
        {
            RugZPosition = RugZPosition + 0.05;
        }
        if (UserInput.MoveRight)
        {
            RugXPosition = RugXPosition + 0.05;
        }
        if (UserInput.MoveLeft)
        {
            RugXPosition = RugXPosition - 0.05;
        }
        FrameStartTime = glfwGetTime();

        //algorithm for screen resizing
        int currWidth;
        int currHeight;
        float widthScaling;
        float heightScaling;
        glfwGetWindowSize(Window, &currHeight, &currWidth);
        glfwSetCursorPos(Window, currWidth / 2, currHeight / 2);
        glm::mat4 w(1.0f);

        w[0][0] = 1.0f;
        w[1][1] = currHeight/ currWidth;
        //w[1][1] = 1.0f;
        float rotationAngle = (float)++angle / 6;

        glUseProgram(LightShader.GetId());

        LightShader.SetViewport(w);
        LightShader.SetProjection(p);
        LightShader.SetView(v);
        LightShader.SetUniform3f("uViewPos", FPSCamera.GetPosition());
        //moon
        for (int i = 0; i < 4; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-2.5, 2.5, -2.5));
            m = glm::rotate(m, glm::radians((i * 15.0f) * 2), glm::vec3(1.0, 1.0, 1.0));
            LightShader.SetColor(1.2, 1.2, 1.2);
            LightShader.SetModel(m);
            cube.Render(MoonTexture,WhiteTexture);
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-2.5, 2.5, -2.5));
            m = glm::rotate(m, glm::radians((i * 15.0f) * 2 + 15.0f), glm::vec3(1.0, 1.0, 1.0));
            LightShader.SetColor(1.2, 1.2, 1.2);
            LightShader.SetModel(m);
            cube.Render(MoonTexture, WhiteTexture);
        }


        glUseProgram(BasicShader.GetId());

        float pointLightPower = 1 - std::max(sin((glfwGetTime() * 15) / 4),0.0);
        BasicShader.SetUniform3f("uPointLight1.Ka", glm::vec3(0.1 + pointLightPower * 0.1, 0.01f, 0.01f));
        BasicShader.SetUniform3f("uPointLight1.Kd", glm::vec3(pointLightPower * 0.4, 0.1f, 0.1f));
        BasicShader.SetUniform3f("uPointLight1.Ks", glm::vec3(pointLightPower, 0.1f, 0.1f));

        pointLightPower = 1 - std::max(sin((60 + glfwGetTime() * 15) / 4), 0.0);
        BasicShader.SetUniform3f("uPointLight2.Ka", glm::vec3(0.1 + pointLightPower * 0.1, 0.01f, 0.01f));
        BasicShader.SetUniform3f("uPointLight2.Kd", glm::vec3(pointLightPower * 0.4, 0.1f, 0.1f));
        BasicShader.SetUniform3f("uPointLight2.Ks", glm::vec3(pointLightPower, 0.1f, 0.1f));

        pointLightPower = 1 - std::max(sin((120 + glfwGetTime() * 15) / 4), 0.0);
        BasicShader.SetUniform3f("uPointLight3.Ka", glm::vec3(0.1 + pointLightPower * 0.1, 0.01f, 0.01f));
        BasicShader.SetUniform3f("uPointLight3.Kd", glm::vec3(pointLightPower * 0.4, 0.1f, 0.1f));
        BasicShader.SetUniform3f("uPointLight3.Ks", glm::vec3(pointLightPower, 0.1f, 0.1f));

        pointLightPower = 1 - std::max(sin((180 + glfwGetTime() * 15) / 4), 0.0);
        BasicShader.SetUniform3f("uPointLight4.Ka", glm::vec3(0.1 + pointLightPower * 0.1, 0.01f, 0.01f));
        BasicShader.SetUniform3f("uPointLight4.Kd", glm::vec3(pointLightPower * 0.4, 0.1f, 0.1f));
        BasicShader.SetUniform3f("uPointLight4.Ks", glm::vec3(pointLightPower, 0.1f, 0.1f));


        //BasicShader.SetUniform3f("uPointLight1.Ka", glm::vec3(0.0f,0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight1.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight1.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

        //BasicShader.SetUniform3f("uPointLight2.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight2.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight2.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

        //BasicShader.SetUniform3f("uPointLight3.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight3.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight3.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

        //BasicShader.SetUniform3f("uPointLight4.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight4.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uPointLight4.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

        BasicShader.SetViewport(w);
        BasicShader.SetProjection(p);
        BasicShader.SetView(v);
        BasicShader.SetUniform3f("uViewPos", FPSCamera.GetPosition());
        BasicShader.SetUniform3f("uPointLight1.Position", glm::vec3(1.34, 1.25 + sin((glfwGetTime() * 15) / 4) / 10, -1.34));
        BasicShader.SetUniform3f("uPointLight2.Position", glm::vec3(-1.34, 1.25 + sin((glfwGetTime() * 15) / 4) / 10, -1.34));
        BasicShader.SetUniform3f("uPointLight3.Position", glm::vec3(1.4, 1.05 + sin((glfwGetTime() * 15) / 4) / 10, 1.4));
        BasicShader.SetUniform3f("uPointLight4.Position", glm::vec3(-1.4, 1.05 + sin((glfwGetTime() * 15) / 4) / 10, 1.4));
        BasicShader.SetUniform3f("uSpotlight.Position", glm::vec3(-2.5, 2.5, -2.5));

        BasicShader.SetUniform3f("uSpotlight.Ka", glm::vec3(0.8f, 0.1f, 0.8f));
        BasicShader.SetUniform3f("uSpotlight.Kd", glm::vec3(0.8f, 0.1f, 0.8f));
        BasicShader.SetUniform3f("uSpotlight.Ks", glm::vec3(0.8f, 0.1f, 0.8f));

        //Rug Model
        for (int widthPolygons = 0; widthPolygons <= 25; widthPolygons++)
        {
            for (int heightPolygons = 0; heightPolygons <= 50; heightPolygons++)
            {
                m = glm::mat4(1.0f);
                if ((10 < widthPolygons && widthPolygons < 15) && (5 <= heightPolygons && heightPolygons <= 45))
                    BasicShader.SetColor(1, 0, 0);
                else if ((5 < widthPolygons && widthPolygons < 20) && (5 <= heightPolygons && heightPolygons <= 45))
                    BasicShader.SetColor(1, 1, 0);
                else
                    BasicShader.SetColor(0, 0, 0);
                if (UserInput.ShouldRotate)
                    m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
                if (widthPolygons == 13 && heightPolygons == 25) 
                {
                    glm::vec3 rugPos = glm::vec3(RugXPosition + (float)widthPolygons * 0.02,
                        0.7 + sin(glfwGetTime() * 1.5) / 4 + sin(((float)widthPolygons + glfwGetTime() * 30) / 4) / 120 + sin(((float)heightPolygons + glfwGetTime() * 30) / 4) / 120
                        , RugZPosition + (float)heightPolygons * 0.02);
                    glm::vec3 moonPos = glm::vec3(-2.5, 2.5, -2.5);
                    glm::vec3 temp = glm::normalize(rugPos-moonPos);
                    BasicShader.SetUniform3f("uSpotlight.Direction", temp);
                }
                    m = glm::translate(m, glm::vec3(RugXPosition + (float)widthPolygons * 0.02,
                        0.7 + sin(glfwGetTime() * 1.5) / 4 + sin(((float)widthPolygons + glfwGetTime() * 30) / 4) / 120 + sin(((float)heightPolygons + glfwGetTime() * 30) / 4) / 120
                        , RugZPosition + (float)heightPolygons * 0.02));
                m = glm::scale(m, glm::vec3(0.05, 0.01, 0.05));

                //dont even ask me(rotation of individual elements for fabric)
                float z = 0.01;

                //for z finding angle
                float y1 = sin(glfwGetTime() * 1.5) / 3 + sin(((float)widthPolygons + glfwGetTime() * 30) / 3) / 120 + sin(((float)heightPolygons + glfwGetTime() * 30) / 3) / 120;
                float y2 = sin(glfwGetTime() * 1.5) / 3 + sin(((float)widthPolygons + glfwGetTime() * 30) / 3) / 120 + sin(((float)heightPolygons + 1 + glfwGetTime() * 30) / 3) / 120;
                float y = y2 - y1;
                float zHypotenuse = pow(pow(y, 2) + pow(0.01, 2), 0.5);
                float angleForZ = asin((y) / (zHypotenuse)) / 2;
                m = glm::rotate(m, glm::radians(angleForZ * 120), glm::vec3(0.0, 0.0, 1.0));

                //for x finding angle
                y1 = sin(glfwGetTime() * 1.5) / 6 + sin(((float)widthPolygons + glfwGetTime() * 30) / 4) / 120 + sin(((float)heightPolygons + glfwGetTime() * 30) / 4) / 120;
                y2 = sin(glfwGetTime() * 1.5) / 6 + sin(((float)widthPolygons + glfwGetTime() * 30) / 4) / 120 + sin(((float)heightPolygons + 1 + glfwGetTime() * 30) / 4) / 120;
                y = y2 - y1;
                float xHypotenuse = pow(pow(y, 2) + pow(0.01, 2), 0.5);
                float angleForX = asin((y) / (xHypotenuse)) / 2;
                m = glm::rotate(m, glm::radians(-angleForX * 120), glm::vec3(1.0, 0.0, 0.0));

                BasicShader.SetModel(m);
                cube.Render(ClothTexture, WhiteTexture);
            }
        }
        //BasicShader.SetUniform3f("uSpotlight.Ka", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uSpotlight.Kd", glm::vec3(0.0f, 0.0f, 0.0f));
        //BasicShader.SetUniform3f("uSpotlight.Ks", glm::vec3(0.0f, 0.0f, 0.0f));

        //base
        m = glm::mat4(1.0f);
        if(UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(0, -0.5, 0));
        m = glm::scale(m, glm::vec3(10, 0.3,10));
        BasicShader.SetColor(0.5, 0.5, 0.2);
        BasicShader.SetModel(m);
        cube.Render(SandTexture, BlackDotsTexture);

        //pyramid 1
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.34, 0.2, -1.34));
        m = glm::scale(m, glm::vec3(3.3));
        BasicShader.SetColor(0.5, 0.5, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickTexture, WhiteTexture);

        //pyramid cap 1
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.34, 0.8, -1.34));
        m = glm::scale(m, glm::vec3(0.375));
        BasicShader.SetColor(0.7, 0.7, 0.2);
        BasicShader.SetModel(m);

        pyramid.Render(BrickSmallTexture, WhiteTexture);

        //star 1
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.34, 1.05 + sin((glfwGetTime() * 15) / 4) / 10, -1.34));
        m = glm::rotate(m, glm::radians(rotationAngle*5), glm::vec3(0.0, 1.0, 0.0));
        m = glm::scale(m, glm::vec3(0.012));
        BasicShader.SetModel(m);
        Star.Render();
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        BasicShader.SetModel(m);
        Star.Render();

        //pyramid 2
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.34, 0.2, -1.34));
        m = glm::scale(m, glm::vec3(3.3));
        BasicShader.SetColor(0.5, 0.5, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickTexture, WhiteTexture);

        //pyramid cap 2
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.34, 0.8, -1.34));
        m = glm::scale(m, glm::vec3(0.375));
        BasicShader.SetColor(0.7, 0.7, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickSmallTexture, WhiteTexture);

        //star 2
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.34, 1.05 + sin((60 + glfwGetTime() * 15) / 4) / 10, -1.34));
        m = glm::rotate(m, glm::radians(rotationAngle * 5), glm::vec3(0.0, 1.0, 0.0));
        m = glm::scale(m, glm::vec3(0.012));
        BasicShader.SetModel(m);
        Star.Render();
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        BasicShader.SetModel(m);
        Star.Render();

        //pyramid 3
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.4, 0, 1.4));
        m = glm::scale(m, glm::vec3(2.2));
        BasicShader.SetColor(0.5, 0.5, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickTexture, WhiteTexture);

        //pyramid cap 3
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.4, 0.4, 1.4));
        m = glm::scale(m, glm::vec3(0.25));
        BasicShader.SetColor(0.7, 0.7, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickSmallTexture, WhiteTexture);

        //star 3
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1.4, 0.6 + sin((120 + glfwGetTime() * 15) / 4) / 10, 1.4));
        m = glm::rotate(m, glm::radians(rotationAngle * 5), glm::vec3(0.0, 1.0, 0.0));
        m = glm::scale(m, glm::vec3(0.012));
        BasicShader.SetModel(m);
        Star.Render();
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        BasicShader.SetModel(m);
        Star.Render();

        //pyramid 4
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.4, 0, 1.4));
        m = glm::scale(m, glm::vec3(2.2));
        BasicShader.SetColor(0.5, 0.5, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickTexture, WhiteTexture);

        //pyramid cap 4
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.4, 0.4, 1.4));
        m = glm::scale(m, glm::vec3(0.25));
        BasicShader.SetColor(0.7, 0.7, 0.2);
        BasicShader.SetModel(m);
        pyramid.Render(BrickSmallTexture, WhiteTexture);

        //star 4
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1.4, 0.6 + sin((180 + glfwGetTime() * 15) / 4) / 10, 1.4));
        m = glm::rotate(m, glm::radians(rotationAngle * 5), glm::vec3(0.0, 1.0, 0.0));
        m = glm::scale(m, glm::vec3(0.012));
        BasicShader.SetModel(m);
        Star.Render();
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        BasicShader.SetModel(m);
        Star.Render();

        //Bee Model 1
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::rotate(m, glm::radians(-5*rotationAngle/2), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(1, 1, 0));
        m = glm::scale(m, glm::vec3(0.03));
        BasicShader.SetModel(m);
        Bee.Render();

        //Bee Model 2
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::rotate(m, glm::radians(-5 * rotationAngle/2), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(-1, 1, 0));
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        m = glm::scale(m, glm::vec3(0.03));
        BasicShader.SetModel(m);
        BasicShader.SetColor(1.0f,1.0f,1.0f);
        Bee.Render();

        //Goku model
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(0, -0.4, -0.2));
        m = glm::scale(m, glm::vec3(0.1));
        BasicShader.SetModel(m);
        Goku.Render();

        //Dragon model
        m = glm::mat4(1.0f);
        if (UserInput.ShouldRotate)
            m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        m = glm::translate(m, glm::vec3(0, 0, -2));
        m = glm::scale(m, glm::vec3(0.3));
        BasicShader.SetModel(m);
        Dragon.Render();

        //Detailed tree 1
        //trunk part1 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(1.5, 0, 0));
            m = glm::scale(m, glm::vec3(0.5, 2, 0.5));
            m = glm::rotate(m, glm::radians(i*30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            cube.Render(TreeTexture, WhiteTexture);
        }

        //trunk part2 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(1.5, -0.3, 0));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            m = glm::scale(m, glm::vec3(0.8, 0.4, 0.8));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(TreeTexture, WhiteTexture);
        }

        //switching culling bcs negative y scaling
        glCullFace(GL_FRONT);
        //leafs part1 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(1.5, 0.3, 0));
            m = glm::scale(m, glm::vec3(1.55, -0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }
        glCullFace(GL_BACK);
        
        //leafs part2 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(1.5, 0.52, 0));
            m = glm::scale(m, glm::vec3(1.5, 0.6, 1.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            cube.Render(LeafTexture, WhiteTexture);
        }
        //leafs part3 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(1.5, 0.77, 0));
            m = glm::scale(m, glm::vec3(1.55, 0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }

        //Detailed tree 2
        //trunk part1 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-1.5, 0, 0));
            m = glm::scale(m, glm::vec3(0.5, 2, 0.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            cube.Render(TreeTexture, WhiteTexture);
        }

        //trunk part2 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-1.5, -0.3, 0));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            m = glm::scale(m, glm::vec3(0.8, 0.4, 0.8));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(TreeTexture, WhiteTexture);
        }

        //switching culling bcs negative y scaling
        glCullFace(GL_FRONT);
        //leafs part1 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-1.5, 0.3, 0));
            m = glm::scale(m, glm::vec3(1.55, -0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }
        glCullFace(GL_BACK);
        //leafs part2 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-1.5, 0.52, 0));
            m = glm::scale(m, glm::vec3(1.5, 0.6, 1.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            cube.Render(LeafTexture, WhiteTexture);
        }
        //leafs part3 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(-1.5, 0.77, 0));
            m = glm::scale(m, glm::vec3(1.55, 0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }
       
        //Detailed tree 3
        //trunk part1 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(0, 0, -1.5));
            m = glm::scale(m, glm::vec3(0.5, 2, 0.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            cube.Render(TreeTexture, WhiteTexture);
        }

        //trunk part2 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(0, -0.3, -1.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            m = glm::scale(m, glm::vec3(0.8, 0.4, 0.8));
            BasicShader.SetColor(0.3, 0.2, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(TreeTexture, WhiteTexture);
        }

        //switching culling bcs negative y scaling
        glCullFace(GL_FRONT);
        //leafs part1 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(0, 0.3, -1.5));
            m = glm::scale(m, glm::vec3(1.55, -0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }
        glCullFace(GL_BACK);

        //leafs part2 (cube)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(0, 0.52, -1.5));
            m = glm::scale(m, glm::vec3(1.5, 0.6, 1.5));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            cube.Render(LeafTexture, WhiteTexture);
        }

        //leafs part3 (pyramid)
        for (int i = 0; i < 3; i++)
        {
            m = glm::mat4(1.0f);
            if (UserInput.ShouldRotate)
                m = glm::rotate(m, glm::radians(rotationAngle), glm::vec3(0.0, 1.0, 0.0));
            m = glm::translate(m, glm::vec3(0, 0.77, -1.5));
            m = glm::scale(m, glm::vec3(1.55, 0.6, 1.55));
            m = glm::rotate(m, glm::radians(i * 30.0f), glm::vec3(0.0, 1.0, 0.0));
            BasicShader.SetColor(0.1, 0.3, 0.1);
            BasicShader.SetModel(m);
            pyramid.Render(LeafTexture, WhiteTexture);
        }

        glUseProgram(0);
        glfwSwapBuffers(Window);

        FrameEndTime = glfwGetTime();
        dt = FrameEndTime - FrameStartTime;
        if (dt < TargetFPS) {
            int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            FrameEndTime = glfwGetTime();
        }
        State.mDT = FrameEndTime - FrameStartTime;
    }

    glfwTerminate();
    return 0;
}



