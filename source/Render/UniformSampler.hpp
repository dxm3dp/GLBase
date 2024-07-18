#ifndef _UNIFORM_SAMPLER_HPP_
#define _UNIFORM_SAMPLER_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Render/ShaderProgram.hpp"
#include "Render/Texture.hpp"
#include "Render/UniformBase.hpp"

BEGIN_NAMESPACE(GLBase)

#define BIND_TEX_OPENGL(n) case n: glActiveTexture(GL_TEXTURE##n); break;

class UniformSampler : UniformBase
{
public:
    explicit UniformSampler(const std::string &name, TextureType type, TextureFormat format) : UniformBase(name), m_texType(type), m_texFormat(format) {}

public:
    int getLocation(ShaderProgram &program) override
    {
        return glGetUniformLocation(program.getId(), name.c_str());
    }

    void bindProgram(ShaderProgram &program, int location) override
    {
        if (location < 0)
            return;

        int binding = program.getUniformSamplerBinding();
        switch (binding)
        {
            BIND_TEX_OPENGL(0)
            BIND_TEX_OPENGL(1)
            BIND_TEX_OPENGL(2)
            BIND_TEX_OPENGL(3)
            BIND_TEX_OPENGL(4)
            BIND_TEX_OPENGL(5)
            BIND_TEX_OPENGL(6)
            BIND_TEX_OPENGL(7)
            default:
                LOGE("UniformSampler::bindProgram error: texture unit not support");
                break;
        }
        glBindTexture(m_texTarget, m_texId);
        glUniform1i(location, binding);
    }

    void setTexture(const std::shared_ptr<Texture> &tex)
    {
        switch (tex->type)
        {
            case TextureType::Texture2D:
                m_texTarget = GL_TEXTURE_2D;
                break;
            case TextureType::TextureCube:
                m_texTarget = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                LOGE("UniformSampler::setTexture error: texture type not support");
                break;
        }
        m_texId = tex->getId();
    }

private:
    TextureType m_texType;
    TextureFormat m_texFormat;
    GLuint m_texTarget = 0;
    GLuint m_texId = 0;
};

END_NAMESPACE(GLBase)

#endif // _UNIFORM_SAMPLER_HPP_