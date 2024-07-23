#ifndef _HASH_UTILS_HPP_
#define _HASH_UTILS_HPP_

#include "Common/cpplang.hpp"

BEGIN_NAMESPACE(GLBase)

class HashUtils
{
public:
    template<typename T>
    static void hashCombine(size_t &seed, const T &v)
    {
        seed ^= std::hash<T>()(v) + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    }
};

END_NAMESPACE(GLBase)

#endif // _HASH_UTILS_HPP_