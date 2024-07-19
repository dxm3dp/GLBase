#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include "Common/cpplang.hpp"

#include "Common/MemoryUtils.hpp"

BEGIN_NAMESPACE(GLBase)

template<typename T>
class Buffer
{
public:
    static std::shared_ptr<Buffer<T>> makeDefault(size_t width, size_t height);

    virtual void initLayout()
    {
        m_innerWidth = m_width;
        m_innerHeight = m_height;
    }

    virtual size_t convertIndex(size_t x, size_t y) const
    {
        return x + y * m_innerWidth;
    }

    void create(size_t width, size_t height, const uint8_t *data = nullptr)
    {
        if ( width > 0 && height > 0)
        {
            if (m_width == width  && m_height == height)
                return;

            m_width = width;
            m_height = height;

            initLayout();

            m_dataSize = m_innerWidth * m_innerHeight;
            m_data = MemoryUtils::makeBuffer<T>(m_dataSize, data);
        }
    }

    T *get(size_t x, size_t y)
    {
        T *ptr = m_data.get();
        if (ptr != nullptr && x < m_width && y < m_height)
        {
            return &ptr[convertIndex(x, y)];
        }
        return nullptr;
    }

    void set(size_t x, size_t y, const T &pixel)
    {
        T *ptr = m_data.get();
        if (ptr != nullptr && x < m_width && y < m_height)
        {
            ptr[convertIndex(x, y)] = pixel;
        }
    }

protected:
    size_t m_width = 0;
    size_t m_height = 0;
    size_t m_innerWidth = 0;
    size_t m_innerHeight = 0;
    std::shared_ptr<T> m_data = nullptr;
    size_t m_dataSize = 0;
};

template<typename T>
std::shared_ptr<Buffer<T>> Buffer<T>::makeDefault(size_t width, size_t height)
{
    std::shared_ptr<Buffer<T>> ret = std::make_shared<Buffer<T>>();
    ret->create(width, height);

    return ret;
}

END_NAMESPACE(GLBase)

#endif // _BUFFER_HPP_