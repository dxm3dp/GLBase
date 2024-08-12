#ifndef _MODEL_LOADER_HPP_
#define _MODEL_LOADER_HPP_

#include "Common/cpplang.hpp"

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Common/GLMInc.hpp"

#include "Common/Buffer.hpp"
#include "Common/ImageUtils.hpp"
#include "Common/ThreadPool.hpp"
#include "Model/Cube.hpp"
#include "Model/Model.hpp"
#include "Render/DemoScene.hpp"

BEGIN_NAMESPACE(GLBase)

class ModelLoader
{
public:
    void loadFloor(ModelMesh &mesh, glm::mat4 transform = glm::mat4(1.0f))
    {
        mesh.vertices.push_back({glm::vec3(25.0f, -0.5f, 25.0f), glm::vec2(25.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        mesh.vertices.push_back({glm::vec3(-25.0f, -0.5f, 25.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        mesh.vertices.push_back({glm::vec3(-25.0f, -0.5f, -25.0f), glm::vec2(0.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        mesh.vertices.push_back({glm::vec3(25.0f, -0.5f, -25.0f), glm::vec2(25.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f)});

        mesh.indices.push_back(0);
        mesh.indices.push_back(2);
        mesh.indices.push_back(1);
        mesh.indices.push_back(0);
        mesh.indices.push_back(3);
        mesh.indices.push_back(2);

        mesh.transform = transform;

        mesh.material = std::make_shared<Material>(); // create a new material
        mesh.material->shadingModel = ShadingModel::BlinnPhong;
        mesh.material->baseColor = glm::vec4(1.0f);

        std::string texturePath = "../assets/Textures/wood.png";
        auto buffer = loadTextureFile(texturePath); // create a new buffer and cache
        if (buffer)
        {
            auto &texData = mesh.material->textureData[(int)MaterialTexType::ALBEDO];
            texData.tag = texturePath;
            texData.width = buffer->getWidth();
            texData.height = buffer->getHeight();
            texData.data = {buffer};
        }
        else
        {
            LOGE("ModelLoader::loadFloor loadTextureFile failed: %s, path: %s", Material::materialTexTypeStr(MaterialTexType::ALBEDO), texturePath.c_str());
        }

        mesh.InitVertexArray();
    }

    void loadCube(ModelMesh &mesh, glm::mat4 transform = glm::mat4(1.0f))
    {
        const float *cubeVertices = Cube::getVertices();

        for(int i = 0; i < 12; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                Vertex vertex{};
                vertex.position.x = cubeVertices[i * 24 + j * 8 + 0];
                vertex.position.y = cubeVertices[i * 24 + j * 8 + 1];
                vertex.position.z = cubeVertices[i * 24 + j * 8 + 2];
                vertex.normal.x = cubeVertices[i * 24 + j * 8 + 3];
                vertex.normal.y = cubeVertices[i * 24 + j * 8 + 4];
                vertex.normal.z = cubeVertices[i * 24 + j * 8 + 5];
                vertex.texCoords.x = cubeVertices[i * 24 + j * 8 + 6];
                vertex.texCoords.y = cubeVertices[i * 24 + j * 8 + 7];
                mesh.vertices.push_back(vertex);
                mesh.indices.push_back(i * 3 + j);
            }
        }

        mesh.transform = transform;

        mesh.material = std::make_shared<Material>();// create a new material
        mesh.material->shadingModel = ShadingModel::BlinnPhong;
        mesh.material->baseColor = glm::vec4(1.0f);
        mesh.material->alphaMode = AlphaMode::Opaque;

        std::string texturePath = "../assets/Textures/wood.png";
        auto buffer = loadTextureFile(texturePath);
        if (buffer)
        {
            auto &texData = mesh.material->textureData[(int)MaterialTexType::ALBEDO];
            texData.tag = texturePath;
            texData.width = buffer->getWidth();
            texData.height = buffer->getHeight();
            texData.data = {buffer};
        }
        else
        {
            LOGE("ModelLoader::loadFloor loadTextureFile failed: %s, path: %s", Material::materialTexTypeStr(MaterialTexType::ALBEDO), texturePath.c_str());
        }

        mesh.InitVertexArray();
    }

    bool loadModel(const std::string &path, glm::mat4 transform = glm::mat4(1.0f))
    {
        std::lock_guard<std::mutex> lock(m_modelLoadMutex);

        if (path.empty())
        {
            LOGE("ModelLoader::loadModel, path is empty");
            return false;
        }

        auto it = m_modelCache.find(path);
        if (it != m_modelCache.end())
        {
            m_scene.model = it->second;
            return true;
        }

        m_modelCache[path] = std::make_shared<Model>();
        m_scene.model = m_modelCache[path];

        Assimp::Importer importer;
        aiScene const *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOGE("Assimp error: %s", importer.GetErrorString());
            return false;
        }

        m_scene.model->resourcePath = path.substr(0, path.find_last_of('/'));

        preloadTextureFiles(scene, m_scene.model->resourcePath);

        if (!processNode(scene->mRootNode, scene, m_scene.model->rootNode, transform))
        {
            LOGE("ModelLoader::loadModel, process node failed.");
            return false;
        }

        return true;
    }

    void preloadTextureFiles(const aiScene *scene, const std::string &resDir)
    {
        std::set<std::string> texPaths;
		for (int materialIdx = 0; materialIdx < scene->mNumMaterials; materialIdx++)
        {
			aiMaterial* material = scene->mMaterials[materialIdx];
			for (int texType = aiTextureType_NONE; texType <= AI_TEXTURE_TYPE_MAX; texType++)
            {
				auto textureType = static_cast<aiTextureType>(texType);
				size_t texCnt = material->GetTextureCount(textureType);
				for (size_t i = 0; i < texCnt; i++)
                {
					aiString textPath;
					aiReturn retStatus = material->GetTexture(textureType, i, &textPath);
					if (retStatus != aiReturn_SUCCESS || textPath.length == 0)
                    {
						continue;
					}
					texPaths.insert(resDir + "/" + textPath.C_Str());
				}
			}
		}
		if (texPaths.empty())
        {
			return;
		}

        ThreadPool pool(std::min(texPaths.size(), (size_t)std::thread::hardware_concurrency()));
        for(auto &path : texPaths)
        {
            pool.pushTask([&](int thread_id)
            {
                loadTextureFile(path);
            });
        }
    }

    std::shared_ptr<Buffer<RGBA>> loadTextureFile(const std::string &path)
    {
        m_texCacheMutex.lock();
        if (m_textureDataCache.find(path) != m_textureDataCache.end())
        {
            m_texCacheMutex.unlock();
            return m_textureDataCache[path];
        }
        m_texCacheMutex.unlock();

        auto buffer = ImageUtils::readImageRGBA(path);
        if (nullptr == buffer)
        {
            LOGE("ModelLoader::loadTextureFile, failed to load texture with path: %s", path.c_str());
            return nullptr;
        }

        m_texCacheMutex.lock();
        m_textureDataCache[path] = buffer;
        m_texCacheMutex.unlock();

        return buffer;
    }

    bool processNode(aiNode *ai_node, const aiScene *ai_scene, ModelNode &outNode, glm::mat4 &transform)
    {
        if (nullptr == ai_node)
        {
            return false;
        }

        outNode.transform = transform * convertMatrix(ai_node->mTransformation);

        for (size_t i = 0; i < ai_node->mNumMeshes; i++)
        {
			const aiMesh* meshPtr = ai_scene->mMeshes[ai_node->mMeshes[i]];
            if (meshPtr != nullptr)
            {
                ModelMesh mesh;
                if (processMesh(meshPtr, ai_scene, mesh))
                {
                    // to do other

                    outNode.meshes.push_back(std::move(mesh));
                }
            }
        }

        for (size_t i = 0; i < ai_node->mNumChildren; i++)
        {
			ModelNode childNode;
			if (processNode(ai_node->mChildren[i], ai_scene, childNode, outNode.transform))
            {
				outNode.children.push_back(std::move(childNode));
			}
		}

        return true;
    }

    bool processMesh(const aiMesh* ai_mesh, const aiScene* ai_scene, ModelMesh& outMesh)
    {
	    std::vector<Vertex> vertices;
		std::vector<int> indices;
        for (size_t i = 0; i < ai_mesh->mNumVertices; i++)
        {
			Vertex vertex{};
			if (ai_mesh->HasPositions())
            {
				vertex.position.x = ai_mesh->mVertices[i].x;
				vertex.position.y = ai_mesh->mVertices[i].y;
				vertex.position.z = ai_mesh->mVertices[i].z;
			}
			if (ai_mesh->HasTextureCoords(0))
            {
				vertex.texCoords.x = ai_mesh->mTextureCoords[0][i].x;
				vertex.texCoords.y = ai_mesh->mTextureCoords[0][i].y;
			}
            else
            {
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			if (ai_mesh->HasNormals())
            {
				vertex.normal.x = ai_mesh->mNormals[i].x;
				vertex.normal.y = ai_mesh->mNormals[i].y;
				vertex.normal.z = ai_mesh->mNormals[i].z;
			}
            else
            {
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }
			if (ai_mesh->HasTangentsAndBitangents())
            {
				vertex.tangent.x = ai_mesh->mTangents[i].x;
				vertex.tangent.y = ai_mesh->mTangents[i].y;
				vertex.tangent.z = ai_mesh->mTangents[i].z;
			}
            else
            {
                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
			vertices.push_back(vertex);
		}

        for (size_t i = 0; i < ai_mesh->mNumFaces; i++)
        {
			aiFace face = ai_mesh->mFaces[i];
			if (face.mNumIndices != 3)
            {
				LOGE("ModelLoader::processMesh, mesh not transformed to triangle mesh.");
				return false;
			}
			for (size_t j = 0; j < face.mNumIndices; ++j)
            {
				indices.push_back((int)(face.mIndices[j]));
			}
		}

        outMesh.material = std::make_shared<Material>();
        outMesh.material->baseColor = glm::vec4(1.0f);
        if (ai_mesh->mMaterialIndex >= 0)
        {
			const aiMaterial* material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

			// alpha mode
            outMesh.material->alphaMode = AlphaMode::Opaque;
            aiString alphaMode;
            if (material->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode) == aiReturn_SUCCESS)
            {
                if (aiString("BLEND") == alphaMode)
                {
                    outMesh.material->alphaMode = AlphaMode::Blend;
                }
            }

			// double side
            outMesh.material->doubleSided = false;
			bool doubleSide;
			if (material->Get(AI_MATKEY_TWOSIDED, doubleSide) == aiReturn_SUCCESS)
            {
				outMesh.material->doubleSided = doubleSide;
			}

			// shading mode
			outMesh.material->shadingModel = ShadingModel::BlinnPhong; // default
			aiShadingMode shading_mode;
			if (material->Get(AI_MATKEY_SHADING_MODEL, shading_mode) == aiReturn_SUCCESS) {
				if (aiShadingMode_PBR_BRDF == shading_mode)
                {
					outMesh.material->shadingModel = ShadingModel::PBR;
				}
			}

            for (int i = 0; i <= AI_TEXTURE_TYPE_MAX; i++)
            {
				processMaterial(material, static_cast<aiTextureType>(i), *outMesh.material);
			}
        }

        outMesh.vertices = std::move(vertices);
        outMesh.indices = std::move(indices);

        LOGI("vertex count: %d, index count: %d", outMesh.vertices.size(), outMesh.indices.size());

        outMesh.InitVertexArray();

        return true;
    }

    void processMaterial(const aiMaterial *ai_material, aiTextureType ai_texType, Material &outMaterial)
    {
        if (ai_material->GetTextureCount(ai_texType) <= 0)
			return;

        for (size_t i = 0; i < ai_material->GetTextureCount(ai_texType); i++)
        {
			aiTextureMapMode texMapMode[2]; // [u, v]
			aiString texPath;
			aiReturn retStatus = ai_material->GetTexture(ai_texType, i, &texPath, nullptr, nullptr, nullptr, nullptr, &texMapMode[0]);
			if (retStatus != aiReturn_SUCCESS || texPath.length == 0)
            {
				LOGW("load texture type=%d, index=%d failed with return value=%d", ai_texType, i, retStatus);
				continue;
			}

            std::string absolutePath = m_scene.model->resourcePath + "/" + texPath.C_Str();
            MaterialTexType texType = MaterialTexType::NONE;
			switch (ai_texType) {
			case aiTextureType_BASE_COLOR:
			case aiTextureType_DIFFUSE:
				texType = MaterialTexType::ALBEDO;
				break;
			case aiTextureType_NORMALS:
				texType = MaterialTexType::NORMAL;
				break;
			case aiTextureType_EMISSIVE:
				texType = MaterialTexType::EMISSIVE;
				break;
			case aiTextureType_LIGHTMAP:
				texType = MaterialTexType::AMBIENT_OCCLUSION;
				break;
            case aiTextureType_SPECULAR:
                texType = MaterialTexType::SPECULAR;
                break;
			case aiTextureType_UNKNOWN:
				texType = MaterialTexType::METAL_ROUGHNESS;
				break;
			default:
				LOGW("texture type: %s not support", aiTextureTypeToString(ai_texType));
				continue; // not support
			}

            auto buffer = loadTextureFile(absolutePath);
			if (buffer)
            {
				auto& texData = outMaterial.textureData[(int)texType];
				texData.tag = absolutePath;
				texData.width = buffer->getWidth();
				texData.height = buffer->getHeight();
				texData.data = {buffer};
			}
            else
            {
				LOGE("load texture failed: %s, path: %s", Material::materialTexTypeStr(texType), absolutePath.c_str());
			}
        }
    }

    DemoScene& getScene()
    {
        return m_scene;
    }

    glm::mat4 convertMatrix(const aiMatrix4x4& m)
    {
		glm::mat4 ret;
		for (int i = 0; i < 4; i++)
        {
			for (int j = 0; j < 4; j++)
            {
				ret[j][i] = m[i][j];
			}
		}
		return ret;
	}

private:
    DemoScene m_scene;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_modelCache;
    std::unordered_map<std::string, std::shared_ptr<Buffer<RGBA>>> m_textureDataCache;
    std::mutex m_modelLoadMutex;
    std::mutex m_texCacheMutex;
};

END_NAMESPACE(GLBase)

#endif // _MODEL_LOADER_HPP_