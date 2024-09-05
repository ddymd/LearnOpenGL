#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    // gl_Position = proj * view * model * vec4(aPosition, 1.0);
    gl_Position = vec4(aPosition, 0.0, 1.0);
    fTexCoord = aTexCoord;
}
