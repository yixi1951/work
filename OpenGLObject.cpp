#include "Models.h"
#include "Shader.h"
#include<glad/glad.h>
#include<map>
#include<OpenGLObject.h>
#include "Camera.h"
#include "Light.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Robot.h"
#include "skyCube.h"
using std::map;

OpenGLObject::OpenGLObject(Shader &_shader) : shader(_shader)
{
    glGenVertexArrays(1, &vao);
}
unsigned OpenGLObject::getVbo(const string &name)
{
    if (vbos.find(name) == vbos.end()) {
        unsigned vbo;
        glGenBuffers(1, &vbo);
        vbos[name] = vbo;
    }
    return vbos[name];
}
unsigned OpenGLObject::getEbo(const string &name)
{
    if (ebos.find(name) == ebos.end()) {
        unsigned ebo;
        glGenBuffers(1, &ebo);
        ebos[name] = ebo;
    }
    return ebos[name];
}
unsigned OpenGLObject::getAttribPointer(const string &name)
{
    if (pointers.find(name) == pointers.end()) {
        unsigned pointer = glGetAttribLocation(shader.ID, name.c_str());
        pointers[name] = pointer;
    }
    return pointers[name];
}
void OpenGLObject::bindVboData(const string &name, const string &pointerName, const vector<vec3> &data)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, getVbo(name));
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(vec3), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(getAttribPointer(pointerName), 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(getAttribPointer(pointerName));
    glBindVertexArray(0);
}
void OpenGLObject::bindVboData(const string &name, const string &pointerName, const vector<vec2> &data)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, getVbo(name));
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(vec2), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(getAttribPointer(pointerName), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(getAttribPointer(pointerName));
    glBindVertexArray(0);
}
void OpenGLObject::bindEboData(const string &name, const vector<int> &data)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getEbo(name));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), &data[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void OpenGLObject::enable(bool enable)
{
    if (enable) {
        glBindVertexArray(vao);
        shader.use();
        return;
    }
    glBindVertexArray(0);
}
void loadModel(OpenGLObject *&objectPtr, Models *&modelsPtr, const string &name, Shader *&shaderPtr)
{
    if (objectPtr) {
        delete objectPtr;
        objectPtr = nullptr;
    }
    if (modelsPtr) {
        delete modelsPtr;
        modelsPtr = nullptr;
    }
    modelsPtr = new Models("assets/" + name + ".off");
    objectPtr = new OpenGLObject(*shaderPtr);
    objectPtr->bindVboData("points", "vPosition", modelsPtr->points);
    objectPtr->bindVboData("colors", "vColor", modelsPtr->colors);
    objectPtr->bindVboData("normals", "vNormal", modelsPtr->normals);
    objectPtr->bindEboData("index", modelsPtr->index);
}

void loadRobot(OpenGLObject *&objectPtr, Robot *&robotPtr, Shader *&shaderPtr)
{
    if (objectPtr) {
        delete objectPtr;
        objectPtr = nullptr;
    }
    if (robotPtr) {
        delete robotPtr;
        robotPtr = nullptr;
    }
    robotPtr = new Robot();
    objectPtr = new OpenGLObject(*shaderPtr);
    objectPtr->bindVboData("points", "vPosition", robotPtr->points);
    //objectPtr->bindVboData("colors", "vColor", robotPtr->colors);
    objectPtr->bindVboData("normals", "vNormal", robotPtr->normals);
    objectPtr->bindEboData("index", robotPtr->index);
}

void loadPlatform(OpenGLObject *&objectPtr2, Platform *&platformPtr, Shader *&shaderPtr, float minY, string texturePath)
{
    if (objectPtr2) {
        delete objectPtr2;
        objectPtr2 = nullptr;
    }
    if (platformPtr) {
        delete platformPtr;
        platformPtr = nullptr;
    }
    platformPtr = new Platform(minY, vec3(0.8f, 0.8f, 0.8f), 50., 50., texturePath != "");
    objectPtr2 = new OpenGLObject(*shaderPtr);
    objectPtr2->bindVboData("platform_points", "vPosition", platformPtr->points);
    objectPtr2->bindVboData("platform_colors", "vColor", platformPtr->colors);
    objectPtr2->bindVboData("platform_normals", "vNormal", platformPtr->normals);
    objectPtr2->bindEboData("platform_index", platformPtr->index);
    // 创建纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (texturePath != "") {
        int width = 0, height = 0, nrChannels = 0;
        unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &(platformPtr->textureID));
        glBindTexture(GL_TEXTURE_2D, platformPtr->textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        objectPtr2->bindVboData("textureCoords", "vTextureCoords", platformPtr->textureCoords);
        shaderPtr->setUniform("textureVar", 0);

    }
}
void loadSkyCube(OpenGLObject *&objectPtr, skyCube *&skyCubePtr, Shader *&shaderPtr)
{
    if (objectPtr) {
        delete objectPtr;
        objectPtr = nullptr;
    }
    if (skyCubePtr) {
        delete skyCubePtr;
        skyCubePtr = nullptr;
    }
    skyCubePtr = new skyCube();
    objectPtr = new OpenGLObject(*shaderPtr);
    objectPtr->bindVboData("points", "vPosition", skyCubePtr->points);
    objectPtr->bindEboData("index", skyCubePtr->index);
    // 创建纹理
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data;
    glGenTextures(1, &(skyCubePtr->textureID));
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubePtr->textureID);
    for (int i = 0; i < 6; i++) {
        data = stbi_load(skyCubePtr->texturePath[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture" << std::endl;
            return;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_image_free(data);
    shaderPtr->setUniform("textureVar", 0);

}
void loadRobotTexture(Robot *&robotPtr, Shader *&shaderPtr, string texturePath)
{
    // 为每一个部件加载纹理

    for (auto &part : robotPtr->textureID) {
        string path = texturePath;
        if (part.first == "torso") path += "body";
        else if (part.first == "head") path += "head";
        else if (part.first == "leftUpperArm" || part.first == "rightUpperArm") path += "arm";
        else if (part.first == "leftUpperLeg" || part.first == "rightUpperLeg") path += "leg";

        loadRobotPartTexture(robotPtr, shaderPtr, part.first, path);
    }

    shaderPtr->setUniform("textureVar", 0);
}

void loadRobotPartTexture(Robot *&robotPtr, Shader *&shaderPtr, string partName, string texturePath)
{
    static vector<string> pathTo = {
        "/right.jpg",
        "/left.jpg",
        "/top.jpg",
        "/bottom.jpg",
        "/back.jpg",
        "/front.jpg"
    };
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data;
    glGenTextures(1, &(robotPtr->textureID[partName]));
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, robotPtr->textureID[partName]);
    cout << robotPtr->textureID[partName] << " " << partName << endl;
    for (int i = 0; i < 6; i++) {
        data = stbi_load((texturePath + pathTo[i]).c_str(), &width, &height, &nrChannels, 0);
        cout << texturePath + pathTo[i] << endl;
        if (!data) {
            std::cerr << "Failed to load texture" << std::endl;
            return;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}

void loadObjModel(OpenGLObject *&objectPtr, Models *&modelsPtr, const string &dataPath, const string &imgPath, Shader *&shaderPtr)
{
    if (objectPtr) {
        delete objectPtr;
        objectPtr = nullptr;
    }
    if (modelsPtr) {
        delete modelsPtr;
        modelsPtr = nullptr;
    }
    modelsPtr = new Models((dataPath + ".obj"), (dataPath + ".mtl"));
    objectPtr = new OpenGLObject(*shaderPtr);
    objectPtr->bindVboData("points", "vPosition", modelsPtr->points);
    objectPtr->bindVboData("colors", "vColor", modelsPtr->colors);
    objectPtr->bindVboData("normals", "vNormal", modelsPtr->normals);
    if (modelsPtr->textureCoords.size()) objectPtr->bindVboData("textureCoords", "vTextureCoords", modelsPtr->textureCoords);
    objectPtr->bindEboData("index", modelsPtr->index);
    // 创建纹理
    for (auto &img : modelsPtr->textureID) {
        glGenTextures(1, &(img.second));
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, img.second);
        int width, height, nrChannels;
        unsigned char *data = stbi_load((imgPath + img.first).c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture" << std::endl;
            return;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

mat4 getShadowMatrix(const Light &light, Platform &platform, const mat4 &model)
{
    mat4 shadowMatrix;
    vec4 lightPosition = normalize(vec4(light.position, 1.f));
    vec4 planeEquation = platform.getPlaneEquation();
    auto d = dot(planeEquation, lightPosition);
    shadowMatrix[0][0] = d - lightPosition.x * planeEquation.x;
    shadowMatrix[1][0] = 0.f - lightPosition.x * planeEquation.y;
    shadowMatrix[2][0] = 0.f - lightPosition.x * planeEquation.z;
    shadowMatrix[3][0] = 0.f - lightPosition.x * planeEquation.w;
    shadowMatrix[0][1] = 0.f - lightPosition.y * planeEquation.x;
    shadowMatrix[1][1] = d - lightPosition.y * planeEquation.y;
    shadowMatrix[2][1] = 0.f - lightPosition.y * planeEquation.z;
    shadowMatrix[3][1] = 0.f - lightPosition.y * planeEquation.w;
    shadowMatrix[0][2] = 0.f - lightPosition.z * planeEquation.x;
    shadowMatrix[1][2] = 0.f - lightPosition.z * planeEquation.y;
    shadowMatrix[2][2] = d - lightPosition.z * planeEquation.z;
    shadowMatrix[3][2] = 0.f - lightPosition.z * planeEquation.w;
    shadowMatrix[0][3] = 0.f - lightPosition.w * planeEquation.x;
    shadowMatrix[1][3] = 0.f - lightPosition.w * planeEquation.y;
    shadowMatrix[2][3] = 0.f - lightPosition.w * planeEquation.z;
    shadowMatrix[3][3] = d - lightPosition.w * planeEquation.w;

    shadowMatrix = shadowMatrix * model;
    return shadowMatrix;
}


