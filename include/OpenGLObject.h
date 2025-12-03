#ifndef OPENGLOBJECT_H
#define OPENGLOBJECT_H
#include "Models.h"
#include "Shader.h"
#include "Robot.h"
#include<glad/glad.h>
#include<map>
#include "skyCube.h"
using std::map;
// 定义一个类，用于管理 OpenGL 对象
struct OpenGLObject {
    unsigned vao;
    map<string, unsigned> vbos;
    map<string, unsigned> ebos;
    map<string, unsigned> pointers;
    Shader &shader;
    OpenGLObject(Shader &);
    unsigned getVbo(const string &name);
    unsigned getEbo(const string &name);
    unsigned getAttribPointer(const string &name);
    void bindVboData(const string &name, const string &pointerName, const vector<vec3> &data);
    void bindVboData(const string &name, const string &pointerName, const vector<vec2> &data);
    void bindEboData(const string &name, const vector<int> &data);
    void enable(bool enable);
    ~OpenGLObject()
    {
        glDeleteVertexArrays(1, &vao);
        for (auto &vbo : vbos) {
            glDeleteBuffers(1, &vbo.second);
        }
        for (auto &ebo : ebos) {
            glDeleteBuffers(1, &ebo.second);
        }
    }

};
void loadPlatform(OpenGLObject *&objectPtr2, Platform *&platformPtr, Shader *&shaderPtr, float minY, string texturePath = "");
void loadModel(OpenGLObject *&objectPtr, Models *&modelsPtr, const string &name, Shader *&shaderPtr);
mat4 getShadowMatrix(const Light &light, Platform &platform, const mat4 &model);

void loadRobot(OpenGLObject *&objectPtr, Robot *&robotPtr, Shader *&shaderPtr);

void loadSkyCube(OpenGLObject *&objectPtr, skyCube *&skyCubePtr, Shader *&shaderPtr);

void loadRobotTexture(Robot *&robotPtr, Shader *&shaderPtr, string texturePath);

void loadRobotPartTexture(Robot *&robotPtr, Shader *&shaderPtr, string partName, string texturePath);

void loadObjModel(OpenGLObject *&objectPtr, Models *&modelsPtr, const string &dataPath, const string &imgPath, Shader *&shaderPtr);

#endif