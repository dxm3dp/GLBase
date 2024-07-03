#ifndef _AS_MODEL_HPP_
#define _AS_MODEL_HPP_

#include "Common/cpplang.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

        processNode(scene->mRootNode, scene);
    }

public:
    void draw()
    {
        for(int i = 0; i < m_meshes.size(); i++)
        {
            m_meshes[i].draw();
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

        return Mesh(vertices, indices);
    }

private:
    std::vector<Mesh> m_meshes;
};

END_NAMESPACE(GLBase)

#endif // _AS_MODEL_HPP_