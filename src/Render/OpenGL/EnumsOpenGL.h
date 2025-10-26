#pragma once

#include "glad/glad.h"
#include "Render/Base/PipelineStates.h"
#include "Render/Base/Texture.h"

namespace Learn
{
    namespace OpenGL
    {
#define CASE_CVT_GL(PRE, TOKEN) \
    case PRE##TOKEN:            \
        return GL_##TOKEN;

        static inline GLint cvtWrap(WrapMode mode)
        {
            switch (mode)
            {
                CASE_CVT_GL(Wrap_, REPEAT);
                CASE_CVT_GL(Wrap_, MIRRORED_REPEAT);
                CASE_CVT_GL(Wrap_, CLAMP_TO_EDGE);
                CASE_CVT_GL(Wrap_, CLAMP_TO_BORDER);
            default:
                break;
            }
            return GL_REPEAT;
        }
        static inline GLint cvtFilter(FilterMode mode)
        {
            switch (mode)
            {
                CASE_CVT_GL(Filter_, LINEAR);
                CASE_CVT_GL(Filter_, NEAREST);
                CASE_CVT_GL(Filter_, LINEAR_MIPMAP_LINEAR);
                CASE_CVT_GL(Filter_, LINEAR_MIPMAP_NEAREST);
                CASE_CVT_GL(Filter_, NEAREST_MIPMAP_LINEAR);
                CASE_CVT_GL(Filter_, NEAREST_MIPMAP_NEAREST);
            default:
                break;
            }
            return GL_NEAREST;
        }
        static inline GLint cvtCubeFace(CubeMapFace face)
        {
            switch (face)
            {
            case CubeMapFace::CubeMapFace_PositiveX:
                return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            case CubeMapFace::CubeMapFace_PositiveY:
                return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            case CubeMapFace::CubeMapFace_PositiveZ:
                return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            case CubeMapFace::CubeMapFace_NegativeX:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            case CubeMapFace::CubeMapFace_NegativeY:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
            case CubeMapFace::CubeMapFace_NegativeZ:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
            default:
                break;
            }
            return 0;
        }
        static inline GLint cvtDepthFunc(DepthFunction func)
        {
            switch (func)
            {
                CASE_CVT_GL(, NEVER);
                CASE_CVT_GL(, LESS);
                CASE_CVT_GL(, EQUAL);
                CASE_CVT_GL(, LEQUAL);
                CASE_CVT_GL(, GREATER);
                CASE_CVT_GL(, NOTEQUAL);
                CASE_CVT_GL(, GEQUAL);
                CASE_CVT_GL(, ALWAYS);
            default:
                break;
            }
            return 0;
        }
        static inline GLint cvtBlendFactor(BlendFactor factor)
        {
            switch (factor)
            {
                CASE_CVT_GL(, ZERO);
                CASE_CVT_GL(, ONE);
                CASE_CVT_GL(, SRC_COLOR);
                CASE_CVT_GL(, SRC_ALPHA);
                CASE_CVT_GL(, DST_COLOR);
                CASE_CVT_GL(, DST_ALPHA);
                CASE_CVT_GL(, ONE_MINUS_SRC_COLOR);
                CASE_CVT_GL(, ONE_MINUS_SRC_ALPHA);
                CASE_CVT_GL(, ONE_MINUS_DST_COLOR);
                CASE_CVT_GL(, ONE_MINUS_DST_ALPHA);
            default:
                break;
            }
            return 0;
        }
        static inline GLint cvtBlendFunction(BlendFunction func)
        {
            switch (func)
            {
            case ADD:
                return GL_FUNC_ADD;
            case SUBTRACT:
                return GL_FUNC_SUBTRACT;
            case REVERSE_SUBTRACT:
                return GL_FUNC_REVERSE_SUBTRACT;
            case MIN:
                return GL_MIN;
            case MAX:
                return GL_MAX;
            default:
                break;
            }
            return 0;
        }
        static inline GLint cvtPolygonMode(PolygonMode mode)
        {
            switch (mode)
            {
                CASE_CVT_GL(, POINT);
                CASE_CVT_GL(, LINE);
                CASE_CVT_GL(, FILL);
            default:
                break;
            }
            return 0;
        }

        static inline GLint cvtDrawMode(PrimitiveType type)
        {
            switch (type)
            {
            case Primitive_POINT:
                return GL_POINTS;
            case Primitive_LINE:
                return GL_LINES;
            case Primitive_TRIANGLE:
                return GL_TRIANGLES;
            default:
                break;
            }
            return 0;
        }

        static inline glm::vec4 cvtBorderColor(BorderColor color)
        {
            switch (color)
            {
            case Border_BLACK:
                return glm::vec4(0.f);
            case Border_WHITE:
                return glm::vec4(1.f);
            default:
                break;
            }
            return glm::vec4(0.f);
        }

    }

}