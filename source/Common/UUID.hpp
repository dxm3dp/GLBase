#ifndef _UUID_HPP_
#define _UUID_HPP_

#include "Common/cpplang.hpp"

BEGIN_NAMESPACE(GLBase)

template<typename T>
class UUID
{
public:
    UUID() : m_uuid(m_uuidCounter++) {}

    int get() const
    {
        return m_uuid;
    }

private:
    int m_uuid = -1;
    static int m_uuidCounter;
};

template<typename T>
int UUID<T>::m_uuidCounter = 0;

END_NAMESPACE(GLBase)

#endif // _UUID_HPP_