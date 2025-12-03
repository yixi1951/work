#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Models.h"

void Shader::setLight(const string &name, const Light &light)
{
    setUniform(name + ".position", light.position);
    setUniform(name + ".ambient", light.ambient);
    setUniform(name + ".diffuse", light.diffuse);
    setUniform(name + ".specular", light.specular);
}
void Shader::setMaterial(const string &name, const Material &material)
{
    setUniform(name + ".ambient", material.ambient);
    setUniform(name + ".diffuse", material.diffuse);
    setUniform(name + ".specular", material.specular);
    setUniform(name + ".shininess", material.shininess);
}
void Shader::setCamera(Camera &camera)
{
    setUniform("view", camera.getViewMatrix());
    setUniform("projection", camera.getProjectionMatrix(800, 600));
    setUniform("eye_position", camera.position);

}



