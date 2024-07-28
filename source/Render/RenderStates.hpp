#ifndef _RENDER_STATES_HPP_
#define _RENDER_STATES_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

BEGIN_NAMESPACE(GLBase)

struct ClearStates
{
    bool depthFlag = false;
    bool colorFlag = false;
    glm::vec4 clearColor = glm::vec4(0.0f);
    float clearDepth = 1.0f;
};

END_NAMESPACE(GLBase)

#endif // _RENDER_STATES_HPP_