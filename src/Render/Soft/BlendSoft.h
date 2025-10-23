#pragma once

#include "Render/Base/PipelineStates.h"

namespace Learn
{
    template <typename T>
    T calcBlendFactor(const T &src, const float &srcAlpha, const T &dst, const float &dstAlpha, const BlendFactor &factor)
    {
        switch (factor)
        {
        case BlendFactor::ZERO:
            return T(0.f);
        case BlendFactor::ONE:
            return T(1.f);
        case BlendFactor::SRC_COLOR:
            return src;
        case BlendFactor::SRC_ALPHA:
            return T(srcAlpha);
        case BlendFactor::DST_COLOR:
            return dst;
        case BlendFactor::DST_ALPHA:
            return T(dstAlpha);
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return T(1.f) - src;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return T(1.f - srcAlpha);
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return T(1.f) - dst;
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return T(1.f - dstAlpha);
        }
        return T(0.f);
    }

    template <typename T>
    T calcBlendFunc(const T &src, const T &dst, const BlendFunction &func)
    {
        switch (func)
        {
        case BlendFunction::ADD:
            return src + dst;
        case BlendFunction::SUBTRACT:
            return src - dst;
        case BlendFunction::REVERSE_SUBTRACT:
            return dst - src;
        case BlendFunction::MIN:
            return glm::min(src, dst);
        case BlendFunction::MAX:
            return glm::max(src, dst);
        }
        return src + dst;
    }

    glm::vec4 calcBlendColor(glm::vec4 &src, glm::vec4 &dst, const BlendParameters &params)
    {
        auto srcRgb = glm::vec3(src);
        auto dstRgb = glm::vec3(dst);
        auto srcRgbF = calcBlendFactor<glm::vec3>(srcRgb, src.a, dstRgb, dst.a, params.Src_RGB);
        auto dstRgbF = calcBlendFactor<glm::vec3>(srcRgb, src.a, dstRgb, dst.a, params.Dst_RGB);
        auto retRgb = calcBlendFunc<glm::vec3>(srcRgb * srcRgbF, dstRgb * dstRgbF, params.Func_RGB);

        auto srcAlphaF = calcBlendFactor<float>(src.a, src.a, dst.a, dst.a, params.Src_Alpha);
        auto dstAlphaF = calcBlendFactor<float>(src.a, src.a, dst.a, dst.a, params.Dst_Alpha);
        auto retAlpha = calcBlendFunc<float>(src.a * srcAlphaF, dst.a * dstAlphaF, params.Func_Alpha);

        return {retRgb, retAlpha};
    }
}