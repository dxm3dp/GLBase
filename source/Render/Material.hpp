#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

#include "Common/Buffer.hpp"
#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

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
};

class Material
{
public:
    ShadingModel shadingModel = ShadingModel::Unknown;
    glm::vec4 baseColor = glm::vec4(1.0f);

    std::unordered_map<int, std::shared_ptr<TextureData>> textureData;
    std::unordered_map<int, std::shared_ptr<Texture>> textures;
};

END_NAMESPACE(GLBase)

#endif // _MATERIAL_HPP_