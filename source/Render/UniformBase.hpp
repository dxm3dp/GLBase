#ifndef _UNIFORM_BASE_HPP_
#define _UNIFORM_BASE_HPP_

#include "Common/cpplang.hpp"

#include "Common/UUID.hpp"

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

    virtual int getLocation(int programId) = 0;
    virtual void bindProgram(int programId, int binding, int location) = 0;

public:
    std::string name;

private:
    UUID<UniformBase> m_uuid;
};

END_NAMESPACE(GLBase)

#endif // _UNIFORM_BASE_HPP_