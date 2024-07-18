#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 ourCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = vec4(ourColor, 1.0);
    // FragColor = texture(texture1, ourCoord) * vec4(ourColor, 1.0);
    FragColor = mix(texture(texture1, vec2(ourCoord.x, ourCoord.y)), texture(texture2, vec2(1-ourCoord.x, ourCoord.y)), 0.3);
}
