#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texback;
uniform sampler2D texfront;

void main() {
    // FragColor = texture(texback, TexCoords);
    if (gl_FrontFacing) {
        FragColor = texture(texfront, TexCoords);
    } else {
        FragColor = texture(texback, TexCoords);
    }
}
