#pragma once

// Geometry.h

#include "GLM_Header.h"

namespace Learn
{

    // 轴对齐包围盒
    /*
    表示一个轴对齐的立方体区域，用于快速检测物体是否在特定范围内（如视锥体裁剪）
    */
    class BoundingBox
    {
    public:
        glm::vec3 min{0.f, 0.f, 0.f};
        glm::vec3 max{0.f, 0.f, 0.f};

    public:
        BoundingBox() = default;
        BoundingBox(const glm::vec3 &min_, const glm::vec3 &max_) : min(min_), max(max_) {}

        void getCorners(glm::vec3 *dst) const;                // 计算包围盒的8个角点
        BoundingBox transform(const glm::mat4 &matrix) const; // 应用变换矩阵并返回新包围盒
        bool intersects(const BoundingBox &box) const;        // 检测两包围盒是否相交
        void merge(const BoundingBox &box);                   // 扩展当前包围盒以包含另一个包围盒

    protected:
        static void updateMinMax(glm::vec3 *point, glm::vec3 *min, glm::vec3 *max);
    };

    /*
    表示一个无限延伸的平面，用于空间分割（如视锥体的六个面）
    */
    class Plane
    {
    public:
        enum PlaneIntersects
        {
            Intersects_Cross = 0,
            Intersects_Tangent = 1,
            Intersects_Front = 2,
            Intersects_Back = 3
        };
        /*
        Intersects_Cross：横跨平面（部分在前，部分在后）。
        Intersects_Tangent：与平面相切（如包围盒的一个面贴在平面上）。
        Intersects_Front：完全在平面正面（法线指向的一侧）。
        Intersects_Back：完全在平面背面。
        */
    private:
        glm::vec3 normal_;
        float d_ = 0;

    public:
        void set(const glm::vec3 &n, const glm::vec3 &pt); // 通过法线和一个点初始化平面
        float distance(const glm::vec3 &pt) const;         // 计算点到平面的有符号距离
        inline const glm::vec3 &getNormal() const;

        // 相交检测方法：
        PlaneIntersects intersects(const BoundingBox &box) const;                                        // 与包围盒
        PlaneIntersects intersects(const glm::vec3 &p0) const;                                           // 与点
        PlaneIntersects intersects(const glm::vec3 &p0, const glm::vec3 &p1) const;                      // 与线段
        PlaneIntersects intersects(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) const; // 与三角形
    };

    //表示相机视锥体（一个平截头体），用于检测物体是否在可见范围内
    struct Frustum
    {
    public:
        bool intersects(const BoundingBox &box) const;

        // check intersect with point (world space)
        bool intersects(const glm::vec3 &p0) const;

        // check intersect with line segment (world space)
        bool intersects(const glm::vec3 &p0, const glm::vec3 &p1) const;

        // check intersect with triangle (world space)
        bool intersects(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) const;

    public:
        /**
         * planes[0]: near;
         * planes[1]: far;
         * planes[2]: top;
         * planes[3]: bottom;
         * planes[4]: left;
         * planes[5]: right;
         */
        Plane planes[6];

        /**
         * corners[0]: nearTopLeft;
         * corners[1]: nearTopRight;
         * corners[2]: nearBottomLeft;
         * corners[3]: nearBottomRight;
         * corners[4]: farTopLeft;
         * corners[5]: farTopRight;
         * corners[6]: farBottomLeft;
         * corners[7]: farBottomRight;
         */
        glm::vec3 corners[8];

        BoundingBox bbox;
    };

    enum FrustumClipMask
    {
        POSITIVE_X = 1 << 0,
        NEGATIVE_X = 1 << 1,
        POSITIVE_Y = 1 << 2,
        NEGATIVE_Y = 1 << 3,
        POSITIVE_Z = 1 << 4,
        NEGATIVE_Z = 1 << 5,
    };

    const int FrustumClipMaskArray[6] = {
        FrustumClipMask::POSITIVE_X,
        FrustumClipMask::NEGATIVE_X,
        FrustumClipMask::POSITIVE_Y,
        FrustumClipMask::NEGATIVE_Y,
        FrustumClipMask::POSITIVE_Z,
        FrustumClipMask::NEGATIVE_Z,
    };

    const glm::vec4 FrustumClipPlane[6] = {
        {-1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, -1, 0, 1},
        {0, 1, 0, 1},
        {0, 0, -1, 1},
        {0, 0, 1, 1}};

}
