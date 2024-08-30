#ifndef _DEMO_SCENE_HPP_
#define _DEMO_SCENE_HPP_

#include "Common/cpplang.hpp"

#include "Model/Model.hpp"
#include "Model/ModelBase.hpp"

BEGIN_NAMESPACE(GLBase)

struct DemoScene
{
    std::shared_ptr<Model> model = nullptr;
    ModelMesh floor;
    ModelMesh cube;
    ModelMesh skybox;
};

END_NAMESPACE(GLBase)

#endif //_DEMO_SCENE_HPP_