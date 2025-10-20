#include <gtest/gtest.h>
#include "Render/Base/Texture.h"
#include "Render/Soft/TextureSoft.h"

namespace Learn {
namespace Test {

class TextureSoftTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建基本的纹理描述
        desc2D.width = 256;
        desc2D.height = 256;
        desc2D.type = TextureType_2D;
        desc2D.format = TextureFormat_RGBA8;
        desc2D.usage = TextureUsage_Sampler;
        desc2D.useMipmaps = false;
        desc2D.multiSample = false;
        desc2D.tag = "TestTexture2D";

        // 立方体贴图描述
        descCube.width = 128;
        descCube.height = 128;
        descCube.type = TextureType_CUBE;
        descCube.format = TextureFormat_RGBA8;
        descCube.usage = TextureUsage_Sampler;
        descCube.tag = "TestTextureCube";
    }

    void TearDown() override {
        // 清理代码
    }

    TextureDesc desc2D;
    TextureDesc descCube;
};

// 测试构造函数和基本属性
TEST_F(TextureSoftTest, ConstructorAndBasicProperties) {
    TextureSoft<RGBA> texture(desc2D);
    
    EXPECT_EQ(texture.width, 256);
    EXPECT_EQ(texture.height, 256);
    EXPECT_EQ(texture.type, TextureType_2D);
    EXPECT_EQ(texture.format, TextureFormat_RGBA8);
    EXPECT_GE(texture.getId(), 0); // ID应该大于0
}

// 测试立方体贴图构造
TEST_F(TextureSoftTest, CubeMapConstructor) {
    TextureSoft<RGBA> texture(descCube);
    
    EXPECT_EQ(texture.width, 128);
    EXPECT_EQ(texture.height, 128);
    EXPECT_EQ(texture.type, TextureType_CUBE);
    EXPECT_EQ(texture.layerCount_, 6); // 立方体贴图应该有6个面
}

// 测试采样器设置
TEST_F(TextureSoftTest, SamplerDesc) {
    TextureSoft<RGBA> texture(desc2D);
    
    SamplerDesc sampler;
    sampler.filterMin = Filter_LINEAR;
    sampler.filterMag = Filter_LINEAR;
    sampler.wrapS = Wrap_REPEAT;
    sampler.wrapT = Wrap_REPEAT;
    sampler.borderColor = Border_WHITE;
    
    texture.setSamplerDesc(sampler);
    auto& retrievedSampler = texture.getSamplerDesc();
    
    EXPECT_EQ(retrievedSampler.filterMin, Filter_LINEAR);
    EXPECT_EQ(retrievedSampler.filterMag, Filter_LINEAR);
    EXPECT_EQ(retrievedSampler.wrapS, Wrap_REPEAT);
    EXPECT_EQ(retrievedSampler.borderColor, Border_WHITE);
}

// 测试图像数据初始化
TEST_F(TextureSoftTest, InitImageData) {
    TextureSoft<RGBA> texture(desc2D);
    
    texture.initImageData();
    
    // 检查第一层是否已初始化
    auto& image = texture.getImage(0);
    EXPECT_FALSE(image.empty());
    EXPECT_EQ(image.getWidth(), 256);
    EXPECT_EQ(image.getHeight(), 256);
    
    // 检查缓冲区是否创建
    auto& buffer = image.getBuffer(0);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->width, 256);
    EXPECT_EQ(buffer->height, 256);
    EXPECT_FALSE(buffer->multiSample);
}

// 测试设置图像数据
TEST_F(TextureSoftTest, SetImageData) {
    TextureSoft<RGBA> texture(desc2D);
    
    // 创建测试缓冲区
    auto buffer = Buffer<RGBA>::makeDefault(256, 256);
    std::vector<std::shared_ptr<Buffer<RGBA>>> buffers = {buffer};
    
    texture.setImageData(buffers);
    
    auto& image = texture.getImage(0);
    EXPECT_FALSE(image.empty());
    EXPECT_EQ(image.getBuffer(0)->width, 256);
}

