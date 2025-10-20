#include <gtest/gtest.h>
#include "Utils/Hash.h"
#include <unordered_set>

namespace Learn {
namespace Test {

// 测试 murmur3 哈希函数
TEST(HashTest, Murmur3Basic) {
    uint32_t key = 0x12345678;
    uint32_t hash = Hash::murmur3(&key, 1, 0);
    EXPECT_NE(hash, 0);  // 基本验证非零值
}


TEST(HashTest, Murmur3KnownValues) {
    // 空数据测试
    uint32_t dummy = 0;
    EXPECT_EQ(Hash::murmur3(&dummy, 0, 0), 0);

    // 已知值验证 (使用标准测试向量)
    uint32_t key1 = 0x00000000;
    EXPECT_EQ(Hash::murmur3(&key1, sizeof(key1), 0), 0x2362F9DE);

    uint32_t key2 = 0xFFFFFFFF;
    EXPECT_EQ(Hash::murmur3(&key2, sizeof(key2), 0), 0x76293B50);

    // 多元素测试
    uint32_t keys[] = {0xDEADBEEF, 0xCAFEBABE};
    EXPECT_EQ(Hash::murmur3(keys, sizeof(keys), 42), 0xB23BDFDE);
}
// 测试 hashCombine 模板函数
TEST(HashTest, HashCombine) {
    std::size_t seed = 0;
    Hash::hashCombine(seed, 42);
    EXPECT_NE(seed, 0);

    // 相同输入产生相同结果
    std::size_t seed1 = 0;
    Hash::hashCombine(seed1, 42);
    EXPECT_EQ(seed, seed1);

    // 不同输入产生不同结果
    std::size_t seed2 = 0;
    Hash::hashCombine(seed2, 43);
    EXPECT_NE(seed, seed2);

    // 顺序敏感性测试
    std::size_t seedA = 0;
    Hash::hashCombine(seedA, 10);
    Hash::hashCombine(seedA, 20);

    std::size_t seedB = 0;
    Hash::hashCombine(seedB, 20);
    Hash::hashCombine(seedB, 10);

    EXPECT_NE(seedA, seedB);
}

// 测试 hashCombineMurmur 模板函数
TEST(HashTest, HashCombineMurmur) {
    struct TestStruct {
        int a;
        float b;
        char c;
        // 确保结构体大小是4的倍数
        char padding[3]; 
    };
    static_assert(sizeof(TestStruct) % 4 == 0, "Size must be multiple of 4");

    TestStruct obj{42, 3.14f, 'X', {}};
    std::size_t seed = 0;
    Hash::hashCombineMurmur(seed, obj);
    EXPECT_NE(seed, 0);

    // 相同对象产生相同哈希
    std::size_t seed1 = 0;
    Hash::hashCombineMurmur(seed1, obj);
    EXPECT_EQ(seed, seed1);

    // 不同对象产生不同哈希
    TestStruct obj2{43, 3.14f, 'X', {}};
    std::size_t seed2 = 0;
    Hash::hashCombineMurmur(seed2, obj2);
    EXPECT_NE(seed, seed2);
}

// 测试 MD5 哈希函数
TEST(HashTest, MD5Basic) {
    // 空字符串
    EXPECT_EQ(Hash::getHashMD5("", 0), "d41d8cd98f00b204e9800998ecf8427e");
    EXPECT_EQ(Hash::getHashMD5(std::string()), "d41d8cd98f00b204e9800998ecf8427e");

    // 标准测试向量
    EXPECT_EQ(Hash::getHashMD5("hello", 5), "5d41402abc4b2a76b9719d911017c592");
    EXPECT_EQ(Hash::getHashMD5(std::string("hello")), "5d41402abc4b2a76b9719d911017c592");

    // 长字符串
    std::string longStr(1000, 'a');
    EXPECT_EQ(Hash::getHashMD5(longStr), "cabe45dcc9ae5b66ba86600cca6b8ba8");
}

TEST(HashTest, MD5Collision) {
    // 验证不同输入产生不同哈希
    EXPECT_NE(Hash::getHashMD5("hello"), Hash::getHashMD5("world"));
}

}  // namespace Test
}  // namespace Learn

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}