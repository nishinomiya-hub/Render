#pragma once

#include <functional>
#include "TextureSoft.h"

namespace Learn
{

#define CoordMod(i, n) ((i) & ((n) - 1) + (n)) & ((n) - 1) // (i % n + n) % n
#define CoordMirror(i) (i) >= 0 ? (i) : (-1 - (i))

    template <typename T>
    class BaseSampler
    {
    protected:
        T borderColor_;

        size_t width_ = 0;
        size_t height_ = 0;

        bool useMipmaps = false;
        WrapMode wrapMode_ = Wrap_CLAMP_TO_EDGE;
        FilterMode filterMode_ = Filter_LINEAR;
        std::function<float(BaseSampler<T> *)> *lodFunc_ = nullptr;

    public:
        virtual bool empty() = 0;
        inline size_t getWidth() const
        {
            return width_;
        }
        inline size_t getHeight() const
        {
            return height_;
        }
        inline T &borderColor()
        {
            return borderColor_;
        }

        T textureImpl(TextureImageSoft<T> *tex, glm::vec2 &uv, float lod = 0.f, glm::ivec2 offset = glm::ivec2(0));
        static T sampleNearest(Buffer<T> *buffer, glm::vec2 &uv, WrapMode wrap, glm::ivec2 &offset, T border);
        static T sampleBilinear(Buffer<T> *buffer, glm::vec2 &uv, WrapMode wrap, glm::ivec2 &offset, T border);

        static T pixelWithWrapMode(Buffer<T> *buffer, int x, int y, WrapMode wrap, T border);
        static void sampleBufferBilinear(Buffer<T> *buffer_out, Buffer<T> *buffer_in, T border);
        static T samplePixelBilinear(Buffer<T> *buffer, glm::vec2 uv, WrapMode wrap, T border);

        inline void setWrapMode(int wrap_mode)
        {
            wrapMode_ = (WrapMode)wrap_mode;
        }

        inline void setFilterMode(int filter_mode)
        {
            filterMode_ = (FilterMode)filter_mode;
        }

        inline void setLodFunc(std::function<float(BaseSampler<T> *)> *func)
        {
            lodFunc_ = func;
        }

        static void generateMipmaps(TextureImageSoft<T> *tex, bool sample);
    };

    template <typename T>
    class BaseSampler2D : public BaseSampler<T>
    {
    private:
        TextureImageSoft<T> *tex_ = nullptr;

    public:
        inline void setImage(TextureImageSoft<T> *tex)
        {
            tex_ = tex;
            BaseSampler<T>::width_ = (tex_ == nullptr) ? 0 : tex_->getWidth();
            BaseSampler<T>::height_ = (tex_ == nullptr) ? 0 : tex_->getHeight();
            BaseSampler<T>::useMipmaps = BaseSampler<T>::filterMode_ > Filter_LINEAR;
        }
        inline bool empty() override
        {
            return tex_ == nullptr;
        }
        T texture2DLodImpl(glm::vec2 &uv, float lod = 0.f, glm::ivec2 offset = glm::ivec2(0))
        {
            return BaseSampler<T>::textureImpl(tex_, uv, lod, offset);
        }
        T texutre2DImpl(glm::vec2 &uv, float bias = 0.f)
        {
            float lod = bias;
            if (BaseSampler<T>::useMipmaps && BaseSampler<T>::lodFunc_)
                lod += (*BaseSampler<T>::lodFunc_)(this);
            return texture2DLodImpl(uv, lod);
        }
    };

    template <typename T>
    void BaseSampler<T>::generateMipmaps(TextureImageSoft<T> *tex, bool sample)
    {
        int width = tex->getWidth();
        int height = tex->getHeight();

        auto level0 = tex->getBuffer();
        tex->levels.reserve(1);
        tex->levels[0] = level0;

        uint32_t levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        for (uint32_t level = 1; level < levelCount; level++)
        {
            tex->levels.push_back(std::make_shared<ImageBufferSoft<T>>(
                std::max(1, width >> level),
                std::max(1, height >> level)));
        }

        if (!sample)
            return;

        for (int i = 1; i < tex->levels.size(); i++)
        {
            BaseSampler<T>::sampleBufferBilinear(tex->levels[i]->buffer.get(), tex->levels[i - 1]->buffer.get(), T(0));
        }
    }

    template <typename T>
    void TextureImageSoft<T>::generateMipmap(bool sample)
    {
        BaseSampler<T>::generateMipmaps(this, sample);
    }

    template <typename T>
    T BaseSampler<T>::textureImpl(TextureImageSoft<T> *tex,
                                  glm::vec2 &uv,
                                  float lod,
                                  glm::ivec2 offset)
    {
        if (tex != nullptr && !tex->empty())
        {
            if (filterMode_ == Filter_NEAREST)
            {
                return sampleNearest(tex->levels[0]->buffer.get(), uv, wrapMode_, offset, borderColor_);
            }
            if (filterMode_ == Filter_LINEAR)
            {
                return sampleBilinear(tex->levels[0]->buffer.get(), uv, wrapMode_, offset, borderColor_);
            }

            int max_level = (int)tex->levels.size() - 1;

            if (filterMode_ == Filter_NEAREST_MIPMAP_NEAREST || filterMode_ == Filter_NEAREST_MIPMAP_LINEAR)
            {
                int level = std::clamp((int)glm::ceil(lod + 0.5f) - 1, 0, max_level);
                if (filterMode_ == Filter_NEAREST_MIPMAP_NEAREST)
                    return sampleNearest(tex->levels[level]->buffer.get(), uv, wrapMode_, offset, borderColor_);
                else
                    return sampleBilinear(tex->levels[level]->buffer.get(), uv, wrapMode_, offset, borderColor_);
            }

            if (filterMode_ == Filter_NEAREST_MIPMAP_LINEAR || filterMode_ == Filter_LINEAR_MIPMAP_LINEAR)
            {
                int level_hi = glm::clamp((int)std::floor(lod), 0, max_level);
                int level_lo = glm::clamp(level_hi + 1, 0, max_level);

                T texel_hi, texel_lo;

                if (filterMode_ == Filter_NEAREST_MIPMAP_LINEAR)
                {
                    texel_hi = sampleNearest(tex->levels[level_hi]->buffer.get(), uv, wrapMode_, offset, borderColor_);
                }
                else
                {
                    texel_hi = sampleBilinear(tex->levels[level_hi]->buffer.get(), uv, wrapMode_, offset, borderColor_);
                }

                if (level_hi == level_lo)
                {
                    return texel_hi;
                }
                else
                {
                    if (filterMode_ == Filter_NEAREST_MIPMAP_LINEAR)
                    {
                        texel_lo = sampleNearest(tex->levels[level_lo]->buffer.get(), uv, wrapMode_, offset, borderColor_);
                    }
                    else
                    {
                        texel_lo = sampleBilinear(tex->levels[level_lo]->buffer.get(), uv, wrapMode_, offset, borderColor_);
                    }
                }

                float f = glm::fract(lod);
                return glm::mix(texel_hi, texel_lo, f);
            }
        }
        return T(0);
    }
}