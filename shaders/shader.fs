#version 460 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;// 采样器(Sampler), 用于采样纹理
void main() {
    FragColor = texture(ourTexture, TexCoord);// 采样纹理的颜色: 纹理采样器; 对应的纹理坐标
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);// 混合纹理颜色与顶点颜色
}