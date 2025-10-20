#include <gtest/gtest.h>
#include "Utils/AlignedMemory.h"  // 包含待测试的头文件

using namespace Learn;

TEST(AlignedMemoryTest, AlignedMalloc) {
    // 测试无效对齐值
    void* invalid_align_ptr = AlignedMemory::alignedMalloc(100, 31);
    EXPECT_EQ(invalid_align_ptr, nullptr); // 应返回 nullptr

    // 测试有效分配
    const size_t alignment = 64;
    void* ptr = AlignedMemory::alignedMalloc(100, alignment);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignment, 0); // 对齐验证

    // 测试 size=0
    void* zero_ptr = AlignedMemory::alignedMalloc(0);
    EXPECT_EQ(zero_ptr, nullptr); // 修复后应返回 nullptr

    AlignedMemory::alignedFree(ptr); // 清理
}

TEST(AlignedMemoryTest, AlignedFree) {
    AlignedMemory::alignedFree(nullptr); // 释放 nullptr 应安全

    void* ptr = AlignedMemory::alignedMalloc(100);
    ASSERT_NE(ptr, nullptr);
    AlignedMemory::alignedFree(ptr); // 正常释放
    // 不再进行双重释放！
}

// 测试 getAlignedSize 函数
TEST(AlignedMemoryTest, GetAlignedSize) {
    EXPECT_EQ(AlignedMemory::getAlignedSize(0), 0);
    EXPECT_EQ(AlignedMemory::getAlignedSize(1), 32);
    EXPECT_EQ(AlignedMemory::getAlignedSize(32), 32);
    EXPECT_EQ(AlignedMemory::getAlignedSize(33), 64);
    EXPECT_EQ(AlignedMemory::getAlignedSize(100), 128);
}

// 测试 makeAlignedBuffer 函数模板
TEST(AlignedMemoryTest, MakeAlignedBuffer) {
    // 测试 size=0
    auto zero_buf = AlignedMemory::makeAlignedBuffer<int>(0);
    EXPECT_EQ(zero_buf, nullptr);

    // 测试正常分配
    const size_t size = 100;
    auto buffer = AlignedMemory::makeAlignedBuffer<float>(size);
    ASSERT_NE(buffer, nullptr);
    
    // 验证内存对齐
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer.get());
    EXPECT_EQ(addr % ALIGNED_MEMORY_ALIGNMENT, 0);

    // 测试内存可用性
    for (size_t i = 0; i < size; ++i) {
        buffer.get()[i] = i * 1.0f;
    }

    // 验证自定义删除器（通过作用域退出自动释放）
}

// 测试 makeBuffer 函数模板
TEST(AlignedMemoryTest, MakeBuffer) {
    // 测试 size=0
    auto zero_buf = AlignedMemory::makeBuffer<int>(0);
    EXPECT_EQ(zero_buf, nullptr);

    // 测试无外部数据（使用 free 释放）
    auto buf_no_data = AlignedMemory::makeBuffer<int>(10);
    ASSERT_NE(buf_no_data, nullptr);
    buf_no_data.get()[0] = 42;  // 测试可写

    // 测试有外部数据（删除器为空）
    int external_data[10];
    auto buf_with_data = AlignedMemory::makeBuffer<int>(
        10, reinterpret_cast<const uint8_t*>(external_data));
    ASSERT_NE(buf_with_data, nullptr);
    EXPECT_EQ(buf_with_data.get(), external_data);  // 指向外部数据

    // 验证删除器行为（不应释放外部数据）
    // 通过作用域退出观察无崩溃
}