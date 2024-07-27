#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

struct FramebufferAttachment
{
    std::shared_ptr<Texture> tex = nullptr;
    uint32_t layer = 0;
    uint32_t level = 0;
};

class Framebuffer
{
public:
    explicit Framebuffer(bool offscreen) : m_offscreen(offscreen)
    {
        glGenFramebuffers(1, &m_fbo);
    }

    ~Framebuffer()
    {
        if (m_fbo != 0)
        {
            glDeleteFramebuffers(1, &m_fbo);
        }
    }

public:
    int getId()
    {
        return (int)m_fbo;
    }

    bool isValid()
    {
        if (m_fbo == 0)
            return false;

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            LOGE("glCheckFramebufferStatus error: %x", status);
            return false;
        }

        return true;
    }

    void setColorAttachment(std::shared_ptr<Texture> &color, int level)
    {
        if (color == m_colorAttachment.tex && level == m_colorAttachment.level)
            return;

        m_colorAttachment.tex = color;
        m_colorAttachment.layer = 0;
        m_colorAttachment.level = level;
        m_colorReady = true;

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               color->multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
                               color->getId(),
                               level);
    }

    void setDepthAttachment(std::shared_ptr<Texture> &depth)
    {
        if (depth == m_depthAttachment.tex)
            return;

        m_depthAttachment.tex = depth;
        m_depthAttachment.layer = 0;
        m_depthAttachment.level = 0;
        m_depthReady = true;

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_COMPONENT,
                               depth->multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
                               depth->getId(),
                               0);
    }

    void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    const FramebufferAttachment &getColorAttachment() const
    {
        return m_colorAttachment;
    }

    const FramebufferAttachment &getDepthAttachment() const
    {
        return m_depthAttachment;
    }

    bool isColorReady() const
    {
        return m_colorReady;
    }

    bool isDepthReady() const
    {
        return m_depthReady;
    }

    bool isOffscreen() const
    {
        return m_offscreen;
    }

    void setOffscreen(bool offscreen)
    {
        m_offscreen = offscreen;
    }

private:
    bool m_offscreen = false;
    bool m_colorReady = false;
    bool m_depthReady = false;
    FramebufferAttachment m_colorAttachment{};
    FramebufferAttachment m_depthAttachment{};

    GLuint m_fbo = 0;
};

END_NAMESPACE(GLBase)

#endif // _FRAMEBUFFER_HPP_