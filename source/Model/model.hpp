#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "Common/cpplang.hpp"

#include <glm/glm.hpp>

#include "Common/Logger.hpp"
#include "Model/tgaimage.hpp"
#include "Render/Vertex.hpp"

BEGIN_NAMESPACE(GLBase)

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};

class Model
{
public:
    Model(const char *filename)
    {
        std::ifstream in(filename, std::ios::in);
        if (!in.is_open())
        {
            LOGE("Cannot open file: %s", filename);
            return;
        }

        std::string line;
        while (!in.eof())
        {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v "))       // vertex
            {
                iss >> trash;
                glm::vec3 v;
                for(int i = 0; i < 3; i++)
                {
                    iss >> v[i];
                }
                m_vertices.emplace_back(v);
            }
            else if (!line.compare(0, 3, "vt ")) // texture coord
            {
                iss >> trash >> trash;
                glm::vec2 vt;
                for(int i = 0; i < 2; i++)
                {
                    iss >> vt[i];
                }
                m_uvs.emplace_back(vt);
            }
            else if (!line.compare(0, 3, "vn ")) // normal
            {
                iss >> trash >> trash;
                glm::vec3 vn;
                for(int i = 0; i < 3; i++)
                {
                    iss >> vn[i];
                }
                m_normals.emplace_back(vn);
            }
            else if (!line.compare(0, 2, "f "))  // face
            {
                iss >> trash;
                std::vector<glm::uvec3> f;
                glm::uvec3 tmp;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
                {
                    for (int i = 0; i < 3; i++)
                    {
                        tmp[i]--; // in wavefront obj all indices start at 1, not zero
                    }
                    f.emplace_back(tmp);
                    m_indices.emplace_back(tmp[0]);
                }
                m_faces.emplace_back(f);
            }
        }
        LOGI("m_vertices: %d, m_uvs: %d, m_normals: %d, m_faces: %d", m_vertices.size(), m_uvs.size(), m_normals.size(), m_faces.size());
        load_texture(filename, "_diffuse.tga", m_diffuseTex);
        load_texture(filename, "_nm_tangent.tga", m_normalTex);
        load_texture(filename, "_spec.tga", m_specularTex);
    }

public:
    int nverts() const
    {
        return (int)m_vertices.size();
    }

    int nfaces() const
    {
        return (int)m_faces.size();
    }

    glm::vec3 vert(int idx) const
    {
        return m_vertices[idx];
    }

    glm::vec3 vert(int iface, int nthvert) const
    {
        return m_vertices[m_faces[iface][nthvert][0]];
    }

    glm::vec2 uv(int iface, int nthvert) const
    {
        return m_uvs[m_faces[iface][nthvert][1]];
    }

    glm::vec3 normal(int iface, int nthvert) const
    {
        int idx = m_faces[iface][nthvert][2];
        return glm::normalize(m_normals[idx]);
    }

    std::vector<glm::uvec3> face(int idx) const
    {
        return m_faces[idx];
    }

    TGAColor diffuse(glm::vec2 uv) const
    {
        glm::uvec2 uvi{(int)(uv[0] * m_diffuseTex.get_width()), (int)(uv[1] * m_diffuseTex.get_height())};
        return m_diffuseTex.get(uvi.x, uvi.y);
    }

    glm::vec3 normal(glm::vec2 uv) const
    {
        glm::uvec2 uvi{(int)(uv[0] * m_normalTex.get_width()), (int)(uv[1] * m_normalTex.get_height())};
        TGAColor color = m_normalTex.get(uvi.x, uvi.y);
        glm::vec3 ret;
        for(int i = 0; i < 3; i++)
        {
            ret[i] = color[i] / 255.f * 2.f - 1.f;
        }
        return ret;
    }

    float specular(glm::vec2 uv) const
    {
        glm::uvec2 uvi{(int)(uv[0] * m_specularTex.get_width()), (int)(uv[1] * m_specularTex.get_height())};
        return m_specularTex.get(uvi.x, uvi.y)[0] / 1.f;
    }

private:
    void load_texture(const char *filename, const char *suffix, TGAImage &image)
    {
        std::string texfile(filename);
        size_t dot = texfile.find_last_of(".");
        if (dot != std::string::npos)
        {
            texfile = texfile.substr(0, dot) + std::string(suffix);
            bool ret = image.read_tga_file(texfile.c_str());
            LOGI("texture file: %s, %s", texfile.c_str(), ret ? "loaded" : "failed");
            image.flip_vertically();
        }
    }

    void initVertexArray()
    {
        m_vertexArray.vertexSize = sizeof(Vertex);

        m_vertexArray.attributes.resize(3);
        m_vertexArray.attributes[0] = {3, sizeof(Vertex), offsetof(Vertex, position)};
        m_vertexArray.attributes[1] = {2, sizeof(Vertex), offsetof(Vertex, uv)};
        m_vertexArray.attributes[2] = {3, sizeof(Vertex), offsetof(Vertex, normal)};

        m_vertexArray.vertexBuffer = m_vertices.empty() ? nullptr : (uint8_t *)(&m_vertices[0]);
        m_vertexArray.vertexBufferLength = m_vertexArray.vertexSize * m_vertices.size();

        m_vertexArray.indexBuffer = m_indices.empty() ? nullptr : &m_indices[0];
        m_vertexArray.indexBufferLength = sizeof(int32_t) * m_indices.size();
    }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    std::vector<std::vector<glm::uvec3>> m_faces;
    std::vector<int> m_indices;

    VertexArray m_vertexArray;

    TGAImage m_diffuseTex;
    TGAImage m_normalTex;
    TGAImage m_specularTex;
};

END_NAMESPACE(GLBase)

#endif // _MODEL_HPP_
