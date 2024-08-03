#ifndef _IMAGE_UTILS_HPP_
#define _IMAGE_UTILS_HPP_

#include "Common/cpplang.hpp"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "Common/Buffer.hpp"
#include "Common/GLMInc.hpp"
#include "Common/Logger.hpp"

BEGIN_NAMESPACE(GLBase)

class ImageUtils
{
public:
    static unsigned int loadTexture(const std::string &path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, components;
        unsigned char * data = stbi_load(path.c_str(), &width, &height, &components, 0);
        if (data != nullptr)
        {
            GLenum format;
            if (components == 1)
            {
                format = GL_RED;
            }
            else if (components == 3)
            {
                format = GL_RGB;
            }
            else if (components == 4)
            {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);

            stbi_image_free(data);
        }
        else
        {
            LOGE("Failed to load texture: %s", path.c_str());
            stbi_image_free(data);
        }

        return textureID;
    }

    static std::shared_ptr<Buffer<RGBA>> readImageRGBA(const std::string &path)
    {
        int width, height, components;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &components, STBI_default);
        if (nullptr == data)
        {
            LOGE("ImageUtils::readImageRGBA failed to load image with path: %s", path.c_str());
            return nullptr;
        }

        auto buffer = Buffer<RGBA>::makeDefault(width, height);
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                auto &to = *buffer->get(x, y);
                size_t index = x + y * width;

                switch(components)
                {
                    case STBI_grey:
                    {
                        to.r = data[index];
                        to.g = to.b = to.r;
                        to.a = 255;
                        break;
                    };
                    case STBI_grey_alpha:
                    {
                        to.r = data[index * 2 + 0];
                        to.g = to.b = to.r;
                        to.a = data[index * 2 + 1];
                        break;
                    };
                    case STBI_rgb:
                    {
                        to.r = data[index * 3 + 0];
                        to.g = data[index * 3 + 1];
                        to.b = data[index * 3 + 2];
                        to.a = 255;
                        break;
                    };
                    case STBI_rgb_alpha:
                    {
                        to.r = data[index * 4 + 0];
                        to.g = data[index * 4 + 1];
                        to.b = data[index * 4 + 2];
                        to.a = data[index * 4 + 3];
                        break;
                    };
                    default:
                        break;
                }
            }
        }

        stbi_image_free(data);

        return buffer;
    }

    static void writeImage(char const *filename, int w, int h, int comp, const void *data, int strideInBytes, bool flipY)
    {
        stbi_flip_vertically_on_write(flipY);
        stbi_write_png(filename, w, h, comp, data, strideInBytes);
    }

    static void convertFloatImage(RGBA *dst, float *src, uint32_t width, uint32_t height)
    {
        float *srcPixel = src;

        float depthMin = FLT_MAX;
        float depthMax = FLT_MIN;
        for (int i = 0; i < width * height; i++)
        {
            float depth = *srcPixel;
            depthMin = std::min(depthMin, depth);
            depthMax = std::max(depthMax, depth);
            srcPixel++;
        }

        srcPixel = src;
        RGBA *dstPixel = dst;
        for (int i = 0; i < width * height; i++)
        {
            float depth = *srcPixel;
            depth = (depth - depthMin) / (depthMax - depthMin);
            dstPixel->r = glm::clamp((int) (depth * 255.f), 0, 255);
            dstPixel->g = dstPixel->r;
            dstPixel->b = dstPixel->r;
            dstPixel->a = 255;

            srcPixel++;
            dstPixel++;
        }
    }
};

END_NAMESPACE(GLBase)

#endif // _IMAGE_UTILS_HPP_