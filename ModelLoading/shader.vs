#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aCoord;

out vec3 fPos;
out vec3 fNor;
out vec2 fCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    fPos = vec3(model * vec4(aPos, 1.0));
    fNor = mat3(transpose(inverse(model))) * aNor;
    fCoord = aCoord;
    gl_Position = proj * view * vec4(fPos, 1.0);
}
