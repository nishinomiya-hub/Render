#pragma once

#include "Render/Base/ShaderProgram.h"
#include "Utils/File.h"
#include "Render/OpenGL/GLSLUtils.h"

namespace Learn
{
    class ShaderProgramOpenGL : public ShaderProgram
    {
    private:
        GLuint programId_ = 0;
        ProgramGLSL programGLSL_;

        int uniformBlockBinding_ = 0;
        int uniformSamplerBinding_ = 0;

    public:
        int getId() const override
        {
            return programId_;
        }
        void addDefine(const std::string &def) override
        {
            programGLSL_.addDefine(def);
        }
        bool compileAndLinkFile(const std::string &vsPath, const std::string &fsPath)
        {
            return compileAndLink(File::readText(vsPath), File::readText(fsPath));
        }
        bool compileAndLink(const std::string &vsSource, const std::string &fsSource)
        {
            bool ret = programGLSL_.loadSource(vsSource, fsSource);
            programId_ = programGLSL_.getId();
            return ret;
        }
        inline void use()
        {
            programGLSL_.use();
            uniformBlockBinding_ = 0;
            uniformSamplerBinding_ = 0;
        }
        inline int getUniformBlockBinding()
        {
            return uniformBlockBinding_++;
        }

        inline int getUniformSamplerBinding()
        {
            return uniformSamplerBinding_++;
        }
    };
}