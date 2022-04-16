#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <rg/Shader.h>
#include <stb_image.h>
#include <learnopengl/filesystem.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <rg/Texture2D.h>
#include <rg/Cubemap2D.h>
#include <rg/Camera.h>
#include <rg/model.h>
#include <rg/Blending2D.h>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void update(GLFWwindow *window);
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    float cutOff;
    float outerCutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "space_walk", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create a window!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);


    camera.Position = glm::vec3(0,0,3);
    camera.Front = glm::vec3(0,0,-1);
    camera.WorldUp = glm::vec3(0,1,0);

    //shaders
    Shader world("resources/shaders/world.vs", "resources/shaders/world.fs");
    Shader my_blending("resources/shaders/blending.vs", "resources/shaders/blending.fs");
    Shader sun("resources/shaders/sun.vs", "resources/shaders/sun.fs");
    Shader crystals("resources/shaders/lights.vs", "resources/shaders/lights.fs");
    Shader lightCube("resources/shaders/lightcube.vs", "resources/shaders/lightcube.fs");
    Shader model_loading("resources/shaders/model.vs", "resources/shaders/model.fs");


    //textures
    Texture2D texture2D0("resources/textures/window_black.png");
    Texture2D texture2D1("resources/textures/crystal.jpg");
    Texture2D texture2D2("resources/textures/crystalspecular.jpg");


    my_blending.use();
    my_blending.setInt("material.diffuse", 0);

    crystals.use();
    crystals.setInt("material.diffuse", 1);
    crystals.setInt("material.specular", 2);

    vector<std::string> faces
            {
                    "resources/textures/skybox/right.png",
                    "resources/textures/skybox/left.png",
                    "resources/textures/skybox/bottom.png",
                    "resources/textures/skybox/top.png",
                    "resources/textures/skybox/front.png",
                    "resources/textures/skybox/back.png"

            };
    Cubemap2D cubemap2D0(faces);
    world.use();
    world.setInt("skybox", 0);


    //models
    Model sunModel("resources/objects/sun/13913_Sun_v2_l3.obj");
    Model ourModel("resources/objects/runestone/Runestones.obj");


    float crystalVertices[] = {
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            //pyramid
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

            0.0f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, -0.75f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, -0.75f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.75f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, -0.75f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    float planeVertices[] = {
            -8.0f, -0.5f,  15.0f,0.0f, 0.0f,
            8.0f, -0.5f,  15.0f,24.0f, 0.0f,
            -8.0f, -0.5f, -35.0f,0.0f, 24.0f,

            -8.0f, -0.5f, -35.0f,0.0f, 24.0f,
            8.0f, -0.5f,  15.0f,24.0f, 0.0f,
            8.0f, -0.5f, -35.0f,24.0f, 24.0f
    };

    glm::vec3 crystalPosition[] = {
                glm::vec3(2.0f, 0.0f, 10.0f),
                glm::vec3(-2.0f, 0.0f, 10.0f),
                glm::vec3(2.0f, 0.0f, 5.0f),
                glm::vec3(-2.0f, 0.0f, 5.0f),
                glm::vec3(2.0f, 0.0f, 0.0f),
                glm::vec3(-2.0f,0.0f, 0.0f),
                glm::vec3(2.0f,0.0f, -5.0f),
                glm::vec3(-2.0f,0.0f, -5.0f),
                glm::vec3(2.0f, 0.0f, -10.0f),
                glm::vec3(-2.0f,0.0f, -10.0f),
                glm::vec3(2.0f, 0.0f, -15.0f),
                glm::vec3(-2.0f, 0.0f, -15.0f),
                glm::vec3(2.0f, 0.0f, -20.0f),
                glm::vec3(-2.0f, 0.0f, -20.0f),
                glm::vec3(2.0f, 0.0f, -25.0f),
                glm::vec3(-2.0f, 0.0f, -25.0f)
    };

    glm::vec3 spotLightPositions[] = {
            glm::vec3( 0.0f,  2.5f, -5.0f),
            glm::vec3( 0.0f, 2.5f, -15.0f),
            glm::vec3( -4.0f,  2.5f, -26.0f),
            glm::vec3( 4.0f,  2.5f, -26.0f)
    };

    glm::vec3 sunPosition = glm::vec3(-90.0f, 50.0f, -70.0f);

    DirLight dirLight;
    dirLight.direction = sunPosition;
    dirLight.ambient = glm::vec3(0.1f, 0.0f, 0.0f);
    dirLight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);;
    dirLight.specular = glm::vec3(0.005f, 0.005f, 0.005f);

    PointLight pointLight;
    pointLight.position = glm::vec3(0.0f, 0.0f, 0.0f);
    pointLight.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.diffuse = glm::vec3(0.6f, 0.5f, 0.6f);
    pointLight.specular = glm::vec3(0.6f, 0.5f, 1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    SpotLight spotLight;
    spotLight.position = glm::vec3(0.0f, 0.0f, 0.0f);
    spotLight.direction = glm::vec3(0.0f, -1.0f, -1.0f);
    spotLight.ambient = glm::vec3(0.05f, 1.0f, 1.0f);
    spotLight.diffuse = glm::vec3(0.6f, 0.5f, 0.6f);
    spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;
    spotLight.cutOff = glm::cos(glm::radians(33.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(35.0f));

    unsigned int planeVBO, planeVAO, crystalVBO, crystalVAO, cubeVAO, worldVAO, worldVBO;

    //plane
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //crystal
    glGenVertexArrays(1, &crystalVAO);
    glGenBuffers(1, &crystalVBO);
    glBindVertexArray(crystalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crystalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crystalVertices), crystalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    //light cubes
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crystalVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //world cube
    glGenVertexArrays(1, &worldVAO);
    glGenBuffers(1, &worldVBO);
    glBindVertexArray(worldVAO);
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // model/view/projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        float time = glfwGetTime();




        crystals.use();
        glBindVertexArray(crystalVAO);

        texture2D1.active(GL_TEXTURE1);
        texture2D2.active(GL_TEXTURE2);


        crystals.setVec3("dirLight.direction", dirLight.direction);
        crystals.setVec3("dirLight.ambient", dirLight.ambient);
        crystals.setVec3("dirLight.diffuse", dirLight.diffuse);
        crystals.setVec3("dirLight.specular", dirLight.specular);

        crystals.setVec3("pointLight.position", glm::vec3(5.0f * cos(time), 5.0f, 8.0f * sin(time) - 10.0f));
        crystals.setVec3("pointLight.ambient", pointLight.ambient);
        crystals.setVec3("pointLight.diffuse", pointLight.diffuse);
        crystals.setVec3("pointLight.specular", pointLight.specular);
        crystals.setFloat("pointLight.constant", pointLight.constant);
        crystals.setFloat("pointLight.linear", pointLight.linear);
        crystals.setFloat("pointLight.quadratic", pointLight.quadratic);

        crystals.setVec3("spotLight[0].position", spotLightPositions[0]);
        crystals.setVec3("spotLight[0].direction", spotLight.direction);
        crystals.setVec3("spotLight[0].ambient", spotLight.ambient);
        crystals.setVec3("spotLight[0].diffuse", spotLight.diffuse);
        crystals.setVec3("spotLight[0].specular", spotLight.specular);
        crystals.setFloat("spotLight[0].constant", spotLight.constant);
        crystals.setFloat("spotLight[0].linear", spotLight.linear);
        crystals.setFloat("spotLight[0].quadratic", spotLight.quadratic);
        crystals.setFloat("spotLight[0].cutOff", spotLight.cutOff);
        crystals.setFloat("spotLight[0].outerCutOff", spotLight.outerCutOff);

        crystals.setVec3("spotLight[1].position", spotLightPositions[1]);
        crystals.setVec3("spotLight[1].direction", spotLight.direction);
        crystals.setVec3("spotLight[1].ambient", spotLight.ambient);
        crystals.setVec3("spotLight[1].diffuse", spotLight.diffuse);
        crystals.setVec3("spotLight[1].specular", spotLight.specular);
        crystals.setFloat("spotLight[1].constant", spotLight.constant);
        crystals.setFloat("spotLight[1].linear", spotLight.linear);
        crystals.setFloat("spotLight[1].quadratic", spotLight.quadratic);
        crystals.setFloat("spotLight[1].cutOff", spotLight.cutOff);
        crystals.setFloat("spotLight[1].outerCutOff", spotLight.outerCutOff);

        crystals.setVec3("spotLight[2].position", spotLightPositions[2]);
        crystals.setVec3("spotLight[2].direction", spotLight.direction);
        crystals.setVec3("spotLight[2].ambient", spotLight.ambient);
        crystals.setVec3("spotLight[2].diffuse", spotLight.diffuse);
        crystals.setVec3("spotLight[2].specular", spotLight.specular);
        crystals.setFloat("spotLight[2].constant", spotLight.constant);
        crystals.setFloat("spotLight[2].linear", spotLight.linear);
        crystals.setFloat("spotLight[2].quadratic", spotLight.quadratic);
        crystals.setFloat("spotLight[2].cutOff", spotLight.cutOff);
        crystals.setFloat("spotLight[2].outerCutOff", spotLight.outerCutOff);

        crystals.setVec3("spotLight[3].position", spotLightPositions[3]);
        crystals.setVec3("spotLight[3].direction", spotLight.direction);
        crystals.setVec3("spotLight[3].ambient", spotLight.ambient);
        crystals.setVec3("spotLight[3].diffuse", spotLight.diffuse);
        crystals.setVec3("spotLight[3].specular", spotLight.specular);
        crystals.setFloat("spotLight[3].constant", spotLight.constant);
        crystals.setFloat("spotLight[3].linear", spotLight.linear);
        crystals.setFloat("spotLight[3].quadratic", spotLight.quadratic);
        crystals.setFloat("spotLight[3].cutOff", spotLight.cutOff);
        crystals.setFloat("spotLight[3].outerCutOff", spotLight.outerCutOff);

        crystals.setVec3("viewPos", camera.Position);
        crystals.setFloat("material.shininess", 8.0f);
        crystals.setMat4("projection", projection);
        crystals.setMat4("view", view);
        for (int i = 0; i < 16; ++i) {
            time = glfwGetTime() + i/2.0f;
            model = glm::mat4(1.0f);
            model = glm::translate(model, crystalPosition[i]);
            model = glm::translate(model, glm::vec3(0.0f, 2*sin(time), 0.0f));
            model = glm::scale(model, glm::vec3(0.4f, 1.5f, 0.4f));
            crystals.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 60);
        }




        lightCube.use();
        glBindVertexArray(cubeVAO);

        lightCube.setMat4("projection", projection);
        lightCube.setMat4("view", view);
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, spotLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCube.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }




        model_loading.use();

        model_loading.setVec3("dirLight.direction", dirLight.direction);
        model_loading.setVec3("dirLight.ambient", dirLight.ambient);
        model_loading.setVec3("dirLight.diffuse", dirLight.diffuse);
        model_loading.setVec3("dirLight.specular", dirLight.specular);

        model_loading.setVec3("pointLight.position", glm::vec3(5.0f * cos(time), 5.0f, 8.0f * sin(time) - 10.0f));
        model_loading.setVec3("pointLight.ambient", pointLight.ambient);
        model_loading.setVec3("pointLight.diffuse", pointLight.diffuse);
        model_loading.setVec3("pointLight.specular", pointLight.specular);
        model_loading.setFloat("pointLight.constant", pointLight.constant);
        model_loading.setFloat("pointLight.linear", pointLight.linear);
        model_loading.setFloat("pointLight.quadratic", pointLight.quadratic);

        model_loading.setVec3("spotLight[0].position", spotLightPositions[0]);
        model_loading.setVec3("spotLight[0].direction", spotLight.direction);
        model_loading.setVec3("spotLight[0].ambient", spotLight.ambient);
        model_loading.setVec3("spotLight[0].diffuse", spotLight.diffuse);
        model_loading.setVec3("spotLight[0].specular", spotLight.specular);
        model_loading.setFloat("spotLight[0].constant", spotLight.constant);
        model_loading.setFloat("spotLight[0].linear", spotLight.linear);
        model_loading.setFloat("spotLight[0].quadratic", spotLight.quadratic);
        model_loading.setFloat("spotLight[0].cutOff", spotLight.cutOff);
        model_loading.setFloat("spotLight[0].outerCutOff", spotLight.outerCutOff);

        model_loading.setVec3("spotLight[1].position", spotLightPositions[1]);
        model_loading.setVec3("spotLight[1].direction", spotLight.direction);
        model_loading.setVec3("spotLight[1].ambient", spotLight.ambient);
        model_loading.setVec3("spotLight[1].diffuse", spotLight.diffuse);
        model_loading.setVec3("spotLight[1].specular", spotLight.specular);
        model_loading.setFloat("spotLight[1].constant", spotLight.constant);
        model_loading.setFloat("spotLight[1].linear", spotLight.linear);
        model_loading.setFloat("spotLight[1].quadratic", spotLight.quadratic);
        model_loading.setFloat("spotLight[1].cutOff", spotLight.cutOff);
        model_loading.setFloat("spotLight[1].outerCutOff", spotLight.outerCutOff);

        model_loading.setVec3("spotLight[2].position", spotLightPositions[2]);
        model_loading.setVec3("spotLight[2].direction", spotLight.direction);
        model_loading.setVec3("spotLight[2].ambient", spotLight.ambient);
        model_loading.setVec3("spotLight[2].diffuse", spotLight.diffuse);
        model_loading.setVec3("spotLight[2].specular", spotLight.specular);
        model_loading.setFloat("spotLight[2].constant", spotLight.constant);
        model_loading.setFloat("spotLight[2].linear", spotLight.linear);
        model_loading.setFloat("spotLight[2].quadratic", spotLight.quadratic);
        model_loading.setFloat("spotLight[2].cutOff", spotLight.cutOff);
        model_loading.setFloat("spotLight[2].outerCutOff", spotLight.outerCutOff);

        model_loading.setVec3("spotLight[3].position", spotLightPositions[3]);
        model_loading.setVec3("spotLight[3].direction", spotLight.direction);
        model_loading.setVec3("spotLight[3].ambient", spotLight.ambient);
        model_loading.setVec3("spotLight[3].diffuse", spotLight.diffuse);
        model_loading.setVec3("spotLight[3].specular", spotLight.specular);
        model_loading.setFloat("spotLight[3].constant", spotLight.constant);
        model_loading.setFloat("spotLight[3].linear", spotLight.linear);
        model_loading.setFloat("spotLight[3].quadratic", spotLight.quadratic);
        model_loading.setFloat("spotLight[3].cutOff", spotLight.cutOff);
        model_loading.setFloat("spotLight[3].outerCutOff", spotLight.outerCutOff);

        model_loading.setVec3("viewPosition", camera.Position);
        model_loading.setMat4("projection", projection);
        model_loading.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, -30.0f));
        model = glm::scale(model, glm::vec3(0.7f));
        model_loading.setMat4("model", model);

        ourModel.Draw(model_loading);






        sun.use();

        sun.setVec3("dirLight.direction", dirLight.direction);
        sun.setVec3("dirLight.ambient", dirLight.ambient);
        sun.setVec3("dirLight.diffuse", dirLight.diffuse);
        sun.setVec3("dirLight.specular", dirLight.specular);

        sun.setVec3("pointLight.position", pointLight.position);
        sun.setVec3("pointLight.ambient", pointLight.ambient);
        sun.setVec3("pointLight.diffuse", pointLight.diffuse);
        sun.setVec3("pointLight.specular", pointLight.specular);
        sun.setFloat("pointLight.constant", pointLight.constant);
        sun.setFloat("pointLight.linear", pointLight.linear);
        sun.setFloat("pointLight.quadratic", pointLight.quadratic);

        sun.setVec3("spotLight[0].position", spotLightPositions[0]);
        sun.setVec3("spotLight[0].direction", spotLight.direction);
        sun.setVec3("spotLight[0].ambient", spotLight.ambient);
        sun.setVec3("spotLight[0].diffuse", spotLight.diffuse);
        sun.setVec3("spotLight[0].specular", spotLight.specular);
        sun.setFloat("spotLight[0].constant", spotLight.constant);
        sun.setFloat("spotLight[0].linear", spotLight.linear);
        sun.setFloat("spotLight[0].quadratic", spotLight.quadratic);
        sun.setFloat("spotLight[0].cutOff", spotLight.cutOff);
        sun.setFloat("spotLight[0].outerCutOff", spotLight.outerCutOff);

        sun.setVec3("spotLight[1].position", spotLightPositions[1]);
        sun.setVec3("spotLight[1].direction", spotLight.direction);
        sun.setVec3("spotLight[1].ambient", spotLight.ambient);
        sun.setVec3("spotLight[1].diffuse", spotLight.diffuse);
        sun.setVec3("spotLight[1].specular", spotLight.specular);
        sun.setFloat("spotLight[1].constant", spotLight.constant);
        sun.setFloat("spotLight[1].linear", spotLight.linear);
        sun.setFloat("spotLight[1].quadratic", spotLight.quadratic);
        sun.setFloat("spotLight[1].cutOff", spotLight.cutOff);
        sun.setFloat("spotLight[1].outerCutOff", spotLight.outerCutOff);

        sun.setVec3("spotLight[2].position", spotLightPositions[2]);
        sun.setVec3("spotLight[2].direction", spotLight.direction);
        sun.setVec3("spotLight[2].ambient", spotLight.ambient);
        sun.setVec3("spotLight[2].diffuse", spotLight.diffuse);
        sun.setVec3("spotLight[2].specular", spotLight.specular);
        sun.setFloat("spotLight[2].constant", spotLight.constant);
        sun.setFloat("spotLight[2].linear", spotLight.linear);
        sun.setFloat("spotLight[2].quadratic", spotLight.quadratic);
        sun.setFloat("spotLight[2].cutOff", spotLight.cutOff);
        sun.setFloat("spotLight[2].outerCutOff", spotLight.outerCutOff);

        sun.setVec3("spotLight[3].position", spotLightPositions[3]);
        sun.setVec3("spotLight[3].direction", spotLight.direction);
        sun.setVec3("spotLight[3].ambient", spotLight.ambient);
        sun.setVec3("spotLight[3].diffuse", spotLight.diffuse);
        sun.setVec3("spotLight[3].specular", spotLight.specular);
        sun.setFloat("spotLight[3].constant", spotLight.constant);
        sun.setFloat("spotLight[3].linear", spotLight.linear);
        sun.setFloat("spotLight[3].quadratic", spotLight.quadratic);
        sun.setFloat("spotLight[3].cutOff", spotLight.cutOff);
        sun.setFloat("spotLight[3].outerCutOff", spotLight.outerCutOff);

        sun.setVec3("viewPosition", camera.Position);
        sun.setMat4("projection", projection);
        sun.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, sunPosition);
        model = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f));
        sun.setMat4("model", model);

        sunModel.Draw(sun);

        sun.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f * cos(time), 5.0f, 8.0f * sin(time) - 10.0f));
        model = glm::rotate(model, time, glm::vec3(1.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.002f));
        model_loading.setMat4("model", model);

        sunModel.Draw(sun);





        glDepthFunc(GL_LEQUAL);
        world.use();
        glBindVertexArray(worldVAO);

        cubemap2D0.active(GL_TEXTURE0);

        glm::mat4 view1 = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        world.setMat4("view", view1);
        world.setMat4("projection", projection);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);




        my_blending.use();
        glBindVertexArray(planeVAO);

        texture2D0.active(GL_TEXTURE0);

        my_blending.setMat4("projection", projection);
        my_blending.setMat4("view", view);
        my_blending.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        update(window);
        glfwSwapBuffers(window);
    }


    glDeleteBuffers(1, &worldVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &crystalVBO);
    glDeleteVertexArrays(1, &worldVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &crystalVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    world.deleteProgram();
    crystals.deleteProgram();
    my_blending.deleteProgram();
    model_loading.deleteProgram();
    lightCube.deleteProgram();
    sun.deleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void update(GLFWwindow *window) {

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {

    if(key == GLFW_KEY_R && action == GLFW_PRESS) {
        glClearColor(1.0, 0.0, 0.0, 1.0);
    }

    if(key == GLFW_KEY_G && action == GLFW_PRESS) {
        glClearColor(0.0, 1.0, 0.0, 1.0);
    }

    if(key == GLFW_KEY_B && action == GLFW_PRESS) {
        glClearColor(0.0, 0.0, 1.0, 1.0);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
















