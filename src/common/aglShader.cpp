#include <common/aglShader.h>
#include <common/aglShaderCompileInfo.h>
#include <detail/aglFileIOMgr.h>
#include <detail/aglPrivateResource.h>

#if RIO_IS_DESKTOP
#include <filedevice/rio_FileDeviceMgr.h>
#endif // RIO_IS_DESKTOP

namespace agl {

Shader::Shader()
    : mBinary(nullptr)
    , mCompileInfo(nullptr)
{
}

u32 Shader::setUp(bool compile_source, bool) const
{
    u32 ret = 2;

    if (mCompileInfo && compile_source)
    {
        std::string* temp_text = &detail::PrivateResource::sShaderText;

#if RIO_IS_CAFE || RIO_IS_DESKTOP
        mCompileInfo->calcCompileSource(getShaderType(), temp_text, ShaderCompileInfo::cTarget_GX2, true);
#else
        mCompileInfo->calcCompileSource(getShaderType(), temp_text, ShaderCompileInfo::cTarget_GL, true);
#endif

#if RIO_IS_CAFE
        /*
        static const char* sSavePath[cShaderType_Num] = {
            "%AGL_ROOT%/tools/temporary/temp_vs.sh",
            "%AGL_ROOT%/tools/temporary/temp_fs.sh",
            "%AGL_ROOT%/tools/temporary/temp_gs.sh"
        };

        detail::FileIOMgr::DialogArg arg;
        arg.mPath = sSavePath[getShaderType()];

        detail::FileIOMgr::instance()->save(temp_text->c_str(), temp_text->length(), arg);
        */
#endif

        ret = 0;
    }

    if (mBinary)
        ret = 0;

    return ret;
}

void Shader::setBinary(const void* binary)
{
    mBinary = binary;
}

}
