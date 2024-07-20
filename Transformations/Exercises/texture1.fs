#version 330 core

out vec4 FragColor;

in vec3 oColor;
in vec2 oCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = texture(texture1, oCoord) * vec4(oColor, 1.0);
    FragColor = mix(texture(texture1, oCoord), texture(texture2, oCoord), 0.3);
}
