#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fNor;
out vec3 fPos;

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    fNor = mat3(transpose(inverse(model)))*aNor;
    fPos = aPos;
}


