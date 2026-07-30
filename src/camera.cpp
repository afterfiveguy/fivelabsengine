#include "camera.h"
namespace fivelabsengine
{
    Camera::Camera(glm::vec3 target, glm::vec3 up) : cameraTarget(target), up(up)
    {
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
        updateView();
    }

    void Camera::updateView()
    {
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    Camera::~Camera()
    {
    }

    void Camera::setAspect(float newAspect)
    {
        aspect = newAspect;
        projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    }

    void Camera::resetMouse() { firstMouse = true; }

    void Camera::move(CameraMovement direction, float dt)
    {
        float velocity = cameraSpeed * dt;
        if (direction == CameraMovement::Forward)
            cameraPos += cameraFront * velocity;
        if (direction == CameraMovement::Backward)
            cameraPos -= cameraFront * velocity;
        if (direction == CameraMovement::Left)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
        if (direction == CameraMovement::Right)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;

        updateView();
    }

    void Camera::onMouseMove(double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

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

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        updateView();
    }
}