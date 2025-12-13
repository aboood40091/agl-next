#include <common/aglUniformBlock.h>

#include <misc/rio_MemUtil.h>

// TODO: Move to the proper headers
#define ROUNDUP(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
#define ROUNDDOWN(x, y) ((x) & ~((y) - 1))

#if RIO_IS_CAFE
#include <cafe.h>
#endif // RIO_IS_CAFE

namespace {

static const u8 sTypeInfo[agl::UniformBlock::cType_Num][3] = {
    // Stride, Alignment, Array Stride
    { 0x01, 0x01, 0x04 },
    { 0x01, 0x01, 0x04 },
    { 0x01, 0x01, 0x04 },
    { 0x02, 0x02, 0x04 },
    { 0x03, 0x04, 0x04 },
    { 0x04, 0x04, 0x04 },
    { 0x08, 0x08, 0x08 },
    { 0x08, 0x08, 0x08 },
    { 0x08, 0x08, 0x08 },
    { 0x0C, 0x0C, 0x0C },
    { 0x0C, 0x0C, 0x0C },
    { 0x0C, 0x0C, 0x0C },
    { 0x10, 0x10, 0x10 },
    { 0x10, 0x10, 0x10 },
    { 0x10, 0x10, 0x10 }
};

}

namespace agl {

UniformBlock::UniformBlock()
    : mpHeader(nullptr)
    , mCurrentBuffer(nullptr)
#if RIO_IS_DESKTOP
    , mpUBO(nullptr)
#endif // RIO_IS_DESKTOP
    , mBlockSize(0)
    , mFlag(0)
{
}

UniformBlock::~UniformBlock()
{
    destroy();
}

void UniformBlock::startDeclare(s32 num)
{
    RIO_ASSERT(0 < num);
    RIO_ASSERT(mpHeader == nullptr);

    mpHeader = new Header;
    mpHeader->mpMember = new Member[num];
    mpHeader->mMemberNum = num;
    mpHeader->mMemberCount = 0;

    mBlockSize = 0;

    mFlag.set(cFlag_OwnHeader);
}

void UniformBlock::declare(Type type, s32 num)
{
    RIO_ASSERT(0 < num);

    Member& member = mpHeader->mpMember[mpHeader->mMemberCount];
    member.mType = type;
    member.mNum = num;

    u8 stride;

    if (member.mNum == 1)
    {
        stride = sTypeInfo[member.mType][0];
        mBlockSize = ROUNDUP(mBlockSize, sTypeInfo[member.mType][1] * sizeof(u32));
    }
    else
    {
        stride = sTypeInfo[member.mType][2];
        mBlockSize = ROUNDUP(mBlockSize, sTypeInfo[member.mType][2] * sizeof(u32));
    }

    member.mOffset = mBlockSize;
    mBlockSize += member.mNum * stride * sizeof(u32);

    mpHeader->mMemberCount++;
}

void UniformBlock::declare(const UniformBlock& block)
{
    RIO_ASSERT(block.mpHeader != nullptr);
    RIO_ASSERT(mpHeader == nullptr);
    RIO_ASSERT(mBlockSize == 0);

    mpHeader = block.mpHeader;
    mBlockSize = block.mBlockSize;

    mFlag.reset(cFlag_OwnHeader);
}

void UniformBlock::create()
{
    RIO_ASSERT(mCurrentBuffer == nullptr);
#if RIO_IS_DESKTOP
    RIO_ASSERT(mpUBO == nullptr);
#endif // RIO_IS_DESKTOP

    if (mFlag.isOn(cFlag_OwnBuffer))
    {
        RIO_LOG("This buffer has original buffer.\n");
        RIO_ASSERT(false);
    }

    mBlockSize = ROUNDUP(mBlockSize, cCPUCacheLineSize);
    mCurrentBuffer = static_cast<u8*>(rio::MemUtil::alloc(mBlockSize, cUniformBlockAlignment));
#if RIO_IS_DESKTOP
    mpUBO = new rio::UniformBlock(mCurrentBuffer, mBlockSize);
#endif // RIO_IS_DESKTOP

    mFlag.set(cFlag_OwnBuffer);
}

void UniformBlock::destroy()
{
    if (mFlag.isOn(cFlag_OwnBuffer))
    {
        if (mCurrentBuffer)
        {
            rio::MemUtil::free(mCurrentBuffer);
            mCurrentBuffer = nullptr;
        }
#if RIO_IS_DESKTOP
        if (mpUBO)
        {
            delete mpUBO;
            mpUBO = nullptr;
        }
#endif // RIO_IS_DESKTOP
        mFlag.reset(cFlag_OwnBuffer);
    }

    mCurrentBuffer = nullptr;
#if RIO_IS_DESKTOP
    mpUBO = nullptr;
#endif // RIO_IS_DESKTOP
    mBlockSize = 0;

    if (mFlag.isOn(cFlag_OwnHeader))
    {
        delete[] mpHeader->mpMember;
        delete mpHeader;
        mpHeader = nullptr;
        mFlag.reset(cFlag_OwnHeader);
    }
}

void UniformBlock::dcbz() const
{
#if RIO_IS_CAFE
    u8* begin_ptr = mCurrentBuffer;
    u8* end_ptr = mCurrentBuffer + ROUNDDOWN(mBlockSize, cCPUCacheLineSize);

    while (begin_ptr < end_ptr)
    {
        asm("dcbz 0, %0" : "+g"(begin_ptr));
        begin_ptr += cCPUCacheLineSize;
    }
#else
    rio::MemUtil::set(mCurrentBuffer, 0, mBlockSize);
#if RIO_IS_DESKTOP
    mpUBO->setData(mCurrentBuffer, mBlockSize);
#endif // RIO_IS_DESKTOP
#endif
}

void UniformBlock::flush(void* p_memory, bool invalidate_gpu) const
{
#if RIO_IS_CAFE
    DCFlushRange(p_memory, mBlockSize);
    if (invalidate_gpu)
        GX2Invalidate(GX2_INVALIDATE_UNIFORM_BLOCK, p_memory, mBlockSize);
#elif RIO_IS_DESKTOP
    RIO_ASSERT(p_memory == mCurrentBuffer);
    mpUBO->setData(mCurrentBuffer, mBlockSize);
#endif
}

bool UniformBlock::setUniform(const void* p_data, const UniformBlockLocation& location, u32 offset, size_t size) const
{
    if (!location.isValid())
        return false;

#if RIO_IS_CAFE
    const u8* ptr = (const u8*)p_data + offset;

    if (location.getVertexLocation() != -1)
        GX2SetVertexUniformBlock(location.getVertexLocation(), size, ptr);

    if (location.getFragmentLocation() != -1)
        GX2SetPixelUniformBlock(location.getFragmentLocation(), size, ptr);

    if (location.getGeometryLocation() != -1)
        GX2SetGeometryUniformBlock(location.getGeometryLocation(), size, ptr);

    return true;
#elif RIO_IS_DESKTOP
    RIO_ASSERT(p_data == mCurrentBuffer && offset == 0);
    RIO_ASSERT(location.getGeometryLocation() == -1);

    u32 index = location.getVertexLocation();
    if (index == u32(-1))
        index = location.getFragmentLocation();

    if (index == u32(-1))
        return false;

    RIO_ASSERT(s32(index) == location.getFragmentLocation());
    mpUBO->setIndex(index);
    mpUBO->bind();

    return true;
#else
    return false;
#endif
}

void UniformBlock::setData_(void* p_memory, s32 index, const void* p_data, s32 array_index, s32 array_num) const
{
    RIO_ASSERT(mpHeader != nullptr && mpHeader->mpMember != nullptr);
    RIO_ASSERT(0 <= index && index < mpHeader->mMemberNum);
    RIO_ASSERT(p_memory != nullptr);
    RIO_ASSERT(p_data != nullptr);

    Member& member = mpHeader->mpMember[index];

    if (member.mType >= 6 && member.mType <= 11)
    {
        RIO_LOG("not implemented yet.\n");
        RIO_ASSERT(false);
    }

    u8 stride_array = sTypeInfo[member.mType][2];
    u8* ptr = (u8*)p_memory + stride_array * array_index * sizeof(u32) + member.mOffset;
    u8 stride = sTypeInfo[member.mType][0];

#if RIO_IS_CAFE
    if ((uintptr_t)ptr % cCPUCacheLineSize == 0)
    {
        s32 aligned_size = ROUNDDOWN(array_num * stride_array * sizeof(u32), cCPUCacheLineSize);
        for (s32 i = 0; i < aligned_size; i += cCPUCacheLineSize)
            asm("dcbz %0, %1" : "+g"(ptr), "+g"(i));
    }
#endif // RIO_IS_CAFE

    const u32* src = (const u32*)p_data;

    for (s32 i = 0; i < array_num; i++)
    {
        u32* const dst = (u32*)ptr;

        for (s32 j = 0; j < stride; j++)
#if RIO_IS_CAFE
            dst[j] = __builtin_bswap32(*src++);
#else
            dst[j] = *src++;
#endif // RIO_IS_CAFE

        ptr += stride_array * sizeof(u32);
    }
}

}
