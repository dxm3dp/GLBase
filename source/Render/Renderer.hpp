#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "Common/cpplang.hpp"

#include "Config/Config.hpp"
#include "Model/ModelBase.hpp"
#include "Render/ShaderProgram.hpp"

// 加载模型
// 针对每个模型
//  配置管线
//  绘制模型

BEGIN_NAMESPACE(GLBase)

#define CASE_CREATE_SHADER_GL(shading, source) case shading: \
  return program.compileAndLinkFile(SHADER_GLSL_DIR + #source + ".vert", \
                                       SHADER_GLSL_DIR + #source + ".frag")

class Renderer
{
public:
    void pipelineSetup(ModelBase &model, ShadingModel shadingModel)
    {
        setupVertexArray(model);

        setupMaterial(model, shadingModel);
    }

    void PipelineDraw(ModelBase &model)
    {

    }

    void setupVertexArray(ModelBase &model)
    {
        if (nullptr == model.vao)
        {
            model.vao = std::make_shared<VertexArrayObject>(model);
        }
    }

    void setupMaterial(ModelBase &model, ShadingModel shadingModel)
    {
        // 开始
        auto program = std::make_shared<ShaderProgram>();
        bool success = loadShaders(*program, shadingModel);
    }

    bool loadShaders(ShaderProgram &program, ShadingModel shadingModel)
    {
        switch (shadingModel)
        {
            CASE_CREATE_SHADER_GL(ShadingModel::BaseColor, BaseColor);
            CASE_CREATE_SHADER_GL(ShadingModel::BlinnPhong, BlinnPhong);
            default:
                break;
        }

        return false;
    }

private:

};

END_NAMESPACE(GLBase)

#endif // _RENDERER_HPP_