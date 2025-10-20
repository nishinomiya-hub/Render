#pragma once

#include <string>

namespace Learn {
/*
文件扩展名检查（如 .png, .mp3）
URL 协议验证（如 https://, ftp://）
字符串格式校验（如日志前缀、指令前缀）
路径处理（如检查路径是否以 /开头）
*/
class StringUtils {
 public:

  inline static bool endsWith(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
  }

  inline static bool startsWith(const std::string &str, const std::string &prefix) {
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
  }

};

}