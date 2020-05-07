#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <math.h>
#include "block/Block.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 4.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // Player attributes
    float PlayerHeight;
    float YVelocity;
    float Gravity = -9.81f;
    float TerminalVelocity = -66;
    float killPlane = -10;
    bool sprinting;
    bool grounded = true;
    bool bHopProt = false;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime, std::vector<Block> blocks) {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 prevPos = Position;
        float sprintFactor = 1;
        glm::vec3 mFront = glm::normalize(glm::vec3(Front.x, 0, Front.z));
        glm::vec3 mRight = glm::normalize(glm::vec3(Right.x, 0, Right.z));
        if (sprinting)
            sprintFactor = 2;
        if (direction == FORWARD)
            Position += sprintFactor * mFront * velocity;
        if (direction == BACKWARD)
            Position -= mFront * velocity;
        if (direction == LEFT)
            Position -= mRight * velocity;
        if (direction == RIGHT)
            Position += mRight * velocity;

        for (Block b : blocks) {
            if (inRange(0, 2, Position.y - b.position.y)) {
                if (Position.x < b.position.x + 1.3 && Position.x > b.position.x - 0.3) {
                    if (Position.z < b.position.z + 1.3 && prevPos.z >= b.position.z + 1.3) {
                        Position.z = b.position.z + 1.3;
                    }
                    else if (Position.z > b.position.z - 0.3 && prevPos.z <= b.position.z - 0.3) {
                        Position.z = b.position.z - 0.3;
                    }
                }
                if (Position.z < b.position.z + 1.3 && Position.z > b.position.z - 0.3) {
                    if (Position.x < b.position.x + 1.3 && prevPos.x >= b.position.x + 1.3) {
                        Position.x = b.position.x + 1.3;
                    }
                    else if (Position.x > b.position.x - 0.3 && prevPos.x <= b.position.x - 0.3) {
                        Position.x = b.position.x - 0.3;
                    }
                }
            }
        }
        // walking diagonally faster. how to fix?
    }

    bool inRange(float low, float high, float x) {
        return (x - high) * (x - low) <= 0;
    }

    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        Pitch = std::fmin(89.0f, std::fmax(-89.0f, Pitch));
        updateCameraVectors();
    }
    void Sprint() {
        if (!sprinting) {
            sprinting = true;
        }
     }
    void Desprint() {
        if (sprinting) {
            sprinting = false;
        }
    }
    void Jump() {
        if (grounded && !bHopProt) {
            grounded = false;
            bHopProt = true;
            YVelocity += 4.4;
        }
    }
    void UnlockJump() {
        bHopProt = false;
    }

private:
    // calculates Front vector from Camera's Euler angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif
