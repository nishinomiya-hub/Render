// #include <gtest/gtest.h>
// #include <vector>
// #include <cstring>

// // 包含被测试的头文件
// #include "Render/Soft/VertexSoft.h"

//如果要运行代码，在vertaxSoft里vertexArray的private注释掉
// namespace Learn {
// namespace Test {

// class VertexArrayObjectSoftTest : public ::testing::Test {
// protected:
//     void SetUp() override {
//         // 创建测试用的顶点数据
//         testVertexData = {
//             0.0f, 0.0f, 0.0f,  // 顶点0: x, y, z
//             1.0f, 0.0f, 0.0f,  // 顶点1: x, y, z
//             0.0f, 1.0f, 0.0f   // 顶点2: x, y, z
//         };

//         // 创建测试用的索引数据
//         testIndexData = {0, 1, 2};

//         // 设置顶点数组描述
//         VertexAttributeDesc desc;
//         desc.size = 3; // 3个float分量 (x, y, z)
//         desc.stride = 3 * sizeof(float); // 每个顶点12字节
//         desc.offset = 0;

//         // 设置顶点数组
//         vertexArray.vertexSize = sizeof(float) * 3;
//         vertexArray.vertexesDesc = {desc};
//         vertexArray.vertexesBuffer = reinterpret_cast<uint8_t*>(testVertexData.data());
//         vertexArray.vertexesBufferLength = testVertexData.size() * sizeof(float);
//         vertexArray.indexBuffer = testIndexData.data();
//         vertexArray.indexBufferLength = testIndexData.size() * sizeof(int32_t);
//     }

//     std::vector<float> testVertexData;
//     std::vector<int32_t> testIndexData;
//     VertexArray vertexArray;
// };

// // 测试构造函数是否正确初始化
// TEST_F(VertexArrayObjectSoftTest, ConstructorInitializesCorrectly) {
//     VertexArrayObjectSoft vao(vertexArray);
    
//     // 验证顶点数据
//     EXPECT_EQ(vao.vertexStride, 3 * sizeof(float));
//     EXPECT_EQ(vao.vertexCnt, 3);
//     EXPECT_EQ(vao.vertexes.size(), 3 * 3 * sizeof(float));
    
//     // 验证顶点数据内容
//     const float* vertexData = reinterpret_cast<const float*>(vao.vertexes.data());
//     for (size_t i = 0; i < testVertexData.size(); ++i) {
//         EXPECT_FLOAT_EQ(vertexData[i], testVertexData[i]);
//     }
    
//     // 验证索引数据
//     EXPECT_EQ(vao.indicesCnt, 3);
//     EXPECT_EQ(vao.indices.size(), 3);
//     for (size_t i = 0; i < testIndexData.size(); ++i) {
//         EXPECT_EQ(vao.indices[i], testIndexData[i]);
//     }
// }

// // 测试updateVertexData方法
// TEST_F(VertexArrayObjectSoftTest, UpdateVertexData) {
//     VertexArrayObjectSoft vao(vertexArray);
    
//     // 创建新的顶点数据
//     std::vector<float> newVertexData = {
//         2.0f, 2.0f, 2.0f,
//         3.0f, 3.0f, 3.0f,
//         4.0f, 4.0f, 4.0f
//     };
    
//     // 更新顶点数据
//     vao.updateVertexData(newVertexData.data(), newVertexData.size() * sizeof(float));
    
//     // 验证数据已更新
//     const float* updatedData = reinterpret_cast<const float*>(vao.vertexes.data());
//     for (size_t i = 0; i < newVertexData.size(); ++i) {
//         EXPECT_FLOAT_EQ(updatedData[i], newVertexData[i]);
//     }
// }

// // 测试updateVertexData边界情况（长度小于缓冲区）
// TEST_F(VertexArrayObjectSoftTest, UpdateVertexDataPartial) {
//     VertexArrayObjectSoft vao(vertexArray);
    
//     // 只更新部分数据
//     std::vector<float> partialData = {5.0f, 5.0f, 5.0f}; // 只更新第一个顶点
    
//     vao.updateVertexData(partialData.data(), 3 * sizeof(float));
    
//     const float* updatedData = reinterpret_cast<const float*>(vao.vertexes.data());
//     EXPECT_FLOAT_EQ(updatedData[0], 5.0f);
//     EXPECT_FLOAT_EQ(updatedData[1], 5.0f);
//     EXPECT_FLOAT_EQ(updatedData[2], 5.0f);
    
