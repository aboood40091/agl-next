#include <common/aglShaderProgramArchive.h>
#include <detail/aglShaderTextUtil.h>

#include <cstring>

#if RIO_IS_CAFE
#include <cafe.h>
#endif // RIO_IS_CAFE

namespace {

void createShaderProgram(
    agl::ShaderProgram* program,
    const char* name,
    const agl::ResShaderVariationArray& variation_macros,
    const agl::ResShaderVariationArray& variation_macro_defaults,
    const agl::ResShaderSymbolArray& attributes,
    const agl::ResShaderSymbolArray& uniforms,
    const agl::ResShaderSymbolArray& uniform_blocks,
    const agl::ResShaderSymbolArray& samplers
)
{
    program->initialize(name);

    program->setResShaderVariationDefaultArray(variation_macro_defaults);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Attribute, attributes);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Uniform, uniforms);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_UniformBlock, uniform_blocks);
    program->setResShaderSymbolArray(agl::cShaderSymbolType_Sampler, samplers);

    u32 num_variation_macro = variation_macros.getNum();
    if (num_variation_macro == 0)
        return;

    program->createVariationBuffer(num_variation_macro);

    for (agl::ResShaderVariationArray::constIterator it = variation_macros.begin(), it_end = variation_macros.end(); it != it_end; ++it)
    {
        const agl::ResShaderVariation variation(&(*it));

        program->createVariationMacro(
            it.getIndex(),
            variation.getName(),
            variation.getID(),
            static_cast<s32>(variation.ref().mValueNum)
        );

        for (s32 i = 0; i < static_cast<s32>(variation.ref().mValueNum); i++)
        {
            program->setVariationMacroValue(
                it.getIndex(),
                i,
                variation.getValue(i)
            );
        }
    }

    program->createVariation();
}

}

namespace agl {

ShaderProgramArchive::ShaderProgramArchive()
    : mResBinary()
    , mResText()
    , mProgram()
    , _20(0)
    , _24(0)
    , _28(0)
    , mFlag()
    , mProgramEx()
#if RIO_IS_CAFE
    , mpDLBuf(nullptr)
#endif // RIO_IS_CAFE
{
  //detail::RootNode::setNodeMeta(this, "Icon = LAYOUT, Security = agl_shader");
}

ShaderProgramArchive::~ShaderProgramArchive()
{
    destroy();
}

void ShaderProgramArchive::destroy()
{
    destroyResFile_();
    mProgram.freeBuffer();
    mResBinary = nullptr;
    _28 = 0;

#if RIO_IS_CAFE
    if (mpDLBuf)
    {
        rio::MemUtil::free(mpDLBuf);
        mpDLBuf = nullptr;
    }
#endif // RIO_IS_CAFE
}

void ShaderProgramArchive::destroyResFile_()
{
    if (mResText.isValid())
    {
        mProgramEx.freeBuffer();

        mSourceVec.clear();
        mSourceMap.clear();
    }
    mResText = nullptr;
}

void ShaderProgramArchive::createWithOption(ResBinaryShaderArchive res_binary_archive, ResShaderArchive res_archive, u32 flag)
{
    mResBinary = res_binary_archive;

    if (uintptr_t(mResBinary.ptr()) % cShaderArchiveAlignment != 0)
    {
        RIO_LOG("agl::ResBinaryShaderArchive must be aligned agl::cShaderArchiveAlignment byte.\n");
        RIO_ASSERT(false);
    }

    if (flag & 2)
        mFlag.set(1);

    if (mResBinary.isValid())
    {
        mResBinary.setUp(true);

        mProgram.allocBuffer(mResBinary.getResBinaryShaderProgramNum());

#if RIO_IS_CAFE
        bool create_dl = !(flag & 1);

        u8* dl_buf = nullptr;
        u32 cur_dl_offs = 0;
        const u32 dl_size = 0x220;

        if (create_dl)
        {
            u32 dl_buf_size = mResBinary.getResShaderBinaryNum() * dl_size;
            dl_buf = static_cast<u8*>(rio::MemUtil::alloc(dl_buf_size, DisplayList::cDisplayListAlignment));
            DCFlushRange(dl_buf, dl_buf_size);
        }

        mpDLBuf = dl_buf;
#endif // RIO_IS_CAFE

        const ResBinaryShaderProgramArray binary_prog_arr = mResBinary.getResBinaryShaderProgramArray();
        [[maybe_unused]] u32 verify_binary_num = 0;

        for (ResBinaryShaderProgramArray::constIterator it = binary_prog_arr.begin(), it_end = binary_prog_arr.end(); it != it_end; ++it)
        {
            const ResBinaryShaderProgram binary_prog(&(*it));

            ShaderProgram& program = mProgram[it.getIndex()];

            createShaderProgram(
                &program,
                binary_prog.getName(),
                binary_prog.getResShaderVariationArray(),
                binary_prog.getResShaderVariationDefaultArray(),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Attribute),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Uniform),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_UniformBlock),
                binary_prog.getResShaderSymbolArray(cShaderSymbolType_Sampler)
            );

