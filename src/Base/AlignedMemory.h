#pragma once

#include <cmath>
#include <memory>
#include "Logger.h"

#define ALIGNED_MEMORY_ALIGNMENT 32

namespace Learn{

class AlignedMemory
{
private:
    /* data */
public:
    static void *alignedMalloc(size_t size, size_t alignment = ALIGNED_MEMORY_ALIGNMENT){
        //2的幂
        if((alignment == 0 ||alignment & (alignment - 1)) != 0){
            LOG_ERROR("AlignedMemory::alignedMalloc: alignment must be a power of two");
            return nullptr;
        }
    if(size == 0) return nullptr;
        //对齐+存储指针
        /*
        size = 100
        extra = 32+8
        add 的地址 设为0  总共分配了 100+ 40字节
        addr = 0+40
        alignedPtr = 40 - （40%32） = 32
        在 alignedPtr之前的前一个指针位置存放 add的地址 32 -8 = 24
        */
        size_t extra = alignment + sizeof(void *);
        void *data = malloc(size + extra);
        if(data == nullptr){
            LOG_ERROR("AlignedMemory::alignedMalloc: malloc failed");
            return nullptr;
        }

            // 计算对齐地址：原始地址 + extra，再对齐到 alignment 的倍数
        size_t addr = (size_t)data + extra;
        void *alignedPtr = (void *)(addr - (addr % alignment));

            // 在对齐地址前存储原始指针（用于释放）
        *((void **)alignedPtr - 1) = data;
        return alignedPtr;
    }

    static void alignedFree(void *ptr){
        if(ptr == nullptr){
            LOG_ERROR("AlignedMemory::alignedFree: ptr is null");
            return;
        }
        // 释放原始指针
        free(*((void **)ptr - 1));
    }

    static size_t getAlignedSize(size_t size){
        if (size == 0) {
        return 0;
        }
        //返回对齐后的大小  ALIGNED_MEMORY_ALIGNMENT的倍数  ceil向上取整
        return ALIGNED_MEMORY_ALIGNMENT * std::ceil((float) size / (float) ALIGNED_MEMORY_ALIGNMENT);
    }


    template<typename T>
    static std::shared_ptr<T> makeAlignedBuffer(size_t size){
        if(size == 0) return nullptr;
        void *ptr = alignedMalloc(size * sizeof(T));
        if(ptr == nullptr){
            LOG_ERROR("AlignedMemory::makeAlignedBuffer: alignedMalloc failed");
            return nullptr;
        }
        //std::shared_ptr<T>( 原始指针, 自定义删除器 );
        return std::shared_ptr<T>(
            static_cast<T *>(ptr), 
            [](const T *ptr){
                alignedFree((void *) ptr);
             }
        );
    }

    template<typename T>
    static std::shared_ptr<T> makeBuffer(size_t size,  const uint8_t *data = nullptr){ 
        if(size == 0) return nullptr;

        if(data != nullptr){
            //当使用外部数据时，调用者​​必须确保​​内存比 shared_ptr生命周期长
            return std::shared_ptr<T>(
                (T *) data,
                [](const T *ptr){}
            );
        }else{
            return std::shared_ptr<T>(
                new T[size], 
                [](const T *ptr) { delete[] ptr; }
            );
        }
    }
};


}