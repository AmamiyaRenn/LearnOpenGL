/***
 * @Description: LearnOpenGL_Task1
 * @Author: Amamiya
 * @Date: 2022-04-22 20:01:26
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
// User Headers
#include "shader.hpp"
#include "camera.hpp"

// third party headers
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float fov = 45;
float pitch, yaw = -90;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // 摄像机位置
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // 摄像机上方向
Camera camera(cameraPos, cameraUp, yaw, pitch);    // 创建摄像头类

bool firstMouse = true; // 是否是第一次获取鼠标输入
double lastX, lastY;

// 在创建窗口之后，渲染循环初始化之前注册这些回调函数

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

int main()
{
    // 1. glfwSetting
    // 1.1. initialize and configure
    glfwInit();                                                    // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                 // GL主版本号(Major)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);                 // GL次版本号(Minor)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式(Core-profile)，只能使用OpenGL功能的一个子集（没有向后兼容特性）
    // 1.2. window creation
    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL); // 创建一个窗口对象
    if (window == NULL)
    { // 创建失败
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // 释放/删除之前的分配的所有资源
        return -1;
    }
    glfwMakeContextCurrent(window);                                    // 通知GLFW将窗口的上下文设置为当前线程的主上下文
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 告诉GLFW每当窗口调整大小的时候调用这个函数；当窗口被第一次显示的时候framebuffer_size_callback也会被调用
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);       // 隐藏光标，并捕捉(Capture)它
    // 1.3. set control
    glfwSetCursorPosCallback(window, mouse_callback); // 鼠标控制
    glfwSetScrollCallback(window, scroll_callback);   // 鼠标滚轮控制

    // 2. glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    { // GLAD用于管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前需要初始化GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 3. build and compile shader program
    Shader shaderProgram("../shaders/shader.vs", "../shaders/shader.fs");

    // 4. set up vertex data (and buffer(s)) and configure vertex attributes
    // 4.1. set up verteices, indices, and texCoords
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};
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
    // unsigned int EBO;                                                                // 生成一个索引缓冲对象(Element Buffer Object)
    // glGenBuffers(1, &EBO);                                                           // 生成一个缓冲区对象
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);                                      // 绑定EBO
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // 把索引复制到缓冲里
    // 4.5. set vertice attributes
    // 4.5.1. set position attributes
    // 顶点属性(location)，顶点属性的数据大小，数据的类型，是否希望数据被标准化(Normalize)，步长(从这个属性第二次出现的地方到整个数组0位置之间有多少字节)，位置数据在缓冲中起始位置的偏移量(Offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // 以顶点属性位置值作为参数，启用顶点属性
    // 4.5.2 set texCoords attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 4.6 解绑不用的对象
    // glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑EBO（由于glVertexAttribPointer()函数已经把VBO登记好了）
    glBindVertexArray(0); // 解绑VAO（需要用到VAO时再绑定）（在需要VAO时请不要解绑EBO）

    // 5. process textures
    // 5.1. 生成并绑定纹理对象
    unsigned int texture1;                  // 创建纹理对象
    glGenTextures(1, &texture1);            // 输入生成纹理的数量，然后把它们储存在第二个参数的unsigned int数组中
    glBindTexture(GL_TEXTURE_2D, texture1); // 绑定这个纹理对象，让之后任何的纹理指令都可以配置当前绑定的纹理
    // 5.2. 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // S轴环绕设置为重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // T轴环绕设置为重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小时用线性插值过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大时用线性插值过滤
    // 5.3. 加载并生成纹理
    int img_width, img_height, nrChannels;                                                                  // 宽度、高度和颜色通道的个数
    stbi_set_flip_vertically_on_load(true);                                                                 // 设置加载图像时翻转y轴（OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部）
    unsigned char *data = stbi_load("../resourses/container.jpg", &img_width, &img_height, &nrChannels, 0); // 加载图片，并填充宽、高、通道数
    if (data)
    {
        //纹理目标; 多级渐远纹理的级别; 纹理储存格式; 最终纹理的宽高; 总是被设为0（历史遗留的问题）; 源图的格式和数据类型(目前设置为char(byte)数组); 图像数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // 让当前绑定的纹理对象附加上纹理图像
        glGenerateMipmap(GL_TEXTURE_2D);                                                                  // 自动生成Mipmap
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    // 5.4. 释放图像内存
    stbi_image_free(data); // 生成了纹理和相应的多级渐远纹理后，释放图像的内存
    // Repeated
    // 5.1. 生成并绑定纹理对象
    unsigned int texture2;                  // 创建纹理对象
    glGenTextures(1, &texture2);            // 输入生成纹理的数量，然后把它们储存在第二个参数的unsigned int数组中
    glBindTexture(GL_TEXTURE_2D, texture2); // 绑定这个纹理对象，让之后任何的纹理指令都可以配置当前绑定的纹理
    // 5.2. 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // S轴环绕设置为重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // T轴环绕设置为重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小时用线性插值过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大时用线性插值过滤
    // 5.3. 加载并生成纹理                                                                    // 宽度、高度和颜色通道的个数
    data = stbi_load("../resourses/怪盗团.png", &img_width, &img_height, &nrChannels, 0); // 加载图片，并填充宽、高、通道数
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 让当前绑定的纹理对象附加上纹理图像
        glGenerateMipmap(GL_TEXTURE_2D);                                                                    // 自动生成Mipmap
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    // 5.4. 释放图像内存
    stbi_image_free(data); // 生成了纹理和相应的多级渐远纹理后，释放图像的内存
    // 5.5. 设置着色器采样器与纹理单元的归属
    shaderProgram.use();                 // 不要忘记在设置uniform变量之前激活着色器程序！
    shaderProgram.setInt("texture1", 0); // 设置uniform texture1为第0纹理单元
    shaderProgram.setInt("texture2", 1);
    // 5.6.
    glEnable(GL_DEPTH_TEST); // 启动深度测试

    // 6. 渲染循环(RenderLoop)
    while (!glfwWindowShouldClose(window)) // 检查GLFW是否被要求退出
    {
        // 6.1. 输入控制
        processInput(window); // 键盘控制

        // 6.2. 渲染指令
        // 6.2.1. 获得渲染时间差
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 6.2.2. 清理屏幕
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);               // 状态设置函数，设置glClear的填充色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 状态使用函数，用于清空屏幕的颜色|深度缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲

        // 6.2.3. 激活着色器程序并改变uniform
        shaderProgram.use();

        // 6.2.4. 绑定纹理坐标
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // 根据索引改变当前program中目标uniform值
        glActiveTexture(GL_TEXTURE0);           // 在绑定纹理之前先激活纹理单元
        glBindTexture(GL_TEXTURE_2D, texture1); // 绑定纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // 6.2.5.  MVP transform
        glm::mat4 view(1.f);
        view = camera.GetViewMatrix();
        glm::mat4 projection(1.f);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screen_width) / float(screen_height), 0.1f, 100.f);
        shaderProgram.setMat4("view", view);
        shaderProgram.setMat4("projection", projection);

        // 6.2.6. 开始绘制
        // shaderProgram.use();
        glBindVertexArray(VAO); // 绑定VAO
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(20.f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderProgram.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 绘制模式，打算绘制顶点的个数，索引类型，指定EBO中的偏移量

        // 6.3. 检查并调用事件，交换缓冲
        glfwSwapBuffers(window); // window对象交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），在这一迭代中被用来绘制，并且将作为输出显示在屏幕上
        glfwPollEvents();        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
    }

    // 7. 回收资源
    // glDeleteBuffers(1, &EBO);
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
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

/***
 * @: 鼠标控制的回调函数(GLFW的原型复写)
 * @param {GLFWwindow} *window
 * @param {double} xpos
 * @param {double} ypos
 */
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

/***
 * @: 鼠标滚轮控制的回调函数(GLFW的原型复写)
 * @param {GLFWwindow} *window
 * @param {double} xoffset
 * @param {double} yoffset
 */
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}