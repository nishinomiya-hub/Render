#pragma once

#include "Base/UID.h"
#include "Render/Vertex.h"

namespace Learn
{

    class VertexArrayObjectSoft : public VertexArrayObject
    {
    public:
        size_t vertexStride = 0;
        size_t vertexCnt = 0;
        size_t indicesCnt = 0;
        std::vector<uint8_t> vertexes;
        std::vector<int32_t> indices;

    private:
        UID<VertexArrayObjectSoft> uid_;
        explicit VertexArrayObjectSoft(const VertexArray &vertexArray)
        {
            vertexStride = vertexArray.vertexesDesc[0].stride;
            vertexCnt = vertexArray.vertexesBufferLength / vertexStride;
            vertexes.resize(vertexCnt * vertexStride);
            memcpy(vertexes.data(), vertexArray.vertexesBuffer, vertexArray.vertexesBufferLength);

            indicesCnt = vertexArray.indexBufferLength / sizeof(int32_t);
            indices.resize(indicesCnt);
            memcpy(indices.data(), vertexArray.indexBuffer, vertexArray.indexBufferLength);
        }

        void updateVertexData(void *data, size_t length) override
        {
            memcpy(vertexes.data(), data, std::min(length, vertexes.size()));
        }
        int getId() const override{
            return uid_.get();
        }
    };
}
