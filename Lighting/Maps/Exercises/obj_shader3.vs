#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fPos;
out vec3 fNor;
out vec2 fCoord;

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    fPos = vec3(model * vec4(aPos, 1.0));
    fNor = mat3(transpose(inverse(model))) * aNor;
    fCoord = aCoord;
}