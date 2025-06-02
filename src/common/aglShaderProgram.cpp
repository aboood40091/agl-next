#include <common/aglDisplayList.h>
#include <common/aglShaderCompileInfo.h>
#include <common/aglShaderProgram.h>
#include <detail/aglFileIOMgr.h>
#include <detail/aglGX2.h>
//#include <gfx/seadGraphics.h>

#include <cstring>

#if RIO_IS_DESKTOP
#include <detail/aglShaderHolder.h>
#include <graphics/win/ShaderUtil.h>
#endif // RIO_IS_DESKTOP

namespace agl {

void ShaderProgram::changeShaderMode(ShaderMode mode)
{
#if RIO_IS_CAFE
    driver::GX2Resource::instance()->setShaderMode(mode);
#endif // RIO_IS_CAFE
}

ShaderProgram::ShaderProgram()
    : mFlag(0)
    , mVariationID(0)
#if RIO_IS_CAFE
    , mDisplayList()
#endif // RIO_IS_CAFE
    , mAttributeLocation()
    , mUniformLocation()
    , mUniformBlockLocation()
    , mSamplerLocation()
    , mVertexShader()
    , mFragmentShader()
    , mGeometryShader()
    , mpSharedData(nullptr)
#if RIO_IS_DESKTOP
    , mVsCfileBlockIdx(-1)
    , mPsCfileBlockIdx(-1)
#endif // RIO_IS_DESKTOP
{
}

ShaderProgram::~ShaderProgram()
{
    cleanUp();

    destroyAttribute();
    destroyUniform();
    destroySamplerLocation();
    destroyUniformBlock();

    if (mpSharedData->mpOriginal == this)
    {
        delete mpSharedData->mpVariationBuffer;
        delete mpSharedData;
    }
}

void ShaderProgram::initialize(const char* name)
{
    mpSharedData = new SharedData();
    mpSharedData->mpOriginal = this;
    mpSharedData->mpVariationBuffer = nullptr;
    mpSharedData->_10 = 0;
    mpSharedData->setName(name);
    mpSharedData->_28 = 0;

    for (UnsafeArray<ResShaderSymbolArray, cShaderSymbolType_Num>::iterator it = mpSharedData->mResShaderSymbolArray.begin(), it_end = mpSharedData->mResShaderSymbolArray.end(); it != it_end; ++it)
        *it = nullptr;
}

void ShaderProgram::createVariationBuffer(s32 macro_num)
{
    mpSharedData->mpVariationBuffer = new VariationBuffer();
    mpSharedData->mpVariationBuffer->initialize(this, macro_num);
}

void ShaderProgram::createVariationMacro(s32 index, const char* name, const char* id, s32 value_num)
{
    mpSharedData->mpVariationBuffer->createMacro(index, name, id, value_num);
}

void ShaderProgram::setVariationMacroValue(s32 macro_index, s32 value_index, const char* value)
{
    mpSharedData->mpVariationBuffer->setMacroValue(macro_index, value_index, value);
}

void ShaderProgram::createVariation()
{
    mpSharedData->mpVariationBuffer->create();

    for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
    {
        it->mpSharedData = mpSharedData;
        it->mVariationID = it.getIndex() + 1;
    }
}

ShaderMode ShaderProgram::activate(ShaderMode current_mode, bool use_dl) const
{
#if RIO_IS_CAFE
    ShaderMode mode = getGeometryShaderBinary()
        ? cShaderMode_GeometryShader
        : mVertexShader.getShaderMode();

    if (current_mode != mode)
        changeShaderMode(mode);

    current_mode = mode;

    if (current_mode == cShaderMode_GeometryShader)
        driver::GX2Resource::instance()->setGeometryShaderRingBuffer();
#elif RIO_IS_DESKTOP
    updateCompile();
#endif

#if RIO_IS_CAFE
    if (use_dl && !mDisplayList.isEmpty())
        mDisplayList.call();
    else
#endif // RIO_IS_CAFE
        setShaderGX2_();

    return current_mode;
}

Shader* ShaderProgram::getShader(ShaderType type)
{
    switch (type)
    {
    case cShaderType_Vertex:
        return &mVertexShader;
    case cShaderType_Fragment:
        return &mFragmentShader;
    case cShaderType_Geometry:
        return &mGeometryShader;
    default:
        return nullptr;
    }
}

const Shader* ShaderProgram::getShader(ShaderType type) const
{
    switch (type)
    {
    case cShaderType_Vertex:
        return &mVertexShader;
    case cShaderType_Fragment:
        return &mFragmentShader;
    case cShaderType_Geometry:
        return &mGeometryShader;
    default:
        return nullptr;
    }
}

void ShaderProgram::createAttribute(s32 num)
{
    RIO_ASSERT(mAttributeLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mAttributeLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation.allocBuffer(num);
    }
}

void ShaderProgram::setAttributeName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mAttributeLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation[index].setName(name);
    }
}

