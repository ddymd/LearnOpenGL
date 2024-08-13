#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Front = glm::vec3(0.f, 0.f, -1.f);
    MovementSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    Zoom = ZOOM;

    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    updateCameraVectors();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    Front = glm::vec3(0.f, 0.f, -1.f);
    MovementSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    Zoom = ZOOM;

    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;

    updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(ECameraMovement direction, float deltaTime) {
    float v = MovementSpeed * deltaTime;
    switch (direction) {
    case ECameraMovement::BACKWARD:
        Position -= Front * v;
        break;
    case ECameraMovement::FORWARD:
        Position += Front * v;
        break;
    case ECameraMovement::LEFT:
        Position -= Right * v;
        break;
    case ECameraMovement::RIGHT:
        Position += Right * v;
        break;
    default:break;
    }
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch < -89.f) Pitch = -89.f;
        if (Pitch > 89.f) Pitch = 89.f;
    }
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 45.0f) Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    float pitch = glm::radians(Pitch);
    float yaw = glm::radians(Yaw);
    Front = glm::normalize(glm::vec3(cos(pitch)*cos(yaw), sin(pitch), cos(pitch)*sin(yaw)));
    // update right and up
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
