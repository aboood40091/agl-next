#include <container/Buffer.h>
#include <math/rio_Math.h>
#include <utility/aglDynamicTextureAllocator.h>

#if RIO_IS_DESKTOP
#include <common/aglTextureFormatInfo.h>

#include <gpu/win/rio_Texture2DUtilWin.h>
#endif // RIO_IS_DESKTOP

namespace agl { namespace utl {

TextureData* DynamicTextureAllocator::alloc(
    const char* name,
    TextureFormat format,
    u32 width, u32 height, u32 mip_level_num
)
{
    TextureDataEx* p_tex = new TextureDataEx();
    RIO_ASSERT(p_tex != nullptr);
    p_tex->initialize(format, width, height, mip_level_num);

#if RIO_IS_CAFE

    p_tex->setImagePtr(rio::MemUtil::alloc(p_tex->getImageByteSize(), p_tex->getAlignment()));

    if (p_tex->getMipLevelNum() > 1)
        p_tex->setMipPtr(rio::MemUtil::alloc(p_tex->getMipByteSize(), p_tex->getAlignment()));
    else
        p_tex->setMipPtr(nullptr);

#elif RIO_IS_DESKTOP

    const auto& handle = std::make_shared<TextureHandle>();
    handle->bind();

    rio::Texture2DUtil::setNumMipsCurrent(p_tex->getMipLevelNum());

    const rio::NativeTextureFormat& native_format = p_tex->getNativeTextureFormat();

    rio::Texture2DUtil::uploadTextureCurrent(
        rio::TextureFormat(TextureFormatInfo::convFormatAGLToGX2(p_tex->getTextureFormat())),
        native_format,
        p_tex->getWidth(),
        p_tex->getHeight(),
        p_tex->getMipLevelNum(),
        p_tex->getImageByteSize(),
        nullptr,
        p_tex->getMipByteSize(),
        nullptr,
        p_tex->getSurface().mipLevelOffset
    );

    p_tex->setHandle(handle);

#endif

    p_tex->mName = name;

    p_tex->invalidateGPUCache();

    return p_tex;
}

void DynamicTextureAllocator::free(const TextureData* ptr)
{
    RIO_ASSERT(ptr != nullptr);
    TextureDataEx* p_tex = static_cast<TextureDataEx*>(const_cast<TextureData*>(ptr));

#if RIO_IS_CAFE

    void* image_ptr = p_tex->getImagePtr();
    void* mip_ptr = p_tex->getMipPtr();

    RIO_ASSERT(image_ptr != nullptr);

    rio::MemUtil::free(image_ptr);
    if (mip_ptr)
        rio::MemUtil::free(mip_ptr);

    p_tex->setImagePtr(nullptr);
    p_tex->setMipPtr(nullptr);

#elif RIO_IS_DESKTOP

    p_tex->setHandle(nullptr);

#endif

    delete p_tex;
}

} }
