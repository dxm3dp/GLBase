#ifndef _SHADER_PROGRAM_HPP_
#define _SHADER_PROGRAM_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/OpenGLUtils.hpp"
#include "Render/ShaderGLSL.hpp"

BEGIN_NAMESPACE(GLBase)

constexpr char const * OpenGL_GLSL_DEFINE = "OpenGL";

class ShaderProgram
{
public:
    ShaderProgram()
    {
        addDefine(OpenGL_GLSL_DEFINE);
    }

    ~ShaderProgram()
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
        ShaderGLSL vs(GL_VERTEX_SHADER);
        ShaderGLSL fs(GL_FRAGMENT_SHADER);

        vs.addDefines(m_defines);
        fs.addDefines(m_defines);

        if (!vs.loadSource(vsSource))
        {
            LOGE("ShaderProgram::loadSource : load vertex shader source failed");
            return false;
        }

        if (!fs.loadSource(fsSource))
        {
            LOGE("ShaderProgram::loadSource : load fragment shader source failed");
            return false;
        }

        return loadShader(vs, fs);
    }

    bool loadFile(const std::string &vsPath, const std::string &fsPath)
    {
        ShaderGLSL vs(GL_VERTEX_SHADER);
        ShaderGLSL fs(GL_FRAGMENT_SHADER);

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

private:
    bool loadShader(ShaderGLSL &vs, ShaderGLSL &fs)
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

#endif // _SHADER_PROGRAM_HPP_