#ifndef MODELS_H
#define MODELS_H
#include<vector>
using std::vector;
#include<string>
using std::string;
#include<fstream>
using std::ifstream;
#include<iostream>
using std::cout; using std::cin; using std::endl;
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "Light.h"
#include<map>
using std::map;
using namespace glm;
struct mtlInfo {
    int index;
    string name;
};
// 定义一个类，用于从 .off 和 .obj 以及 .mtl 文件中读取模型数据
struct Models {
    vec3 rotate, translate, scale;
    vector<vec3> points, colors, normals;
    vector<vec2> textureCoords;
    vector<int> index;
    vector<mtlInfo> mtlInfos;
    map<string, string> imgMap;
    map<string, Material> mtlMap;
    map<string, unsigned int> textureID;
    float minY;
    Models(const string &filename1, const string &filename2 = "")
    {
        if (filename2 == "") getVertexPositions(filename1);
        else readObj(filename1, filename2);

        colors.assign(points.size(), vec3(1.f));
        rotate = vec3(0.f, 0.f, 0.f);
        // translate = vec3(0.f, 0.f, 0.f);
        scale = vec3(1.f, 1.f, 1.f);
        index.resize(points.size());

    }
    // 读 obj 文件
    void readObj(const string &filename1, const string &filename2)
    {
        vector<vec3> p, n;
        vector<vec2> t;
        vector<int> pi, ni, ti;
        ifstream file(filename1);
        if (!file.is_open()) {
            cout << "file open failed" << endl;
            return;
        }
        string line;
        while (getline(file, line)) {
            if (line.substr(0, 2) == "v ") {
                float x, y, z;
                sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
                p.push_back(vec3(x, y, z));
            }
            else if (line.substr(0, 6) == "usemtl") {
                mtlInfo temp;
                temp.name = line.substr(7);
                temp.index = pi.size();
                mtlInfos.push_back(temp);
            }
            else if (line.substr(0, 2) == "f ") {
                int x, x1, x2, y, y1, y2, z, z1, z2, w, w1, w2;
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &x, &x1, &x2, &y, &y1, &y2, &z, &z1, &z2);
                pi.push_back(x - 1);
                pi.push_back(y - 1);
                pi.push_back(z - 1);
                ni.push_back(x2 - 1);
                ni.push_back(y2 - 1);
                ni.push_back(z2 - 1);
                ti.push_back(x1 - 1);
                ti.push_back(y1 - 1);
                ti.push_back(z1 - 1);

            }
            else if (line.substr(0, 2) == "vn") {
                float x, y, z;
                sscanf(line.c_str(), "vn %f %f %f", &x, &y, &z);
                n.push_back(vec3(x, y, z));
            }
            else if (line.substr(0, 2) == "vt") {
                float x, y;
                sscanf(line.c_str(), "vt %f %f", &x, &y);
                t.push_back(vec2(x, y));
            }

        }
        for (int i = 0; i < pi.size(); i++) {
            points.push_back(p[pi[i]]);
            normals.push_back(n[ni[i]]);
            textureCoords.push_back(t[ti[i]]);
            //index.push_back(i);
        }
        if (normals.empty()) getNormals();
        file.close();
        readMtl(filename2);
    }

    // 读 mtl 文件
    void readMtl(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "file open failed" << endl;
            return;
        }
        string line;
        string mtlName;
        while (getline(file, line)) {
            if (line.substr(0, 6) == "newmtl") {
                mtlName = line.substr(7);
            }
            else if (line.substr(0, 2) == "Ka") {
                float x, y, z;
                sscanf(line.c_str(), "Ka %f %f %f", &x, &y, &z);
                mtlMap[mtlName].ambient = vec4(x, y, z, 1.f);
            }
            else if (line.substr(0, 2) == "Kd") {
                float x, y, z;
                sscanf(line.c_str(), "Kd %f %f %f", &x, &y, &z);
                mtlMap[mtlName].diffuse = vec4(x, y, z, 1.f);
            }
            else if (line.substr(0, 2) == "Ks") {
                float x, y, z;
                sscanf(line.c_str(), "Ks %f %f %f", &x, &y, &z);
                mtlMap[mtlName].specular = vec4(x, y, z, 1.f);
            }
            else if (line.substr(0, 2) == "Ns") {
                float x;
                sscanf(line.c_str(), "Ns %f", &x);
                mtlMap[mtlName].shininess = x;
            }
            else if (line.substr(0, 6) == "map_Kd") {
                string imgName = line.substr(7);
                imgMap[mtlName] = imgName;
                textureID[imgName] = 0;
            }
        }
        file.close();
    }

    // 获取顶点坐标
    void getVertexPositions(const string &filename, vec3 color = vec3(1.f, 1.f, 1.f))
    {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "file open failed" << endl;
            return;
        }
        string line;
        file >> line;
        if (line != "OFF") {
            cout << "file format error" << endl;
            return;
        }
        int vertex_num, face_num, edge_num;

        file >> vertex_num >> face_num >> edge_num;
        float tempY = 1;
        for (int i = 0; i < vertex_num; i++) {
            float x, y, z;
            file >> x >> y >> z;
            points.push_back(vec3(x, y, z));
            colors.push_back(color);
            tempY = std::min(tempY, y);
        }
        minY = tempY;
        for (int i = 0; i < face_num; i++) {
            int n, x, y, z;
            file >> n >> x >> y >> z;
            index.push_back(x);
            index.push_back(y);
            index.push_back(z);
        }
        file.close();
        getNormals();
        // 将模型的 Y 轴最低点平移到 y = 0 处
        translate = vec3(0.f, -minY, 0.f);
    }
    // 获取法向量
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

    void setRotate(const vec3 &_rotate)
    {
        rotate = _rotate;
    }
    void setTranslate(const vec3 &_translate)
    {
        translate = _translate;
    }
    void setScale(const vec3 &_scale)
    {
        scale = _scale;
    }
    void setRTS(const vec3 &_rotate, const vec3 &_translate, const vec3 &_scale)
    {
        rotate = _rotate;
        translate = _translate;
        scale = _scale;
    }
    mat4 getModelMatrix()
    {
        mat4 model = mat4(1.f);
        model = glm::translate(model, translate);
        model = glm::rotate(model, rotate.x, vec3(1.f, 0.f, 0.f));
        model = glm::rotate(model, rotate.y, vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, rotate.z, vec3(0.f, 0.f, 1.f));
        model = glm::scale(model, scale);
        return model;
    }
};
// 定义一个类，用于创建一个放置模型的平台
struct Platform {
    vector<vec3> points, colors, normals;
    vector<vec2> textureCoords;
    vector<int> index;
    unsigned int textureID;
    mat4 model = mat4(1.f);
    Platform(float y = -1.0, vec3 color = vec3(0.8f, 0.8f, 0.8f), float width = 1.0, float length = 1.0, bool texture = false)
    {
        // 输入平台的 y 坐标、宽度、长度和颜色
        points.push_back(vec3(-width, y, -length)); // 0
        points.push_back(vec3(width, y, -length));  // 1
        points.push_back(vec3(width, y, length));   // 2
        points.push_back(vec3(-width, y, length));  // 3
        colors.assign(4, color);
        normals.assign(4, vec3(0.f, 1.f, 0.f));
        index = {0, 1, 2, 0, 2, 3};
        // 如果使用纹理
        if (texture) {
            textureCoords.push_back(vec2(-width, -length));
            textureCoords.push_back(vec2(width, -length));
            textureCoords.push_back(vec2(width, length));
            textureCoords.push_back(vec2(-width, length));
        }

    }
    vec4 getPlaneEquation()
    {
        vec3 v1 = points[1] - points[0];
        vec3 v2 = points[2] - points[0];
        vec3 normal = normalize(cross(v1, v2));
        return vec4(normal, -dot(normal, points[0]));
    }
    mat4 getModelMatrix()
    {
        return model;
    }
    void moveUP(float y)
    {
        model = glm::translate(model, vec3(0.f, y, 0.f));
    }

};
#endif