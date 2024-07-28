#ifndef _TEXTURE_2D_HPP_
#define _TEXTURE_2D_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include "Common/GLMInc.hpp"

#include "Render/EnumsOpenGL.hpp"
#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

class Texture2D : public Texture
{
public:
    explicit Texture2D(const TextureDesc &desc)
    {
        assert(desc.type == TextureType::Texture2D);

        width = desc.width;
        height = desc.height;
        type = TextureType::Texture2D;
        format = desc.format;
        usage = desc.usage;
        useMipmaps = desc.useMipmaps;
        multiSample = desc.multiSample;
        m_target = multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        m_glDesc = getOpenGLDesc(format);
        glGenTextures(1, &m_texId);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &m_texId);
    }

public:
    void setSamplerDesc(SamplerDesc &sampler) override
    {
        if (multiSample)
            return;

        glBindTexture(m_target, m_texId);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, cvtWrap(sampler.wrapS));
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, cvtWrap(sampler.wrapT));
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, cvtFilter(sampler.filterMin));
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, cvtFilter(sampler.filterMag));

        glm::vec4 borderColor = cvtBorderColor(sampler.borderColor);
        glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
    }

    void initImageData() override
    {
        glBindTexture(m_target, m_texId);
        if (multiSample)
        {
            glTexImage2DMultisample(m_target, 4, m_glDesc.internalformat, width, height, GL_TRUE);
        }
        else
        {
            glTexImage2D(m_target, 0, m_glDesc.internalformat, width, height, 0, m_glDesc.format, m_glDesc.type, nullptr);

            if (useMipmaps)
            {
                glGenerateMipmap(m_target);
            }
        }
    }

    void setImageData(const std::vector<std::shared_ptr<Buffer<RGBA>>> &buffers) override
    {
        if (multiSample)
        {
            LOGE("setImageData not support: multi sample texture");
            return;
        }

        if (format != TextureFormat::RGBA8)
        {
            LOGE("setImageData error: format not match");
            return;
        }

        if (width != buffers[0]->getWidth() || height != buffers[0]->getHeight())
        {
            LOGE("setImageData error: size not match");
            return;
        }

        glBindTexture(m_target, m_texId);
        glTexImage2D(m_target, 0, m_glDesc.internalformat, width, height, 0, m_glDesc.format, m_glDesc.type, buffers[0]->getRawDataPtr());

        if (useMipmaps)
        {
            glGenerateMipmap(m_target);
        }
    }

private:
    GLenum m_target = 0;
};

END_NAMESPACE(GLBase)

#endif // _TEXTURE_2D_HPP_