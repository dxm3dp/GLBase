#ifndef _SHADOW_MAPPING_HPP_
#define _SHADOW_MAPPING_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

BEGIN_NAMESPACE(GLBase)

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class ShadowMapping
{
public:
    ShadowMapping()
    {
        // create depth texture
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // attach depth texture as FBO's depth buffer
        glGenFramebuffers(1, &m_depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

public:
    unsigned int getDepthMapFBO() const
    {
        return m_depthMapFBO;
    }

    unsigned int getDepthMap() const
    {
        return m_depthMap;
    }

private:
    unsigned int m_depthMapFBO;
    unsigned int m_depthMap;
};

END_NAMESPACE(GLBase)

#endif // _SHADOW_MAPPING_HPP_
