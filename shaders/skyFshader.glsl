#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube textureVar;

void main() {
    FragColor = texture(textureVar, TexCoords);
}