//     // 验证其他顶点数据保持不变
//     EXPECT_FLOAT_EQ(updatedData[3], 1.0f); // 第二个顶点x
// }

// // 测试updateVertexData边界情况（长度大于缓冲区）
// TEST_F(VertexArrayObjectSoftTest, UpdateVertexDataExceedsBuffer) {
//     VertexArrayObjectSoft vao(vertexArray);
    
//     // 创建比缓冲区大的数据
//     std::vector<float> largeData(100, 10.0f);
    
//     // 应该只拷贝缓冲区大小的数据
//     vao.updateVertexData(largeData.data(), largeData.size() * sizeof(float));
    
//     // 验证数据正确更新（没有越界）
//     const float* updatedData = reinterpret_cast<const float*>(vao.vertexes.data());
//     EXPECT_EQ(vao.vertexes.size(), 3 * 3 * sizeof(float));
    
//     // 前几个数据应该是10.0f
//     for (size_t i = 0; i < 3; ++i) {
//         EXPECT_FLOAT_EQ(updatedData[i], 10.0f);
//     }
// }

// // 测试getId方法
// TEST_F(VertexArrayObjectSoftTest, GetId) {
//     VertexArrayObjectSoft vao1(vertexArray);
//     VertexArrayObjectSoft vao2(vertexArray);
    
//     int id1 = vao1.getId();
//     int id2 = vao2.getId();
    
//     // ID应该是有效的（非零）
//     EXPECT_NE(id1, 0);
//     EXPECT_NE(id2, 0);
    
//     // 不同对象的ID应该不同
//     EXPECT_NE(id1, id2);
    
//     // 同一个对象的ID应该保持不变
//     EXPECT_EQ(vao1.getId(), id1);
// }

// // 测试空数据的情况
// TEST_F(VertexArrayObjectSoftTest, EmptyData) {
//     VertexArray emptyArray;
//     emptyArray.vertexesBuffer = nullptr;
//     emptyArray.vertexesBufferLength = 0;
//     emptyArray.indexBuffer = nullptr;
//     emptyArray.indexBufferLength = 0;
    
//     // 应该能正常构造，不崩溃
//     VertexArrayObjectSoft vao(emptyArray);
    
//     EXPECT_EQ(vao.vertexCnt, 0);
//     EXPECT_EQ(vao.indicesCnt, 0);
//     EXPECT_TRUE(vao.vertexes.empty());
//     EXPECT_TRUE(vao.indices.empty());
// }

// // 测试复杂顶点格式
// TEST_F(VertexArrayObjectSoftTest, ComplexVertexFormat) {
//     // 创建包含位置、法线、纹理坐标的复杂顶点数据
//     struct ComplexVertex {
//         float position[3];
//         float normal[3];
//         float texCoord[2];
//     };
    
//     std::vector<ComplexVertex> complexVertices = {
//         {{0,0,0}, {0,0,1}, {0,0}},
//         {{1,0,0}, {0,0,1}, {1,0}},
//         {{0,1,0}, {0,0,1}, {0,1}}
//     };
    
//     VertexArray complexArray;
//     VertexAttributeDesc posDesc, normalDesc, texDesc;
    
//     posDesc.size = 3;
//     posDesc.stride = sizeof(ComplexVertex);
//     posDesc.offset = offsetof(ComplexVertex, position);
    
//     normalDesc.size = 3;
//     normalDesc.stride = sizeof(ComplexVertex);
//     normalDesc.offset = offsetof(ComplexVertex, normal);
    
//     texDesc.size = 2;
//     texDesc.stride = sizeof(ComplexVertex);
//     texDesc.offset = offsetof(ComplexVertex, texCoord);
    
//     complexArray.vertexesDesc = {posDesc, normalDesc, texDesc};
//     complexArray.vertexesBuffer = reinterpret_cast<uint8_t*>(complexVertices.data());
//     complexArray.vertexesBufferLength = complexVertices.size() * sizeof(ComplexVertex);
    
//     // 应该能正常构造
//     VertexArrayObjectSoft vao(complexArray);
    
//     EXPECT_EQ(vao.vertexStride, sizeof(ComplexVertex));
//     EXPECT_EQ(vao.vertexCnt, 3);
// }

// } // namespace Test
// } // namespace SoftGL

// // 主函数
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }