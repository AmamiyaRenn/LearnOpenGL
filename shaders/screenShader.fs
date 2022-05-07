#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main() {
    FragColor = texture(screenTexture, TexCoords);
    // FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);// 反相
    // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;// 灰度加权
    // FragColor = vec4(average, average, average, 1.0);// 灰度
    // vec2 offsets[9] = vec2[](vec2(-offset, offset), // 左上
    // vec2(0.0, offset), // 正上
    // vec2(offset, offset), // 右上
    // vec2(-offset, 0.0),   // 左
    // vec2(0.0, 0.0),   // 中
    // vec2(offset, 0.0),   // 右
    // vec2(-offset, -offset), // 左下
    // vec2(0.0, -offset), // 正下
    // vec2(offset, -offset)  // 右下
    // );

    // float kernel[9] = float[](-1, -1, -1, -1, 9, -1, -1, -1, -1);// Sharpen
    // float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16);// Blur
    // float kernel[9] = float[](1, 1, 1, 1, -8, 1, 1, 1, 1);// Edge-detection

    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++) {
    //     sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    // }
    // vec3 col = vec3(0.0);
    // for(int i = 0; i < 9; i++) col += sampleTex[i] * kernel[i];

    // FragColor = vec4(col, 1.0);
}
