#ifndef _VERTEX_ARRAY_OBJECT_HPP_
#define _VERTEX_ARRAY_OBJECT_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>

#include "Common/OpenGLUtils.hpp"
#include "Render/Vertex.hpp"

BEGIN_NAMESPACE(GLBase)

class VertexArrayObject
{
public:
    explicit VertexArrayObject(const VertexArray &vertexArray)
    {
        if (nullptr == vertexArray.vertexBuffer || nullptr == vertexArray.indexBuffer)
            return;

        m_indicesCount = vertexArray.indexBufferLength / sizeof(int32_t);

        // vao
        GL_CHECK(glGenVertexArrays(1, &m_vao));
        GL_CHECK(glBindVertexArray(m_vao));
        // vbo
        GL_CHECK(glGenBuffers(1, &m_vbo));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexArray.vertexBufferLength, vertexArray.vertexBuffer, GL_STATIC_DRAW));
        for(int i = 0; i < vertexArray.attributes.size(); i++)
        {
            const auto &attr = vertexArray.attributes[i];
            GL_CHECK(glVertexAttribPointer(i, attr.size, GL_FLOAT, GL_FALSE, attr.stride, (void*)attr.offset));
            GL_CHECK(glEnableVertexAttribArray(i));
        }
        // ebo
        GL_CHECK(glGenBuffers(1, &m_ebo));
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexArray.indexBufferLength, vertexArray.indexBuffer, GL_STATIC_DRAW));
    }

    ~VertexArrayObject()
    {
        if (m_vbo != 0)
            GL_CHECK(glDeleteBuffers(1, &m_vbo));
        if (m_ebo != 0)
            GL_CHECK(glDeleteBuffers(1, &m_ebo));
        if (m_vao != 0)
            GL_CHECK(glDeleteVertexArrays(1, &m_vao));
    }

public:
    inline int getId() const
    {
        return (int)m_vao;
    }

    inline int getIndicesCount() const
    {
        return m_indicesCount;
    }

    void bind() const
    {
        if (m_vao != 0)
            GL_CHECK(glBindVertexArray(m_vao));
    }

    void updateVertexData(void *data, size_t length)
    {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW));
    }

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    size_t m_indicesCount = 0;
};

END_NAMESPACE(GLBase)

#endif // _VERTEX_ARRAY_OBJECT_HPP_