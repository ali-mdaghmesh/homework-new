#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;   // موقع الكاميرا
    glm::vec3 Front;      // اتجاه النظر
    glm::vec3 Up;         // الاتجاه العلوي
    glm::vec3 Right;      // الاتجاه الجانبي
    glm::vec3 WorldUp;    // اتجاه الأعلى العالمي

    float Yaw;            // زاوية Yaw (يمين/يسار)
    float Pitch;          // زاوية Pitch (أعلى/أسفل)
    float Speed;          // سرعة الحركة
    float Sensitivity;    // حساسية الماوس

    // ---------------- CONSTRUCTOR ----------------
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f,
        float speed = 2.5f,
        float sensitivity = 0.1f);

    // ---------------- VIEW MATRIX ----------------
    glm::mat4 GetViewMatrix();

    // ---------------- INPUTS ----------------
    void ProcessKeyboard(GLFWwindow* window, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
    void UpdateCameraVectors(); // لتحديث Front, Right, Up بعد تغير الزوايا
};
