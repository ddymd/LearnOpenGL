#version 330 core
out vec4 FragColor;
in vec2 ourCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    FragColor = mix(texture(texture1, ourCoord), texture(texture2, ourCoord), 0.3);
}
