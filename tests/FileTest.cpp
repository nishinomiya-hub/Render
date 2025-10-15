#include <gtest/gtest.h>
#include "Base/File.h" // 包含被测试类的头文件
#include <filesystem>
#include <fstream>
#include <cstdlib> // 用于设置权限

namespace fs = std::filesystem;
using namespace Learn;

class FileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时测试目录
        testDir = fs::temp_directory_path() / "FileTest";
        fs::create_directory(testDir);
        
        // 创建测试文件
        testFilePath = testDir / "test.txt";
        std::ofstream(testFilePath) << "Hello World";
        
        // 创建空文件
        emptyFilePath = testDir / "empty.txt";
        std::ofstream(emptyFilePath).close();
    }

    void TearDown() override {
        // 清理测试目录
        fs::remove_all(testDir);
    }

    fs::path testDir;
    fs::path testFilePath;
    fs::path emptyFilePath;
};

// 测试 exists() 方法
TEST_F(FileTest, ExistsReturnsTrueForExistingFile) {
    EXPECT_TRUE(File::exists(testFilePath.string()));
}

TEST_F(FileTest, ExistsReturnsFalseForNonExistingFile) {
    EXPECT_FALSE(File::exists((testDir / "non_existent.txt").string()));
}

// 测试 readBytes() 方法
TEST_F(FileTest, ReadBytesReturnsContentForExistingFile) {
    std::vector<uint8_t> content = File::readBytes(testFilePath.string());
    std::string result(content.begin(), content.end());
    EXPECT_EQ("Hello World", result);
}

TEST_F(FileTest, ReadBytesReturnsEmptyForNonExistingFile) {
    std::vector<uint8_t> content = File::readBytes((testDir / "ghost.txt").string());
    EXPECT_TRUE(content.empty());
}

TEST_F(FileTest, ReadBytesReturnsEmptyForEmptyFile) {
    std::vector<uint8_t> content = File::readBytes(emptyFilePath.string());
    EXPECT_TRUE(content.empty());
}

// 测试 readText() 方法
TEST_F(FileTest, ReadTextReturnsContentForExistingFile) {
    std::string content = File::readText(testFilePath.string());
    EXPECT_EQ("Hello World", content);
}

TEST_F(FileTest, ReadTextReturnsEmptyForNonExistingFile) {
    std::string content = File::readText((testDir / "phantom.txt").string());
    EXPECT_TRUE(content.empty());
}

TEST_F(FileTest, ReadTextReturnsEmptyForEmptyFile) {
    std::string content = File::readText(emptyFilePath.string());
    EXPECT_TRUE(content.empty());
}

// 测试 writeBytes() 方法
TEST_F(FileTest, WriteBytesCreatesFileWithContent) {
    fs::path outputPath = testDir / "output.bin";
    std::vector<uint8_t> data = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
    
    EXPECT_TRUE(File::writeBytes(outputPath.string(), data));
    
    std::vector<uint8_t> readData = File::readBytes(outputPath.string());
    EXPECT_EQ(data, readData);
}

TEST_F(FileTest, WriteBytesFailsForInvalidPath) {
    fs::path invalidPath = "/proc/invalid_write_test.bin"; // 通常不可写的路径
    std::vector<uint8_t> data = {0x01, 0x02, 0x03};
    
    EXPECT_FALSE(File::writeBytes(invalidPath.string(), data));
}

// 测试 writeText() 方法
TEST_F(FileTest, WriteTextCreatesFileWithContent) {
    fs::path outputPath = testDir / "output.txt";
    std::string text = "Test writeText content";
    
    EXPECT_TRUE(File::writeText(outputPath.string(), text));
    
    std::string readText = File::readText(outputPath.string());
    EXPECT_EQ(text, readText);
}

TEST_F(FileTest, WriteTextHandlesLargeFiles) {
    fs::path largeFilePath = testDir / "large.txt";
    std::string largeText(10'000'000, 'A'); // 10MB文件
    
    EXPECT_TRUE(File::writeText(largeFilePath.string(), largeText));
    
    std::string readText = File::readText(largeFilePath.string());
    EXPECT_EQ(largeText.size(), readText.size());
    EXPECT_EQ(largeText, readText);
}

// TEST_F(FileTest, WriteTextFailsForInvalidPath) {
//     fs::path invalidPath = "/root/invalid_write_test.txt"; // 需要root权限的路径
//     if (geteuid() == 0) GTEST_SKIP() << "Skipping in root mode";
    
//     EXPECT_FALSE(File::writeText(invalidPath.string(), "test"));
// }

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}