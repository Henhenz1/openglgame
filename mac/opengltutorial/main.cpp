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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // use glad to load opengl pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // build and compile shaders shader
    Shader ourShader("/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/3d.vs", "/Users/georgewu/openglgame/mac/opengltutorial/resources/shaders/3d.fs");
    
    // create vertices of triangle
    float vertices[] = {
        // positions         // texture coords
         -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.5f,  0.0f, 1.0f,
         -0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 1.0f,

         0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 0.0f,
         0.5f, 0.5f, -0.5f,  1.0f, 1.0f,

         0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,0.0f, 1.0f,
         0.5f, 0.5f, -0.5f,  1.0f, 0.0f,
         -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,

         -0.5f, -0.5f, -0.5f,0.0f, 0.0f,
         -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
         -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
         -0.5f, 0.5f, 0.5f,  1.0f, 1.0f,

         -0.5f, -0.5f, -0.5f,0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f,  1.0f, 1.0f,

         -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
         0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
         -0.5f, 0.5f, -0.5f,1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    
    // generate a texture
    unsigned int texture_top, texture_side, texture_bottom;
    glGenTextures(1, &texture_top);
    glBindTexture(GL_TEXTURE_2D, texture_top);
    // set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate textures
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    
    // top of cube
    unsigned char *data = stbi_load("/Users/georgewu/openglgame/mac/opengltutorial/resources/textures/grass/grass_top.png", &width, &height, &nrChannels, 3);
    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    // sides of cube
    glGenTextures(1, &texture_side);
    glBindTexture(GL_TEXTURE_2D, texture_side);
    
    data = stbi_load("/Users/georgewu/openglgame/mac/opengltutorial/resources/textures/grass/grass_side.png", &width, &height, &nrChannels, 3);
    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    // bottom of cube
    glGenTextures(1, &texture_bottom);
    glBindTexture(GL_TEXTURE_2D, texture_bottom);
    
    data = stbi_load("/Users/georgewu/openglgame/mac/opengltutorial/resources/textures/grass/grass_bottom.png", &width, &height, &nrChannels, 3);
    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    ourShader.use();
    
    // set uniforms
    ourShader.setInt("texture_top", 0);
    ourShader.setInt("texture_side", 1);
    ourShader.setInt("texture_bottom", 5);
    
    unsigned int texture[] = {
        texture_side,
        texture_side,
        texture_side,
        texture_side,
        texture_bottom,
        texture_top
    };
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);
    
    while(!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // to undo wireframe mode
        
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        
        int index;
        
        glBindVertexArray(VAO);
        for(float j = 0.0; j < 5.0; j+=0.5)
        {
            for(float i = 0.0; i < 5.0; i+=0.5)
            {
              glm::mat4 model = glm::mat4(1.0f);
              model = glm::translate(model, glm::vec3( i, -1, j));
              //float angle = 20.0f * i;
              //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
              ourShader.setMat4("model", model);

              for(int k=0; k<6; ++k)
              {
                  glBindTexture(GL_TEXTURE_2D, texture[k]);   //use texture of ith face
                  ourShader.use();
                  
                  index = 6*k;                //select ith face

                  //draw 2 triangles making up this face
                  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index * sizeof(GLuint)));
              }
            }
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

