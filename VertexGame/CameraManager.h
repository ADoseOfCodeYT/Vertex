
#ifndef CAMERAMANAGER
#define CAMERAMANAGER

#include <glad/glad.h>

#include "Math.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float NEAR_PLANE = 0.1f;


// The global camera class
class Camera
{
public:
    //Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera Variables
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float NearPlane;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), NearPlane(NEAR_PLANE)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        UpdateCameraVectors();
    }
    // constructor with scalar values
    Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), NearPlane(NEAR_PLANE)
    {
        Position = glm::vec3(PosX, PosY, PosZ);
        WorldUp = glm::vec3(UpX, UpY, UpZ);
        Yaw = yaw;
        Pitch = pitch;
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement WishDir, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (WishDir == FORWARD)
            Position += Front * velocity;
        if (WishDir == BACKWARD)
            Position -= Front * velocity;
        if (WishDir == LEFT)
            Position -= Right * velocity;
        if (WishDir == RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean ConstrainPitch = true)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        // clamp up/down movement
        if (ConstrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset)
    {
        Zoom -= (float)yOffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:

    void UpdateCameraVectors()
    {
        // calculate the front vector
        glm::vec3 frontVec;
        frontVec.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        frontVec.y = sin(glm::radians(Pitch));
        frontVec.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(frontVec);
        // also re-calculate the right and up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // then normalize that shit
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif