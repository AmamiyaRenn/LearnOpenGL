#version 460 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;// sampler2D: 采样器(Sampler), 用于采样纹理
uniform sampler2D texture2;
uniform float visibility = 0.2;
void main() {
    // FragColor = texture(ourTexture, TexCoord);// 采样纹理的颜色: 纹理采样器; 对应的纹理坐标
    // FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);// 混合纹理颜色与顶点颜色
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), visibility);
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2((1 - TexCoord.x), TexCoord.y)), 0.2);//TODO: practice 1
}