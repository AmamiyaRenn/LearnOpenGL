/***
 * @Description:着色器类
 * @Author: Amamiya
 * @Date: 2022-04-23 15:17:48
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
#ifndef _SHADER_H
#define _SHADER_H

// Standard Headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

// System Headers
#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    Shader(const char *vertexFilePath, const char *fragmentFilePath);

    // 使用/激活程序
    void use() { glUseProgram(ID); }

    // uniform工具函数
    void setBool(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
    void setInt(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
    void setFloat(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
    void setVec2(const std::string &name, const glm::vec2 &value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec2(const std::string &name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
    void setVec3(const std::string &name, const glm::vec3 &value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec3(const std::string &name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
    void setVec4(const std::string &name, const glm::vec4 &value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec4(const std::string &name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
    void setMat2(const std::string &name, const glm::mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void setMat3(const std::string &name, const glm::mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void setMat4(const std::string &name, const glm::mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
};

/***
 * @: 构造器读取并构建着色器
 * @param {char} *vertexFilePath 顶点着色器代码路径
 * @param {char} *fragmentFilePath 片段着色器代码路径
 */
Shader::Shader(const char *vertexFilePath, const char *fragmentFilePath)
{
    // 1. 从文件路径中获取顶点/片段着色器代码
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream可以抛出异常
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        vShaderFile.open(vertexFilePath);
        fShaderFile.open(fragmentFilePath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vertexShaderSource = vertexCode.c_str(); // 把string中的数据转化为char[]
    const char *fragmentShaderSource = fragmentCode.c_str();

    // 2. 编译着色器+链接着色器到程序中
    unsigned int vertexShader, fragmentShader; // 创建着色器对象，这个对象的类型为unsigned int
    int success;                               // 是否成功编译
    char infoLog[512];                         // 储存错误消息（如果有的话）的容器
    // 2.1. 顶点着色器
    vertexShader = glCreateShader(GL_VERTEX_SHADER);            // 把需要创建的着色器类型以参数形式提供
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 要编译的着色器对象，传递的源码字符串数量，顶点着色器的源码
    glCompileShader(vertexShader);                              // 编译目标shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);   // 检查是否编译成功
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // 获取错误消息
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl; // 打印错误消息
    }
    // 2.2. 片段着色器
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 2.3. 链接着色器到程序
    ID = glCreateProgram();           // 创造一个着色器程序对象(Shader Program Object)，并返回新创建程序对象的ID引用
    glAttachShader(ID, vertexShader); // 把之前编译的着色器附加到程序对象上
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);                            // 要使用刚才编译的着色器，则必须把它们链接(Link)为一个着色器程序对象
    glGetProgramiv(ID, GL_LINK_STATUS, &success); // 检查是否链接成功
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // 2.4. 删除已经链接到程序对象的着色器对象
    glDeleteShader(vertexShader); // 把着色器对象链接到程序对象以后，可以删除着色器对象（不再需要这些了）
    glDeleteShader(fragmentShader);
}

#endif
