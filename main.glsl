// #version 460 core
// layout(location = 0) in vec3 aPos;//layout关键字把输入(location)链接到顶点数据 ；使用location关键字，从而在CPU上配置顶点属性；使用in关键字，在顶点着色器中声明所有的输入顶点属性(Input Vertex Attribute)
// out vec4 vertexColor;// 为片段着色器指定一个颜色输出

// void main() {
// 	gl_Position = vec4(aPos, 1.0f); // 把位置数据赋值给预定义的gl_Position变量（这个变量是vec4类型，所以要设置w）
// 	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
// }

// #version 460 core
// out vec4 FragColor;

// in vec4 vertexColor;// 从顶点着色器传来的输入变量（名称相同、类型相同）

// void main() {
// 	FragColor = vertexColor;
// }

#version 460 core
layout(location = 0) in vec3 aPos;
void main() {
	gl_Position = vec4(aPos, 1.0f); // 把位置数据赋值给预定义的gl_Position变量（这个变量是vec4类型，所以要设置w）
}

#version 460 core
out vec4 FragColor;
uniform vec4 ourColor;// 在OpenGL程序代码中设定这个变量；Uniform是一种从CPU中的应用向GPU中的着色器发送数据的方式(Global)
void main() {
	FragColor = ourColor;
}