#pragma once

#include <common/aglDisplayList.h>
#include <common/aglResShaderSymbol.h>
#include <common/aglResShaderVariation.h>
#include <common/aglShader.h>
#include <common/aglShaderLocation.h>
#include <container/Buffer.h>
#include <container/SafeArray.h>
#include <misc/rio_BitFlag.h>
#include <misc/Namable.h>

#if RIO_IS_WIN
#include <gpu/rio_Shader.h>
#include <cafe/gfd.h>
#endif // RIO_IS_WIN

#include <string>
#include <unordered_map>
#include <vector>

namespace agl {

class ShaderProgram
{
public:
    static const s32 cVariationMacroMax = 1024;
    static const s32 cVariationValueMax = 1024;

    static void changeShaderMode(ShaderMode mode);

public:
    ShaderProgram();
    virtual ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    s32 getVariationID() const
    {
        return mVariationID;
    }

#if RIO_IS_CAFE

    DisplayList& getDisplayList()
    {
        return mDisplayList;
    }

    const DisplayList& getDisplayList() const
    {
        return mDisplayList;
    }

#endif // RIO_IS_CAFE

    const char* getName() const
    {
        return mpSharedData->getName();
    }

    const ResShaderSymbolArray& getResShaderSymbolArray(ShaderSymbolType type) const
    {
        return mpSharedData->mResShaderSymbolArray[type];
    }

    void setResShaderSymbolArray(ShaderSymbolType type, const ResShaderSymbolArray& array)
    {
        mpSharedData->mResShaderSymbolArray[type] = array;
    }

    void setResShaderVariationDefaultArray(const ResShaderVariationArray& array)
    {
        mpSharedData->mResShaderVariationDefaultArray = array;
    }

    void initialize(const char* name);

    void createVariationBuffer(s32 macro_num);

    void createVariationMacro(s32 index, const char* name, const char* id, s32 value_num);
    void setVariationMacroValue(s32 macro_index, s32 value_index, const char* value);

    void createVariation();

    ShaderMode activate(ShaderMode current_mode = cShaderMode_Invalid, bool use_dl = true) const;

    Shader* getShader(ShaderType type);
    const Shader* getShader(ShaderType type) const;

#if RIO_IS_CAFE || RIO_IS_WIN
    GX2VertexShader* getVertexShaderBinary()
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mVertexShader.getBinary() : nullptr;
#else
        return mVertexShader.getBinary();
#endif // RIO_IS_WIN
    }

    const GX2VertexShader* getVertexShaderBinary() const
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mVertexShader.getBinary() : nullptr;
#else
        return mVertexShader.getBinary();
#endif // RIO_IS_WIN
    }

    GX2PixelShader* getFragmentShaderBinary()
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mFragmentShader.getBinary() : nullptr;
#else
        return mFragmentShader.getBinary();
#endif // RIO_IS_WIN
    }

    const GX2PixelShader* getFragmentShaderBinary() const
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mFragmentShader.getBinary() : nullptr;
#else
        return mFragmentShader.getBinary();
#endif // RIO_IS_WIN
    }

    GX2GeometryShader* getGeometryShaderBinary()
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mGeometryShader.getBinary() : nullptr;
#else
        return mGeometryShader.getBinary();
#endif // RIO_IS_WIN
    }

    const GX2GeometryShader* getGeometryShaderBinary() const
    {
        validate_();
#if RIO_IS_WIN
        return isUseBinaryProgram() ? mGeometryShader.getBinary() : nullptr;
#else
        return mGeometryShader.getBinary();
#endif // RIO_IS_WIN
    }
#endif // RIO_IS_CAFE || RIO_IS_WIN

#if RIO_IS_WIN
    rio::Shader* getShaderRIO()
    {
        updateCompile();
        return mShader.isLoaded() ? &mShader : nullptr;
    }

    const rio::Shader* getShaderRIO() const
    {
        updateCompile();
        return mShader.isLoaded() ? &mShader : nullptr;
    }

    void setUseBinaryProgram(bool enable)
    {
        mFlag.change(16, enable);

        if (getVariation_())
        {
            for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
                it->mFlag.change(16, enable);
        }
    }

    bool isUseBinaryProgram() const
    {
        return mFlag.isOn(16);
    }

    u32 updateCompile() const
    {
        if (mFlag.isOff(1))
            mFlag.set(1 | 2 | 8);

        return validate_();
    }
#endif // RIO_IS_WIN

    void createAttribute(s32 num);
    void setAttributeName(s32 index, const char* name);

    void createUniform(s32 num);
    void setUniformName(s32 index, const char* name);

    void createUniformBlock(s32 num);
    void setUniformBlockName(s32 index, const char* name);

    void createSamplerLocation(s32 num);
    void setSamplerLocationName(s32 index, const char* name);

    u32 setUpAllVariation(); // I don't know the actual return type
    void reserveSetUpAllVariation();

    s32 getVariationNum() const;
    s32 getVariationMacroNum() const;

    s32 searchVariationShaderProgramIndex(const std::unordered_map<std::string, std::string>& macro_map) const;

    ShaderProgram* getVariation(s32 index);
    const ShaderProgram* getVariation(s32 index) const;

    const ShaderProgram* searchVariationShaderProgram(const std::unordered_map<std::string, std::string>& macro_map) const
    {
        s32 index = searchVariationShaderProgramIndex(macro_map);
        return getVariation(index);
    }

    const std::string* searchVariationMacroName(const char* id) const;

    s32 getVariationMacroValueVariationNum(s32 macro_index) const;

    u32 updateVariation(s32 index) // I don't know the actual name
    {
        ShaderProgram* program = getVariation(index);
        program->mFlag.set(8 | 2);
        return program->validate_();
    }

    const AttributeLocation& getAttributeLocation(s32 index) const { return mAttributeLocation[index]; }
    const UniformLocation& getUniformLocation(s32 index) const { return mUniformLocation[index]; }
    const UniformBlockLocation& getUniformBlockLocation(s32 index) const { return mUniformBlockLocation[index]; }
    const SamplerLocation& getSamplerLocation(s32 index) const { return mSamplerLocation[index]; }

