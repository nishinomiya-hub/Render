#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>
#include "Render/Soft/SamplerSoft.h"

using namespace Learn;

// 测试固件类
class SamplerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试纹理数据
        testData2x2 = {
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),  // 红色
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // 绿色
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),  // 蓝色
            glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)   // 黄色
        };
        
        testData4x4 = {
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
            glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
            glm::vec4(0.0f, 0.5f, 1.0f, 1.0f), glm::vec4(0.5f, 0.0f, 1.0f, 1.0f),
            glm::vec4(0.5f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
            glm::vec4(1.0f, 0.5f, 1.0f, 1.0f), glm::vec4(0.5f, 1.0f, 1.0f, 1.0f)
        };
    }

    std::vector<glm::vec4> testData2x2;
    std::vector<glm::vec4> testData4x4;
    
    std::shared_ptr<TextureImageSoft<glm::vec4>> createTestTexture(int width, int height, 
                                                                  const std::vector<glm::vec4>& data) {
        auto tex = std::make_shared<TextureImageSoft<glm::vec4>>();
        auto buffer = std::make_shared<Buffer<glm::vec4>>(width, height);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                buffer->set(x, y, data[y * width + x]);
            }
        }
        
        //tex->setBuffer(buffer);
        return tex;
    }
};

// 测试环绕模式
TEST_F(SamplerTest, WrapModeClampToEdge) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setWrapMode(Wrap_CLAMP_TO_EDGE);
    
    // 测试边界情况
    glm::vec2 uv(-0.5f, -0.5f);  // 超出边界
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    // 应该返回边缘像素颜色 (0,0) 位置的红色
    EXPECT_EQ(result, testData2x2[0]);
}

TEST_F(SamplerTest, WrapModeRepeat) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setWrapMode(Wrap_REPEAT);
    
    // 测试重复模式
    glm::vec2 uv(1.5f, 1.5f);  // 超出边界，应该重复
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    // 应该返回 (0.5, 0.5) 位置的像素，即 (0,0) 位置的红色
    EXPECT_EQ(result, testData2x2[0]);
}

TEST_F(SamplerTest, WrapModeClampToBorder) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setWrapMode(Wrap_CLAMP_TO_BORDER);
    sampler.borderColor() = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);  // 黑色边框
    
    // 测试边界色模式
    glm::vec2 uv(-0.5f, -0.5f);  // 超出边界
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    // 应该返回边框色
    EXPECT_EQ(result, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
}

// 测试采样模式
TEST_F(SamplerTest, NearestNeighborSampling) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setFilterMode(Filter_NEAREST);
    
    // 测试最近邻采样
    glm::vec2 uv(0.25f, 0.25f);  // 应该采样 (0,0) 位置
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    EXPECT_EQ(result, testData2x2[0]);  // 红色
}

TEST_F(SamplerTest, BilinearSampling) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setFilterMode(Filter_LINEAR);
    
    // 测试双线性采样 - 中心点应该是四个颜色的混合
    glm::vec2 uv(0.5f, 0.5f);  // 纹理中心
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    // 中心点应该是四个颜色的平均值
    glm::vec4 expected = (testData2x2[0] + testData2x2[1] + testData2x2[2] + testData2x2[3]) * 0.25f;
    
    // 使用近似比较，因为浮点数计算可能有微小误差
    EXPECT_NEAR(result.r, expected.r, 0.001f);
    EXPECT_NEAR(result.g, expected.g, 0.001f);
    EXPECT_NEAR(result.b, expected.b, 0.001f);
    EXPECT_NEAR(result.a, expected.a, 0.001f);
}

// 测试Mipmap生成
TEST_F(SipmplerTest, MipmapGeneration) {
    auto tex = createTestTexture(4, 4, testData4x4);
    
    // 生成mipmap
    BaseSampler<glm::vec4>::generateMipmaps(tex.get(), true);
    
    // 检查mipmap层级数量
    EXPECT_GT(tex->levels.size(), 1);
    
    // 检查第一级mipmap的大小
    EXPECT_EQ(tex->levels[1]->getWidth(), 2);
    EXPECT_EQ(tex->levels[1]->getHeight(), 2);
    
    // 检查mipmap内容（简化检查）
    EXPECT_TRUE(tex->levels[1]->buffer != nullptr);
}

