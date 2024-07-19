#ifndef _MODEL_LOADER_HPP_
#define _MODEL_LOADER_HPP_

#include "Common/cpplang.hpp"

#include "Common/Buffer.hpp"
#include "Common/GLMInc.hpp"
#include "Common/ImageUtils.hpp"
#include "Render/DemoScene.hpp"

BEGIN_NAMESPACE(GLBase)

class ModelLoader
{
public:
    explicit ModelLoader() {}

public:
    void loadFloor()
    {
        m_scene.floor.vertices.push_back({glm::vec3(25.0f, -0.5f, 25.0f), glm::vec2(25.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        m_scene.floor.vertices.push_back({glm::vec3(-25.0f, -0.5f, 25.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        m_scene.floor.vertices.push_back({glm::vec3(-25.0f, -0.5f, -25.0f), glm::vec2(0.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f)});
        m_scene.floor.vertices.push_back({glm::vec3(25.0f, -0.5f, -25.0f), glm::vec2(25.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f)});

        m_scene.floor.indices.push_back(0);
        m_scene.floor.indices.push_back(2);
        m_scene.floor.indices.push_back(1);
        m_scene.floor.indices.push_back(0);
        m_scene.floor.indices.push_back(3);
        m_scene.floor.indices.push_back(2);

        m_scene.floor.material = std::make_shared<Material>();
        m_scene.floor.material->shadingModel = ShadingModel::BaseColor;
        m_scene.floor.material->baseColor = glm::vec4(1.0f);
        m_scene.floor.InitVertexArray();
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

private:
    DemoScene m_scene;
    std::unordered_map<std::string, std::shared_ptr<Buffer<RGBA>>> m_textureDataCache;
    std::mutex m_texCacheMutex;
};

END_NAMESPACE(GLBase)

#endif // _MODEL_LOADER_HPP_