#pragma once

#include "Utils/UID.h"
#include "Render/Base/Framebuffer.h"
#include "TextureSoft.h"

namespace Learn
{

    class FrameBufferSoft : public FrameBuffer
    {
    private:
        UID<FrameBufferSoft> uuid_;

    public:
        explicit FrameBufferSoft(bool offscreen) : FrameBuffer(offscreen) {}

        int getId() const override
        {
            return uuid_.get();
        }

        bool isValid() override
        {
            return colorReady_ || depthReady_;
        }

        std::shared_ptr<ImageBufferSoft<RGBA>> getColorBuffer() const
        {
            if (!colorReady_)
            {
                return nullptr;
            }
            auto *colorTex = dynamic_cast<TextureSoft<RGBA> *>(colorAttachment_.tex.get());
            return colorTex->getImage(colorAttachment_.layer).getBuffer(colorAttachment_.level);
        };

        std::shared_ptr<ImageBufferSoft<float>> getDepthBuffer() const
        {
            if (!depthReady_)
            {
                return nullptr;
            }
            auto *depthTex = dynamic_cast<TextureSoft<float> *>(depthAttachment_.tex.get());
            return depthTex->getImage(depthAttachment_.layer).getBuffer(depthAttachment_.level);
        };
    };

}