#ifndef _PROGRAM_GLSL_HPP_
#define _PROGRAM_GLSL_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#include "Common/GLMInc.hpp"

#include "Common/OpenGLUtils.hpp"
#include "Render/GLSLUtils.hpp"

BEGIN_NAMESPACE(GLBase)

constexpr char const * OpenGL_GLSL_DEFINE = "OpenGL";

class ProgramGLSL
{
public:
    ProgramGLSL()
    {
        addDefine(OpenGL_GLSL_DEFINE);
    }

    ~ProgramGLSL()
    {
        destroy();
    }

public:
    void addDefine(const std::string &def)
    {
        if (def.empty())
            return;

        m_defines += ("#define " + def + "\n");
    }

    bool loadSource(const std::string &vsSource, const std::string &fsSource)
    {
        GLSLUtils vs(GL_VERTEX_SHADER);
        GLSLUtils fs(GL_FRAGMENT_SHADER);

        vs.addDefines(m_defines);
        fs.addDefines(m_defines);

        if (!vs.loadSource(vsSource))
        {
            LOGE("ProgramGLSL::loadSource : load vertex shader source failed");
            return false;
        }

        if (!fs.loadSource(fsSource))
        {
            LOGE("ProgramGLSL::loadSource : load fragment shader source failed");
            return false;
        }

        return loadShader(vs, fs);
    }

    bool loadFile(const std::string &vsPath, const std::string &fsPath)
    {
        GLSLUtils vs(GL_VERTEX_SHADER);
        GLSLUtils fs(GL_FRAGMENT_SHADER);

        vs.addDefines(m_defines);
        fs.addDefines(m_defines);

        if (!vs.loadFile(vsPath))
        {
            LOGE("load vertex shader file failed: %s", vsPath.c_str());
            return false;
        }

        if (!fs.loadFile(fsPath))
        {
            LOGE("load fragment shader file failed: %s", fsPath.c_str());
            return false;
        }

        return loadShader(vs, fs);
    }

    void use() const
    {
        if (m_id != 0)
        {
            GL_CHECK(glUseProgram(m_id));
        }
        else
        {
            LOGE("failed to use program, not ready");
        }
    }

    void destroy()
    {
        if (m_id != 0)
        {
            GL_CHECK(glDeleteProgram(m_id));
            m_id = 0;
        }
    }

    inline bool empty() const
    {
        return 0 == m_id;
    }

    inline GLuint getId() const
    {
        return m_id;
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void setVec3(const std::string &name, float v0, float v1, float v2) const
    {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), v0, v1, v2);
    }

    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    bool loadShader(GLSLUtils &vs, GLSLUtils &fs)
    {
        m_id = glCreateProgram();
        GL_CHECK(glAttachShader(m_id, vs.getId()));
        GL_CHECK(glAttachShader(m_id, fs.getId()));
        GL_CHECK(glLinkProgram(m_id));
        GL_CHECK(glValidateProgram(m_id));

        GLint isLinked = 0;
        GL_CHECK(glGetProgramiv(m_id, GL_LINK_STATUS, (int *) &isLinked));
        if (GL_FALSE == isLinked)
        {
            GLint maxLength = 0;
            GL_CHECK(glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CHECK(glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]));
            LOGE("link program failed: %s", &infoLog[0]);

            destroy();
            return false;
        }

        return true;
    }

private:
    GLuint m_id = 0;
    std::string m_defines;
};

END_NAMESPACE(GLBase)

#endif // _PROGRAM_GLSL_HPP_