#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Block.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void handleGravity();

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// world
std::vector<Block> blocks;

// camera
Camera camera(&blocks, glm::vec3(0.0f, 2.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // enable usage of alpha value for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);


    Shader ourShader("resources/shaders/3d.vs", "resources/shaders/3d.fs");

    float vertices[] = {
        // positions         // texture coords
         0.0f, 0.0f, 1.0f,  0.5f, 0.5f,
         1.0f, 0.0f, 1.0f,  1.0f, 0.5f,
         0.0f, 1.0f, 1.0f,  0.5f, 0.0f,
         1.0f, 1.0f, 1.0f,  1.0f, 0.0f,

         1.0f, 0.0f, 1.0f,  0.5f, 0.5f,
         1.0f, 0.0f, 0.0f, 1.0f, 0.5f,
         1.0f, 1.0f, 1.0f,   0.5f, 0.0f,
         1.0f, 1.0f, 0.0f,  1.0f, 0.0f,

         1.0f, 0.0f, 0.0f, 0.5f, 0.5f,
         0.0f, 0.0f, 0.0f,1.0f, 0.5f,
         1.0f, 1.0f, 0.0f,  0.5f, 0.0f,
         0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

         0.0f, 0.0f, 0.0f,0.5f, 0.5f,
         0.0f, 0.0f, 1.0f, 1.0f, 0.5f,
         0.0f, 1.0f, 0.0f, 0.5f, 0.0f,
         0.0f, 1.0f, 1.0f,  1.0f, 0.0f,

         0.0f, 0.0f, 0.0f,0.0f, 0.5f,
         1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
         1.0f, 0.0f, 1.0f,  0.5f, 1.0f,

         0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 1.0f,  0.0f, 0.5f,
         0.0f, 1.0f, 0.0f,0.5f, 0.0f,
         1.0f, 1.0f, 0.0f, 0.5f, 0.5f
    };

    float placeable_vertices[] = {
        // positions         // texture coords
         0.0f, 0.0f, 1.0f, 0.5f, 1.0f,
         1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         0.0f, 1.0f, 1.0f,  0.5f, 0.5f,
         1.0f, 1.0f, 1.0f,   1.0f, 0.5f,

         1.0f, 0.0f, 1.0f,  0.5f, 1.0f,
         1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,   0.5f, 0.5f,
         1.0f, 1.0f, 0.0f,  1.0f, 0.5f,

         1.0f, 0.0f, 0.0f, 0.5f, 1.0f,
         0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 0.0f,  0.5f, 0.5f,
         0.0f, 1.0f, 0.0f, 1.0f, 0.5f,

         0.0f, 0.0f, 0.0f, 0.5f, 1.0f,
         0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 0.0f, 0.5f, 0.5f,
         0.0f, 1.0f, 1.0f,  1.0f, 0.5f,

         0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
         1.0f, 0.0f, 1.0f,  0.5f, 0.5f,

         0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
         0.0f, 1.0f, 0.0f, 0.5f, 0.5f,
         1.0f, 1.0f, 0.0f, 0.5f, 0.5f
    };

    unsigned int indices[] = {
        //faces
        0,1,3,      0,3,2,          // Face front
        4,5,7,      4,7,6,          // Face right
        8,9,11,     8,11,10,        // Face back
        12,13,15,   12,15,14,       // Face left
        16,17,19,   16,19,18,
        20,21,23,   20,23,22,
    };


    // generate vertex buffer objects, vertex array objects, element buffer objects
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    


    // generate a texture
    unsigned int alltexture = loadTexture("resources/textures/ccsprites1.png");
    ourShader.use();
    ourShader.setInt("alltexture", 3);
    /*glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, alltexture);*/

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

    glEnable(GL_CULL_FACE);

    // generating the initial plain of grass
    for (float j = -20.0; j < 20.0; j += 1)
    {
        for (float i = -20.0; i < 20.0; i += 1)
        {
            blocks.emplace_back(glm::vec3(i, -1, j), grass, true, &blocks);
            
        }
    }
    for (float j = -5.0; j < 5.0; j += 1)
    {
        for (float i = -5.0; i < 5.0; i += 1)
        {
            blocks.emplace_back(glm::vec3(i, 0, j), grass, true, &blocks);
        }
    }

    /*glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[10000];
    int blockIndex = 0;
    for (Block b : blocks) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, b.position);
        modelMatrices[blockIndex] = model;
        blockIndex++;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);*/




    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleGravity();
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, alltexture);

        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        int index = 0;

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, alltexture);   //use texture of ith face
        ourShader.use();

        index = 0;
        
        for (Block b : blocks)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, b.position);
            ourShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(index * sizeof(GLuint)));
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete resources after use
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Jump();
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        camera.UnlockJump();
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.Sprint();
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.Desprint();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        camera.DestroyBlock();
    }
    else if (button = GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        camera.PlaceBlock();
    }

    /*if (button = GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        camera.UnlockDestroy();
    }
    if (button = GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        camera.UnlockPlace();
    }*/
}

void handleGravity() {
    glm::vec3 pos = camera.Position;
    float x = floor(pos.x);
    float z = floor(pos.z);
    float highest = -INFINITY;
    for (Block b : blocks) {
        if (b.position.x == x && b.position.z == z) {
            highest = std::fmax(highest, b.position.y);
        }
    }
    camera.Position.y = std::fmax(highest + 2.9f, camera.Position.y + camera.YVelocity * deltaTime);
    if (camera.Position.y < camera.killPlane) {
        camera.Position = glm::vec3(0, 2.9, 0);
    }
    if (camera.Position.y == highest + 2.9f) {
        camera.YVelocity = 0;
        camera.Gravity = -9.81f;
        camera.grounded = true;
    }
    else {
        camera.YVelocity += deltaTime * camera.Gravity;
        camera.YVelocity = fmax(camera.TerminalVelocity, camera.YVelocity);
    }
}



