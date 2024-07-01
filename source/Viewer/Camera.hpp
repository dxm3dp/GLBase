#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

BEGIN_NAMESPACE(GLBase)

class Camera
{
public:
    void lookat(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
    {
        m_eye = eye;
        m_center = center;
        m_up = up;
    }

    void setPerspective(float fov, float aspect, float near, float far)
    {
        m_fov = fov;
        m_aspect = aspect;
        m_near = near;
        m_far = far;
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 z = glm::normalize(m_eye - m_center);
        glm::vec3 x = glm::normalize(glm::cross(m_up, z));
        glm::vec3 y = glm::normalize(glm::cross(z, x));

        glm::mat4 viewMatrix(1.f);

        viewMatrix[0][0] = x.x;
        viewMatrix[1][0] = x.y;
        viewMatrix[2][0] = x.z;

        viewMatrix[0][1] = y.x;
        viewMatrix[1][1] = y.y;
        viewMatrix[2][1] = y.z;

        viewMatrix[0][2] = z.x;
        viewMatrix[1][2] = z.y;
        viewMatrix[2][2] = z.z;

        viewMatrix[3][0] = -glm::dot(x, m_eye);
        viewMatrix[3][1] = -glm::dot(y, m_eye);
        viewMatrix[3][2] = -glm::dot(z, m_eye);

        return viewMatrix;
    }

    glm::mat4 getPerspectiveMatrix() const
    {
        float fax = 1.f / tan(m_fov * 0.5f);

        glm::mat4 projectionMatrix(0.f);

        projectionMatrix[0][0] = fax / m_aspect;
        projectionMatrix[1][1] = fax;
        projectionMatrix[2][2] = (m_far + m_near) / (m_far - m_near);
        projectionMatrix[3][2] = 2 * m_far * m_near / (m_far - m_near);
        projectionMatrix[2][3] = -1.f;
    }

    inline float fov() const { return m_fov; }
    inline float aspect() const { return m_aspect; }
    inline float near() const { return m_near; }
    inline float far() const { return m_far; }

    inline const glm::vec3 &eye() const { return m_eye; }
    inline const glm::vec3 &center() const { return m_center; }
    inline const glm::vec3 &up() const { return m_up; }

private:
    float m_fov = glm::radians(60.0f);
    float m_aspect = 1.f;
    float m_near = 0.01f;
    float m_far = 100.f;
    glm::vec3 m_eye{};
    glm::vec3 m_center{};
    glm::vec3 m_up{};
};

END_NAMESPACE(GLBase)

#endif // _CAMERA_HPP_