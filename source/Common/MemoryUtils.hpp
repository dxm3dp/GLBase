#ifndef _MEMORY_UTILS_HPP_
#define _MEMORY_UTILS_HPP_

#include "Common/cpplang.hpp"

#include "Common/Logger.hpp"

BEGIN_NAMESPACE(GLBase)

const int DEFAULT_ALIGNMENT = 32;

class MemoryUtils
{
public:
    static void *alignedMalloc(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
    {
        if ((alignment & (alignment - 1)) != 0)
        {
            LOGE("failed to malloc, invalid alignment: %d", alignment);
            return nullptr;
        }

        size_t extra = alignment + sizeof(void *);
        void *data = malloc(size + extra);
        if (nullptr == data)
        {
            LOGE("failed to malloc with size: %d", size);
            return nullptr;
        }

        size_t addr = (size_t)data + extra;
        void *alignPtr = (void *)(addr - (addr % alignment));
        *((void **)(alignPtr - 1)) = data;

        return alignPtr;
    }

    static void alignedFree(void *ptr)
    {
        if (ptr != nullptr)
        {
            free(((void **)ptr)[-1]);
        }
    }

    static size_t alignedSize(size_t size)
    {
        if (0 == size)
            return 0;

        return DEFAULT_ALIGNMENT * std::ceil((float) size / (float)DEFAULT_ALIGNMENT);
    }

    template<typename T>
    static std::shared_ptr<T> makeAlignedBuffer(size_t elementCount)
    {
        if (0 == elementCount)
            return nullptr;

        return std::shared_ptr<T>((T *)alignedMalloc(sizeof(T) * elementCount),
            [](const T *ptr)
            {
                alignedFree((void *)ptr);
            });
    }

    template<typename T>
    static std::shared_ptr<T> makeBuffer(size_t elementCount,  const uint8_t *data = nullptr)
    {
        if (0 == elementCount)
            return nullptr;

        if (data != nullptr)
        {
            return std::shared_ptr<T>((T *)data, [](const T *ptr){});
        }
        else
        {
            return std::shared_ptr<T>(new T[elementCount], [](const T *ptr){ delete[] ptr; });
        }
    }
};

END_NAMESPACE(GLBase)

#endif // _MEMORY_UTILS_HPP_