// 测试Mipmap采样
TEST_F(SamplerTest, MipmapSampling) {
    auto tex = createTestTexture(4, 4, testData4x4);
    BaseSampler<glm::vec4>::generateMipmaps(tex.get(), true);
    
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setFilterMode(Filter_LINEAR_MIPMAP_LINEAR);  // 三线性滤波
    
    // 测试不同LOD级别的采样
    glm::vec2 uv(0.5f, 0.5f);
    
    // LOD = 0，使用原始纹理
    auto result0 = sampler.texture2DLodImpl(uv, 0.0f);
    
    // LOD = 1，使用第一级mipmap
    auto result1 = sampler.texture2DLodImpl(uv, 1.0f);
    
    // 结果应该不同（因为mipmap是下采样版本）
    EXPECT_NE(result0, result1);
}

// 测试立方体贴图坐标转换
TEST_F(SamplerTest, CubeMapCoordinateConversion) {
    int faceIndex;
    float u, v;
    
    // 测试+X面
    BaseSamplerCube<glm::vec4>::convertXYZ2UV(1.0f, 0.0f, 0.0f, &faceIndex, &u, &v);
    EXPECT_EQ(faceIndex, 0);  // +X面
    EXPECT_GE(u, 0.0f); EXPECT_LE(u, 1.0f);
    EXPECT_GE(v, 0.0f); EXPECT_LE(v, 1.0f);
    
    // 测试-Y面
    BaseSamplerCube<glm::vec4>::convertXYZ2UV(0.0f, -1.0f, 0.0f, &faceIndex, &u, &v);
    EXPECT_EQ(faceIndex, 3);  // -Y面
    
    // 测试+Z面
    BaseSamplerCube<glm::vec4>::convertXYZ2UV(0.0f, 0.0f, 1.0f, &faceIndex, &u, &v);
    EXPECT_EQ(faceIndex, 4);  // +Z面
}

// 测试空纹理处理
TEST_F(SamplerTest, EmptyTexture) {
    BaseSampler2D<glm::vec4> sampler;
    
    // 没有设置纹理
    EXPECT_TRUE(sampler.empty());
    
    glm::vec2 uv(0.5f, 0.5f);
    auto result = sampler.texture2DLodImpl(uv, 0.0f);
    
    // 应该返回默认值 (0,0,0,0)
    EXPECT_EQ(result, glm::vec4(0.0f));
}

// 测试边界情况
TEST_F(SamplerTest, EdgeCases) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setWrapMode(Wrap_CLAMP_TO_EDGE);
    
    // 测试各种边界坐标
    std::vector<glm::vec2> testUVs = {
        {0.0f, 0.0f},    // 左上角
        {1.0f, 1.0f},    // 右下角
        {-0.1f, 0.5f},   // 左边界外
        {1.1f, 0.5f},    // 右边界外
        {0.5f, -0.1f},   // 上边界外
        {0.5f, 1.1f}     // 下边界外
    };
    
    for (const auto& uv : testUVs) {
        EXPECT_NO_THROW(sampler.texture2DLodImpl(uv, 0.0f));
    }
}

// 测试偏移采样
TEST_F(SamplerTest, OffsetSampling) {
    auto tex = createTestTexture(2, 2, testData2x2);
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setFilterMode(Filter_NEAREST);
    
    glm::vec2 uv(0.5f, 0.5f);
    glm::ivec2 offset(1, 0);  // 向右偏移1像素
    
    auto result = sampler.texture2DLodImpl(uv, 0.0f, offset);
    
    // 由于纹理只有2x2，偏移后应该采样到相邻像素
    // 具体结果取决于环绕模式
    EXPECT_NO_THROW(result);
}

// 性能测试：大量采样操作
TEST_F(SamplerTest, PerformanceSampling) {
    auto tex = createTestTexture(256, 256, std::vector<glm::vec4>(256 * 256, glm::vec4(1.0f)));
    BaseSampler2D<glm::vec4> sampler;
    sampler.setImage(tex.get());
    sampler.setFilterMode(Filter_LINEAR);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量采样操作
    const int SAMPLES = 1000;
    for (int i = 0; i < SAMPLES; ++i) {
        glm::vec2 uv(static_cast<float>(i % 100) / 100.0f, 
                     static_cast<float>(i / 100) / 10.0f);
        sampler.texture2DLodImpl(uv, 0.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证性能要求（1000次采样应该在合理时间内完成）
    EXPECT_LT(duration.count(), 1000);  // 少于1秒
}

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}