void ShaderProgram::createUniform(s32 num)
{
    RIO_ASSERT(mUniformLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation.allocBuffer(num);
    }
}

void ShaderProgram::setUniformName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation[index].setName(name);
    }
}

void ShaderProgram::createUniformBlock(s32 num)
{
    RIO_ASSERT(mUniformBlockLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformBlockLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation.allocBuffer(num);
    }
}

void ShaderProgram::setUniformBlockName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mUniformBlockLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation[index].setName(name);
    }
}

void ShaderProgram::createSamplerLocation(s32 num)
{
    RIO_ASSERT(mSamplerLocation.size() == 0);
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mSamplerLocation.allocBuffer(num);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation.allocBuffer(num);
    }
}

void ShaderProgram::setSamplerLocationName(s32 index, const char* name)
{
    RIO_ASSERT(getVariation_() == nullptr || getVariation_()->mpOriginal == this);

    mSamplerLocation[index].setName(name);

    if (getVariation_())
    {
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation[index].setName(name);
    }
}

u32 ShaderProgram::setUpAllVariation()
{
    u32 ret;

    if (getVariation_())
    {
        ret = getVariation_()->mpOriginal->validate_();
        if (ret == 0)
        {
            for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            {
                ret = it->validate_();
                if (ret != 0)
                    break;
            }
        }
    }
    else
    {
        ret = validate_();
    }

    return ret;
}

void ShaderProgram::reserveSetUpAllVariation()
{
    if (getVariation_())
    {
        getVariation_()->mpOriginal->mFlag.set(2);
        for (Buffer<ShaderProgram>::iterator it = getVariation_()->mProgram.begin(), it_end = getVariation_()->mProgram.end(); it != it_end; ++it)
            it->mFlag.set(2);
    }
    else
    {
        mFlag.set(2);
    }
}

s32 ShaderProgram::getVariationNum() const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 1;

    return variation_buffer->mProgram.size() + 1;
}

s32 ShaderProgram::getVariationMacroNum() const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->mMacroData.size();
}

s32 ShaderProgram::searchVariationShaderProgramIndex(const std::unordered_map<std::string, std::string>& macro_map) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->searchShaderProgramIndex(macro_map, mVariationID);
}

ShaderProgram* ShaderProgram::getVariation(s32 index)
{
    VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return this;

    if (index <= 0)
        return variation_buffer->mpOriginal;

    return &variation_buffer->mProgram[index - 1];
}

const ShaderProgram* ShaderProgram::getVariation(s32 index) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return this;

    if (index <= 0)
        return variation_buffer->mpOriginal;

    return &variation_buffer->mProgram[index - 1];
}

const std::string* ShaderProgram::searchVariationMacroName(const char* id) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return nullptr;

    return variation_buffer->searchMacroName(id);
}

s32 ShaderProgram::getVariationMacroValueVariationNum(s32 macro_index) const
{
    const VariationBuffer* variation_buffer = getVariation_();
    if (!variation_buffer)
        return 0;

    return variation_buffer->getMacroValueVariationNum(macro_index);
}

