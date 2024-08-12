#ifndef _PIPELINE_STATES_HPP_
#define _PIPELINE_STATES_HPP_

#include "Common/cpplang.hpp"

#include "Render/RenderStates.hpp"

BEGIN_NAMESPACE(GLBase)

class PipelineStates
{
public:
    explicit PipelineStates(const RenderStates &states) : renderStates(states) {}

    virtual ~PipelineStates() = default;

public:
    RenderStates renderStates;
};

END_NAMESPACE(GLBase)

#endif // _PIPELINE_STATES_HPP_