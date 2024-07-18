#ifndef _SHADER_PROGRAM_HPP_
#define _SHADER_PROGRAM_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/FileUtils.hpp"
#include "Render/ProgramGLSL.hpp"
#include "Render/UniformBlock.hpp"

BEGIN_NAMESPACE(GLBase)

class ShaderProgram
{
public:
    int getId() const
    {
        return (int)m_programId;
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

    int getUniformBlockBinding()
    {
        return m_uniformBlockBinding++;
    }

    int getUniformSamplerBinding()
    {
        return m_uniformSamplerBinding++;
    }

private:
    bool bindUniform(UniformBlock &uniformBlock)
    {
        int hash = uniformBlock.getHash();
        int location = -1;
        if (m_uniformLocations.find(hash) == m_uniformLocations.end())
        {
            location = uniformBlock.getLocation(*this);
            m_uniformLocations[hash] = location;
        }
        else
        {
            location = m_uniformLocations[hash];
        }

        if (location < 0)
            return false;

        uniformBlock.bindProgram(*this, location);

        return true;
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