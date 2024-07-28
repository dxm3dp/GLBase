#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

BEGIN_NAMESPACE(GLBase)

constexpr float CAMERA_FOV = 60.f;
constexpr float CAMERA_NEAR = 0.01f;
constexpr float CAMERA_FAR = 100.f;

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float SPEED = 2.f;
const float SENSITIVITY = 0.1f;
const float PITCH = 0.f;
const float YAW = -90.f;

class Camera
{
public:
    Camera() = default;

    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
        : m_position(position), m_front(target - position), m_up(up)
    {
        m_right = glm::normalize(glm::cross(m_up, m_front));
        m_pitch = PITCH;
        m_yaw = YAW;
    }

public:
    void setPerspective(float fov, float aspect, float near, float far)
    {
        m_fov = fov;
        m_aspect = aspect;
        m_near = near;
        m_far = far;
    }

    void lookat(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
    {
        m_position = position;
        m_front = target - position;
        m_up = up;
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 z = glm::normalize(-m_front);
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

        viewMatrix[3][0] = -glm::dot(x, m_position);
        viewMatrix[3][1] = -glm::dot(y, m_position);
        viewMatrix[3][2] = -glm::dot(z, m_position);

        return viewMatrix;
    }

    glm::mat4 getPerspectiveMatrix() const
    {
        float tanHalfFovy = tan(m_fov * 0.5f);

        glm::mat4 projectionMatrix(0.f);

        projectionMatrix[0][0] = 1.f / (tanHalfFovy * m_aspect);
        projectionMatrix[1][1] = 1.f / tanHalfFovy;
        projectionMatrix[2][2] = -(m_far + m_near) / (m_far - m_near);
        projectionMatrix[2][3] = -1.f;
        projectionMatrix[3][2] = -2 * m_far * m_near / (m_far - m_near);

        return projectionMatrix;
    }

    inline float fov() const { return m_fov; }
    inline float aspect() const { return m_aspect; }
    inline float near() const { return m_near; }
    inline float far() const { return m_far; }

    inline const glm::vec3 &position() const { return m_position; }
    inline const glm::vec3 &up() const { return m_up; }

    void movement(CameraMovement moveDir, float deltaTime)
    {
        float speed = SPEED * deltaTime;
        if (moveDir == FORWARD)
        {
            m_position += m_front * speed;
        }
        if (moveDir == BACKWARD)
        {
            m_position -= m_front * speed;
        }
        if (moveDir == LEFT)
        {
            m_position += m_right * speed;
        }
        if (moveDir == RIGHT)
        {
            m_position -= m_right * speed;
        }
    }

    void lookAround(float xoffset, float yoffset)
    {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        m_pitch += yoffset;
        m_yaw += xoffset;

        if (m_pitch > 89.f)
            m_pitch = 89.f;
        if (m_pitch < -89.f)
            m_pitch = -89.f;

        glm::vec3 front;
        front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        front.y = glm::sin(glm::radians(m_pitch));
        front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_up, m_front));
    }

private:
    float m_fov = glm::radians(60.0f);
    float m_aspect = 1.f;
    float m_near = 0.01f;
    float m_far = 100.f;
    glm::vec3 m_position{};
    glm::vec3 m_up{};
    glm::vec3 m_front{};
    glm::vec3 m_right{};
    float m_pitch;
    float m_yaw;
};

END_NAMESPACE(GLBase)

#endif // _CAMERA_HPP_