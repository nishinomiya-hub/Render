#pragma once

#include <string>
#include <cstdint>
#include "md5.h"

namespace Learn
{

  class Hash
  {
  public:
    // 实现自定义哈希函数（如 std::unordered_map的键类型为自定义结构体时）
    template <typename T>
    inline static void hashCombine(std::size_t &seed, const T &v)
    {
      seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    }

    // 计算 32 位整数数组的 MurmurHash3哈希值
    static inline uint32_t rotl32(uint32_t x, int r)
    {
      return (x << r) | (x >> (32 - r));
    }
    inline static uint32_t murmur3(const uint32_t *key, size_t len, uint32_t seed) noexcept
    {
      const uint8_t *data = (const uint8_t *)key;
      const int nblocks = len / 4;
      uint32_t h1 = seed;
      const uint32_t c1 = 0xcc9e2d51;
      const uint32_t c2 = 0x1b873593;

      // 处理每个 4 字节块
      const uint32_t *blocks = (const uint32_t *)(data);
      for (int i = 0; i < nblocks; i++)
      {
        uint32_t k1 = blocks[i];
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
      }
      // 处理尾部剩余字节
      const uint8_t *tail = data + nblocks * 4;
      uint32_t k1 = 0;
      switch (len & 3)
      {
      case 3:
        k1 ^= tail[2] << 16;
      case 2:
        k1 ^= tail[1] << 8;
      case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = rotl32(k1, 16);
        k1 *= c2;
        h1 ^= k1;
      }
      // 末尾混合
      h1 ^= len;
      h1 ^= h1 >> 16;
      h1 *= 0x85ebca6b;
      h1 ^= h1 >> 13;
      h1 *= 0xc2b2ae35;
      h1 ^= h1 >> 16;
      return h1;
    }

    // 用 MurmurHash3 计算对象 key的哈希值，并合并到 seed
    template <typename T>
    inline static void hashCombineMurmur(size_t &seed, const T &key)
    {
      static_assert(0 == (sizeof(key) & 3u), "Hashing requires a size that is a multiple of 4.");
      // 确保指针有效
      if constexpr (sizeof(T) > 0)
      {
        uint32_t keyHash = Hash::murmur3(
            reinterpret_cast<const uint32_t *>(&key),
            sizeof(T) / 4,
            0);
        seed ^= keyHash + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
      }
    }
    // 计算输入数据的 ​​MD5 哈希值​​（128 位），返回十六进制字符串
    inline static std::string getHashMD5(const char *data, size_t length)
    {
      unsigned char digest[17] = {0};

      MD5_CTX ctx;
      MD5_Init(&ctx);
      MD5_Update(&ctx, (unsigned char *)data, length);
      MD5_Final(digest, &ctx);

      char str[33] = {0};
      hexToStr(str, digest, 16);
      return {str};
    }

    inline static std::string getHashMD5(const std::string &text)
    {
      return getHashMD5(text.c_str(), text.length());
    }

  private:
    static void hexToStr(char *str, const unsigned char *digest, int length)
    {
      uint8_t hexDigit;
      for (int i = 0; i < length; i++)
      {
        hexDigit = (digest[i] >> 4) & 0xF;
        str[i * 2] = (hexDigit <= 9) ? (hexDigit + '0') : (hexDigit + 'a' - 10);
        hexDigit = digest[i] & 0xF;
        str[i * 2 + 1] = (hexDigit <= 9) ? (hexDigit + '0') : (hexDigit + 'a' - 10);
      }
    }
  };
}
