#ifndef _UNIFORM_BLOCK_HPP_
#define _UNIFORM_BLOCK_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/UUID.hpp"
#include "Render/ShaderProgram.hpp"
#include "Render/UniformBase.hpp"

BEGIN_NAMESPACE(GLBase)

class UniformBlock : UniformBase
{
public:
    UniformBlock(const std::string &name, int size) : UniformBase(name), m_blockSize(size)
    {
        glGenBuffers(1, &m_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    }

    ~UniformBlock()
    {
        glDeleteBuffers(1, &m_ubo);
    }

public:
    int getLocation(ShaderProgram &program) override
    {
        return glGetUniformBlockIndex(program.getId(), name.c_str());
    }

    void bindProgram(ShaderProgram &program, int location)
    {
        if (location < 0)
            return;

        int binding = program.getUniformBlockBinding();
        glUniformBlockBinding(program.getId(), location, binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_ubo);
    }

    void setData(void *data, int len)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferData(GL_UNIFORM_BUFFER, len, data, GL_STATIC_DRAW);
    }

    void setSubData(void *data, int len, int offset)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, len, data);
    }

private:
    GLuint m_ubo = 0;
    int m_blockSize;
};

END_NAMESPACE(GLBase)

#endif // _UNIFORM_BLOCK_HPP_