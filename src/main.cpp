/***
 * @Description: LearnOpenGL_Task1
 * @Author: Amamiya
 * @Date: 2022-04-22 20:01:26
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
// User Headers
#include "camera.hpp"
#include "models.hpp"
#include "shader.hpp"

// standard headers
#include <map>

// third party headers
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 在创建窗口之后，渲染循环初始化之前注册这些回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void         processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// camera
float     fov = 45;
float     pitch, yaw = -90;
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 5.0f); // 摄像机位置
glm::vec3 cameraUp  = glm::vec3(0.0f, 1.0f, 0.0f); // 摄像机上方向
Camera    camera(cameraPos, cameraUp, yaw, pitch); // 创建摄像头类
bool      firstMouse = true;                       // 是否是第一次获取鼠标输入
double    lastX, lastY;

int main()
{
    // 1. glfwSetting
    // 1.1. initialize and configure
    glfwInit();                                    // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // GL主版本号(Major)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // GL次版本号(Minor)
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE); // 使用核心模式(Core-profile)，只能使用OpenGL功能的一个子集（没有向后兼容特性）
    // 1.2. window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr); // 创建一个窗口对象
    if (window == nullptr)
    { // 创建失败
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // 释放/删除之前的分配的所有资源
        return -1;
    }
    glfwMakeContextCurrent(window); // 通知GLFW将窗口的上下文设置为当前线程的主上下文
    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback); // 告诉GLFW每当窗口调整大小的时候调用这个函数；当窗口被第一次显示的时候framebuffer_size_callback也会被调用
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏光标，并捕捉(Capture)它
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // 隐藏光标，并捕捉(Capture)它
    // 1.3. set control
    glfwSetCursorPosCallback(window, mouse_callback); // 鼠标控制
    glfwSetScrollCallback(window, scroll_callback);   // 鼠标滚轮控制

    // 2. glad: load all OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    { // GLAD用于管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前需要初始化GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); // 启动深度测试
    glEnable(GL_BLEND);      // 启动透明度混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_CULL_FACE); // 启动面剔除
    // // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW); // 将顺时针的面（而不是逆时针的面）定义为正向面

    // 3. build and compile shader program
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs");
    Shader screen_shader("../shaders/screenShader.vs", "../shaders/screenShader.fs");
    Shader skybox_shader("../shaders/skyboxShader.vs", "../shaders/skyboxShader.fs");
    Shader model_shader("../shaders/modelShader.vs", "../shaders/modelShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cube_vertices[]  = {-0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,
                              0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
                              -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

                              -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
                              0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                              -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

                              -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,
                              -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
                              -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

                              0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,
                              0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
                              0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

                              -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,
                              0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
                              -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

                              -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
                              0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                              -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};
    float plane_vertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture
        // wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f, -5.0f, -0.5f, 5.0f,  0.0f, 0.0f, -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f, -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, 5.0f,  -0.5f, -5.0f, 2.0f, 2.0f};
    float quad_vertices[]   = {// vertex attributes for a quad that fills the entire screen in Normalized Device
                             // Coordinates. positions   // texCoords
                             -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

                             -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
      float skybox_vertices[] = {// positions
                               -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                               1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                               -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                               -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                               1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                               1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                               -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                               1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                               -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                               1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                               -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                               1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
    // cube VAO
    unsigned int cube_vao, cube_vbo;
    glGenVertexArrays(1, &cube_vao);
    glGenBuffers(1, &cube_vbo);
    glBindVertexArray(cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int plane_vao, plane_vbo;
    glGenVertexArrays(1, &plane_vao);
    glGenBuffers(1, &plane_vbo);
    glBindVertexArray(plane_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);
    // screen quad VAO
    unsigned int quad_vao, quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    // skybox VAO
    unsigned int skybox_vao, skybox_vbo;
    glGenVertexArrays(1, &skybox_vao);
    glGenBuffers(1, &skybox_vbo);
    glBindVertexArray(skybox_vao);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

    unsigned int framebuffer; // 创建一个帧缓冲
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int texture_colorbuffer; // 创建一个纹理图像，并将它作为一个颜色附件附加到帧缓冲上
    glGenTextures(1, &texture_colorbuffer);
    glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer, 0); // 将它附加到当前绑定的帧缓冲对象

    unsigned int rbo; // 创建一个渲染缓冲对象
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // 深度和模板附件渲染缓冲对象
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              rbo); // 将渲染缓冲对象附加到帧缓冲的深度和模板附件上

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // 检查帧缓冲是否完整
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // load textures
    // -------------
    // unsigned int cubeTexture = loadTexture("../resources/textures/container.jpg");
    unsigned int             floor_texture = loadTexture("../resources/textures/metal.png");
    std::vector<std::string> faces {
        // 在 opengl 中，传送 2D 纹理数据的时候，data 的起始位置应该是图片的左下角像素（这里的纹理坐标是
        // 0,0），然后逐行向上扫描
        // 但是：cubemap 却是个例外，据说 cubemap 是沿用了一个叫做 RenderMan 的约定
        // 即传送 cubemap 的 face 的时候，data 的起始位置应该是图片的左上角像素（然后逐行向下扫描）
        // 所以在skyboxShader.vs中做了把y轴上下取负的操作
        // 也就是说，朝天空方向看去，纹理坐标实际上是指着地面方向了
        // 所以此处bottom和top要对调
        "../resources/skybox/right.jpg",
        "../resources/skybox/left.jpg",
        "../resources/skybox/bottom.jpg",
        "../resources/skybox/top.jpg",
        "../resources/skybox/front.jpg",
        "../resources/skybox/back.jpg"};
    unsigned int cubemap_texture = loadCubemap(faces);

    Model nanosuit("../resources/models/nanosuit_reflection/nanosuit.obj");
    // Model nanosuit("../resources/models/nanosuit/nanosuit.obj");
    // Model nanosuit("../resources/models/ganyu/ganyu.pmx");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("skybox", 0);

    screen_shader.use();
    screen_shader.setInt("screenTexture", 0);

    skybox_shader.use();
    skybox_shader.setInt("skybox", 0);

    model_shader.use();
    model_shader.setInt(
        "skybox",
        3); // 由于模型加载器本身就已经在着色器中占用了3个纹理单元了，所以需要将天空盒绑定到第4个纹理单元上（要从同一个着色器中对天空盒采样）
    model_shader.setVec3("envLight.ambient", 0.1f, 0.1f, 0.1f);
    model_shader.setVec3("envLight.diffuse", 1.0f, 1.0f, 1.0f);
    model_shader.setVec3("envLight.specular", 0.9f, 0.9f, 0.9f);

    // draw as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 6. 渲染循环(RenderLoop)
    while (!glfwWindowShouldClose(window)) // 检查GLFW是否被要求退出
    {
        // 6.2.1. 获得渲染时间差
        float current_frame = glfwGetTime();
        deltaTime           = current_frame - lastFrame;
        lastFrame           = current_frame;

        // 6.1. 输入控制
        processInput(window); // 键盘控制

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // 6.2.2. 清理屏幕
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 状态设置函数，设置glClear的填充色
        glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT); // 状态使用函数，用于清空屏幕的颜色|深度缓冲|模板缓冲，它接受一个缓冲位(Buffer
                                      // Bit)来指定要清空的缓冲

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view  = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // draw objects
        view = camera.getViewMatrix();
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // cubes
        glBindVertexArray(cube_vao);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setVec3("cameraPos", camera.position);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        // glBindVertexArray(planeVAO);
        // glBindTexture(GL_TEXTURE_2D, floorTexture);
        // model = glm::mat4(1.0f);
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // model
        model_shader.use();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.25f));
        model_shader.setMat4("projection", projection);
        model_shader.setMat4("view", view);
        model_shader.setMat4("model", model);
        model_shader.setVec3("viewPos", camera.position);
        nanosuit.draw(model_shader);

        // draw skybox
        glDepthFunc(
            GL_LEQUAL); // 由于我们强行把深度写为1.0，这样会导致1.0（skybox）=1.0（默认深度）错误，所以要改为小于等于，让skybox通过深度测试
        skybox_shader.use();
        skybox_shader.setMat4("projection", projection);
        view = glm::mat4(glm::mat3(camera.getViewMatrix())); // 移除变换矩阵的位移部分
        skybox_shader.setMat4("view", view);
        glBindVertexArray(skybox_vao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); // set depth function back to default

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't
                                              // be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screen_shader.use();
        glBindVertexArray(quad_vao);
        glBindTexture(GL_TEXTURE_2D,
                      texture_colorbuffer); // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 6.3. 检查并调用事件，交换缓冲
        glfwSwapBuffers(
            window); // window对象交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），在这一迭代中被用来绘制，并且将作为输出显示在屏幕上
        glfwPollEvents(); // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
    }

    // 7. 回收资源
    glfwTerminate(); // 释放/删除之前的分配的所有资源

    return 0;
}

/***
 * @: 对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用
 * @param {GLFWwindow} *window 被设置的窗口对象
 * @param {int} width 新维度W
 * @param {int} height 新维度H
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 注意：对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。
    glViewport(0, 0, width, height); // 告诉OpenGL渲染窗口的尺寸大小，即视口(Viewport)；前两个参数控制窗口左下角的位置
}

/***
 * @: 输入控制
 * @param {GLFWwindow} *window
 */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) ==
        GLFW_PRESS) // 检查用户是否按下了返回键(Esc)（如果没有按下，glfwGetKey将会返回GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true); // 把WindowShouldClose属性设置为true，从而关闭GLFW
    float camera_speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
}

