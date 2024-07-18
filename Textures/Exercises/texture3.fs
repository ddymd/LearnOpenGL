#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 ourCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = texture(texture1, ourCoord) * vec4(ourColor, 1.0);
    // FragColor = texture(texture2, ourCoord);
    FragColor = mix(texture(texture1, ourCoord), texture(texture2, ourCoord), 0.2);
}
