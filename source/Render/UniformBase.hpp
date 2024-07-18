#ifndef _UNIFORM_BASE_HPP_
#define _UNIFORM_BASE_HPP_

#include "Common/cpplang.hpp"

#include "Common/UUID.hpp"
#include "Render/ShaderProgram.hpp"

BEGIN_NAMESPACE(GLBase)

class UniformBase
{
public:
    explicit UniformBase(std::string name) : name(std::move(name)) {}

public:
    int getHash() const
    {
        return m_uuid.get();
    }

    virtual int getLocation(ShaderProgram &program) = 0;
    virtual void bindProgram(ShaderProgram &program, int location) = 0;

public:
    std::string name;

private:
    UUID<UniformBase> m_uuid;
};

END_NAMESPACE(GLBase)

#endif // _UNIFORM_BASE_HPP_