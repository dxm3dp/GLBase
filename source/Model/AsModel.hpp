#ifndef _AS_MODEL_HPP_
#define _AS_MODEL_HPP_

#include "Common/cpplang.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Common/Logger.hpp"
#include "Model/Mesh.hpp"

BEGIN_NAMESPACE(GLBase)

class AsModel
{
public:
    AsModel(const std::string &filename)
    {
        Assimp::Importer importer;
        aiScene const *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOGE("Assimp error: %s", importer.GetErrorString());
            return;
        }

        m_directory = filename.substr(0, filename.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

public:
    void draw(ShaderProgram &shader)
    {
        for(int i = 0; i < m_meshes.size(); i++)
        {
            m_meshes[i].draw(shader);
        }
    }

private:
    void processNode(aiNode *node, const aiScene *scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;
            if (mesh->HasTextureCoords(0))
            {
                vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.texCoords = glm::vec2(0.f, 0.f);
            }
            if (mesh->HasNormals())
            {
                vertex.normal.x = mesh->mNormals[i].x;
                vertex.normal.y = mesh->mNormals[i].y;
                vertex.normal.z = mesh->mNormals[i].z;
            }
            else
            {
                vertex.normal = glm::vec3(0.f, 0.f, 0.f);
            }
            vertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseTex = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseTex.begin(), diffuseTex.end());
        std::vector<Texture> specularTex = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularTex.begin(), specularTex.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        unsigned int texCount = material->GetTextureCount(type);
        for(unsigned int i = 0; i < texCount; i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);

            bool skip = false;
            for(int j = 0; j < m_textures.size(); j++)
            {
                if (std::strcmp(m_textures[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(m_textures[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.type = typeName;
                texture.path = str.C_Str();
                std::string path = m_directory + '/' + str.C_Str();
                glGenTextures(1, &texture.id);

                int width, height, nrComponents;
                unsigned char *image = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
                if (image != nullptr)
                {
                    GLenum format;
                    if (nrComponents == 1)
                    {
                        format = GL_RED;
                    }
                    else if (nrComponents == 3)
                    {
                        format = GL_RGB;
                    }
                    else if (nrComponents == 4)
                    {
                        format = GL_RGBA;
                    }

                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    stbi_image_free(image);
                }
                else
                {
                    LOGE("Failed to load texture at path : %s", path.c_str());
                    stbi_image_free(image);
                }

                textures.push_back(texture);
                m_textures.push_back(texture);
            }
        }

        return textures;
    }

private:
    std::vector<Mesh> m_meshes;
    std::vector<Texture> m_textures;
    std::string m_directory;
};

END_NAMESPACE(GLBase)

#endif // _AS_MODEL_HPP_