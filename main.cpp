#include "Camera.h"
#include "Models.h"
#include <algorithm>
#include "Models.h"
#include "Shader.h"
#include<glad/glad.h>
#include<OpenGLObject.h>
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include<map>
#include<GLFW/glfw3.h>
#include<Window.h>
#include "Robot.h"
#include "skyCube.h"
std::string userGuide = R"(
#### Keyboard Controls:
1. **Move Robot**
	- Press the **R**, then use the WSAD keys to move the character
   
2. **Exit the Program**:
   - Press **ESC** to exit the application.

3. **Adjusting Lighting and Material Effects**:
   - Press and hold the **Shift** key while scrolling the mouse wheel to adjust the **light intensity**. Scrolling up increases the light intensity, and scrolling down decreases it.
   
4. **Camera Controls**:
   - Use the **W, A, S, D** keys to move the camera forward, left, backward, and right, respectively.

#### Mouse Controls:
1. **Mouse Movement**:
   - Move the mouse to navigate the camera view in the 3D space. You can look around the scene by moving the mouse.
   
2. **Left Mouse Button**:
   - Press the **left mouse button** to **set the light position** to the current camera position.

3. **Mouse Scroll**:
   - Scroll the mouse wheel to zoom in and out or light intensity (when **Shift** is held).

)";
using std::map;
Models *objModelsPtr, *objModelObject1;
Platform *platformPtr;
Camera *cameraPtr;
Shader *shaderPtr, *skyShaderPtr, *robotShaderPtr;
Light light;
Material material;
Window *windowPtr;
OpenGLObject *robotObject, *platformObject, *skyboxObject, *objModelObject;
Robot *robotPtr;
skyCube *skyCubePtr;

bool isShift = false; bool isCtrl = false, moveModel = false;

// 所有模型名称
// 当前模型索引
void init()
{
	// 创建着色器程序

	string vshaderPath, fshaderPath;
	vshaderPath = "shaders/vshader.glsl";
	fshaderPath = "shaders/fshader.glsl";

	shaderPtr = new Shader(vshaderPath, fshaderPath);

	string skyVshaderPath, skyFshaderPath;
	skyVshaderPath = "shaders/skyVshader.glsl";
	skyFshaderPath = "shaders/skyFshader.glsl";

	skyShaderPtr = new Shader(skyVshaderPath, skyFshaderPath);

	string robotVshaderPath, robotFshaderPath;
	robotVshaderPath = "shaders/robotVshader.glsl";
	robotFshaderPath = "shaders/robotFshader.glsl";

	robotShaderPtr = new Shader(robotVshaderPath, robotFshaderPath);


	// 读取树的 obj 模型
	loadObjModel(objModelObject, objModelsPtr, "assets/obj/Tree/Tree", "assets/obj/Tree/", shaderPtr);

	// loadPlatform 函数自动化加载平台
	loadPlatform(platformObject, platformPtr, shaderPtr, 0, "assets/texture/grass.jpg");

	// 读取天空盒
	loadSkyCube(skyboxObject, skyCubePtr, skyShaderPtr);

	// 读取机器人
	loadRobot(robotObject, robotPtr, robotShaderPtr);

	// 读取机器人纹理
	loadRobotTexture(robotPtr, robotShaderPtr, "assets/robot/");




	glClearColor(0., 0., 0., 1.);


	// 初始相机
	cameraPtr = new Camera(vec3(0.f, 5.f, 5.f));
}

// 负责绘制模型
void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制天空盒
	glDepthMask(GL_FALSE);
	skyboxObject->enable(true);
	skyShaderPtr->setUniform("view", mat4(mat3(cameraPtr->getViewMatrix())));
	skyShaderPtr->setUniform("projection", cameraPtr->getProjectionMatrix(1, 1));
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubePtr->textureID);
	glDrawElements(GL_TRIANGLES, skyCubePtr->index.size(), GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);


	// 启用当前的模型对象
	robotObject->enable(true);
	// 设置 uniform 变量
	robotShaderPtr->setUniform("view", cameraPtr->getViewMatrix());
	robotShaderPtr->setUniform("projection", cameraPtr->getProjectionMatrix(1, 1));
	robotShaderPtr->setUniform("eye_position", cameraPtr->position);
	robotShaderPtr->setUniform("isShadow", 0);
	robotShaderPtr->setLight("light", light);
	robotShaderPtr->setMaterial("material", material);
	// 绘制机器人
	robotPtr->getModel();
	for (auto &model : robotPtr->modelsMap) {
		robotShaderPtr->setUniform("model", model.second);
		glBindTexture(GL_TEXTURE_CUBE_MAP, robotPtr->textureID[model.first]);
		glDrawElements(GL_TRIANGLES, robotPtr->index.size(), GL_UNSIGNED_INT, 0);
	}
	// 绘制阴影
	robotShaderPtr->setUniform("isShadow", 1);
	// 绘制机器人阴影
	for (auto &model : robotPtr->modelsMap) {
		robotShaderPtr->setUniform("model", getShadowMatrix(light, *platformPtr, model.second));
		glDrawElements(GL_TRIANGLES, robotPtr->index.size(), GL_UNSIGNED_INT, 0);
	}

	// 绘制树的 obj 模型
	objModelObject->enable(true);

	shaderPtr->setUniform("view", cameraPtr->getViewMatrix());
	shaderPtr->setUniform("projection", cameraPtr->getProjectionMatrix(1, 1));
	shaderPtr->setUniform("eye_position", cameraPtr->position);
	shaderPtr->setLight("light", light);
	shaderPtr->setUniform("useTexture", 1);

	shaderPtr->setMaterial("material", material);

	for (int i = 0; i < objModelsPtr->mtlInfos.size(); i++) {
		shaderPtr->setUniform("isShadow", 0);
		shaderPtr->setUniform("model", glm::translate(mat4(1.f), vec3(2.f, 0.f, 2.f)));
		auto &mtlInfo = objModelsPtr->mtlInfos[i];
		auto name = mtlInfo.name;
		if (name == "Green") {
			shaderPtr->setUniform("isGreen", 1);
		}
		else {
			shaderPtr->setUniform("isGreen", 0);
		}
		auto start = mtlInfo.index;
		auto end = i == objModelsPtr->mtlInfos.size() - 1 ? objModelsPtr->points.size() : objModelsPtr->mtlInfos[i + 1].index;
		auto textureID = objModelsPtr->textureID[objModelsPtr->imgMap[name]];
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, start, end - start);

		// 绘制阴影
		shaderPtr->setUniform("isShadow", 1);
		shaderPtr->setUniform("model", getShadowMatrix(light, *platformPtr, glm::translate(mat4(1.f), vec3(2.f, 0.f, 2.f))));
		glDrawArrays(GL_TRIANGLES, start, end - start);
	}


	for (int i = 0; i < objModelsPtr->mtlInfos.size(); i++) {
		shaderPtr->setUniform("model", glm::translate(mat4(1.f), vec3(-2.f, 0.f, -2.f)));
		shaderPtr->setUniform("isShadow", 0);
		shaderPtr->setUniform("isShadow", 0);

		auto &mtlInfo = objModelsPtr->mtlInfos[i];
		auto name = mtlInfo.name;
		if (name == "Green") {
			shaderPtr->setUniform("isGreen", 1);
		}
		else {
			shaderPtr->setUniform("isGreen", 0);
		}
		auto start = mtlInfo.index;
		auto end = i == objModelsPtr->mtlInfos.size() - 1 ? objModelsPtr->points.size() : objModelsPtr->mtlInfos[i + 1].index;
		auto textureID = objModelsPtr->textureID[objModelsPtr->imgMap[name]];
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, start, end - start);

		// 绘制阴影
		shaderPtr->setUniform("isShadow", 1);
		shaderPtr->setUniform("model", getShadowMatrix(light, *platformPtr, glm::translate(mat4(1.f), vec3(-2.f, 0.f, -2.f))));
		glDrawArrays(GL_TRIANGLES, start, end - start);
	}


	// 启用平台对象
	platformObject->enable(true);
	shaderPtr->setUniform(("isShadow"), 0);
	shaderPtr->setUniform("isGreen", 0);
	shaderPtr->setUniform("useTexture", 1);
	shaderPtr->setMaterial("material", material);
	shaderPtr->setUniform("model", platformPtr->getModelMatrix());
	// 启用深度补偿，防止深度冲突
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	// 绘制平台
	glBindTexture(GL_TEXTURE_2D, platformPtr->textureID);
	glDrawElements(GL_TRIANGLES, platformPtr->index.size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

	// 按下 ESC 键退出程序
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}
	// 按下 shift 键 + 鼠标滚轮调整光照强度
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		isShift = true;
		return;
	}
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		isShift = false;
		return;
	}
	// 按下 R 键切换 moveModel
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		moveModel = !moveModel;
		return;
	}

	// 相机移动
	else if (!moveModel && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		cameraPtr->key_callback(window, key, scancode, action, mods, platformPtr->points[0].y);
		return;
	}
	// 机器人移动
	else if (moveModel && (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE)) {

		robotPtr->key_callback(window, key, scancode, action, mods);
		return;
	}
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	// 调整光照强度
	if (isShift && !isCtrl)
		light.changeIntensity(yoffset > 0);
	// 调整相机视角

	else if (!isCtrl && !isShift)
		cameraPtr->scroll_callback(window, xoffset, yoffset);
}
void cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
	// 转动相机
	cameraPtr->mouse_callback(window, xpos, ypos, windowPtr->width, windowPtr->height);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	windowPtr->updateWH(width, height);
	cameraPtr->mouse_callback(window, width / 2, height / 2, width, height, true);
}
void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
	// 移动光源
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		light.position = cameraPtr->position;
	}
	// 按下鼠标中键
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		windowPtr->lockCursor();
	}
}
int main()
{
	windowPtr = new Window(1600, 1600, "2022080182_CaoBoyu_Final");
	auto &window = *windowPtr;
	glfwSetKeyCallback(window.window, key_callback);
	glfwSetCursorPosCallback(window.window, cursor_callback);
	glfwSetScrollCallback(window.window, scroll_callback);
	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window.window, mouse_callback);
	init();
	cout << userGuide << endl;
	while (!glfwWindowShouldClose(window.window)) {
		display();
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
	glfwTerminate();

}