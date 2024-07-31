#ifndef _SHADER_RESOURCES_HPP_
#define _SHADER_RESOURCES_HPP_

#include "Common/cpplang.hpp"

#include "Render/UniformBlock.hpp"
#include "Render/UniformSampler.hpp"

BEGIN_NAMESPACE(GLBase)

class ShaderResources
{
public:
    std::unordered_map<int, std::shared_ptr<UniformBlock>> blocks;
    std::unordered_map<int, std::shared_ptr<UniformSampler>> samplers;
};

END_NAMESPACE(GLBase)

#endif // _SHADER_RESOURCES_HPP_