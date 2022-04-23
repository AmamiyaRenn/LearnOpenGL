/***
 * @Description: LearnOpenGL_Task1
 * @Author: Amamiya
 * @Date: 2022-04-22 20:01:26
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
// User Headers
#include "shader.hpp"

// 在创建窗口之后，渲染循环初始化之前注册这些回调函数

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const char *vertexShaderSource = "#version 460 core\n"
                                 "layout(location = 0) in vec3 aPos;\n"
                                 "layout(location = 1) in vec3 aColor;\n"
                                 "out vec3 ourColor;\n"
                                 "void main() {\n"
                                 "	gl_Position = vec4(aPos, 1.0f);\n"
                                 "  ourColor = aColor;\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 460 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 ourColor;\n"
                                   "void main() {\n"
                                   "	FragColor = vec4(ourColor, 1.0f);\n"
                                   "}\0";

int main()
{
    // 1. glfwSetting
    // 1.1. initialize and configure
    glfwInit();                                                    // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                 // GL主版本号(Major)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);                 // GL次版本号(Minor)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式(Core-profile)，只能使用OpenGL功能的一个子集（没有向后兼容特性）
    // 1.2. window creation
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); // 创建一个窗口对象
    if (window == NULL)
    { // 创建失败
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // 释放/删除之前的分配的所有资源
        return -1;
    }
    glfwMakeContextCurrent(window);                                    // 通知GLFW将窗口的上下文设置为当前线程的主上下文
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 告诉GLFW每当窗口调整大小的时候调用这个函数；当窗口被第一次显示的时候framebuffer_size_callback也会被调用

    // 2. glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    { // GLAD用于管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前需要初始化GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 3. build and compile our shader program
    Shader shaderProgram("./shaders/shader.vs", "./shaders/shader.fs");

    // 4. set up vertex data (and buffer(s)) and configure vertex attributes
    // 4.1. set up verteices and indices
    float vertices[] = {
        // 位置             // 颜色
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部
    };
    unsigned int indices[] = {
        // 注意索引从0开始!
        0, 1, 2, // 第一个三角形
    };
    // 4.2. set VBO
    unsigned int VBO;      // 通过一个缓冲ID(1)生成一个顶点缓冲对象(Vertex Buffer Object)，它会在GPU内存（通常被称为显存）中储存大量顶点
    glGenBuffers(1, &VBO); // 从这一刻起，使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)
    // 4.3. set VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);         // 生成一个顶点数组对象(Vertex Array Object)
    glBindVertexArray(VAO);             // 绑定VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 把VBO复制到缓冲中供OpenGL使用
    // 把用户定义的数据复制到当前绑定缓冲的函数；目标缓冲的类型，参数指定传输数据的大小(以字节为单位)，希望发送的实际数据，希望显卡如何管理给定的数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 4.4. set EBO
    unsigned int EBO; // 生成一个索引缓冲对象(Element Buffer Object)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);                                      // 绑定EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // 把索引复制到缓冲里
    // 4.5. set vertice attributes
    // 4.5.1. set position attributes
    // 顶点属性(location)，顶点属性的数据大小，数据的类型，是否希望数据被标准化(Normalize)，步长(从这个属性第二次出现的地方到整个数组0位置之间有多少字节)，位置数据在缓冲中起始位置的偏移量(Offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // 以顶点属性位置值作为参数，启用顶点属性
    // 4.5.2. set color attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // location=1, offset=3*sizeof(float)
    glEnableVertexAttribArray(1);                                                                    // 启用1号(location=1)属性

    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑EBO（由于glVertexAttribPointer()函数已经把VBO登记好了）
    glBindVertexArray(0);             // 解绑VAO（需要用到VAO时再绑定）（在需要VAO时请不要解绑EBO）

    // 5. 渲染循环(RenderLoop)
    while (!glfwWindowShouldClose(window)) // 检查GLFW是否被要求退出
    {
        // 5.1. 输入
        processInput(window);

        // 5.2. 渲染指令
        // 5.2.1. 清理屏幕
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 状态设置函数，设置glClear的填充色
        glClear(GL_COLOR_BUFFER_BIT);         // 状态使用函数，用于清空屏幕的颜色缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲
        // 5.2.2. 激活着色器程序
        shaderProgram.use();
        // // 5.2.3. 更新uniform参数
        // float timeValue = glfwGetTime(); // 获取运行的秒数
        // float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // 在目标程序中查询uniform ourColor的位置值
        // 5.2.4. 画图
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // 根据索引改变当前program中目标uniform值
        glBindVertexArray(VAO);                              // 绑定VAO
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0); // 绘制模式，打算绘制顶点的个数，索引类型，指定EBO中的偏移量

        // 5.3. 检查并调用事件，交换缓冲
        glfwSwapBuffers(window); // window对象交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），在这一迭代中被用来绘制，并且将作为输出显示在屏幕上
        glfwPollEvents();        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
    }

    // 6. 回收资源
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);
    glfwTerminate(); // 释放/删除之前的分配的所有资源

    return 0;
}

/***
 * @: 对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用
 * @param {GLFWwindow} *window 被设置的窗口对象
 * @param {int} width 新维度W
 * @param {int} height 新维度H
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // 注意：对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。
    glViewport(0, 0, width, height); // 告诉OpenGL渲染窗口的尺寸大小，即视口(Viewport)；前两个参数控制窗口左下角的位置
}

/***
 * @: 输入控制
 * @param {GLFWwindow} *window
 */
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 检查用户是否按下了返回键(Esc)（如果没有按下，glfwGetKey将会返回GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);            // 把WindowShouldClose属性设置为true，从而关闭GLFW
}
