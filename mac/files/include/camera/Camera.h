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
    // Block selection attributes
    float maxSelectDist = 6;
    int CurrentBlockIndex;
    glm::vec3 NextBlock;
    std::vector<Block>* Blocks;
    bool blockFound = false;
    bool leftClicked = false;
    bool rightClicked = false;

    Camera(std::vector<Block>* blocks, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Blocks = blocks;
        updateCameraVectors();
    }

    Camera(std::vector<Block>* blocks, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Blocks = blocks;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
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

        for (Block b : *Blocks) {
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
        //updateLook();
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

    void PlaceBlock() {
        updateLook();
        if (NextBlock != (glm::vec3)NULL) {
            for (int i = 0; i < (*Blocks).size(); i++) {
                Block b = (*Blocks)[i];
                if (b.position == NextBlock) {
                    return;
                }
            }
            (*Blocks).emplace_back(NextBlock, grass, true);
            std::cout << "Block placed at (" << NextBlock.x << ", " << NextBlock.y << ", " << NextBlock.z << ")" << std::endl;
        }
        updateLook();
    }

    void DestroyBlock() {
        updateLook();
        if (CurrentBlockIndex != Blocks->size() && CurrentBlockIndex != -1) {
            std::cout << "Block removed at (" << (*Blocks)[CurrentBlockIndex].position.x << ", " << (*Blocks)[CurrentBlockIndex].position.y << ", " << (*Blocks)[CurrentBlockIndex].position.z << ")" << std::endl;
            (*Blocks).erase((*Blocks).begin() + CurrentBlockIndex);
        }
        //Blocks->erase();
        updateLook();
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

    void updateLook() {    // with inspiration from https://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game?rq=1
        glm::vec3 rayEnd = Position + maxSelectDist * Front;
        glm::vec3 curr = Position;
        glm::vec3 candidate;
        float t = 0;
        float currT;
        float eps = 0.00001;
        blockFound = false;
        CurrentBlockIndex = -1;
        NextBlock = (glm::vec3)NULL;
        std::vector<Block>& vecRef = *Blocks;
        while (blockFound == false && t < maxSelectDist) {
            // position; self-explanatory
            float x = Position.x;
            float y = Position.y;
            float z = Position.z;

            // represents direction of front-facing vector; 0, 1, or -1
            int dirX = signum(Front.x);
            int dirY = signum(Front.y);
            int dirZ = signum(Front.z);

            // represents distance along view direction on each axis until next unit; always nonnegative
            float dx = (dirX > 0) ? ceil(x) - x : (dirX < 0) ? x - floor(x) : INFINITY;
            float dy = (dirY > 0) ? ceil(y) - y : (dirY < 0) ? y - floor(y) : INFINITY;
            float dz = (dirZ > 0) ? ceil(z) - z : (dirZ < 0) ? z - floor(z) : INFINITY;

            // how many times the Front vector can be added in each direction before reaching next unit; always nonnegative
            float tX = dx / abs(Front.x);
            float tY = dy / abs(Front.y);
            float tZ = dy / abs(Front.z);

            currT = fmin(tX, fmin(tY, tZ)) + eps;
            if (currT < 2 * eps) {
                break;
            }

            glm::vec3 axis;  // 1 2 3 for x y z; positive or negative depending on which side of the block the ray enters from (opposite of ray direction)
            if (tX <= tY && tX <= tZ) {
                if (dirX > 0) {
                    axis = glm::vec3(-1, 0, 0);
                }
                else {
                    axis = glm::vec3(1, 0, 0);
                }
            }
            else if (tY <= tX && tY <= tZ) {
                if (dirY > 0) {
                    axis = glm::vec3(0, -1, 0);
                }
                else {
                    axis = glm::vec3(0, 1, 0);
                }
            }
            else if (tZ <= tY && tZ <= tX) {
                if (dirX > 0) {
                    axis = glm::vec3(0, 0, -1);
                }
                else {
                    axis = glm::vec3(0, 0, 1);
                }
            }

            t += currT;
            //std::cout << "t = " << t << std::endl;
            curr += currT * Front;
            candidate = glm::vec3(int(floor(curr.x)), int(floor(curr.y)), int(floor(curr.z)));

            for (int i = 0; i < vecRef.size(); i++) {
                Block b = vecRef[i];
                if (b.position == candidate) {
                    CurrentBlockIndex = i;
                    blockFound = true;
                    /*std::cout << "dx, dy, dz= " << dx << ", " << dy << ", " << dz << std::endl;
                    std::cout << "tX, tY, tZ = " << tX << ", " << tY << ", " << tZ << std::endl;
                    std::cout << candidate.x << ", " << candidate.y << ", " << candidate.z << std::endl;*/
                    NextBlock = candidate + axis;
                    break;
                }
            }

            

        }
    }

    int signum(float x) {
        return x > 0 ? 1 : x < 0 ? -1 : 0;
    }

};

#endif
