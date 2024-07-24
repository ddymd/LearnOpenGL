#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aCoord;

out vec2 ourCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
    ourCoord = aCoord;
}
