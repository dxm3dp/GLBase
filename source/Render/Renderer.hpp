#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "Common/cpplang.hpp"

#include "Common/HashUtils.hpp"
#include "Config/Config.hpp"
#include "Model/ModelBase.hpp"
#include "Render/DemoScene.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/RenderStates.hpp"
#include "Render/ShaderProgram.hpp"
#include "Render/Texture2D.hpp"
#include "Render/UniformBlock.hpp"
#include "Render/UniformSampler.hpp"
#include "Viewer/Camera.hpp"

BEGIN_NAMESPACE(GLBase)

#define SHADOW_MAP_WIDTH 1024
#define SHADOW_MAP_HEIGHT 1024

#define CREATE_UNIFORM_BLOCK(name) createUniformBlock(#name, sizeof(name))

#define CASE_CREATE_SHADER_GL(shading, source) case shading: \
  return program.compileAndLinkFile(SHADER_GLSL_DIR + #source + ".vert", \
                                       SHADER_GLSL_DIR + #source + ".frag")

class Renderer
{
public:
    void create(std::shared_ptr<Camera> &camera, DemoScene &scene)
    {
        m_cameraMain = camera;
        m_scene = scene;

        m_cameraCurrent = m_cameraMain.get();

        if (nullptr == m_cameraDepth)
        {
            m_cameraDepth = std::make_shared<Camera>();
            m_cameraDepth->setPerspective(glm::radians(CAMERA_FOV), (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT, CAMERA_NEAR, CAMERA_FAR);
        }

        m_uniformBlockScene = CREATE_UNIFORM_BLOCK(UniformsScene);
        m_uniformBlockModel = CREATE_UNIFORM_BLOCK(UniformsModel);
        m_uniformBlockMaterial = CREATE_UNIFORM_BLOCK(UniformsMaterial);

        m_shadowPlaceholder = createTexture2DDefault(1, 1, TextureFormat::FLOAT32, (int)TextureUsage::Sampler, false);
    }

    void destroy()
    {
        // to do
    }

    void drawFrame()
    {
        setupShadowMapBuffer();

        setupScene();

        drawShadowMap();

        drawMainPass();
    }

    void setupScene()
    {
        pipelineSetup(m_scene.floor, m_scene.floor.material->shadingModel, {(int)GLBase::UniformBlockType::Scene, (int)GLBase::UniformBlockType::Model, (int)GLBase::UniformBlockType::Material});

        pipelineSetup(m_scene.cube, m_scene.cube.material->shadingModel, {(int)GLBase::UniformBlockType::Scene, (int)GLBase::UniformBlockType::Model, (int)GLBase::UniformBlockType::Material});

        setupModelNode(m_scene.model->rootNode);
    }

    void drawScene(bool shadowPass)
    {
        updateUniformScene();
        updateUniformModel(glm::mat4(1.0f), m_cameraCurrent->getViewMatrix());

        if (!shadowPass)
        {
            updateUniformModel(m_scene.floor.transform, m_cameraCurrent->getViewMatrix());
            drawModelMesh(m_scene.floor, shadowPass, 0.5f);
        }

        updateUniformModel(m_scene.cube.transform, m_cameraCurrent->getViewMatrix());
        drawModelMesh(m_scene.cube, shadowPass, 0.5f);

        ModelNode &rootNode = m_scene.model->rootNode;
        drawModelNode(rootNode, shadowPass);
    }

private:
    void setupModelNode(ModelNode &node)
    {
        for (auto &mesh : node.meshes)
        {
            pipelineSetup(mesh, mesh.material->shadingModel, {(int)UniformBlockType::Scene, (int)UniformBlockType::Model, (int)UniformBlockType::Material});
        }

        for (auto &child : node.children)
        {
            setupModelNode(child);
        }
    }

    void drawModelNode(ModelNode &node, bool shadowPass)
    {
        glm::mat4 modelMatrix = node.transform;

        updateUniformModel(modelMatrix, m_cameraCurrent->getViewMatrix());

        for(auto &mesh : node.meshes)
        {
            drawModelMesh(mesh, shadowPass, 0.5f);
        }

        for(auto &child : node.children)
        {
            drawModelNode(child, shadowPass);
        }
    }

    void drawModelMesh(ModelMesh &mesh, bool shadowPass, float specular)
    {
        updateUniformMaterial(*mesh.material, specular);

        updateShadowTextures(mesh.material->materialObj.get(), shadowPass);

        pipelineDraw(mesh);
    }

    void pipelineSetup(ModelMesh &model, ShadingModel shadingModel, const std::set<int> &uniformBlocks)
    {
        setupVertexArray(model);

        setupMaterial(model, shadingModel, uniformBlocks);
    }

    void pipelineDraw(ModelMesh &model)
    {
        // set vao
        setVertexArrayObject(model.vao);

        // set shader program
        setShaderProgram(model.material->materialObj->shaderProgram);

        // set shader resources
        setShaderResources(model.material->materialObj->shaderResources);

        // draw
        glDrawElements(GL_TRIANGLES, (GLsizei) model.vao->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    void drawMainPass()
    {
        ClearStates clearStates{};
        clearStates.colorFlag = true;
        clearStates.depthFlag = true;
        clearStates.clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);
        clearStates.clearDepth = 1.0f;

        beginRenderPass(clearStates);
        setViewport(0, 0, 1024, 1024);

        drawScene(false);

        endRenderPass();
    }

    void drawShadowMap()
    {
        ClearStates clearStates{};
        clearStates.depthFlag = true;
        clearStates.clearDepth = 1.0f;

        beginRenderPass(m_fboShadow, clearStates);
        setViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

        m_cameraDepth->lookat(glm::vec3(5.0f, 5.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_cameraCurrent = m_cameraDepth.get();

        drawScene(true);

        endRenderPass();

        m_cameraCurrent = m_cameraMain.get();
    }

    void beginRenderPass(std::shared_ptr<Framebuffer> &fbo, const ClearStates &clearStates)
    {
        fbo->bind();

        GLbitfield clearBit = 0;
        if(clearStates.colorFlag)
        {
            glClearColor(clearStates.clearColor.r, clearStates.clearColor.g, clearStates.clearColor.b, clearStates.clearColor.a);
            clearBit |= GL_COLOR_BUFFER_BIT;
        }
        if(clearStates.depthFlag)
        {
            glClearDepth(clearStates.clearDepth);
            clearBit |= GL_DEPTH_BUFFER_BIT;
        }

        glClear(clearBit);
    }

    void beginRenderPass(const ClearStates &clearStates)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLbitfield clearBit = 0;
        if(clearStates.colorFlag)
        {
            glClearColor(clearStates.clearColor.r, clearStates.clearColor.g, clearStates.clearColor.b, clearStates.clearColor.a);
            clearBit |= GL_COLOR_BUFFER_BIT;
        }
        if(clearStates.depthFlag)
        {
            glClearDepth(clearStates.clearDepth);
            clearBit |= GL_DEPTH_BUFFER_BIT;
        }

        glClear(clearBit);
    }

    void endRenderPass()
    {
        // do nothing
    }

    void setViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }

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

        if (material.shadingModel != ShadingModel::Skybox)
        {
            material.textures[(int)MaterialTexType::SHADOWMAP] = m_shadowPlaceholder;
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
        program->addDefines(material.shaderDefines);

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
            CASE_CREATE_SHADER_GL(ShadingModel::PBR, BlinnPhongWS);
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
        uniformScene.u_cameraPosition = m_cameraCurrent->position();
        uniformScene.u_pointLightPosition = glm::vec3(5.0f, 5.0f, 3.0f);
        uniformScene.u_pointLightColor = glm::vec3(0.6f, 0.5f, 0.9f);

        m_uniformBlockScene->setData(&uniformScene, sizeof(UniformsScene));
    }

    void updateUniformModel(const glm::mat4 &model, const glm::mat4 &view)
    {
        static UniformsModel uniformModel{};

        uniformModel.u_modelMatrix = model;
        uniformModel.u_modelViewProjectionMatrix = m_cameraCurrent->getPerspectiveMatrix() * view * model;
        uniformModel.u_inverseTransposeModelMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        if (m_cameraDepth != nullptr)
        {
            const glm::mat4 biasMatrix = {0.5f, 0.0f, 0.0f, 0.0f,
                                          0.0f, 0.5f, 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0f,
                                          0.5f, 0.5f, 0.0f, 1.0f};
            uniformModel.u_shadowMVPMatrix = biasMatrix * m_cameraDepth->getPerspectiveMatrix() * m_cameraDepth->getViewMatrix() * model;
        }

        m_uniformBlockModel->setData(&uniformModel, sizeof(UniformsModel));
    }

    void updateUniformMaterial(Material &material, float specular)
    {
        static UniformsMaterial uniformMaterial{};

        uniformMaterial.u_baseColor = material.baseColor;
        uniformMaterial.u_kSpecular = specular;

        m_uniformBlockMaterial->setData(&uniformMaterial, sizeof(UniformsMaterial));
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

    void setupShadowMapBuffer()
    {
        if (nullptr == m_fboShadow)
        {
            m_fboShadow = createFramebuffer(true);
        }

        if (nullptr == m_texDepthShadow)
        {
            TextureDesc texDesc{};
            texDesc.width = SHADOW_MAP_WIDTH;
            texDesc.height = SHADOW_MAP_HEIGHT;
            texDesc.type = TextureType::Texture2D;
            texDesc.format = TextureFormat::FLOAT32;
            texDesc.usage = (int)TextureUsage::Sampler | (int)TextureUsage::AttachmentDepth;
            texDesc.useMipmaps = false;
            texDesc.multiSample = false;
            m_texDepthShadow = createTexture(texDesc);

            SamplerDesc sampler{};
            sampler.filterMin = FilterMode::NEAREST;
            sampler.filterMag = FilterMode::NEAREST;
            sampler.wrapS = WrapMode::CLAMP_TO_BORDER;
            sampler.wrapT = WrapMode::CLAMP_TO_BORDER;
            sampler.borderColor = BorderColor::WHITE;
            m_texDepthShadow->setSamplerDesc(sampler);

            m_texDepthShadow->initImageData();
            m_fboShadow->setDepthAttachment(m_texDepthShadow);
        }

        if (!m_fboShadow->isValid())
        {
            LOGE("setupShadowMapBuffer failed");
        }
    }

    std::shared_ptr<Framebuffer> createFramebuffer(bool offscreen)
    {
        return std::make_shared<Framebuffer>(offscreen);
    }

    void updateShadowTextures(MaterialObject *materialObj, bool shadowPass)
    {
        if (nullptr == materialObj->shaderResources)
            return;

        auto &samplers = materialObj->shaderResources->samplers;
        if (shadowPass)
        {
            samplers[(int)MaterialTexType::SHADOWMAP]->setTexture(m_shadowPlaceholder);
        }
        else
        {
            samplers[(int)MaterialTexType::SHADOWMAP]->setTexture(m_texDepthShadow);
        }
    }

    std::shared_ptr<Texture> createTexture2DDefault(int width, int height, TextureFormat format, uint32_t usage, bool mipmaps)
    {
        TextureDesc texDesc{};
        texDesc.width = width;
        texDesc.height = height;
        texDesc.type = TextureType::Texture2D;
        texDesc.format = format;
        texDesc.usage = usage;
        texDesc.useMipmaps = mipmaps;
        texDesc.multiSample = false;

        auto texture2d = createTexture(texDesc);
        if (nullptr == texture2d)
            return nullptr;

        SamplerDesc sampler{};
        sampler.filterMin = mipmaps ? FilterMode::LINEAR_MIPMAP_LINEAR : FilterMode::LINEAR;
        sampler.filterMag = FilterMode::LINEAR;
        texture2d->setSamplerDesc(sampler);

        texture2d->initImageData();
        return texture2d;
    }

private:
    DemoScene m_scene;

    std::shared_ptr<Camera> m_cameraMain = nullptr;
    std::shared_ptr<Camera> m_cameraDepth = nullptr;
    Camera *m_cameraCurrent = nullptr;

    ShaderProgram *m_shaderProgram = nullptr;

    // caches
    std::unordered_map<size_t, std::shared_ptr<ShaderProgram>> m_programCache;

    // uniform blocks
    std::shared_ptr<UniformBlock> m_uniformBlockScene;
    std::shared_ptr<UniformBlock> m_uniformBlockModel;
    std::shared_ptr<UniformBlock> m_uniformBlockMaterial;

    // shadow map
    std::shared_ptr<Framebuffer> m_fboShadow = nullptr;
    std::shared_ptr<Texture> m_texDepthShadow = nullptr;
    std::shared_ptr<Texture> m_shadowPlaceholder = nullptr;
};

END_NAMESPACE(GLBase)

#endif // _RENDERER_HPP_