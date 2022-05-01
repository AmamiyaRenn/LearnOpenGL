#version 460 core

layout(location = 0) in vec3 aVextex;// 位置变量的属性位置值为 0 
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aVextex, 1.0);// 为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量，它在幕后是vec4类型的
}