            for (s32 i = 0; i < program.getVariationNum(); i++)
            {
                ShaderProgram* var_program = program.getVariation(i);

                for (s32 type = 0; type < cShaderType_Num; type++)
                {
                    Shader* shader = var_program->getShader(ShaderType(type));
                    if (binary_prog.ref().mKind & 1 << type)
                    {
                        u32 num_shader = 3;
                        if ((binary_prog.ref().mKind >> cShaderType_Geometry & 1) == 0)
                            num_shader = 2;

                        u32 index = binary_prog.ref().mBaseIndex + type + num_shader * i;
                        if (index != 0xFFFFFFFF)
                        {
                            const ResShaderBinary binary = mResBinary.getResShaderBinaryArray().get(index);
                            RIO_ASSERT(binary.isValid());
                            RIO_ASSERT(binary.getShaderType() == type);
#if RIO_IS_CAFE
                            DCFlushRangeNoSync(binary.getData(), binary.ref().mDataSize);
#endif // RIO_IS_CAFE
                            shader->setBinary(binary.getData());

                            RIO_ASSERT(verify_binary_num == index);
                            verify_binary_num++;
                        }
                    }
                }

#if RIO_IS_CAFE
                if (create_dl)
                {
                    var_program->getDisplayList().setBuffer(dl_buf + cur_dl_offs, dl_size);
                    cur_dl_offs += dl_size;
                }
#endif // RIO_IS_CAFE
            }
        }

        RIO_ASSERT(verify_binary_num == mResBinary.getResShaderBinaryArray().getNum());
    }

    setResShaderArchive_(res_archive);

    for (Buffer<ShaderProgram>::iterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
        it->reserveSetUpAllVariation();

#if RIO_IS_DESKTOP
    if (!res_archive.isValid())
        for (Buffer<ShaderProgram>::iterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
            it->setUseBinaryProgram(true);
#endif // RIO_IS_DESKTOP

    if (mProgram.size() > 0)
        return;

    RIO_LOG("No shader program.\n");
    RIO_ASSERT(false);
}

bool ShaderProgramArchive::setUp()
{
    updateCompileInfo();
    return setUp_(mFlag.isOn(1));
}

s32 ShaderProgramArchive::searchShaderProgramIndex(const char* name) const
{
    for (Buffer<ShaderProgram>::constIterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
        if (std::strcmp(it->getName(), name) == 0)
            return it.getIndex();

    return -1;
}

void ShaderProgramArchive::updateCompileInfo()
{
    for (auto& source : mSourceVec)
        if (source.mFlag.isOn(1))
            source.expand();

    for (Buffer<ShaderProgramEx>::iterator it = mProgramEx.begin(), it_end = mProgramEx.end(); it != it_end; ++it)
        it->updateRawText();

    for (auto& source : mSourceVec)
        source.mFlag.reset(1);
}

void ShaderProgramArchive::setResShaderArchive_(ResShaderArchive res_archive)
{
    destroyResFile_();

    if (!res_archive.isValid())
        return;

    mResText = res_archive;
    mResText.setUp();

    bool do_init_programs = false;

    if (!mResBinary.isValid())
    {
        mProgram.allocBuffer(mResText.getResShaderProgramNum());
        do_init_programs = true;
    }
    else
    {
        RIO_ASSERT(mResText.getResShaderProgramNum() == mResBinary.getResBinaryShaderProgramNum());
    }

    mSourceVec.resize(mResText.getResShaderSourceNum());
    mSourceMap.reserve(mSourceVec.size());

    mProgramEx.allocBuffer(mProgram.size());

    UnsafeArray<bool, 1024> source_is_used;

    const ResShaderSourceArray source_arr = mResText.getResShaderSourceArray();
    const ResShaderProgramArray prog_arr = mResText.getResShaderProgramArray();

    for (ResShaderSourceArray::constIterator it = source_arr.begin(), it_end = source_arr.end(); it != it_end; ++it)
    {
        const ResShaderSource source(&(*it));
        source_is_used[it.getIndex()] = false;

        for (ResShaderProgramArray::constIterator it_prog = prog_arr.begin(), it_prog_end = prog_arr.end(); it_prog != it_prog_end; ++it_prog)
        {
            const ResShaderProgram prog(&(*it_prog));
            bool found = false;

            for (s32 type = 0; type < cShaderType_Num; type++)
            {
                if (prog.ref().mSourceIndex[type] == it.getIndex())
                {
                    source_is_used[it.getIndex()] = true;
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    for (ResShaderSourceArray::constIterator it = source_arr.begin(), it_end = source_arr.end(); it != it_end; ++it)
    {
        const ResShaderSource source(&(*it));
        mSourceVec[it.getIndex()].initialize(this, it.getIndex(), source, source_is_used[it.getIndex()]);
    }

    for (const ShaderSource& source : mSourceVec)
    {
        [[maybe_unused]] const auto& itr = mSourceMap.try_emplace(source.mOriginalName, source.mOriginalText);
        RIO_ASSERT(itr.second);
    }

    for (ResShaderProgramArray::constIterator it = prog_arr.begin(), it_end = prog_arr.end(); it != it_end; ++it)
    {
        const ResShaderProgram prog(&(*it));

        if (do_init_programs)
        {
            ShaderProgram& program = mProgram[it.getIndex()];

            createShaderProgram(
                &program,
                prog.getName(),
                prog.getResShaderVariationArray(),
                prog.getResShaderVariationDefaultArray(),
                prog.getResShaderSymbolArray(cShaderSymbolType_Attribute),
                prog.getResShaderSymbolArray(cShaderSymbolType_Uniform),
                prog.getResShaderSymbolArray(cShaderSymbolType_UniformBlock),
                prog.getResShaderSymbolArray(cShaderSymbolType_Sampler)
            );
        }

        mProgramEx[it.getIndex()].initialize(this, it.getIndex(), prog);
    }

    updateCompileInfo();
}

bool ShaderProgramArchive::setUp_(bool unk)
{
    _28 += 1;

    for (Buffer<ShaderProgramEx>::iterator it = mProgramEx.begin(), it_end = mProgramEx.end(); it != it_end; ++it)
        it->updateAnalyze();

    for (Buffer<ShaderProgram>::iterator it = mProgram.begin(), it_end = mProgram.end(); it != it_end; ++it)
    {
        // TODO
        // it->mpSharedData->_10 = _20;

        if ((!unk || _28 <= 1) && it->setUpAllVariation() != 0)
            return false;
    }

    // TODO
    // if (_24)
    //     _24->vf0C(this);

    return true;
}

ShaderProgramArchive::ShaderProgramEx::ShaderProgramEx()
    : mIndex(0)
    , mpArchive(nullptr)
    , mVariationIndex(0)
    , mFlag(0)
{
}

ShaderProgramArchive::ShaderProgramEx::~ShaderProgramEx()
{
    if (_110.isBufferReady())
        _110.freeBuffer();
}

void ShaderProgramArchive::ShaderProgramEx::initialize(ShaderProgramArchive* archive, s32 index, ResShaderProgram res)
{
    mIndex = index;
    mpArchive = archive;

    ShaderProgram& program = archive->mProgram[index];

    for (UnsafeArray<ShaderCompileInfoEx, cShaderType_Num>::iterator it = mCompileInfoEx.begin(), it_end = mCompileInfoEx.end(); it != it_end; ++it)
    {
        s32 source_index = res.ref().mSourceIndex[it.getIndex()];
        ShaderType type = ShaderType(it.getIndex());

        if (source_index != -1)
        {
            it->mSource = &mpArchive->mSourceVec[source_index];

            const ResShaderMacroArray macro_arr = res.getResShaderMacroArray(type);

            it->mCompileInfo.setName(it->mSource->getName());
            it->mCompileInfo.setRawText(&it->mRawTextEx);

            for (ResShaderMacroArray::constIterator macro_it = macro_arr.begin(), macro_it_end = macro_arr.end(); macro_it != macro_it_end; ++macro_it)
            {
                const ResShaderMacro macro(&(*macro_it));
                it->mCompileInfo.pushBackMacro(std::string(macro.getName(), macro.getNameLength()), std::string(macro.getValue(), macro.getValueLength()));
            }

            program.getShader(type)->setCompileInfo(&it->mCompileInfo);
        }
        else
        {
            it->mSource = nullptr;
        }
    }

    if (program.getVariationMacroNum() > 0)
    {
        _110.allocBuffer(program.getVariationMacroNum());
        for (Buffer<u32>::iterator it = _110.begin(), it_end = _110.end(); it != it_end; ++it)
            *it = 0;
    }

  //detail::RootNode::setNodeMeta(this, "Icon = CIRCLE_ORENGE");
}

void ShaderProgramArchive::ShaderProgramEx::updateRawText()
{
    for (UnsafeArray<ShaderCompileInfoEx, cShaderType_Num>::iterator it = mCompileInfoEx.begin(), it_end = mCompileInfoEx.end(); it != it_end; ++it)
    {
        ShaderSource* source = it->mSource;
        if (source && source->mFlag.isOn(1))
        {
            it->mCompileInfo.setSourceText(&source->mRawText);
            mpArchive->getShaderProgram(mIndex).reserveSetUpAllVariation();
        }
    }
}

void ShaderProgramArchive::ShaderProgramEx::updateAnalyze()
{
    if (mFlag.isOn(1))
        mpArchive->mProgram[mIndex].updateVariation(mVariationIndex);
}

ShaderProgramArchive::ShaderSource::ShaderSource()
    : mFlag(1)
    , mpArchive(nullptr)
    , mRes()
{
}

void ShaderProgramArchive::ShaderSource::initialize(ShaderProgramArchive* archive, s32 index, ResShaderSource res, bool is_used)
{
    mpArchive = archive;
    mRes = res;
    mIndex = index;

    mFlag.change(1 << 1, is_used);

    mOriginalName.assign(mRes.getName(), mRes.getNameLength());
    mOriginalText.assign(mRes.getText(), mRes.getTextLength());

  //detail::RootNode::setNodeMeta(this, "Icon = NOTE");
}

void ShaderProgramArchive::ShaderSource::expand()
{
    if (mFlag.isOff(1 << 1))
        return;

    mRawText = mOriginalText;

    detail::ShaderTextUtil::createRawText(
        &mRawText,
        mpArchive->mSourceMap
    );
}

}
