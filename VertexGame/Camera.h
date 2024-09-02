
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
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // constructor with scalar values
    Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float yaw, float pitch);

    glm::mat4 GetViewMatrix();

    void ProcessMovement(Camera_Movement WishDir, float deltaTime);

    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean ConstrainPitch = true);

    void ProcessMouseScroll(float yOffset);

private:

    void UpdateCameraVectors();
};
#endif