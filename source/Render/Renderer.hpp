#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "Common/cpplang.hpp"

#include "Common/HashUtils.hpp"
#include "Config/Config.hpp"
#include "Model/ModelBase.hpp"
#include "Render/ShaderProgram.hpp"
#include "Render/Texture2D.hpp"
#include "Render/UniformBlock.hpp"
#include "Render/UniformSampler.hpp"
#include "Viewer/Camera.hpp"

BEGIN_NAMESPACE(GLBase)

#define CREATE_UNIFORM_BLOCK(name) createUniformBlock(#name, sizeof(name))

#define CASE_CREATE_SHADER_GL(shading, source) case shading: \
  return program.compileAndLinkFile(SHADER_GLSL_DIR + #source + ".vert", \
                                       SHADER_GLSL_DIR + #source + ".frag")

class Renderer
{
public:
    void create(std::shared_ptr<Camera> &camera)
    {
        m_camera = camera;
        m_uniformBlockScene = CREATE_UNIFORM_BLOCK(UniformsScene);
        m_uniformBlockModel = CREATE_UNIFORM_BLOCK(UniformsModel);
        m_uniformBlockMaterial = CREATE_UNIFORM_BLOCK(UniformsMaterial);
    }

    void pipelineSetup(ModelMesh &model, ShadingModel shadingModel, const std::set<int> &uniformBlocks)
    {
        setupVertexArray(model);

        setupMaterial(model, shadingModel, uniformBlocks);
    }

    void pipelineDraw(ModelMesh &model)
    {
        // update uniforms
        updateUniformScene();
        updateUniformModel(model.transform, m_camera->getViewMatrix());
        updateUniformMaterial(*model.material, 0.5f);

        // set vao
        setVertexArrayObject(model.vao);

        // set shader program
        setShaderProgram(model.material->materialObj->shaderProgram);

        // set shader resources
        setShaderResources(model.material->materialObj->shaderResources);

        // draw
        glDrawElements(GL_TRIANGLES, (GLsizei) model.vao->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

private:
    void setupVertexArray(ModelBase &model)
    {
        if (nullptr == model.vao)
        {
            model.vao = std::make_shared<VertexArrayObject>(model);
        }
    }

    void setupMaterial(ModelBase &model, ShadingModel shadingModel, const std::set<int> &uniformBlocks)
    {
        auto &material = *model.material;
        if (material.textures.empty())
        {
            setupTextures(material);
            material.shaderDefines = generateShaderDefines(material);
        }

        if (nullptr == material.materialObj)
        {
            material.materialObj = std::make_shared<MaterialObject>();
            material.materialObj->shadingModel = shadingModel;

            if (setupShaderProgram(material, shadingModel))
            {
                setupSamplerUniforms(material);
            }

            for(auto &key : uniformBlocks)
            {
                std::shared_ptr<UniformBlock> uniform = nullptr;
                switch(key)
                {
                    case (int)UniformBlockType::Scene:
                        uniform = m_uniformBlockScene;
                        break;
                    case (int)UniformBlockType::Model:
                        uniform = m_uniformBlockModel;
                        break;
                    case (int)UniformBlockType::Material:
                        uniform = m_uniformBlockMaterial;
                        break;
                    default:
                        break;
                }
                if (uniform != nullptr)
                {
                    material.materialObj->shaderResources->blocks[key] = uniform;
                }
            }
        }
    }

    void setupTextures(Material &material)
    {
        for(auto &kv : material.textureData)
        {
            TextureDesc texDesc{};
            texDesc.width = kv.second.width;
            texDesc.height = kv.second.height;
            texDesc.format = TextureFormat::RGBA8;
            texDesc.usage = (int)TextureUsage::Sampler | (int)TextureUsage::UploadData;
            texDesc.useMipmaps = true;
            texDesc.multiSample = false;

            SamplerDesc sampler{};
            sampler.wrapS = kv.second.wrapModeU;
            sampler.wrapT = kv.second.wrapModeV;
            sampler.filterMin = FilterMode::LINEAR_MIPMAP_LINEAR;
            sampler.filterMag = FilterMode::LINEAR_MIPMAP_LINEAR;

            std::shared_ptr<Texture> texture = nullptr;
            switch(kv.first)
            {
                default:
                    texDesc.type = TextureType::Texture2D;
                    sampler.filterMin = FilterMode::LINEAR_MIPMAP_LINEAR;
                    break;
            }
            texture = createTexture(texDesc);
            texture->setSamplerDesc(sampler);
            texture->setImageData(kv.second.data);
            texture->tag = kv.second.tag;
            material.textures[kv.first] = texture;
        }
    }

    bool setupShaderProgram(Material &material, ShadingModel shadingModel)
    {
        size_t cacheKey = getShaderProgramCacheKey(shadingModel, material.shaderDefines);

        auto cachedProgram = m_programCache.find(cacheKey);
        if (cachedProgram != m_programCache.end())
        {
            material.materialObj->shaderProgram = cachedProgram->second;
            material.materialObj->shaderResources = std::make_shared<ShaderResources>();
            return true;
        }

        auto program = createShaderProgram();

        // to do addDefine

        bool success = loadShaders(*program, shadingModel);
        if (success)
        {
            m_programCache[cacheKey] = program;
            material.materialObj->shaderProgram = program;
            material.materialObj->shaderResources = std::make_shared<ShaderResources>();
        }
        else
        {
            LOGE("setupShaderProgram failed: %s", Material::shadingModelStr(shadingModel));
        }

        return success;
    }

    void setupSamplerUniforms(Material &material)
    {
        for (auto &kv : material.textures)
        {
            const char *samplerName = Material::samplerName((MaterialTexType) kv.first);
            if (samplerName != nullptr)
            {
                auto uniform = createUniformSampler(samplerName, *kv.second);
                uniform->setTexture(kv.second);
                material.materialObj->shaderResources->samplers[kv.first] = std::move(uniform);
            }
        }
    }

    std::shared_ptr<Texture> createTexture(const TextureDesc &desc)
    {
        switch(desc.type)
        {
            case TextureType::Texture2D:
                return std::make_shared<Texture2D>(desc);
            default:
                break;
        }

        return nullptr;
    }

    std::shared_ptr<ShaderProgram> createShaderProgram()
    {
        return std::make_shared<ShaderProgram>();
    }

    std::shared_ptr<UniformSampler> createUniformSampler(const std::string &name, const TextureDesc &desc)
    {
        return std::make_shared<UniformSampler>(name, desc.type, desc.format);
    }

    bool loadShaders(ShaderProgram &program, ShadingModel shadingModel)
    {
        switch (shadingModel)
        {
            CASE_CREATE_SHADER_GL(ShadingModel::BaseColor, BasicGLSL);
            CASE_CREATE_SHADER_GL(ShadingModel::BlinnPhong, BlinnPhongWS);
            default:
                break;
        }

        return false;
    }

    size_t getShaderProgramCacheKey(ShadingModel shadingModel, const std::set<std::string> &shaderDefines)
    {
        size_t seed = 0;
        HashUtils::hashCombine(seed, (int)shadingModel);
        for(auto &define : shaderDefines)
        {
            HashUtils::hashCombine(seed, define);
        }

        return seed;
    }

    std::shared_ptr<UniformBlock> createUniformBlock(const std::string &name, int size)
    {
        return std::make_shared<UniformBlock>(name, size);
    }

    void updateUniformScene()
    {
        static UniformsScene uniformScene{};

        uniformScene.u_ambientColor = glm::vec3(0.4f, 0.4f, 0.4f);
        uniformScene.u_cameraPosition = m_camera->position();
        uniformScene.u_pointLightPosition = glm::vec3(2.0f, 3.0f, 2.0f);
        uniformScene.u_pointLightColor = glm::vec3(0.6f, 0.5f, 0.9f);

        m_uniformBlockScene->setData(&uniformScene, sizeof(uniformScene));
    }

    void updateUniformModel(const glm::mat4 &model, const glm::mat4 &view)
    {
        static UniformsModel uniformModel{};

        uniformModel.u_modelMatrix = model;
        uniformModel.u_modelViewProjectionMatrix = m_camera->getPerspectiveMatrix() * view * model;
        uniformModel.u_inverseTransposeModelMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        m_uniformBlockModel->setData(&uniformModel, sizeof(uniformModel));
    }

    void updateUniformMaterial(Material &material, float specular)
    {
        static UniformsMaterial uniformMaterial{};

        uniformMaterial.u_baseColor = material.baseColor;
        uniformMaterial.u_kSpecular = specular;

        m_uniformBlockMaterial->setData(&uniformMaterial, sizeof(uniformMaterial));
    }

    void setVertexArrayObject(std::shared_ptr<VertexArrayObject> &vao)
    {
        if (nullptr == vao)
            return;

        vao->bind();
    }

    void setShaderProgram(std::shared_ptr<ShaderProgram> &program)
    {
        if (nullptr == program)
            return;

        m_shaderProgram = program.get();
        m_shaderProgram->use();
    }

    void setShaderResources(std::shared_ptr<ShaderResources> &resources)
    {
        if (nullptr == resources)
            return;

        if (m_shaderProgram != nullptr)
        {
            m_shaderProgram->bindResources(*resources);
        }
    }

    std::set<std::string> generateShaderDefines(Material &material)
    {
        std::set<std::string> shaderDefines;
        for(auto &kv : material.textures)
        {
            const char * samplerDefine = material.samplerDefine((MaterialTexType)kv.first);
            if (samplerDefine != nullptr)
            {
                shaderDefines.insert(samplerDefine);
            }
        }

        return shaderDefines;
    }

private:
    std::shared_ptr<Camera> m_camera = nullptr;
    ShaderProgram *m_shaderProgram = nullptr;

    // caches
    std::unordered_map<size_t, std::shared_ptr<ShaderProgram>> m_programCache;

    // uniform blocks
    std::shared_ptr<UniformBlock> m_uniformBlockScene;
    std::shared_ptr<UniformBlock> m_uniformBlockModel;
    std::shared_ptr<UniformBlock> m_uniformBlockMaterial;
};

END_NAMESPACE(GLBase)

#endif // _RENDERER_HPP_