//
//  main.cpp
//  opengltutorial
//
//  Created by George Wu on 4/26/20.
//  Copyright © 2020 George Wu. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "shader/shader_s.h"

#include "camera/Camera.h"

#include "block/Block.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;    // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;

// world
std::vector<Block> blocks;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
        camera.ProcessKeyboard(FORWARD, deltaTime, blocks);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, blocks);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, blocks);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, blocks);
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
    camera.Position.y = std::fmax(highest + 3, camera.Position.y + camera.YVelocity * deltaTime);
    if (camera.Position.y < camera.killPlane) {
        camera.Position = glm::vec3(0, 3, 0);
    }
    if (camera.Position.y == highest + 3) {
        camera.YVelocity = 0;
        camera.Gravity = -9.81f;
        camera.grounded = true;
    }
    else {
        camera.YVelocity += deltaTime * camera.Gravity;
        camera.YVelocity = fmax(camera.TerminalVelocity, camera.YVelocity);
    }
}

GLFWcursor* customCursor() {
    
    int width, height, nrChannels;
    
    unsigned char *cursor_img = stbi_load("/Users/georgewu/openglgame/mac/opengltutorial/resources/crosshair.png", &width, &height, &nrChannels, 0);
    
    
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = cursor_img;

    return glfwCreateCursor(&image, width/2, height/2);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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


int main()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
  
    
    // use glfw to create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Make Game World", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
//    // make crosshair cursor
//    GLFWcursor* cursor = customCursor();
//    glfwSetCursor(window, cursor);

    
    // use glad to load opengl pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // enable usage of alpha value for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    // build and compile shaders shader
    Shader ourShader("/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/3d_lighting.vs", "/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/3d_lighting.fs");
    
    Shader chShader("/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/ch_shader.vs", "/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/ch_shader.fs");
    
    // create vertices of cube
    float cubeVertices[] = {
        // positions         // texture coords // normals
         0.0f, 0.0f, 1.0f, 0.5f, 0.5f,    -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 1.0f,  1.0f, 0.5f,    -1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 1.0f,  0.5f, 0.0f,    -1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,

         1.0f, 0.0f, 1.0f,  0.5f, 0.5f,    0.0f, -1.0f, 0.0f,
         1.0f, 0.0f, 0.0f, 1.0f, 0.5f,    0.0f, -1.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   0.5f, 0.0f,    0.0f, -1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,  1.0f, 0.0f,    0.0f, -1.0f, 0.0f,

         1.0f, 0.0f, 0.0f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,1.0f, 0.5f,    1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,

         0.0f, 0.0f, 0.0f,0.5f, 0.5f,    0.0f, -1.0f, 0.0f,
         0.0f, 0.0f, 0.5f, 1.0f, 0.5f,    0.0f, -1.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.5f, 0.0f,    0.0f, -1.0f, 0.0f,
         0.0f, 1.0f, 1.0f,  1.0f, 0.0f,    0.0f, -1.0f, 0.0f,

         0.0f, 0.0f, 0.0f,0.0f, 0.5f,    0.0f, 0.0f, -1.0f,
         1.0f, 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, 1.0f, 0.5f, 0.5f,    0.0f, 0.0f, -1.0f,
         1.0f, 0.0f, 1.0f,  0.5f, 1.0f,    0.0f, 0.0f, -1.0f,

         0.0f, 1.0f, 1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 1.0f,  0.0f, 0.5f,     0.0f, 0.0f, 1.0f,
         0.0f, 1.0f, 0.0f,0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 0.0f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f
    };
    
    unsigned int cubeIndices[] = {
        //faces
        0,1,3,      0,3,2,          // Face front
        4,5,7,      4,7,6,          // Face right
        8,9,11,     8,11,10,        // Face back
        12,13,15,   12,15,14,       // Face left
        16,17,19,   16,19,18,
        20,21,23,   20,23,22,
    };
    
    float chVertices[] = {
        // positions
         -0.004f, -0.05f, 0.5f,
         0.004f, -0.05f, 0.5f,
         -0.004f, 0.05f, 0.5f,
         0.004f, 0.05f, 0.5f,
        -0.05f, -0.005f, 0.5f,
        0.05f, -0.005f, 0.5f,
        -0.05f, 0.005f, 0.5f,
        0.05f, 0.005f, 0.5f
    };
    
    unsigned int chIndices[] = {
        // lines
        0,1,3,      0,3,2,          // vertical crosshair
        4,5,7,      4,7,6           // horizontal crosshair
    };
    
    // generate vertex buffer objects, vertex array objects, element buffer objects
    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenBuffers(2, VBOs);
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, EBOs);
    
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    
    // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
                 
    
    glBindVertexArray(VAOs[1]);    // note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);    // and a different VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(chVertices), chVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chIndices), chIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray(0);

    unsigned int texture_all = loadTexture("/Users/georgewu/openglgame/mac/opengltutorial/resources/textures/grass_all.png");
    
    ourShader.use();
    
    // set uniforms
    ourShader.setInt("texture_all", 3);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);
    
    // generating the initial plain of grass
    for (float j = -20.0; j < 20.0; j += 1)
    {
        for (float i = -20.0; i < 20.0; i += 1)
        {
            blocks.emplace_back(glm::vec3(i, -1, j), grass, true);
        }
    }
    for (float j = -5.0; j < 5.0; j += 1)
    {
        for (float i = -5.0; i < 5.0; i += 1)
        {
            blocks.emplace_back(glm::vec3(i, 0, j), grass, true);
        }
    }
    
    
    glm::vec3 lightPos(0.0f, 7.0f, 0.0f);
    
    while(!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleGravity();
        processInput(window);
        
        ourShader.use();
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // to undo wireframe mode
        
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // set light uniforms
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("lightPos", lightPos);

        int index = 0;
        
        glBindVertexArray(VAOs[0]);
        glBindTexture(GL_TEXTURE_2D, texture_all);   //use texture of ith face
        ourShader.use();
        for (Block b : blocks)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, b.position);
            ourShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(index * sizeof(GLuint)));
        }
        
        glBindVertexArray(VAOs[1]);
        chShader.use();
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // delete resources after use
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    glDeleteBuffers(1, EBOs);
    
    glfwTerminate();
    return 0;
}




