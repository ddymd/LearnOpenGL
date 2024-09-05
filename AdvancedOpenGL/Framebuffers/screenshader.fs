#version 330 core
out vec4 FragColor;
in vec2 fTexCoord;

uniform sampler2D uTexture;

const float offset = 1.0 / 300.0;

vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2( 0.0f, offset), // top-center
    vec2( offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2( 0.0f, 0.0f), // center-center
    vec2( offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f, -offset), // bottom-center
    vec2( offset, -offset) // bottom-right
 );

// sharpen
float kernelsharpen[9] = float[](
    -1,-1,-1,
    -1, 9,-1,
    -1,-1,-1
);
// blur
float kernelblur[9] = float[](
    1.f/16, 2.f/16, 1.f/16,
    2.f/16, 4.f/16, 2.f/16,
    1.f/16, 2.f/16, 1.f/16
);


// edge detection
float kerneled[9] = float[](
    1, 1, 1,
    1,-8, 1,
    1, 1, 1
);

void main() {
    // FragColor = texture(uTexture, fTexCoord);

    // 1.inverse
    // FragColor = vec4(vec3(1.0-texture(uTexture, fTexCoord)), 1.0);

    // 2.GrayScale
    // FragColor = texture(uTexture, fTexCoord);
    // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    // FragColor = vec4(average, average, average, 1.0);

    // 3.Kernel effects
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(uTexture, fTexCoord.st +  offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * kernelsharpen[i];
    }
    FragColor = vec4(col, 1.0);
}
