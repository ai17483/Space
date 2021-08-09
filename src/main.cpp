#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <rg/Shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <rg/Texture2D.h>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void update(GLFWwindow *window);
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 position;
int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hello_window", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create a window!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Shader shader("resources/shaders/vertexShader.vs", "resources/shaders/fragmentShader.fs");

    float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
                        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
                        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
                        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

                        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
                        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
                        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

    };

    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    //saljemo niz na graficku karticu
    unsigned int VBO, VAO, EBO;

    //ucitavanje iz ram memorije u memoriju graficke kartice
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //definisemo sta znace koordinate koje smo poslali
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    Texture2D texture2D1("resources/textures/container.jpg", GL_LINEAR, GL_REPEAT, GL_RGB);
    Texture2D texture2D2("resources/textures/awesomeface.png", GL_LINEAR, GL_REPEAT, GL_RGBA);

    shader.use();
    shader.setInt("t0", 0);
    shader.setInt("t1", 1);

    //oslobadjanje
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //petlja za renderovanje
    while(!glfwWindowShouldClose(window)) {

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // postavljanje boje

        texture2D1.active(GL_TEXTURE0);
        texture2D2.active(GL_TEXTURE1);

        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        m = glm::scale(m, glm::vec3(0.5, 0.5, 1.0));


        //crtanje trougla
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5, 1.0, 0.0));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 100.0f);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        //shader.setMat4("m", m);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        update(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    shader.deleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position.y += 0.05;

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position.y -= 0.05;
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




















