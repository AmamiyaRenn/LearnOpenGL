#version 460 core
layout(location = 0) in vec3 aPos;// 位置变量的属性位置值为 0 
layout(location = 1) in vec3 aColor;// 颜色变量的属性位置值为 1
layout(location = 2) in vec2 aTexCoord;// 纹理坐标变量的属性位置值为 2
out vec3 ourColor;// 向片段着色器输出颜色
out vec2 TexCoord;// 向片段着色器输出纹理坐标
void main() {
    gl_Position = vec4(aPos, 1.0f);// 为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量，它在幕后是vec4类型的
    ourColor = aColor;
    TexCoord = aTexCoord;
}