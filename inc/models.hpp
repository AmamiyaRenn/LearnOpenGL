/***
 * @Description: 模型类
 * @Author: Amamiya
 * @Date: 2022-05-01 08:56:17
 * @TechChangeTheWorld
 * @WHUROBOCON_Alright_Reserved
 */
#ifndef _MODELS_H
#define _MODELS_H

// User Headers
#include "meshes.hpp"

// third party headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
    /*  函数   */
    Model(const char *path) { loadModel(path); }
    void Draw(Shader shader);

private:
    /*  模型数据  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    /*  函数   */
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); // 三角形化|翻转y轴

    if (scene == nullptr || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    { // 场景为NULL|根节点为NULL|模型数据不完整
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 处理当前节点所有的网格
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 处理子节点所有的网格
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

void Model::Draw(Shader shader)
{
    for (auto mesh : meshes)
        mesh.Draw(shader);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // 处理顶点位置
        vector.x = mesh->mVertices[i].x; // 使用这样一个临时变量的原因是Assimp对向量、矩阵、字符串等都有自己的一套数据类型，它们并不能完美地转换到GLM的数据类型中
        vector.y = mesh->mVertices[i].y; // TODO：对于上述注释，需要做测试
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // 处理法线
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // 处理纹理坐标
        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            glm::vec2 textureVec;
            textureVec.x = mesh->mTextureCoords[0][i].x; // Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标, 但目前只关心第一组
            textureVec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = textureVec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f);
        // 填充
        vertices.push_back(vertex);
    }
    // 索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // 材质

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str); // 注意，我们假设了模型文件中纹理文件的路径是相对于模型文件的本地(Local)路径，比如说与模型文件处于同一目录下。我们可以将纹理位置字符串拼接到之前（在loadModel中）获取的目录字符串上，来获取完整的纹理路径（这也是为什么GetTexture函数也需要一个目录字符串）
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++)
            if (std::strcmp(str.C_Str(), textures_loaded[j].path.data()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
            std::cout << "loaded texture.type " << typeName << " texture.id " << texture.id << std::endl;
        }
    }
    return textures;
}

/***
 * @:
 * @param {char} *path 纹理的文件位置
 * @param {string} &directory 文件所属目录
 * @param {bool} gamma 是否开启gamma校正
 * @return {unsigned int} textureID
 */
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif
