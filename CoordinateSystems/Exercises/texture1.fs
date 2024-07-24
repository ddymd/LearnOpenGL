#version 330 core
in vec2 ourCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = vec4(0.3f, 0.6f, 0.2f, 1.f);
    FragColor = mix(texture(texture1, ourCoord), texture(texture2, ourCoord), 0.2);
}