/***
 * @: 鼠标控制的回调函数(GLFW的原型复写)
 * @param {GLFWwindow} *window
 * @param {double} xpos
 * @param {double} ypos
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX      = xpos;
        lastY      = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX         = xpos;
    lastY         = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

/***
 * @: 鼠标滚轮控制的回调函数(GLFW的原型复写)
 * @param {GLFWwindow} *window
 * @param {double} xoffset
 * @param {double} yoffset
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { camera.processMouseScroll(yoffset); }

/***
 * @: 从一个文件加载2D纹理
 * @param {char const*} 纹理贴图文件路径
 * @return {unsigned int} textureID
 */
unsigned int loadTexture(const char* path)
{
    // 1. 生成并绑定纹理对象
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // 2. 加载并生成纹理
    int width, height, nr_channels; // 宽度、高度和颜色通道的个数
    stbi_set_flip_vertically_on_load(
        true); // 设置加载图像时翻转y轴（OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部）
    unsigned char* data = stbi_load(path, &width, &height, &nr_channels, 0); // 加载图片，并填充宽、高、通道数
    if (data)
    {
        GLenum format;
        if (nr_channels == 1)
            format = GL_RED;
        else if (nr_channels == 3)
            format = GL_RGB;
        else if (nr_channels == 4)
            format = GL_RGBA;

        //纹理目标; 多级渐远纹理的级别; 纹理储存格式; 最终纹理的宽高; 总是被设为0（历史遗留的问题）;
        //源图的格式和数据类型(目前设置为char(byte)数组); 图像数据
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     width,
                     height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     data); // 让当前绑定的纹理对象附加上纹理图像

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

    return texture_id;
}

/***
 * @: 加载天空盒
 * @param {vector<std::string>} faces 包含6个纹理路径的vector
 * @return {unsigned int} textureID
 */
unsigned int loadCubemap(std::vector<std::string> faces)
{
    // order:
    // +X (right)
    // -X (left)
    // +Y (top)
    // -Y (bottom)
    // +Z (front)
    // -Z (back)
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width, height, nr_channels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}