#version 460 core
layout(location = 0) in vec3 aPos;// 位置变量的属性位置值为 0 
layout(location = 1) in vec3 aColor;// 颜色变量的属性位置值为 1
out vec3 ourColor;// 向片段着色器输出一个颜色
// uniform float bias;//TODO: practice2
void main() {
    // gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0f); // 把位置数据赋值给预定义的gl_Position变量（这个变量是vec4类型，所以要设置w）//TODO: practice1
    // gl_Position = vec4(aPos.x + bias, aPos.y, aPos.z, 1.0f); // 把位置数据赋值给预定义的gl_Position变量（这个变量是vec4类型，所以要设置w）//TODO: practice2
    gl_Position = vec4(aPos, 1.0f);
    // ourColor = aPos;// 将ourColor设置为我们从顶点数据那里得到的输入颜色//TODO: practice3
    ourColor = aColor;
}