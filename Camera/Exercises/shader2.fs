#version 330 core
out vec4 FragColor;
in vec2 ourCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = vec4(0.2f, 0.3f, 0.5f, 1.f);
    FragColor = mix(texture(texture1, ourCoord), texture(texture2, ourCoord), 0.3);
}
