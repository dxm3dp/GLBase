#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "Common/cpplang.hpp"

#include "Model/ModelBase.hpp"

BEGIN_NAMESPACE(GLBase)

struct ModelNode
{
    glm::mat4 transform = glm::mat4(1.0f);
    std::vector<ModelMesh> meshes;
    std::vector<ModelNode> children;
};

struct Model
{
    std::string resourcePath;
    ModelNode rootNode;
    glm::mat4 centeredMatrix;
};

END_NAMESPACE(GLBase)

#endif // _MODEL_HPP_