#if RIO_IS_WIN
    const AttributeLocation& getAttributeLocationValidate(s32 index) const { updateCompile(); return mAttributeLocation[index]; }
    const UniformLocation& getUniformLocationValidate(s32 index) const { updateCompile(); return mUniformLocation[index]; }
    const UniformBlockLocation& getUniformBlockLocationValidate(s32 index) const { updateCompile(); return mUniformBlockLocation[index]; }
    const SamplerLocation& getSamplerLocationValidate(s32 index) const { updateCompile(); return mSamplerLocation[index]; }
#else
    const AttributeLocation& getAttributeLocationValidate(s32 index) const { validate_(); return mAttributeLocation[index]; }
    const UniformLocation& getUniformLocationValidate(s32 index) const { validate_(); return mUniformLocation[index]; }
    const UniformBlockLocation& getUniformBlockLocationValidate(s32 index) const { validate_(); return mUniformBlockLocation[index]; }
    const SamplerLocation& getSamplerLocationValidate(s32 index) const { validate_(); return mSamplerLocation[index]; }
#endif // RIO_IS_WIN

    void updateAttributeLocation() const;
    void updateUniformLocation() const;
    void updateUniformBlockLocation() const;
    void updateSamplerLocation() const;

    void cleanUp();

    void destroyAttribute();
    void destroyUniform();
    void destroyUniformBlock();
    void destroySamplerLocation();

private:
    u32 validate_() const;
    u32 forceValidate_() const;

    void setUpForVariation_() const;

    void setShaderGX2_() const;

    class SharedData;
    class VariationBuffer;

private:
    class VariationBuffer
    {
        VariationBuffer();
        virtual ~VariationBuffer();

        VariationBuffer(const VariationBuffer&) = delete;
        VariationBuffer(VariationBuffer&&) = delete;
        VariationBuffer& operator=(const VariationBuffer&) = delete;
        VariationBuffer& operator=(VariationBuffer&&) = delete;

        void initialize(ShaderProgram* program, s32 macro_num);

        void createMacro(s32 index, const char* name, const char* id, s32 value_num);
        void setMacroValue(s32 macro_index, s32 value_index, const char* value);

        s32 searchShaderProgramIndex(const std::unordered_map<std::string, std::string>& macro_map, s32 index) const;

        const std::string* searchMacroName(const char* id) const;

        void create();

        void getMacroAndValueArray(s32 index, std::unordered_map<std::string, std::string>* p_macro_map) const;
        void getMacroValueIndexArray(s32 index, std::vector<s32>* p_value_index_array) const;
        s32 calcVariationIndex(const std::vector<s32>& value_index_array) const;

        s32 getMacroValueVariationNum(s32 macro_index) const
        {
            return mMacroData[macro_index].mValueVariationNum;
        }

        class MacroData
        {
            std::string mName;
            const char* mID;
            std::vector<std::string> mValue;
            u16 mValueVariationNum;                 // Number of variations using *succeeding* macros (i.e., ignoring preceding macros), if value of this macro is fixed.
                                                    // i.e., In a matrix where variations are the rows, macros are the columns, and a cell would be a macro's value
                                                    // in a certain variation, this field is the number a macro's value occurs before the next value in the macro's column.

            friend class VariationBuffer;
        };
      //static_assert(sizeof(MacroData) == 0x1C, "agl::ShaderProgram::VariationBuffer::MacroData size mismatch");

        ShaderProgram* mpOriginal;
        Buffer<ShaderProgram> mProgram;
        Buffer<MacroData> mMacroData;

        friend class ShaderProgram;
        friend class SharedData;
    };
    static_assert(sizeof(VariationBuffer) == 0x18, "agl::ShaderProgram::VariationBuffer size mismatch");

    class SharedData : public INamable
    {
        ShaderProgram* mpOriginal;
        VariationBuffer* mpVariationBuffer;
        u32 _10;
        ResShaderVariationArray mResShaderVariationDefaultArray; // Variation macro defaults
        UnsafeArray<ResShaderSymbolArray, cShaderSymbolType_Num> mResShaderSymbolArray;
        u32 _28;

        friend class ShaderProgram;
    };
    static_assert(sizeof(SharedData) == 0x2C, "agl::ShaderProgram::SharedData size mismatch");

    VariationBuffer* getVariation_() { return mpSharedData->mpVariationBuffer; }
    const VariationBuffer* getVariation_() const { return mpSharedData->mpVariationBuffer; }

private:
    mutable rio::BitFlag8 mFlag;
    u16 mVariationID;
#if RIO_IS_CAFE
    mutable DisplayList mDisplayList;
#endif // RIO_IS_CAFE
    mutable Buffer<AttributeLocation> mAttributeLocation;
    mutable Buffer<UniformLocation> mUniformLocation;
    mutable Buffer<UniformBlockLocation> mUniformBlockLocation;
    mutable Buffer<SamplerLocation> mSamplerLocation;
    VertexShader mVertexShader;
    FragmentShader mFragmentShader;
    GeometryShader mGeometryShader;
    SharedData* mpSharedData;

#if RIO_IS_WIN
    // Custom
    mutable rio::Shader mShader;
    mutable s32 mVsCfileBlockIdx;
    mutable s32 mPsCfileBlockIdx;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(ShaderProgram) == 0x60, "agl::ShaderProgram size mismatch");

}
