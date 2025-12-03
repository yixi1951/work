#version 330 core
in vec3 vPosition;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = vPosition;
    gl_Position = projection * view * vec4(vPosition, 1.0);
}