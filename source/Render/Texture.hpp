#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/Buffer.hpp"

BEGIN_NAMESPACE(GLBase)

enum class WrapMode
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class FilterMode
{
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR,
};

enum class BorderColor
{
    BLACK = 0,
    WHITE,
};

struct SamplerDesc
{
    FilterMode filterMin = FilterMode::NEAREST;
    FilterMode filterMag = FilterMode::NEAREST;

    WrapMode wrapS = WrapMode::CLAMP_TO_EDGE;
    WrapMode wrapT = WrapMode::CLAMP_TO_EDGE;
    WrapMode wrapR = WrapMode::CLAMP_TO_EDGE;

    BorderColor borderColor = BorderColor::BLACK;
};

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

enum class TextureUsage
{
    Sampler = 1 << 0,
    UploadData = 1 << 1,
    AttachmentColor = 1 << 2,
    AttachmentDepth = 1 << 3,
    RendererOutput = 1 << 4,
};

struct TextureDesc
{
    int width = 0;
    int height = 0;
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::RGBA8;
    uint32_t usage = (uint32_t)TextureUsage::Sampler;
    bool useMipmaps = false;
    bool multiSample = false;
    std::string tag;
};

struct TextureOpenGLDesc
{
    GLint internalformat;
    GLenum format;
    GLenum type;
};

class Texture : public TextureDesc
{
public:
    static TextureOpenGLDesc getOpenGLDesc(TextureFormat format)
    {
        TextureOpenGLDesc ret{};

        switch(format)
        {
            case TextureFormat::RGBA8:
                ret.internalformat = GL_RGBA8;
                ret.format = GL_RGBA;
                ret.type = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::FLOAT32:
                ret.internalformat = GL_DEPTH_COMPONENT;
                ret.format = GL_DEPTH_COMPONENT;
                ret.type = GL_FLOAT;
                break;
        }

        return ret;
    }

public:
    int getId() const
    {
        return (int)m_texId;
    }

    virtual void setSamplerDesc(SamplerDesc &sampler){};
    virtual void initImageData() {};
    virtual void setImageData(const std::vector<std::shared_ptr<Buffer<RGBA>>> &buffers){};

protected:
    GLuint m_texId = 0;
    TextureOpenGLDesc m_glDesc{};
};

END_NAMESPACE(GLBase)

#endif // _TEXTURE_HPP_