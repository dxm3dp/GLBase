#ifndef _ENUMS_OPENGL_HPP_
#define _ENUMS_OPENGL_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Render/Material.hpp"
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

static inline glm::vec4 cvtBorderColor(BorderColor color)
{
    switch (color)
    {
        case BorderColor::BLACK:          return glm::vec4(0.f);
        case BorderColor::WHITE:          return glm::vec4(1.f);
        default:
            break;
    }
    return glm::vec4(0.f);
}

static inline GLint cvtDepthFunction(DepthFunction func)
{
    switch (func)
    {
        CASE_CVT_GL(DepthFunction::NEVER, NEVER);
        CASE_CVT_GL(DepthFunction::LESS, LESS);
        CASE_CVT_GL(DepthFunction::EQUAL, EQUAL);
        CASE_CVT_GL(DepthFunction::LEQUAL, LEQUAL);
        CASE_CVT_GL(DepthFunction::GREATER, GREATER);
        CASE_CVT_GL(DepthFunction::NOTEQUAL, NOTEQUAL);
        CASE_CVT_GL(DepthFunction::GEQUAL, GEQUAL);
        CASE_CVT_GL(DepthFunction::ALWAYS, ALWAYS);
        default:
            break;
    }
    return 0;
}

static inline GLint cvtBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        CASE_CVT_GL(BlendFactor::ZERO, ZERO);
        CASE_CVT_GL(BlendFactor::ONE, ONE);
        CASE_CVT_GL(BlendFactor::SRC_COLOR, SRC_COLOR);
        CASE_CVT_GL(BlendFactor::SRC_ALPHA, SRC_ALPHA);
        CASE_CVT_GL(BlendFactor::DST_COLOR, DST_COLOR);
        CASE_CVT_GL(BlendFactor::DST_ALPHA, DST_ALPHA);
        CASE_CVT_GL(BlendFactor::ONE_MINUS_SRC_COLOR, ONE_MINUS_SRC_COLOR);
        CASE_CVT_GL(BlendFactor::ONE_MINUS_SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
        CASE_CVT_GL(BlendFactor::ONE_MINUS_DST_COLOR, ONE_MINUS_DST_COLOR);
        CASE_CVT_GL(BlendFactor::ONE_MINUS_DST_ALPHA, ONE_MINUS_DST_ALPHA);
        default:
            break;
    }
    return 0;
}

static inline GLint cvtBlendFunction(BlendFunction func)
{
    switch (func)
    {
        case BlendFunction::ADD:               return GL_FUNC_ADD;
        case BlendFunction::SUBTRACT:          return GL_FUNC_SUBTRACT;
        case BlendFunction::REVERSE_SUBTRACT:  return GL_FUNC_REVERSE_SUBTRACT;
        case BlendFunction::MIN:               return GL_MIN;
        case BlendFunction::MAX:               return GL_MAX;
        default:
            break;
    }
    return 0;
}

static inline GLint cvtPolygonMode(PolygonMode mode)
{
    switch (mode)
    {
        CASE_CVT_GL(PolygonMode::POINT, POINT);
        CASE_CVT_GL(PolygonMode::LINE, LINE);
        CASE_CVT_GL(PolygonMode::FILL, FILL);
        default:
            break;
    }
    return 0;
}

END_NAMESPACE(GLBase)

#endif // _ENUMS_OPENGL_HPP_