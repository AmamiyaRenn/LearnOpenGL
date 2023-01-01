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
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// third party headers
#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件，需要包含在gl文件(如glfw)前

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader
{
public:
    // 程序ID
    unsigned int id;

    Shader(const char* vertexFilePath, const char* fragmentFilePath);

    // 使用/激活程序
    void use() const { glUseProgram(id); }

    // uniform工具函数
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

/***
 * @: 构造器读取并构建着色器
 * @param {char} *vertexFilePath 顶点着色器代码路径
 * @param {char} *fragmentFilePath 片段着色器代码路径
 */
Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath)
{
    // 1. 从文件路径中获取顶点/片段着色器代码
    std::string   vertex_code;
    std::string   fragment_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;
    // 保证ifstream可以抛出异常
    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        v_shader_file.open(vertexFilePath);
        f_shader_file.open(fragmentFilePath);
        std::stringstream v_shader_stream, f_shader_stream;
        // 读取文件的缓冲内容到数据流中
        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();
        // 关闭文件处理器
        v_shader_file.close();
        f_shader_file.close();
        // 转换数据流到string
        vertex_code   = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vertex_shader_source   = vertex_code.c_str(); // 把string中的数据转化为char[]
    const char* fragment_shader_source = fragment_code.c_str();

    // 2. 编译着色器+链接着色器到程序中
    unsigned int vertex_shader, fragment_shader; // 创建着色器对象，这个对象的类型为unsigned int
    int          success;                        // 是否成功编译
    char         info_log[512];                  // 储存错误消息（如果有的话）的容器
    // 2.1. 顶点着色器
    vertex_shader = glCreateShader(GL_VERTEX_SHADER); // 把需要创建的着色器类型以参数形式提供
    glShaderSource(
        vertex_shader, 1, &vertex_shader_source, nullptr); // 要编译的着色器对象，传递的源码字符串数量，顶点着色器的源码
    glCompileShader(vertex_shader);                            // 编译目标shader
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success); // 检查是否编译成功
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);                           // 获取错误消息
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl; // 打印错误消息
    }
    // 2.2. 片段着色器
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }
    // 2.3. 链接着色器到程序
    id = glCreateProgram(); // 创造一个着色器程序对象(Shader Program Object)，并返回新创建程序对象的ID引用
    glAttachShader(id, vertex_shader); // 把之前编译的着色器附加到程序对象上
    glAttachShader(id, fragment_shader);
    glLinkProgram(id); // 要使用刚才编译的着色器，则必须把它们链接(Link)为一个着色器程序对象
    glGetProgramiv(id, GL_LINK_STATUS, &success); // 检查是否链接成功
    if (!success)
    {
        glGetProgramInfoLog(id, 512, nullptr, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }
    // 2.4. 删除已经链接到程序对象的着色器对象
    glDeleteShader(vertex_shader); // 把着色器对象链接到程序对象以后，可以删除着色器对象（不再需要这些了）
    glDeleteShader(fragment_shader);
}

#endif
