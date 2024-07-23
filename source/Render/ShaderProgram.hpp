#ifndef _SHADER_PROGRAM_HPP_
#define _SHADER_PROGRAM_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/FileUtils.hpp"
#include "Render/ProgramGLSL.hpp"
#include "Render/ShaderResources.hpp"
#include "Render/UniformBase.hpp"

BEGIN_NAMESPACE(GLBase)

class ShaderProgram
{
public:
    int getId() const
    {
        return (int)m_programId;
    }

    void bindResources(ShaderResources &resources)
    {
        for (auto &kv : resources.blocks)
        {
            bindUniform(*kv.second, false);
        }

        for (auto &kv : resources.samplers)
        {
            bindUniform(*kv.second, true);
        }
    }

    bool compileAndLinkFile(const std::string &vsPath, const std::string &fsPath)
    {
        return compileAndLink(FileUtils::readText(vsPath), FileUtils::readText(fsPath));
    }

    bool compileAndLink(const std::string &vsSource, const std::string &fsSource)
    {
        bool ret = m_programGLSL.loadSource(vsSource, fsSource);
        m_programId = m_programGLSL.getId();

        return ret;
    }

    void use()
    {
        m_programGLSL.use();
        m_uniformBlockBinding = 0;
        m_uniformSamplerBinding = 0;
    }

private:
    bool bindUniform(UniformBase &uniform, bool isSampler)
    {
        int hash = uniform.getHash();
        int location = -1;
        if (m_uniformLocations.find(hash) == m_uniformLocations.end())
        {
            location = uniform.getLocation(m_programId);
            m_uniformLocations[hash] = location;
        }
        else
        {
            location = m_uniformLocations[hash];
        }

        if (location < 0)
            return false;

        int binding = isSampler ? getUniformSamplerBinding() : getUniformBlockBinding();
        uniform.bindProgram(m_programId, binding, location);

        return true;
    }

    int getUniformBlockBinding()
    {
        return m_uniformBlockBinding++;
    }

    int getUniformSamplerBinding()
    {
        return m_uniformSamplerBinding++;
    }

private:
    GLuint m_programId = 0;
    ProgramGLSL m_programGLSL;

    int m_uniformBlockBinding = 0;
    int m_uniformSamplerBinding = 0;

    std::unordered_map<int, int> m_uniformLocations;
};

END_NAMESPACE(GLBase)

#endif // _SHADER_PROGRAM_HPP_