void ShaderProgram::updateAttributeLocation() const
{
    for (Buffer<AttributeLocation>::iterator it = mAttributeLocation.begin(), it_end = mAttributeLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateUniformLocation() const
{
    for (Buffer<UniformLocation>::iterator it = mUniformLocation.begin(), it_end = mUniformLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateUniformBlockLocation() const
{
    for (Buffer<UniformBlockLocation>::iterator it = mUniformBlockLocation.begin(), it_end = mUniformBlockLocation.end(); it != it_end; ++it)
        it->search(*this);
}

void ShaderProgram::updateSamplerLocation() const
{
    for (Buffer<SamplerLocation>::iterator it = mSamplerLocation.begin(), it_end = mSamplerLocation.end(); it != it_end; ++it)
        it->search(*this);
}

u32 ShaderProgram::validate_() const
{
    if (mFlag.isOn(2))
    {
        mFlag.reset(2);
        return forceValidate_();
    }

    return 0;
}

u32 ShaderProgram::forceValidate_() const
{
    u32 ret = 0;
    bool compile_source = mFlag.isOn(1);

    setUpForVariation_();

#if RIO_IS_DESKTOP
    if (mFlag.isOn(1) && isUseBinaryProgram())
    {
        const GX2VertexShader* p_vertex_shader = getVertexShaderBinary();
        const GX2PixelShader* p_pixel_shader = getFragmentShaderBinary();

        RIO_ASSERT(p_vertex_shader && p_pixel_shader);

        const GX2VertexShader& vertex_shader = *p_vertex_shader;
        const GX2PixelShader& pixel_shader = *p_pixel_shader;

        static const std::string sDecompPath[cShaderType_Num - 1] = {
            "shaders/agl_shader_temp_out.vert",
            "shaders/agl_shader_temp_out.frag"
        };

        [[maybe_unused]] bool decompile_ret = ShaderUtil::decompileGsh(
            vertex_shader,
            pixel_shader,
            sDecompPath[cShaderType_Vertex],
            sDecompPath[cShaderType_Fragment]
        );

        RIO_ASSERT(decompile_ret);

        mShader.load("agl_shader_temp_out");

        if (vertex_shader.shaderMode == GX2_SHADER_MODE_UNIFORM_REGISTERS)
            mVsCfileBlockIdx = mShader.getVertexUniformBlockIndex("VS_CFILE_DATA");

        else
            mVsCfileBlockIdx = -1;

        if (pixel_shader.shaderMode == GX2_SHADER_MODE_UNIFORM_REGISTERS)
            mPsCfileBlockIdx = mShader.getFragmentUniformBlockIndex("PS_CFILE_DATA");

        else
            mPsCfileBlockIdx = -1;
    }
    else
#endif // RIO_IS_DESKTOP
    if (mVertexShader.setUp(compile_source, mFlag.isOn(8)) != 0)
    {
        ret = 1;
    }
    else if (mFragmentShader.setUp(compile_source, mFlag.isOn(8)) != 0)
    {
        ret = 2;
    }
    else if (mGeometryShader.setUp(compile_source, mFlag.isOn(8)) == 1)
    {
        ret = 3;
    }
    else
    {
        if (mFlag.isOn(1))
        {
#if RIO_IS_DESKTOP
            mShader.unload();

            const ShaderCompileInfo* p_vert_compile_info = mVertexShader.getCompileInfo();
            RIO_ASSERT(p_vert_compile_info != nullptr);

            const std::string* p_vert_src = p_vert_compile_info->getRawText();
            RIO_ASSERT(p_vert_src != nullptr);

            const ShaderCompileInfo* p_frag_compile_info = mFragmentShader.getCompileInfo();
            RIO_ASSERT(p_frag_compile_info != nullptr);

            const std::string* p_frag_src = p_frag_compile_info->getRawText();
            RIO_ASSERT(p_frag_src != nullptr);

            mShader.load(p_vert_src->c_str(), p_frag_src->c_str());

            mVsCfileBlockIdx = -1;
            mPsCfileBlockIdx = -1;
#elif RIO_IS_CAFE
            if (mDisplayList.getBuffer() != nullptr)
            {
                void* p_dl;
                size_t size = DisplayList::suspend(&p_dl);
                {
                    mDisplayList.beginDisplayList();
                    {
                        setShaderGX2_();
                    }
                    mDisplayList.endDisplayList();
                }
                DisplayList::resume(p_dl, size);
            }
#endif
        }
#if RIO_IS_CAFE
        else
        {
            if (mDisplayList.getBuffer() != nullptr)
            {
              //sead::Graphics::instance()->lockDrawContext();
                {
                    mDisplayList.beginDisplayList();
                    {
                        setShaderGX2_();
                    }
                    mDisplayList.endDisplayList();
                }
              //sead::Graphics::instance()->unlockDrawContext();
            }
        }

        mFlag.setDirect(1);
#endif // RIO_IS_CAFE
    }

    if (mFlag.isOn(1))
    {
        updateUniformLocation();
        updateUniformBlockLocation();
        updateAttributeLocation();
        updateSamplerLocation();
    }

    // TODO
    // if (mpSharedData->_10)
    //     mpSharedData->_10->vf0C(this);

    return ret;
}

void ShaderProgram::setUpForVariation_() const
{
    if (!getVariation_())
        return;

    const ShaderProgram* program = getVariation_()->mpOriginal;

    std::unordered_map<std::string, std::string> macro_map;
    getVariation_()->getMacroAndValueArray(mVariationID, &macro_map);

    for (s32 type = 0; type < cShaderType_Num; type++)
    {
        ShaderCompileInfo* compile_info = program->getShader(ShaderType(type))->getCompileInfo();
        if (!compile_info)
            continue;

        compile_info->clearVariation();

        for (const auto& itr_variation : macro_map)
            compile_info->pushBackVariation(itr_variation.first, itr_variation.second);

        getShader(ShaderType(type))->setCompileInfo(compile_info);
    }
}

void ShaderProgram::setShaderGX2_() const
{
#if RIO_IS_CAFE
    if (mVertexShader.getBinary())
        GX2SetVertexShader(mVertexShader.getBinary());

    if (mFragmentShader.getBinary())
        GX2SetPixelShader(mFragmentShader.getBinary());

    if (mGeometryShader.getBinary())
        GX2SetGeometryShader(mGeometryShader.getBinary());
#elif RIO_IS_DESKTOP
    mShader.bind();

    mShader.setUniform(7u,      u32(-1), mShader.getFragmentUniformLocation("PS_PUSH.alphaFunc"));
    mShader.setUniform(u32(-1), u32(-1), mShader.getFragmentUniformLocation("PS_PUSH.uItemID"));
    mShader.setUniform(0,       u32(-1), mShader.getFragmentUniformLocation("PS_PUSH.uIsSelected"));

    if (isUseBinaryProgram())
    {
        mShader.setUniform(rio::BaseVec4f{ 1.0f, -1.0f, 0.0f, 0.0f }, mShader.getVertexUniformLocation("VS_PUSH.posMulAdd"), u32(-1));
        mShader.setUniform(rio::BaseVec4f{ 0.0f,  1.0f, 1.0f, 1.0f }, mShader.getVertexUniformLocation("VS_PUSH.zSpaceMul"), u32(-1));
        mShader.setUniform(1.0f,                                      mShader.getVertexUniformLocation("VS_PUSH.pointSize"), u32(-1));

        mShader.setUniform(0u, u32(-1), mShader.getFragmentUniformLocation("PS_PUSH.needsPremultiply"));

        if (mVsCfileBlockIdx != -1)
        {
            detail::ShaderHolder::instance()->mVsCfile.bind(mVsCfileBlockIdx);
        }

        if (mPsCfileBlockIdx != -1)
        {
            detail::ShaderHolder::instance()->mPsCfile.bind(mPsCfileBlockIdx);
        }
    }
#endif
}

void ShaderProgram::cleanUp()
{
    if (mFlag.isOn(1))
        mFlag.reset(1);
}

void ShaderProgram::destroyAttribute()
{
    mAttributeLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mAttributeLocation.freeBuffer();
    }
}

void ShaderProgram::destroyUniform()
{
    mUniformLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mUniformLocation.freeBuffer();
    }
}

void ShaderProgram::destroyUniformBlock()
{
    mUniformBlockLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mUniformBlockLocation.freeBuffer();
    }
}

void ShaderProgram::destroySamplerLocation()
{
    mSamplerLocation.freeBuffer();

    VariationBuffer* variation_buffer = getVariation_();
    if (variation_buffer)
    {
        for (Buffer<ShaderProgram>::iterator it = variation_buffer->mProgram.begin(), it_end = variation_buffer->mProgram.end(); it != it_end; ++it)
            it->mSamplerLocation.freeBuffer();
    }
}

ShaderProgram::VariationBuffer::VariationBuffer()
    : mProgram()
    , mMacroData()
{
}

ShaderProgram::VariationBuffer::~VariationBuffer()
{
    mProgram.freeBuffer();
    mMacroData.freeBuffer();
}

void ShaderProgram::VariationBuffer::initialize(ShaderProgram* program, s32 macro_num)
{
    mpOriginal = program;
    mMacroData.allocBuffer(macro_num);
}

void ShaderProgram::VariationBuffer::createMacro(s32 index, const char* name, const char* id, s32 value_num)
{
    MacroData& macro = mMacroData[index];
    macro.mName = name;
    macro.mID = id;
    macro.mValueVariationNum = 1;
    macro.mValue.resize(value_num);
}

void ShaderProgram::VariationBuffer::setMacroValue(s32 macro_index, s32 value_index, const char* value)
{
    mMacroData[macro_index].mValue[value_index] = value;
}

s32 ShaderProgram::VariationBuffer::searchShaderProgramIndex(const std::unordered_map<std::string, std::string>& macro_map, s32 index) const
{
    std::vector<s32> value_index_array;

    if (index == -1)
    {
        value_index_array.resize(mMacroData.size());
    }
    else
    {
        getMacroValueIndexArray(index, &value_index_array);
    }

    if (!macro_map.empty())
    {
        for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
        {
            const auto& itr_match_macro = macro_map.find(itr_type->mName);
            if (itr_match_macro == macro_map.end())
                continue;

            const std::string& macro_value = itr_match_macro->second;
            for (std::vector<std::string>::const_iterator itr_value = itr_type->mValue.begin(), value_it_end = itr_type->mValue.end(); itr_value != value_it_end; ++itr_value)
            {
                if (macro_value == *itr_value)
                {
                    value_index_array[itr_type.getIndex()] = itr_value - itr_type->mValue.begin();
                    break;
                }
            }
        }
    }

    return calcVariationIndex(value_index_array);
}

const std::string* ShaderProgram::VariationBuffer::searchMacroName(const char* id) const
{
    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
        if (std::strcmp(id, itr_type->mID) == 0)
            return &itr_type->mName;

    return nullptr;
}

void ShaderProgram::VariationBuffer::create()
{
    s32 variation_num = 1;

    for (Buffer<MacroData>::iterator it = mMacroData.begin(), it_end = mMacroData.end(); it != it_end; ++it)
    {
        for (s32 i = it.getIndex() + 1; i < mMacroData.size(); i++)
            it->mValueVariationNum *= mMacroData[i].mValue.size();

        variation_num *= it->mValue.size();
    }

    if (variation_num - 1 > 0)
        mProgram.allocBuffer(variation_num - 1);
}

void ShaderProgram::VariationBuffer::getMacroAndValueArray(s32 index, std::unordered_map<std::string, std::string>* p_macro_map) const
{
    RIO_ASSERT(p_macro_map != nullptr);
    p_macro_map->clear();
    p_macro_map->reserve(mMacroData.size());

    for (const MacroData& macro : mMacroData)
    {
        s32 value_index; // = 0;
      // if (macro.mValueVariationNum != 0)
            value_index = index / macro.mValueVariationNum;

        [[maybe_unused]] const auto& itr = p_macro_map->try_emplace(
            macro.mName,
            macro.mValue[value_index]
        );
        RIO_ASSERT(itr.second);

        index -= value_index * macro.mValueVariationNum;
    }
}

void ShaderProgram::VariationBuffer::getMacroValueIndexArray(s32 index, std::vector<s32>* p_value_index_array) const
{
    RIO_ASSERT(p_value_index_array != nullptr);
    p_value_index_array->clear();
    p_value_index_array->reserve(mMacroData.size());

    for (const MacroData& macro : mMacroData)
    {
        s32 value_index; // = 0;
      // if (macro.mValueVariationNum != 0)
            value_index = index / macro.mValueVariationNum;

        p_value_index_array->push_back(value_index);

        index -= value_index * macro.mValueVariationNum;
    }
}

s32 ShaderProgram::VariationBuffer::calcVariationIndex(const std::vector<s32>& value_index_array) const
{
    RIO_ASSERT(value_index_array.size() >= size_t(mMacroData.size()));

    s32 index = 0;
    for (Buffer<MacroData>::constIterator itr_type = mMacroData.begin(), it_end = mMacroData.end(); itr_type != it_end; ++itr_type)
    {
        s32 value_index = value_index_array[itr_type.getIndex()];
        index += value_index * itr_type->mValueVariationNum;
    }
    return index;
}

}
