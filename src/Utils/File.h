#pragma once

#include <fstream>
#include <vector>
#include "Logger.h"

namespace Learn{

class File {
public:
    static bool exists(const std::string &path){
        return std::ifstream(path).good();
    }

    static std::vector<uint8_t> readBytes(const std::string &path){
        std::vector<uint8_t> bytes;
        std::ifstream file(path, std::ios::binary| std::ios::ate | std::ios::in);
        if(!file.is_open()){
            LOG_ERROR("File %s does not exist", path.c_str());
            return bytes;
        }

        size_t size = file.tellg();
        if(size <= 0){
            LOG_ERROR("File %s is empty", path.c_str());
            return bytes;
        }

        file.seekg(0, std::ios::beg);
        bytes.resize(size);
        file.read((char*)bytes.data(), size);
        file.close();

        if (file.gcount() != size) {
            LOG_ERROR("Failed to read full content of %s", path.c_str());
            bytes.clear(); // 清空向量
        }

        return bytes;
    }

    static std::string readText(const std::string &path){
        auto data = readBytes(path);
        if (data.empty()) {
        return "";
        }

        return std::string((char *) data.data(), data.size());

    }
    
    static bool writeBytes(const std::string &path, const std::vector<uint8_t> &bytes){
        std::ofstream file(path, std::ios::binary | std::ios::out);
        if(!file.is_open()){
            LOG_ERROR("Failed to open file %s for writing", path.c_str());
            return false;
        }

        file.write((char*)bytes.data(), bytes.size());
        file.close();
        return true;
    }

    static bool writeText(const std::string &path, const std::string &text){
        std::ofstream file(path, std::ios::out);
        if(!file.is_open()){
            LOG_ERROR("Failed to open file %s for writing", path.c_str());
            return false;
        }

        //file << text;
        //大文件
        file.write(text.c_str(), text.size());
        file.close();
        return true;
    }
    
};
}