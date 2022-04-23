/***
 * @Description:着色器类
 * @Author: Amamiya
 * @Date: 2022-04-23 15:17:48
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
#ifndef _SHADER_H
#define _SHADER_H

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char *vertexFilePath, const char *fragmentFilePath);

    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};

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

    // 2. 编译着色器
    unsigned int vertexShader, fragmentShader;                  // 创建着色器对象，这个对象的类型为unsigned int
    int success;                                                // 是否成功编译
    char infoLog[512];                                          // 储存错误消息（如果有的话）的容器
    vertexShader = glCreateShader(GL_VERTEX_SHADER);            // 把需要创建的着色器类型以参数形式提供
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 要编译的着色器对象，传递的源码字符串数量，顶点着色器的源码
    glCompileShader(vertexShader);                              // 编译目标shader
}

#endif
