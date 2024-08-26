#version 330 core

out vec4 FragColor;
in vec2 fCoord;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
    vec4 texcolor = texture(texture0, fCoord);
    // if (texcolor.a < 0.1) {
    //     discard;
    // }
    FragColor = texcolor;
}
