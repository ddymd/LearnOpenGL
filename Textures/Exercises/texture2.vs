#version 330 core
in vec3 aPosition;
in vec3 aColor;
in vec2 aCoord;

out vec3 ourColor;
out vec2 ourCoord;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    ourColor = aColor;
    ourCoord = aCoord;
}