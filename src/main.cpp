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
#include "models.hpp"

// standard headers

// third party headers
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 在创建窗口之后，渲染循环初始化之前注册这些回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);

// settings
const unsigned int screen_width = 1280;
const unsigned int screen_height = 720;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// camera
float fov = 45;
float pitch, yaw = -90;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // 摄像机位置
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // 摄像机上方向
Camera camera(cameraPos, cameraUp, yaw, pitch);    // 创建摄像头类
bool firstMouse = true;                            // 是否是第一次获取鼠标输入
double lastX, lastY;

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);         // 隐藏光标，并捕捉(Capture)它
    // 1.3. set control
    glfwSetCursorPosCallback(window, mouse_callback); // 鼠标控制
    glfwSetScrollCallback(window, scroll_callback);   // 鼠标滚轮控制

    // 2. glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    { // GLAD用于管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前需要初始化GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); // 启动深度测试

    // 3. build and compile shader program
    Shader objectShader("../shaders/shader.vs", "../shaders/shader.fs");
    Shader lightShader("../shaders/lightShader.vs", "../shaders/lightShader.fs");
    Shader modelShader("../shaders/modelShader.vs", "../shaders/modelShader.fs");

    // 4. set up vertex data (and buffer(s)) and configure vertex attributes
    // 4.1. set up verteices, indices, and texCoords
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

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

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // 4.2. set VBO
    unsigned int VBO;      // 通过一个缓冲ID(1)生成一个顶点缓冲对象(Vertex Buffer Object)，它会在GPU内存（通常被称为显存）中储存大量顶点
    glGenBuffers(1, &VBO); // 从这一刻起，使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)

    // 4.3. set VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); // 生成一个顶点数组对象(Vertex Array Object)

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 把VBO复制到缓冲中供OpenGL使用
    // 把用户定义的数据复制到当前绑定缓冲的函数；目标缓冲的类型，参数指定传输数据的大小(以字节为单位)，希望发送的实际数据，希望显卡如何管理给定的数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO); // 绑定后才能设置对应的顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // 这行代码必须要加

    // 5. process textures
    unsigned int diffuseMap = loadTexture("../resources/container2.png");
    unsigned int specularMap = loadTexture("../resources/container2_specular.png");

    objectShader.use();
    // light settings
    // Dirlight
    objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    objectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    objectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    objectShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("pointLights[0].constant", 1.0f);
    objectShader.setFloat("pointLights[0].linear", 0.09f);
    objectShader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    objectShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("pointLights[1].constant", 1.0f);
    objectShader.setFloat("pointLights[1].linear", 0.09f);
    objectShader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    objectShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("pointLights[2].constant", 1.0f);
    objectShader.setFloat("pointLights[2].linear", 0.09f);
    objectShader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    objectShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("pointLights[3].constant", 1.0f);
    objectShader.setFloat("pointLights[3].linear", 0.09f);
    objectShader.setFloat("pointLights[3].quadratic", 0.032f);
    // SpotLight
    // objectShader.setVec3("spotLight.position", camera.Position);
    // objectShader.setVec3("spotLight.direction", camera.Front);
    objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.09f);
    objectShader.setFloat("spotLight.quadratic", 0.032f);
    objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // object settings
    objectShader.setInt("material.diffuse", 0); // 将要用的纹理单元赋值到material.diffuse这个uniform采样器
    objectShader.setInt("material.specular", 1);
    objectShader.setFloat("material.shininess", 32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // Model Ganyu("../resources/models/Ganyu.fbx");
    Model Ganyu("../resources/models/nanosuit/nanosuit.obj");

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);               // 状态设置函数，设置glClear的填充色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 状态使用函数，用于清空屏幕的颜色|深度缓冲，它接受一个缓冲位(Buffer Bit)来指定要清空的缓冲

        // 6.2.3. 激活着色器程序并改变uniform
        objectShader.use();

        // 6.2.5.  MVP transform
        glm::mat4 model(1.0f);
        glm::mat4 view(1.f);
        view = camera.GetViewMatrix();
        glm::mat4 projection(1.f);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screen_width) / float(screen_height), 0.1f, 100.f);

        // 6.2.6. 开始绘制
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // 把光源缩小一点
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        objectShader.use();
        // 绑定箱子的纹理到这个纹理单元

        objectShader.setVec3("viewPos", camera.Position);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);
        objectShader.setVec3("spotLight.position", camera.Position);
        objectShader.setVec3("spotLight.direction", camera.Front);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        Ganyu.Draw(modelShader);

        // 6.3. 检查并调用事件，交换缓冲
        glfwSwapBuffers(window); // window对象交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），在这一迭代中被用来绘制，并且将作为输出显示在屏幕上
        glfwPollEvents();        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
    }

    // 7. 回收资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

/***
 * @: 从一个文件加载2D纹理
 * @param {char const*} 纹理贴图文件路径
 * @return {unsigned int} textureID
 */
unsigned int loadTexture(const char *path)
{
    // 1. 生成并绑定纹理对象
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 2. 加载并生成纹理
    int width, height, nrChannels;                                          // 宽度、高度和颜色通道的个数
    stbi_set_flip_vertically_on_load(true);                                 // 设置加载图像时翻转y轴（OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部）
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0); // 加载图片，并填充宽、高、通道数
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        //纹理目标; 多级渐远纹理的级别; 纹理储存格式; 最终纹理的宽高; 总是被设为0（历史遗留的问题）; 源图的格式和数据类型(目前设置为char(byte)数组); 图像数据
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // 让当前绑定的纹理对象附加上纹理图像

        // 为当前绑定的纹理对象进行设置
        glGenerateMipmap(GL_TEXTURE_2D);                                  // 自动生成Mipmap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // S轴环绕设置为重复
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // T轴环绕设置为重复
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小时用线性插值过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大时用线性插值过滤
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    // 3. 释放图像内存
    stbi_image_free(data); // 生成了纹理和相应的多级渐远纹理后，释放图像的内存

    return textureID;
}