#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/Buffer.hpp"

BEGIN_NAMESPACE(GLBase)

enum class TextureType
{
    Texture2D = 0,
    TextureCube,
};

enum class TextureFormat
{
    RGBA8 = 0,
    FLOAT32,
};

struct TextureDesc
{
    int width = 0;
    int height = 0;
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::RGBA8;
};

class Texture : public TextureDesc
{
public:
    int getId() const
    {
        return (int)m_texId;
    }

private:
    GLuint m_texId = 0;
};

END_NAMESPACE(GLBase)

#endif // _TEXTURE_HPP_