#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aCoord;

out vec3 ourColor;
out vec2 ourCoord;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    ourColor = aColor;
    ourCoord = aCoord;
}
