#ifndef _RENDER_STATES_HPP_
#define _RENDER_STATES_HPP_

#include "Common/cpplang.hpp"

#include "Common/GLMInc.hpp"

BEGIN_NAMESPACE(GLBase)

enum class DepthFunction
{
    NEVER = 0,
    LESS,
    EQUAL,
    LEQUAL,
    GREATER,
    GEQUAL,
    NOTEQUAL,
    ALWAYS
};

enum class BlendFactor
{
    ZERO = 0,
    ONE,
    SRC_COLOR,
    SRC_ALPHA,
    DST_COLOR,
    DST_ALPHA,
    ONE_MINUS_SRC_COLOR,
    ONE_MINUS_SRC_ALPHA,
    ONE_MINUS_DST_COLOR,
    ONE_MINUS_DST_ALPHA
};

enum class BlendFunction
{
    ADD = 0,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX
};

struct BlendParameters
{
    BlendFunction blendFuncRgb = BlendFunction::ADD;
    BlendFactor blendSrcRgb = BlendFactor::ONE;
    BlendFactor blendDstRgb = BlendFactor::ZERO;

    BlendFunction blendFuncAlpha = BlendFunction::ADD;
    BlendFactor blendSrcAlpha = BlendFactor::ONE;
    BlendFactor blendDstAlpha = BlendFactor::ZERO;

    void setBlendFactor(BlendFactor src, BlendFactor dst)
    {
        blendSrcRgb = src;
        blendSrcAlpha = src;
        blendDstRgb = dst;
        blendDstAlpha = dst;
    }

    void setBlendFunction(BlendFunction func)
    {
        blendFuncRgb = func;
        blendFuncAlpha = func;
    }
};

enum class PrimitiveType
{
    POINT = 0,
    LINE,
    TRIANGLE,
};

enum class PolygonMode
{
    POINT = 0,
    LINE,
    FILL,
};

struct RenderStates
{
    bool blend = false;
    BlendParameters blendParams;

    bool depthTest = false;
    bool depthMask = true;
    DepthFunction depthFunc = DepthFunction::LESS;

    bool cullFace = false;
    PrimitiveType primitiveType = PrimitiveType::TRIANGLE;
    PolygonMode polygonMode = PolygonMode::FILL;
};

struct ClearStates
{
    bool depthFlag = false;
    bool colorFlag = false;
    glm::vec4 clearColor = glm::vec4(0.0f);
    float clearDepth = 1.0f;
};

END_NAMESPACE(GLBase)

#endif // _RENDER_STATES_HPP_