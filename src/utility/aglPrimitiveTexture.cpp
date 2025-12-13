#include <misc/rio_MemUtil.h>
#include <utility/aglPrimitiveTexture.h>

#include <bit>

namespace agl { namespace utl {

PrimitiveTexture* PrimitiveTexture::sInstance = nullptr;

bool PrimitiveTexture::createSingleton()
{
    if (sInstance)
        return false;

    sInstance = new PrimitiveTexture();
    return true;
}

void PrimitiveTexture::destroySingleton()
{
    if (!sInstance)
        return;

    delete sInstance;
    sInstance = nullptr;
}

PrimitiveTexture::PrimitiveTexture()
{
}

PrimitiveTexture::~PrimitiveTexture()
{
#if RIO_IS_CAFE
    for (TextureSampler& sampler : mTextureSampler)
    {
        void* image_ptr = sampler.getTextureData().getImagePtr();
        if (image_ptr)
            rio::MemUtil::free(image_ptr);
    }
#endif // RIO_IS_CAFE
}

void PrimitiveTexture::initialize()
{
    for (SafeArray<TextureSampler, cSampler_Num>::iterator itr_sampler = mTextureSampler.begin(), itr_sampler_end = mTextureSampler.end(); itr_sampler != itr_sampler_end; ++itr_sampler)
    {
        TextureSampler& sampler = *itr_sampler;
        TextureSamplerType type = TextureSamplerType(itr_sampler.getIndex());

        TextureFormat format = cTextureFormat_R8_G8_B8_A8_uNorm;
        u32 pixel = 0x00000000;

        switch (type)
        {
        case cSampler_White2D:
            pixel = 0xffffffff;
            break;
        case cSampler_Gray2D:
            pixel = 0x808080ff;
            break;
        case cSampler_Black2D:
      //case cSampler_Black2DArray:
            pixel = 0x000000ff;
            break;
        /*
        case cSampler_BlackCube:
        case cSampler_BlackCubeArray:
            pixel = 0x00000000;
            break;
        */
        case cSampler_Depth32_0:
            format = cTextureFormat_Depth_32;
          //pixel = 0x00000000;
            break;
        case cSampler_Depth32_1:
        case cSampler_DepthShadow:
      //case cSampler_DepthShadowArray:
            format = cTextureFormat_Depth_32;
            pixel = std::bit_cast<u32, f32>(1.0f);
            break;
        default:
            break;
        }

        /*
        if (type == cSampler_MipLevel)
            initializeMipLevelTexture_();

        else
        */
        {
            TextureData texture_data;

            switch (type)
            {
          //case cSampler_White2D:
          //case cSampler_Gray2D:
          //case cSampler_Black2D:
          //case cSampler_Depth32_0:
          //case cSampler_Depth32_1:
          //case cSampler_DepthShadow:
            default:
                texture_data.initialize(format, 4, 4, 1);
                break;
            /*
            case cSampler_Black2DArray:
            case cSampler_DepthShadowArray:
                texture_data.initializeArray(format, 4, 4, 1, 1);
                break;
            case cSampler_BlackCube:
                texture_data.initializeCubeMap(format, 4, 4, 1);
                break;
            case cSampler_BlackCubeArray:
                texture_data.initializeCubeMapArray(format, 4, 4, 1, 1);
                break;
            */
            }

            u32 alignment =
#if RIO_IS_CAFE
                texture_data.getAlignment();
#else
                1;
#endif

            void* image_ptr = rio::MemUtil::alloc(texture_data.getImageByteSize(), alignment);
            u32 count = texture_data.getImageByteSize() / sizeof(u32);

            for (u32 i = 0; i < count; i++)
                ((u32*)image_ptr)[i] = pixel;

#if RIO_IS_CAFE
            texture_data.setImagePtr(image_ptr);
#else
#if RIO_IS_DESKTOP
            const auto& handle = std::make_shared<TextureHandle>();
            handle->bind();

            RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
            RIO_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texture_data.getMipLevelNum() - 1));

            RIO_GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

            RIO_GL_CALL(glTexStorage2D(
                GL_TEXTURE_2D,
                texture_data.getMipLevelNum(),
                texture_data.getNativeTextureFormat().internalformat,
                texture_data.getWidth(),
                texture_data.getHeight()
            ));

            texture_data.setHandle(handle);
#endif // RIO_IS_DESKTOP
            rio::MemUtil::free(image_ptr);
#endif

            sampler.applyTextureData(texture_data);

            if (/* type >= cSampler_DepthShadow && type <= cSampler_DepthShadowArray */ type == cSampler_DepthShadow)
            {
                sampler.setDepthCompareEnable(true);
                sampler.setWrap(cTextureWrapType_Clamp, cTextureWrapType_Clamp, cTextureWrapType_Clamp);
                sampler.setDepthCompareFunc(rio::Graphics::COMPARE_FUNC_ALWAYS);
            }
        }
    }
}

} }

