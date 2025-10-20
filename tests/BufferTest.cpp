#include <gtest/gtest.h>
#include <memory>
#include <cstring>
#include "Utils/Buffer.h"

// 假设这是测试用的像素类型
struct TestPixel {
    uint8_t r, g, b, a;
    
    TestPixel() : r(0), g(0), b(0), a(255) {}
    TestPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
    
    bool operator==(const TestPixel& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

using namespace Learn;

class BufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试用的像素数据
        testPixel = TestPixel(100, 150, 200, 255);
        testPixel2 = TestPixel(50, 75, 100, 255);
    }
    
    void TearDown() override {}
    
    TestPixel testPixel;
    TestPixel testPixel2;
};

TEST_F(BufferTest, CreateAndDestroy) {
    auto buffer = Buffer<TestPixel>::makeDefault(100, 100);
    
    EXPECT_FALSE(buffer->isempty());
    EXPECT_EQ(buffer->getWidth(), 100);
    EXPECT_EQ(buffer->getHeight(), 100);
    EXPECT_EQ(buffer->getRawDataSize(), 100 * 100);
    
    buffer->destroy();
    EXPECT_TRUE(buffer->isempty());
    EXPECT_EQ(buffer->getWidth(), 0);
    EXPECT_EQ(buffer->getHeight(), 0);
}

TEST_F(BufferTest, PixelAccess) {
    auto buffer = Buffer<TestPixel>::makeDefault(10, 10);
    
    // 测试设置和获取像素
    buffer->set(5, 5, testPixel);
    TestPixel* pixel = buffer->get(5, 5);
    
    ASSERT_NE(pixel, nullptr);
    EXPECT_EQ(*pixel, testPixel);
    
    // 测试边界条件
    EXPECT_EQ(buffer->get(15, 15), nullptr); // 超出边界
    EXPECT_EQ(buffer->get(10, 5), nullptr);   // 边界值
}

TEST_F(BufferTest, ClearAndSetAll) {
    auto buffer = Buffer<TestPixel>::makeDefault(10, 10);
    
    // 先设置所有像素为特定值
    buffer->setAll(testPixel);
    
    // 验证设置成功
    TestPixel* pixel = buffer->get(3, 3);
    EXPECT_EQ(*pixel, testPixel);
    
    // 清空缓冲区
    buffer->clear();
    
    // 验证清空成功
    pixel = buffer->get(3, 3);
    EXPECT_EQ(pixel->r, 0);
    EXPECT_EQ(pixel->g, 0);
    EXPECT_EQ(pixel->b, 0);
}


TEST_F(BufferTest, LinearLayoutIndexConversion) {
    auto buffer = Buffer<TestPixel>::makeLayout(8, 8, Layout_Linear);
    
    // 测试线性布局的索引计算
    for (size_t y = 0; y < 8; y++) {
        for (size_t x = 0; x < 8; x++) {
            size_t expectedIndex = x + y * 8;
            size_t actualIndex = buffer->convertIndex(x, y);
            EXPECT_EQ(actualIndex, expectedIndex);
        }
    }
}

TEST_F(BufferTest, TiledLayoutIndexConversion) {
    auto buffer = Buffer<TestPixel>::makeLayout(8, 8, Layout_Tiled);
    
    // 测试分块布局的索引计算
    // 4x4分块，验证几个关键点的索引
    EXPECT_EQ(buffer->convertIndex(0, 0), 0);    // 第一个块的第一个像素
    EXPECT_EQ(buffer->convertIndex(3, 3), 15);   // 第一个块的最后一个像素
    EXPECT_EQ(buffer->convertIndex(4, 0), 16);   // 第二个块的第一个像素
    EXPECT_EQ(buffer->convertIndex(4, 3), 28);   // 第二个块的第三行第一个像素
}

TEST_F(BufferTest, MortonLayoutIndexConversion) {
    auto buffer = Buffer<TestPixel>::makeLayout(64, 64, Layout_Morton);
    
    // 测试莫顿布局的索引计算
    // 验证几个已知的莫顿编码点
    EXPECT_EQ(buffer->convertIndex(0, 0), 0);    // 原点
    
    // 验证Z-order曲线的连续性
    size_t index1 = buffer->convertIndex(0, 0);
    size_t index2 = buffer->convertIndex(1, 0);
    size_t index3 = buffer->convertIndex(0, 1);
    size_t index4 = buffer->convertIndex(1, 1);
    
    // 在莫顿编码中，相邻像素的索引应该是连续的
    EXPECT_LT(index1, index2);
    EXPECT_LT(index2, index3);
    EXPECT_LT(index3, index4);
}

