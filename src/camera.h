#pragma once
#include "config.h"

namespace fivelabsengine
{
    enum class CameraMovement
    {
        Forward,
        Backward,
        Left,
        Right
    };

    class Camera
    {
    public:
        Camera(glm::vec3 target, glm::vec3 up);
        ~Camera();

        const float cameraSpeed = 0.5f;
        glm::mat4 getProjection() { return projection; };
        glm::vec3 getPosition() const { return cameraPos; }
        glm::mat4 getView() { return view; };

        void move(CameraMovement direction, float dt);
        void onMouseMove(double xpos, double ypos);
        void setAspect(float newAspect);
        void resetMouse();

    private:
        void updateView();

        bool firstMouse{true};
        float yaw{-90.0f}; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        float pitch = 0.0f;
        float lastX = 800.0f / 2.0;
        float lastY = 600.0 / 2.0;
        float fov = 45.0f;
        float aspect{(float)WIDTH / (float)HEIGHT};
        glm::vec3 cameraPos{glm::vec3(0.0f, 0.0f, 3.0f)};
        glm::vec3 cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection;
        glm::vec3 up;
        glm::vec3 cameraRight;
        glm::vec3 cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};
        glm::mat4 view;
        glm::mat4 projection;
    };

}