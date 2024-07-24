#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

#include "Common/Buffer.hpp"
#include "Render/ShaderProgram.hpp"
#include "Render/ShaderResources.hpp"
#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

#define CASE_ENUM_STR(type) case type: return #type

struct TextureData
{
    std::string tag;
    size_t width = 0;
    size_t height = 0;
    std::vector<std::shared_ptr<Buffer<RGBA>>> data;
    WrapMode wrapModeU = WrapMode::REPEAT;
    WrapMode wrapModeV = WrapMode::REPEAT;
    WrapMode wrapModeW = WrapMode::REPEAT;
};

enum class ShadingModel
{
    Unknown = 0,
    BaseColor,
    BlinnPhong,
    PBR,
};

enum MaterialTexType
{
  NONE = 0,

  ALBEDO,
  NORMAL,
  EMISSIVE,
  AMBIENT_OCCLUSION,
  METAL_ROUGHNESS,

  CUBE,
  EQUIRECTANGULAR,

  IBL_IRRADIANCE,
  IBL_PREFILTER,

  QUAD_FILTER,

  SHADOWMAP,
};

enum class UniformBlockType
{
    Scene,
    Model,
    Material,
};

struct UniformsScene
{
    glm::vec3 u_ambientColor;
    glm::vec3 u_cameraPosition;
    //glm::vec3 u_pointLightPosition;
    //glm::vec3 u_pointLightColor;
};

struct UniformsModel
{
    glm::mat4 u_modelMatrix;
    glm::mat4 u_modelViewProjectionMatrix;
    glm::mat3 u_inverseTransposeModelMatrix;
    //glm::mat4 u_shadowMVPMatrix;
};

struct UniformsMaterial
{
    glm::float32_t u_kSpecular;
    glm::vec4 u_baseColor;
};

class MaterialObject
{
public:
    ShadingModel shadingModel = ShadingModel::Unknown;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ShaderResources> shaderResources;
};

class Material
{
public:
    static const char *shadingModelStr(ShadingModel model)
    {
        switch (model)
        {
            CASE_ENUM_STR(ShadingModel::Unknown);
            CASE_ENUM_STR(ShadingModel::BaseColor);
            CASE_ENUM_STR(ShadingModel::BlinnPhong);
            CASE_ENUM_STR(ShadingModel::PBR);
            //CASE_ENUM_STR(ShadingModel::Skybox);
            //CASE_ENUM_STR(ShadingModel::IBL_Irradiance);
            //CASE_ENUM_STR(ShadingModel::IBL_Prefilter);
            //CASE_ENUM_STR(ShadingModel::FXAA);
            default:
                break;
        }
        return "";
    }

    static const char *materialTexTypeStr(MaterialTexType usage)
    {
        switch (usage)
        {
            CASE_ENUM_STR(MaterialTexType::NONE);
            CASE_ENUM_STR(MaterialTexType::ALBEDO);
            CASE_ENUM_STR(MaterialTexType::NORMAL);
            CASE_ENUM_STR(MaterialTexType::EMISSIVE);
            CASE_ENUM_STR(MaterialTexType::AMBIENT_OCCLUSION);
            CASE_ENUM_STR(MaterialTexType::METAL_ROUGHNESS);
            CASE_ENUM_STR(MaterialTexType::CUBE);
            CASE_ENUM_STR(MaterialTexType::EQUIRECTANGULAR);
            CASE_ENUM_STR(MaterialTexType::IBL_IRRADIANCE);
            CASE_ENUM_STR(MaterialTexType::IBL_PREFILTER);
            CASE_ENUM_STR(MaterialTexType::QUAD_FILTER);
            CASE_ENUM_STR(MaterialTexType::SHADOWMAP);
            default:
                break;
        }
        return "";
    }

    static const char *samplerName(MaterialTexType usage)
    {
        switch (usage)
        {
            case MaterialTexType::ALBEDO:             return "u_albedoMap";
            case MaterialTexType::NORMAL:             return "u_normalMap";
            case MaterialTexType::EMISSIVE:           return "u_emissiveMap";
            case MaterialTexType::AMBIENT_OCCLUSION:  return "u_aoMap";
            case MaterialTexType::METAL_ROUGHNESS:    return "u_metalRoughnessMap";
            case MaterialTexType::CUBE:               return "u_cubeMap";
            case MaterialTexType::EQUIRECTANGULAR:    return "u_equirectangularMap";
            case MaterialTexType::IBL_IRRADIANCE:     return "u_irradianceMap";
            case MaterialTexType::IBL_PREFILTER:      return "u_prefilterMap";
            case MaterialTexType::QUAD_FILTER:        return "u_screenTexture";
            case MaterialTexType::SHADOWMAP:          return "u_shadowMap";
            default:
                break;
        }

        return nullptr;
    }

public:
    ShadingModel shadingModel = ShadingModel::Unknown;
    glm::vec4 baseColor = glm::vec4(1.0f);

    std::unordered_map<int, TextureData> textureData;
    std::unordered_map<int, std::shared_ptr<Texture>> textures;
    std::shared_ptr<MaterialObject> materialObj = nullptr;
};



END_NAMESPACE(GLBase)

#endif // _MATERIAL_HPP_