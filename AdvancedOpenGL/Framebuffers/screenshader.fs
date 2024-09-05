#version 330 core
out vec4 FragColor;
in vec2 fTexCoord;

uniform sampler2D uTexture;

void main() {
    // FragColor = texture(uTexture, fTexCoord);
    // inverse
    // FragColor = vec4(vec3(1.0-texture(uTexture, fTexCoord)), 1.0);

    // GrayScale
    FragColor = texture(uTexture, fTexCoord);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);
}
