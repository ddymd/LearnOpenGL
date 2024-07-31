#version 330 core

out vec4 FragColor;
in vec3 ourNor;
in vec2 ourCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 objcolor;

void main() {
    FragColor = vec4(objcolor, 1.0);
}

