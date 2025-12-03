#ifndef ROBOT_H
#define ROBOT_H
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
struct Robot {
    float scale = 20;
    float minY = 0.0;
    map<string, vec2> size = {
        {"torso", vec2(13 / scale, 19 / scale)},
        {"head", vec2(13 / scale, 13 / scale)},
        {"upperArm", vec2(6 / scale, 19 / scale)},
        //{"lowerArm", vec2(0.5 / scale, 1.8 / scale)},
        {"upperLeg", vec2(6 / scale, 19 / scale)},
        //{"lowerLeg", vec2(0.5 / scale, 2.2 / scale)},
    };
    map<string, float> jointAngle = {
        {"torso", 180.},
        {"head", 0.0},
        //{"leftLowerLeg", 0.0},
        {"leftUpperLeg", 0.0},
        //{"rightLowerLeg", 0.0},
        {"rightUpperLeg", 0.0},
        //{"leftLowerArm", 0.0},
        {"leftUpperArm", 0.0},
        //{"rightLowerArm", 0.0},
        {"rightUpperArm", 0.0},
    };
    // 立方体的 8 个顶点坐标
    vector<vec3> points = {
        glm::vec3(-0.5, -0.5, -0.5),
        glm::vec3(0.5, -0.5, -0.5),
        glm::vec3(-0.5, 0.5, -0.5),
        glm::vec3(0.5, 0.5, -0.5),
        glm::vec3(-0.5, -0.5, 0.5),
        glm::vec3(0.5, -0.5, 0.5),
        glm::vec3(-0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5)
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
    // 立方体的 8 个顶点颜色
    vector<vec3> colors = vector<vec3>(8, vec3(1.0, 1.0, 1.0));
    // 立方体的 6 个面法向量
    vector<vec3> normals;
    map<string, mat4> modelsMap = {
        {"torso", mat4(1.0)},
        {"head", mat4(1.0)},
        {"leftUpperArm", mat4(1.0)},
        //{"leftLowerArm", mat4(1.0)},
        {"rightUpperArm", mat4(1.0)},
        //{"rightLowerArm", mat4(1.0)},
        {"leftUpperLeg", mat4(1.0)},
        //{"leftLowerLeg", mat4(1.0)},
        {"rightUpperLeg", mat4(1.0)},
        //{"rightLowerLeg", mat4(1.0)},
    };
    mat4 model = mat4(1.0);
    vector<mat4> modelStack;
    vec3 position = vec3(0.0, 0.0, 0.0);
    map<string, unsigned int> textureID = {
        {"torso", 0},
        {"head", 0},
        {"leftUpperArm", 0},
        //{"leftLowerArm", 0},
        {"rightUpperArm", 0},
        //{"rightLowerArm", 0},
        {"leftUpperLeg", 0},
        //{"leftLowerLeg", 0},
        {"rightUpperLeg", 0},
        //{"rightLowerLeg", 0},
    };
    Robot()
    {
        minY = -size["upperLeg"].y;
        getModel();
        getNormals();
    }
    mat4 getPartModel(string partName)
    {
        mat4 resize(1.0);
        float y;
        if (partName == "head" || partName == "torso")
            y = size[partName].y / 2;   // 对于头或身体，将缩放后部件的下端点放在原点
        else
            y = -size[partName].y / 2; // 对于其他部件，将缩放后部件的上端点放在原点

        // 调整缩放后部件的位置
        resize = glm::translate(resize, vec3(0.0, y, 0.0));
        // 缩放部件
        resize = glm::scale(resize, vec3(size[partName].x, size[partName].y, size[partName].x));
        // 保存模型矩阵
        return model * resize;

    }
    void getModel()
    {
        model = glm::translate(mat4(1.0), vec3(0.0, -minY, 0.0));
        // 身体
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(jointAngle["torso"]), vec3(0.0, 1.0, 0.0));
        modelsMap["torso"] = getPartModel("torso");
        modelStack.push_back(model);
        // 头
        model = glm::translate(model, vec3(0., size["torso"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["head"]), vec3(0.0, 1.0, 0.0));
        modelsMap["head"] = getPartModel("head");

        model = modelStack.back();

        // 左臂
        // 大臂
        model = glm::translate(model, vec3(-(size["torso"].x / 2) - size["upperArm"].x / 2, size["torso"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["leftUpperArm"]), vec3(1., 0.0, 0.0));
        modelsMap["leftUpperArm"] = getPartModel("upperArm");
        // 小臂
/*         model = glm::translate(model, vec3(0., -(size["upperArm"].y), 0.));
        model = glm::rotate(model, glm::radians(jointAngle["leftLowerArm"]), vec3(1., 0.0, 0.0));
        modelsMap["leftLowerArm"] = getPartModel("lowerArm"); */

        model = modelStack.back();

        // 右臂
        // 大臂
        model = glm::translate(model, vec3(size["torso"].x / 2 + size["upperArm"].x / 2, size["torso"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["rightUpperArm"]), vec3(1., 0.0, 0.0));
        modelsMap["rightUpperArm"] = getPartModel("upperArm");
        // 小臂
/*         model = glm::translate(model, vec3(0., -size["upperArm"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["rightLowerArm"]), vec3(1., 0.0, 0.0));
        modelsMap["rightLowerArm"] = getPartModel("lowerArm"); */

        model = modelStack.back();

        // 左腿
        // 大腿
        model = glm::translate(model, vec3(-(size["torso"].x) * 0.35, 0., 0.));
        model = glm::rotate(model, glm::radians(jointAngle["leftUpperLeg"]), vec3(1., 0.0, 0.0));
        modelsMap["leftUpperLeg"] = getPartModel("upperLeg");
        // 小腿
/*         model = glm::translate(model, vec3(0., -size["upperLeg"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["leftLowerLeg"]), vec3(1., 0.0, 0.0));
        modelsMap["leftLowerLeg"] = getPartModel("lowerLeg"); */

        model = modelStack.back();

        // 右腿
        // 大腿
        model = glm::translate(model, vec3(size["torso"].x * 0.35, 0., 0.));
        model = glm::rotate(model, glm::radians(jointAngle["rightUpperLeg"]), vec3(1., 0.0, 0.0));
        modelsMap["rightUpperLeg"] = getPartModel("upperLeg");
        // 小腿
/*         model = glm::translate(model, vec3(0., -size["upperLeg"].y, 0.));
        model = glm::rotate(model, glm::radians(jointAngle["rightLowerLeg"]), vec3(1., 0.0, 0.0));
        modelsMap["rightLowerLeg"] = getPartModel("lowerLeg"); */

        model = modelStack.back();
    }

    // 计算法向量
    void getNormals()
    {
        vector<vec3> faceNorm;
        for (int i = 0; i < index.size(); i += 3) {
            vec3 v1 = points[index[i + 1]] - points[index[i]];
            vec3 v2 = points[index[i + 2]] - points[index[i]];
            faceNorm.push_back(normalize(cross(v1, v2)));
        }
        normals.assign(points.size(), vec3(0.f));
        for (int i = 0; i < index.size(); i += 3) {
            normals[index[i]] += faceNorm[i / 3];
            normals[index[i + 1]] += faceNorm[i / 3];
            normals[index[i + 2]] += faceNorm[i / 3];
        }
        for (int i = 0; i < normals.size(); i++) {
            normals[i] = normalize(normals[i]);
        }
    }

    // 键盘回调函数
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        // 按下 A D 键切换方向
        // 当按下 W 键时，转动大腿，并向前移动
        static float angle = 5;
        if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
            if (key == GLFW_KEY_W) {
                // 转动身体到前方
                jointAngle["torso"] = 180;
            }
            else if (key == GLFW_KEY_S) {
                // 转动身体到后方
                jointAngle["torso"] = 0;
            }
            else if (key == GLFW_KEY_A) {
                // 转动身体到左方
                jointAngle["torso"] = 90;
            }
            else if (key == GLFW_KEY_D) {
                // 转动身体到右方
                jointAngle["torso"] = -90;
            }

            // 如果大腿的角度大于 45 度，那么切换转动方向
            if (jointAngle["leftUpperLeg"] >= 30 || jointAngle["leftUpperLeg"] <= -30) {
                angle *= -1;
            }
            jointAngle["leftUpperLeg"] += angle;
            jointAngle["rightUpperLeg"] -= angle;
            jointAngle["leftUpperArm"] -= angle;
            jointAngle["rightUpperArm"] += angle;
            // 移动机器人
            if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
                position.z += (key == GLFW_KEY_W ? 0.1 : -0.1);
            }
            if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
                position.x += (key == GLFW_KEY_A ? -0.1 : 0.1);
            }
        }
        // 当释放 W 键时，转动大腿，回到原来的位置
        if ((key == GLFW_KEY_S || key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_D) && action == GLFW_RELEASE) {
            jointAngle["leftUpperLeg"] = 0;
            jointAngle["rightUpperLeg"] = 0;
            jointAngle["leftUpperArm"] = 0;
            jointAngle["rightUpperArm"] = 0;
        }

    }

};

#endif