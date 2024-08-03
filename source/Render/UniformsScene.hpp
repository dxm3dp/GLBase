#ifndef _UNIFORMS_SCENE_HPP_
#define _UNIFORMS_SCENE_HPP_

#include "Common/cpplang.hpp"

#include "Common/GLMInc.hpp"

BEGIN_NAMESPACE(GLBase)

struct UniformsScene
{
    glm::vec3 u_lightPos;
    glm::vec3 u_viewPos;
};

END_NAMESPACE(GLBase)

#endif // _UNIFORMS_SCENE_HPP_