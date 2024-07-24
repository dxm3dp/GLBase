#ifndef _ENUMS_OPENGL_HPP_
#define _ENUMS_OPENGL_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Render/Texture.hpp"

BEGIN_NAMESPACE(GLBase)

#define CASE_CVT_GL(PRE, TOKEN) case PRE: return GL_##TOKEN

static inline GLint cvtWrap(WrapMode mode)
{
    switch (mode)
    {
        CASE_CVT_GL(WrapMode::REPEAT, REPEAT);
        CASE_CVT_GL(WrapMode::MIRRORED_REPEAT, MIRRORED_REPEAT);
        CASE_CVT_GL(WrapMode::CLAMP_TO_EDGE, CLAMP_TO_EDGE);
        CASE_CVT_GL(WrapMode::CLAMP_TO_BORDER, CLAMP_TO_BORDER);
        default:
        break;
    }
    return GL_REPEAT;
}

static inline GLint cvtFilter(FilterMode mode)
{
    switch (mode)
    {
        CASE_CVT_GL(FilterMode::LINEAR, LINEAR);
        CASE_CVT_GL(FilterMode::NEAREST, NEAREST);
        CASE_CVT_GL(FilterMode::LINEAR_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR);
        CASE_CVT_GL(FilterMode::LINEAR_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST);
        CASE_CVT_GL(FilterMode::NEAREST_MIPMAP_LINEAR, NEAREST_MIPMAP_LINEAR);
        CASE_CVT_GL(FilterMode::NEAREST_MIPMAP_NEAREST, NEAREST_MIPMAP_NEAREST);
        default:
            break;
    }
    return GL_NEAREST;
}

END_NAMESPACE(GLBase)

#endif // _ENUMS_OPENGL_HPP_