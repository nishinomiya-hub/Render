#pragma once

#include <unordered_map>
#include <set>
#include "Uniform.h"

namespace Learn{

    class ShaderProgram {
    protected:
        std::unordered_map<int, int> UniformLocations_;
        virtual bool bindUniform(Uniform &uniform) {
            int hash = uniform.getHash();
            int location = -1;
            if (UniformLocations_.find(hash) != UniformLocations_.end()) {
                location = UniformLocations_[hash];
            } else {
                location = uniform.getLocation(*this);
                UniformLocations_[hash] = location;
            }

            if(location < 0) return false;
            uniform.bindProgram(*this, location);
            return true;
        }
    public: 
        virtual int getId() const = 0;
        virtual void addDefine(const std::string &define) = 0;
        virtual void addDefines(const std::set<std::string> &defines){
            for (auto &define: defines) {
                addDefine(define);
            }
        }
        virtual void bindUniform(ShaderResources & resources) {
            for (auto &kv : resources.blocks) {
            bindUniform(*kv.second);
            }

            for (auto &kv : resources.samplers) {
            bindUniform(*kv.second);
            }
        }
        
    };

}