TEST_F(BufferTest, CopyRawData) {
    auto buffer = Buffer<TestPixel>::makeDefault(3, 2);
    
    // 设置测试数据
    buffer->set(0, 0, testPixel);
    buffer->set(1, 0, testPixel2);
    buffer->set(2, 0, testPixel);
    buffer->set(0, 1, testPixel2);
    buffer->set(1, 1, testPixel);
    buffer->set(2, 1, testPixel2);
    
    // 测试正常拷贝
    std::vector<TestPixel> output(6);
    buffer->copyRawDataTo(output.data(), false);
    
    EXPECT_EQ(output[0], testPixel);   // (0,0)
    EXPECT_EQ(output[1], testPixel2);  // (1,0)
    EXPECT_EQ(output[2], testPixel);   // (2,0)
    EXPECT_EQ(output[3], testPixel2);  // (0,1)
    
    // 测试垂直翻转拷贝
    std::vector<TestPixel> outputFlipped(6);
    buffer->copyRawDataTo(outputFlipped.data(), true);
    
    EXPECT_EQ(outputFlipped[0], testPixel2);  // (0,1) 翻转到顶部
    EXPECT_EQ(outputFlipped[3], testPixel);   // (0,0) 翻转到底部
}


TEST_F(BufferTest, LayoutConsistency) {
    const size_t width = 16;
    const size_t height = 16;
    
    // 创建不同布局的缓冲区
    auto linearBuffer = Buffer<TestPixel>::makeLayout(width, height, Layout_Linear);
    auto tiledBuffer = Buffer<TestPixel>::makeLayout(width, height, Layout_Tiled);
    auto mortonBuffer = Buffer<TestPixel>::makeLayout(width, height, Layout_Morton);
    
    // 验证布局类型
    EXPECT_EQ(linearBuffer->getLayout(), Layout_Linear);
    EXPECT_EQ(tiledBuffer->getLayout(), Layout_Tiled);
    EXPECT_EQ(mortonBuffer->getLayout(), Layout_Morton);
    
    // 在所有缓冲区中设置相同的像素模式
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            TestPixel pixel(x, y, (x + y) % 256, 255);
            linearBuffer->set(x, y, pixel);
            tiledBuffer->set(x, y, pixel);
            mortonBuffer->set(x, y, pixel);
        }
    }
    
    // 验证所有缓冲区都能正确读取数据
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            TestPixel expected(x, y, (x + y) % 256, 255);
            
            TestPixel* linearPixel = linearBuffer->get(x, y);
            TestPixel* tiledPixel = tiledBuffer->get(x, y);
            TestPixel* mortonPixel = mortonBuffer->get(x, y);
            
            ASSERT_NE(linearPixel, nullptr);
            ASSERT_NE(tiledPixel, nullptr);
            ASSERT_NE(mortonPixel, nullptr);
            
            EXPECT_EQ(*linearPixel, expected);
            EXPECT_EQ(*tiledPixel, expected);
            EXPECT_EQ(*mortonPixel, expected);
        }
    }
}

TEST_F(BufferTest, BoundaryConditions) {
    auto buffer = Buffer<TestPixel>::makeDefault(10, 10);
    
    // 测试边界值访问
    EXPECT_NE(buffer->get(9, 9), nullptr);   // 最后一个像素
    EXPECT_EQ(buffer->get(10, 9), nullptr);  // 超出宽度
    EXPECT_EQ(buffer->get(9, 10), nullptr);  // 超出高度
    EXPECT_EQ(buffer->get(10, 10), nullptr); // 超出两个维度
    
    // 测试边界设置不会崩溃
    buffer->set(9, 9, testPixel);  // 应该成功
    buffer->set(10, 9, testPixel); // 应该被忽略
}

TEST_F(BufferTest, ZeroSizeBuffer) {
    // 测试创建0尺寸缓冲区
    auto buffer = Buffer<TestPixel>::makeDefault(0, 0);
    EXPECT_TRUE(buffer->isempty());
    EXPECT_EQ(buffer->getWidth(), 0);
    EXPECT_EQ(buffer->getHeight(), 0);
    
    // 在空缓冲区上操作应该安全
    EXPECT_EQ(buffer->get(0, 0), nullptr);
    buffer->set(0, 0, testPixel); // 应该被安全忽略
    buffer->clear(); // 应该不崩溃
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}