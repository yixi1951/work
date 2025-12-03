#ifndef SKYCUBE_H
#define SKYCUBE_H
#include<map>
using std::map;
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
using namespace glm;

struct skyCube {
    // 立方体的 8 个顶点坐标
    vector<vec3> points = {
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(-1, 1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        glm::vec3(-1, 1, 1),
        glm::vec3(1, 1, 1)
    };
    // 立方体的 6 个面
    vector<int> index = {
        0, 2, 3, 3, 1, 0,
        4, 5, 7, 7, 6, 4,
        0, 1, 5, 5, 4, 0,
        1, 3, 7, 7, 5, 1,
        3, 2, 6, 6, 7, 3,
        2, 0, 4, 4, 6, 2
    };
    vector<string> texturePath = {"assets/skybox/right.jpg", "assets/skybox/left.jpg", "assets/skybox/top.jpg", "assets/skybox/bottom.jpg", "assets/skybox/back.jpg", "assets/skybox/front.jpg", };
    unsigned int textureID;
};
#endif