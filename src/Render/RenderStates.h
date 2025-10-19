#pragma once

#include "Base/GLM_Header.h"

namespace Learn
{
    enum DepthFunction
    {
        LESS,
        LEQUAL,
        GREATER,
        GEQUAL,
        EQUAL,
        NOTEQUAL,
        ALWAYS,
        NEVER
    };

    enum BlendFactor
    {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
    };

    enum BlendFunction
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    enum PolygonMode
    {
        FILL,
        LINE,
        POINT
    };

    struct BlendParameters
    {
        BlendFunction Func_RGB = BlendFunction::ADD;
        BlendFactor Src_RGB = BlendFactor::ONE;
        BlendFactor Dst_RGB = BlendFactor::ZERO;

        BlendFunction Func_Alpha = BlendFunction::ADD;
        BlendFactor Src_Alpha = BlendFactor::ONE;
        BlendFactor Dst_Alpha = BlendFactor::ZERO;

        void setBlendFactor(BlendFactor src, BlendFactor dst)
        {
            Src_RGB = src;
            Src_Alpha = src;
            Dst_RGB = dst;
            Dst_Alpha = dst;
        }

        void setBlendFunction(BlendFunction func)
        {
            Func_RGB = func;
            Func_Alpha = func;
        }
    };

    enum PrimitiveType
    {
        Primitive_POINT,
        Primitive_LINE,
        Primitive_TRIANGLE,
    };

    struct RenderStates
    {
        bool blend = false;
        BlendParameters blendParams;

        bool depthTest = false;
        bool depthMask = true;
        DepthFunction depthFunc = LESS;

        bool cullFace = false;
        PrimitiveType primitiveType = Primitive_TRIANGLE;
        PolygonMode polygonMode = FILL;

        float lineWidth = 1.f;
    };
    
    struct ClearStates
    {
        bool depthFlag = false;
        bool colorFlag = false;
        glm::vec4 clearColor = glm::vec4(0.f);
        float clearDepth = 1.f;
    };

}