#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Utils/Buffer.h"
#include "Utils/GLM_Header.h"

// Texture基类

namespace Learn
{
    enum WrapMode
    {
        Wrap_REPEAT,
        Wrap_MIRRORED_REPEAT,
        Wrap_CLAMP_TO_EDGE,
        Wrap_CLAMP_TO_BORDER,
    };

    enum FilterMode
    {
        Filter_NEAREST,
        Filter_LINEAR,
        Filter_NEAREST_MIPMAP_NEAREST,
        Filter_LINEAR_MIPMAP_NEAREST,
        Filter_NEAREST_MIPMAP_LINEAR,
        Filter_LINEAR_MIPMAP_LINEAR,
    };

    enum CubeMapFace
    {
        CubeMapFace_PositiveX,
        CubeMapFace_NegativeX,
        CubeMapFace_PositiveY,
        CubeMapFace_NegativeY,
        CubeMapFace_PositiveZ,
        CubeMapFace_NegativeZ,
    };

    enum BorderColor
    {
        Border_BLACK = 0,
        Border_WHITE,
    };

    struct SamplerDesc
    {
        FilterMode filterMin = Filter_NEAREST;
        FilterMode filterMag = Filter_NEAREST;

        WrapMode wrapS = Wrap_CLAMP_TO_EDGE;
        WrapMode wrapT = Wrap_CLAMP_TO_EDGE;
        WrapMode wrapR = Wrap_CLAMP_TO_EDGE;

        BorderColor borderColor = Border_BLACK;
    };

    enum TextureType
    {
        TextureType_2D,
        TextureType_CUBE,
    };

    enum TextureFormat
    {
        TextureFormat_RGBA8 = 0,   // RGBA8888
        TextureFormat_FLOAT32 = 1, // Float32
    };
    /* 纹理用途
    TextureUsage_Sampler = 1 << 0(1): 表示这个纹理可以被​​采样（Sampled）​​。
        这意味着它可以在着色器（Shader）中被读取，例如作为颜色贴图、法线贴图等。这是最常见的用途。
    TextureUsage_UploadData = 1 << 1(2): 表示这个纹理可以被用作​​上传数据的源​​。
        这通常发生在初始化纹理内容时，例如将 CPU 内存中的图像数据上传（复制）到 GPU 上的这个纹理对象中。
    TextureUsage_AttachmentColor = 1 << 2(4): 表示这个纹理可以被用作​​颜色附件（Color Attachment）​​。
        这意味着它可以被绑定到帧缓冲区（Framebuffer）上，作为渲染管线输出颜色的目标。渲染的结果会写入到这个纹理中。
    TextureUsage_AttachmentDepth = 1 << 3(8): 表示这个纹理可以被用作​​深度/模板附件（Depth/Stencil Attachment）​​。
        这意味着它可以被绑定到帧缓冲区上，用于存储深度值（可能还有模板值），在渲染过程中进行深度测试和模板测试。
    TextureUsage_RendererOutput = 1 << 4(16): 表示这个纹理是​​渲染器的最终输出目标​​。
        这通常指的是交换链（Swapchain）中的后缓冲（Back Buffer），即最终要呈现（Present）到屏幕上的图像。它通常也隐含了 TextureUsage_AttachmentColor的用途。
    */
    enum TextureUsage
    {
        TextureUsage_Sampler = 1 << 0,
        TextureUsage_UploadData = 1 << 1,
        TextureUsage_AttachmentColor = 1 << 2,
        TextureUsage_AttachmentDepth = 1 << 3,
        TextureUsage_RendererOutput = 1 << 4,
    };

    struct TextureDesc
    {
        int width = 0;
        int height = 0;
        TextureType type = TextureType_2D;
        TextureFormat format = TextureFormat_RGBA8;
        uint32_t usage = TextureUsage_Sampler;
        bool useMipmaps = false;
        bool multiSample = false;
        std::string tag;
    };
    
    class Texture : public TextureDesc
    {
    public:
        virtual ~Texture() = default;

        inline uint32_t getLevelWidth(uint32_t level)
        {
            return std::max(1, width >> level);
        }

        inline uint32_t getLevelHeight(uint32_t level)
        {
            return std::max(1, height >> level);
        }

        virtual int getId() const = 0;
        virtual void setSamplerDesc(SamplerDesc &sampler) {};
        virtual void initImageData() {};
        virtual void setImageData(const std::vector<std::shared_ptr<Buffer<RGBA>>> &buffers) {};
        virtual void setImageData(const std::vector<std::shared_ptr<Buffer<float>>> &buffers) {};
        virtual void dumpImage(const char *path, uint32_t layer, uint32_t level) = 0;
    };
}