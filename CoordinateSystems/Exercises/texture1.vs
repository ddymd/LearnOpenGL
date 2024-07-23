#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aCoord;

out vec3 oColor;
out vec2 oCoord;

uniform mat4 trans;

void main() {
    gl_Position = trans * vec4(aPosition, 1.0);
    oColor = aColor;
    oCoord = aCoord;
}
