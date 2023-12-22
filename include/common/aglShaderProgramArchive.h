#pragma once

#include <common/aglResShaderArchive.h>
#include <common/aglResShaderSource.h>
#include <common/aglShaderCompileInfo.h>
#include <common/aglShaderProgram.h>
#include <container/Buffer.h>
#include <container/SafeArray.h>
#include <misc/rio_BitFlag.h>

namespace agl {

class ShaderProgramArchive
{
    class ShaderProgramEx;
    class ShaderSource;

    class ShaderCompileInfoEx
    {
    public:
        ShaderCompileInfoEx()
            : mCompileInfo()
        {
        }

        ~ShaderCompileInfoEx()
        {
        }

    private:
        ShaderCompileInfo mCompileInfo;
        ShaderSource* mSource;

        // Custom
        std::string mRawTextEx;

        friend class ShaderProgramArchive;
    };
  //static_assert(sizeof(ShaderCompileInfoEx) == 0x58, "agl::ShaderProgramArchive::ShaderCompileInfoEx size mismatch");

    class ShaderProgramEx
    {
    public:
        ShaderProgramEx();
        virtual ~ShaderProgramEx();

        ShaderProgramEx(const ShaderProgramEx&) = delete;
        ShaderProgramEx(ShaderProgramEx&&) = delete;
        ShaderProgramEx& operator=(const ShaderProgramEx&) = delete;
        ShaderProgramEx& operator=(ShaderProgramEx&&) = delete;

        void initialize(ShaderProgramArchive* archive, s32 index, ResShaderProgram res);
        void updateRawText();
        void updateAnalyze();

    private:
        s32 mIndex;
        ShaderProgramArchive* mpArchive;
        UnsafeArray<ShaderCompileInfoEx, cShaderType_Num> mCompileInfoEx;
        Buffer<u32> _110; // No idea buffer of what
        s32 mVariationIndex;
        rio::BitFlag8 mFlag;

        friend class ShaderProgramArchive;
    };
  //static_assert(sizeof(ShaderProgramEx) == 0x124, "agl::ShaderProgramArchive::ShaderProgramEx size mismatch");

    class ShaderSource
    {
    public:
        ShaderSource();

        void initialize(ShaderProgramArchive* archive, s32 index, ResShaderSource res, bool is_used);
        void expand();

        const char* getName() const
        {
            return mRes.getName();
        }

    private:
        s32 mIndex;
        rio::BitFlag32 mFlag;
        ShaderProgramArchive* mpArchive;
        ResShaderSource mRes;

        std::string mOriginalName;
        std::string mOriginalText;
        std::string mRawText;

        friend class ShaderProgramArchive;
    };
  //static_assert(sizeof(ShaderSource) == 0x30, "agl::ShaderProgramArchive::ShaderSource size mismatch");

public:
    ShaderProgramArchive();
    ~ShaderProgramArchive();

    ShaderProgramArchive(const ShaderProgramArchive&) = delete;
    ShaderProgramArchive(ShaderProgramArchive&&) = delete;
    ShaderProgramArchive& operator=(const ShaderProgramArchive&) = delete;
    ShaderProgramArchive& operator=(ShaderProgramArchive&&) = delete;

    void destroy();

    bool isValid() const
    {
        return mResBinary.isValid() || mResText.isValid();
    }

    const char* getName() const
    {
        if (mResBinary.isValid())
            return mResBinary.getName();
        else
            return mResText.getName();
    }

    s32 getShaderProgramNum() const
    {
        return mProgram.size();
    }

    ShaderProgram& getShaderProgram(s32 idx)
    {
        return mProgram[idx];
    }

    const ShaderProgram& getShaderProgram(s32 idx) const
    {
        return mProgram[idx];
    }

    void createWithOption(ResBinaryShaderArchive res_binary_archive, ResShaderArchive res_archive, u32 flag);
    bool setUp();

    s32 searchShaderProgramIndex(const char* name) const;

    const ShaderProgram* searchShaderProgram(const char* name) const
    {
        s32 index = searchShaderProgramIndex(name);
        if (index >= 0 && index < mProgram.size())
            return mProgram.unsafeGet(index);

        return nullptr;
    }

    const ShaderProgram* searchShaderProgram(s32 index) const
    {
        return mProgram.get(index);
    }

    void updateCompileInfo();

private:
    void destroyResFile_();
    void setResShaderArchive_(ResShaderArchive res_archive);
    bool setUp_(bool);

private:
    ResBinaryShaderArchive mResBinary;
    ResShaderArchive mResText;
    Buffer<ShaderProgram> mProgram;
    u32 _20;
    u32 _24;
    u16 _28;
    rio::BitFlag16 mFlag;
    Buffer<ShaderProgramEx> mProgramEx;

    // Custom
    std::vector<ShaderSource> mSourceVec;
    std::unordered_map<std::string, const std::string> mSourceMap;

#if RIO_IS_CAFE
    void* mpDLBuf;
#endif // RIO_IS_CAFE

    friend class ShaderSource;
    friend class ShaderProgramEx;
};
//static_assert(sizeof(ShaderProgramArchive) == 0x4C, "agl::ShaderProgramArchive size mismatch");

}
