#include "CameraManager.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}
Camera::Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), NearPlane(NEAR_PLANE)
{
    Position = glm::vec3(PosX, PosY, PosZ);
    WorldUp = glm::vec3(UpX, UpY, UpZ);
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMovement(Camera_Movement WishDir, float deltaTime)
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

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean ConstrainPitch)
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

void Camera::ProcessMouseScroll(float yOffset)
{
    Zoom -= (float)yOffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::UpdateCameraVectors()
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