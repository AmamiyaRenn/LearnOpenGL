#version 460 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;// sampler2D: 采样器(Sampler), 用于采样纹理
uniform sampler2D texture2;

void main() {
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.4f);
}