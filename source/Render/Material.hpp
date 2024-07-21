#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

#include "Common/Buffer.hpp"
#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

#define CASE_ENUM_STR(type) case type: return #type

struct TextureData
{
    std::string tag;
    size_t width = 0;
    size_t height = 0;
    std::shared_ptr<Buffer<RGBA>> data = nullptr;
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

class Material
{
public:
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

public:
    ShadingModel shadingModel = ShadingModel::Unknown;
    glm::vec4 baseColor = glm::vec4(1.0f);

    std::unordered_map<int, TextureData> textureData;
    std::unordered_map<int, std::shared_ptr<Texture>> textures;
};



END_NAMESPACE(GLBase)

#endif // _MATERIAL_HPP_