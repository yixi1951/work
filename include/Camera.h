#ifndef CAMERA_H
#define CAMERA_H
#include<glad/glad.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
using namespace glm;
struct Camera {
    // 位置、方向、上方向、右方向、世界上方向
    vec3 position, front, up, right, worldUp;
    // 偏航角、俯仰角、速度、灵敏度、视野
    float yaw, pitch, speed, sensitivity, fov;

    Camera(vec3 position = vec3(0.0f, 0.0f, 1.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) : front(vec3(0.0f, 0.0f, -1.0f)), speed(0.2f), sensitivity(0.2f), fov(45.0f)
    {   // 根据位置和世界上方向以及偏航角和俯仰角初始化摄像机
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors(true);
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(vec3(0.0f, 0.0f, -1.0f)), speed(0.2f), sensitivity(0.2f), fov(45.0f)
    {
        // 根据位置和世界上方向以及偏航角和俯仰角初始化摄像机
        this->position = vec3(posX, posY, posZ);
        this->worldUp = vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors(true);
    }
    Camera(vec3 position, vec3 target, vec3 up) : front(normalize(target - position)), speed(0.2f), sensitivity(0.2f), fov(45.0f)
    {
        // 根据相机坐标和注视点坐标以及世界上方向初始化摄像机
        this->position = position;
        this->worldUp = up;
        updateCameraVectors(false);
    }
    void updateCameraVectors(bool updateYawPitch = true)
    {
        if (updateYawPitch) {
            vec3 front;
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            this->front = normalize(front);
            this->right = normalize(cross(this->front, this->worldUp));
            this->up = normalize(cross(this->right, this->front));
            return;
        }
        // 根据 front、right 和 up 更新 yaw 和 pitch
        yaw = degrees(atan2(front.z, front.x));
        pitch = degrees(asin(front.y));

    }
    mat4 getViewMatrix()
    {   // 返回观察矩阵
        return lookAt(position, position + front, up);
    }
    mat4 getProjectionMatrix(float width, float height)
    {   // 返回投影矩阵
        return perspective(radians(fov), width / height, 0.1f, 100.0f);
    }
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods, int minY)
    {
        if (key == GLFW_KEY_W) {
            position += front * speed;
        }
        if (key == GLFW_KEY_S) {
            position -= front * speed;
        }
        if (key == GLFW_KEY_A) {
            position -= right * speed;
        }
        if (key == GLFW_KEY_D) {
            position += right * speed;
        }
        if (position.y < minY) {
            position.y = minY;
        }
    }
    void mouse_callback(GLFWwindow *window, double xpos, double ypos, int width = 800, int height = 600, bool windowChange = false)
    {
        static double lastX = width / 2, lastY = height / 2;
        if (windowChange) {
            lastX = width / 2;
            lastY = height / 2;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        yaw += xoffset;
        pitch += yoffset;
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
        updateCameraVectors();
    }
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        if (fov >= 1.0f && fov <= 45.0f) {
            fov -= yoffset;
        }
        if (fov <= 1.0f) {
            fov = 1.0f;
        }
        if (fov >= 45.0f) {
            fov = 45.0f;
        }
    }
};
#endif