// 测试边界颜色获取
TEST_F(TextureSoftTest, BorderColor) {
    TextureSoft<RGBA> texture(desc2D);
    
    SamplerDesc sampler;
    sampler.borderColor = Border_WHITE;
    texture.setSamplerDesc(sampler);
    
    RGBA borderColor;
    texture.getBorderColor(borderColor);
    
    // 白色边界颜色应该是(255, 255, 255, 255)
    EXPECT_EQ(borderColor.r, 255);
    EXPECT_EQ(borderColor.g, 255);
    EXPECT_EQ(borderColor.b, 255);
    EXPECT_EQ(borderColor.a, 255);
}

// 测试mipmap级别计算
TEST_F(TextureSoftTest, MipmapLevelCalculation) {
    TextureSoft<RGBA> texture(desc2D);
    
    EXPECT_EQ(texture.getLevelWidth(0), 256);
    EXPECT_EQ(texture.getLevelWidth(1), 128);
    EXPECT_EQ(texture.getLevelWidth(2), 64);
    EXPECT_EQ(texture.getLevelHeight(0), 256);
    EXPECT_EQ(texture.getLevelHeight(1), 128);
    
    // 测试边界情况
    EXPECT_EQ(texture.getLevelWidth(10), 1); // 最小为1
    EXPECT_EQ(texture.getLevelHeight(10), 1);
}

// 测试多采样纹理
TEST_F(TextureSoftTest, MultiSampleTexture) {
    TextureDesc descMS = desc2D;
    descMS.multiSample = true;
    
    TextureSoft<RGBA> texture(descMS);
    texture.initImageData();
    
    auto& image = texture.getImage(0);
    auto& buffer = image.getBuffer(0);
    
    EXPECT_TRUE(buffer->multiSample);
    EXPECT_EQ(buffer->sampleCnt, SOFT_MS_CNT);
    EXPECT_NE(buffer->bufferMs4x, nullptr);
}

// 测试Float32格式纹理
TEST_F(TextureSoftTest, Float32Texture) {
    TextureDesc descFloat = desc2D;
    descFloat.format = TextureFormat_FLOAT32;
    
    TextureSoft<float> texture(descFloat);
    texture.initImageData();
    
    EXPECT_EQ(texture.format, TextureFormat_FLOAT32);
    
    auto& image = texture.getImage(0);
    EXPECT_FALSE(image.empty());
}

// 测试错误处理
TEST_F(TextureSoftTest, ErrorHandling) {
    TextureSoft<RGBA> texture(desc2D);
    
    // 测试尺寸不匹配
    auto smallBuffer = Buffer<RGBA>::makeDefault(128, 128); // 错误尺寸
    std::vector<std::shared_ptr<Buffer<RGBA>>> buffers = {smallBuffer};
    
    // 这里应该记录错误，但不会崩溃
    texture.setImageData(buffers);
    
    // 测试多采样纹理设置图像数据（应该报错）
    TextureDesc descMS = desc2D;
    descMS.multiSample = true;
    TextureSoft<RGBA> textureMS(descMS);
    
    textureMS.setImageData(buffers); // 应该记录错误
}

// 测试文件IO（需要准备测试文件）
// TEST_F(TextureSoftTest, FileIO) {
//     TextureSoft<RGBA> texture(desc2D);
//     texture.initImageData();
    
//     const char* testFile = "test_texture.bin";
    
//     // 存储到文件
//     texture.storeToFile(testFile);
    
//     // 从文件加载
//     TextureSoft<RGBA> textureLoaded(desc2D);
//     bool loadSuccess = textureLoaded.loadFromFile(testFile);
    
//     EXPECT_TRUE(loadSuccess);
    
//     // 清理测试文件
//     std::remove(testFile);
// }

} // namespace Test
} // namespace SoftGL

// 主函数
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}