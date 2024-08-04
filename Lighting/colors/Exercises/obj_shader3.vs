#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

out vec3 fPos;
out vec3 fNor;
out vec3 fLitPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 litPos;

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    fPos = vec3(view*model*vec4(aPos, 1.0));
    fNor = mat3(transpose(inverse(view*model)))*aNor;
    fLitPos = vec3(view*model*vec4(litPos, 1.0));
}
