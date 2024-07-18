#ifndef _GLSL_UTILS_HPP_
#define _GLSL_UTILS_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/FileUtils.hpp"
#include "Common/Logger.hpp"
#include "Common/OpenGLUtils.hpp"

BEGIN_NAMESPACE(GLBase)

constexpr char const * OpenGL_GLSL_VERSION = "#version 430 core\n";

class GLSLUtils
{
public:
    explicit GLSLUtils(GLenum type) : m_type(type)
    {
        m_header = OpenGL_GLSL_VERSION;
    }
    ~GLSLUtils() { }

    void setHeader(const std::string &header)
    {
        m_header = header;
    }

    void addDefines(const std::string &defines)
    {
        m_defines = defines;
    }

    bool loadSource(const std::string &source)
    {
        m_id = glCreateShader(m_type);
        std::string shaderStr;
        if (GL_VERTEX_SHADER == m_type)
        {
            shaderStr = compatibleVertexPreprocess(m_header + m_defines + source);
        }
        else if (GL_FRAGMENT_SHADER == m_type)
        {
            shaderStr = compatibleFragmentPreprocess(m_header + m_defines + source);
        }
        if (shaderStr.empty())
        {
            LOGE("GLSLUtils::loadSource failed: empty source");
            return false;
        }

        const char *shaderStrPtr = shaderStr.c_str();
        auto length = (GLint)shaderStr.length();
        GL_CHECK(glShaderSource(m_id, 1, &shaderStrPtr, &length));
        GL_CHECK(glCompileShader(m_id));

        GLint isCompiled = 0;
        GL_CHECK(glGetShaderiv(m_id, GL_COMPILE_STATUS, &isCompiled));
        if (GL_FALSE == isCompiled)
        {
            GLint maxLength = 0;
            GL_CHECK(glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength));
            std::vector<GLchar> infoLog(maxLength);
            GL_CHECK(glGetShaderInfoLog(m_id, maxLength, &maxLength, &infoLog[0]));
            LOGE("compile shader failed: %s", &infoLog[0]);

            destroy();
            return false;
        }

        return true;
    }

    bool loadFile(const std::string &path)
    {
        std::string source = FileUtils::readText(path);
        if (source.length() <= 0 )
        {
            LOGE("read shader source failed");
            return false;
        }

        return loadSource(source);
    }

    void destroy()
    {
        if (m_id)
        {
            GL_CHECK(glDeleteShader(m_id));
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
    std::string compatibleVertexPreprocess(const std::string &source)
    {
        std::regex outLocationRegex(R"(layout\s*\(\s*location\s*=\s*\d+\s*\)\s*out\s*)");
        std::regex std140Regex(R"(layout\s*\(.*std140.*\)\s*uniform\s*)");
        std::regex uniformBindingRegex(R"(layout\s*\(.*binding\s*=.*\)\s*uniform\s*)");

        std::string result = std::regex_replace(source, outLocationRegex, "out ");
        result = std::regex_replace(result, std140Regex, "layout (std140) uniform ");
        result = std::regex_replace(result, uniformBindingRegex, "uniform ");

        return result;
    }

    std::string compatibleFragmentPreprocess(const std::string &source)
    {
        std::regex inLocationRegex(R"(layout\s*\(\s*location\s*=\s*\d+\s*\)\s*in\s*)");
        std::regex outLocationRegex(R"(layout\s*\(\s*location\s*=\s*\d+\s*\)\s*out\s*)");
        std::regex std140Regex(R"(layout\s*\(.*std140.*\)\s*uniform\s*)");
        std::regex uniformBindingRegex(R"(layout\s*\(.*binding\s*=.*\)\s*uniform\s*)");

        std::string result = std::regex_replace(source, inLocationRegex, "in ");
        result = std::regex_replace(result, outLocationRegex, "out ");
        result = std::regex_replace(result, std140Regex, "layout (std140) uniform ");
        result = std::regex_replace(result, uniformBindingRegex, "uniform ");

        return result;
    }

private:
    GLenum m_type;
    GLuint m_id;
    std::string m_header;
    std::string m_defines;
};

END_NAMESPACE(GLBase)

#endif // _GLSL_UTILS_HPP_