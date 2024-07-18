#ifndef _VERTEX_HPP_
#define _VERTEX_HPP_

#include "Common/cpplang.hpp"

BEGIN_NAMESPACE(GLBase)

struct VertexAttributeDesc
{
    size_t size; // number of components
    size_t stride;
    size_t offset;
};

struct VertexArray
{
    size_t vertexSize = 0;
    std::vector<VertexAttributeDesc> attributes;

    uint8_t *vertexBuffer = nullptr;
    size_t vertexBufferLength = 0;

    int32_t *indexBuffer = nullptr;
    size_t indexBufferLength = 0;
};

END_NAMESPACE(GLBase)

#endif // _VERTEX_HPP_