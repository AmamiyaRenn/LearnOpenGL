/***
 * @Description: 摄像头类
 * @Author: Amamiya
 * @Date: 2022-04-24 21:45:57
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
#ifndef _CAMERA_H
#define _CAMERA_H

// Standard Headers
#include <vector>

// third party headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM        = 45.0f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 position; // 摄像头位置（世界坐标系）
    glm::vec3 front;    // 摄像头朝向（摄像头坐标系）
    glm::vec3 up;       // 摄像头上方向（摄像头坐标系）
    glm::vec3 right;    // 摄像头右方向（摄像头坐标系）
    glm::vec3 world_up; // 摄像头上方向（世界坐标系）
    // Euler Angles
    float yaw;
    float pitch;
    // camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    // constructor with vectors
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
                    float     yaw      = YAW,
                    float     pitch    = PITCH) :
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = position;
        this->world_up = up;
        this->yaw      = yaw;
        this->pitch    = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
        front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->world_up = glm::vec3(upX, upY, upZ);
        this->yaw      = yaw;
        this->pitch    = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + front, up); }
    void      processKeyboard(CameraMovement direction, float deltaTime);
    void      processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void      processMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined
// ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movement_speed * deltaTime;
    if (direction == CameraMovement::FORWARD)
        position += front * velocity;
    if (direction == CameraMovement::BACKWARD)
        position -= front * velocity;
    if (direction == CameraMovement::LEFT)
        position -= right * velocity;
    if (direction == CameraMovement::RIGHT)
        position += right * velocity;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset)
{
    zoom -= yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front       = glm::normalize(new_front);
    // also re-calculate the Right and Up vector
    right =
        glm::normalize(glm::cross(front, world_up)); // normalize the vectors, because their length gets closer to 0 the
                                                     // more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

#endif