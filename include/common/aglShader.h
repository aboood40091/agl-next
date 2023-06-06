#pragma once

#include <common/aglShaderEnum.h>

#if RIO_IS_CAFE
#include <cafe/gx2.h>
#endif // RIO_IS_CAFE

namespace agl {

class ShaderCompileInfo;

class Shader
{
public:
    Shader();
    virtual ~Shader() { }

    virtual ShaderType getShaderType() const = 0;
    virtual ShaderMode getShaderMode() const { return cShaderMode_Invalid; }
    virtual u32 getRingItemSize() const { return 0; }

    u32 setUp(bool compile_source, bool) const;

    void* getBinary() { return const_cast<void*>(mBinary); }
    const void* getBinary() const { return mBinary; }

    void setBinary(const void* binary);

    ShaderCompileInfo* getCompileInfo() const { return mCompileInfo; }

    void setCompileInfo(ShaderCompileInfo* compile_info) const
    {
        mCompileInfo = compile_info;
    }

private:
    const void* mBinary;
    mutable ShaderCompileInfo* mCompileInfo;
};
static_assert(sizeof(Shader) == 0xC, "agl::Shader size mismatch");

class VertexShader : public Shader
{
public:
    VertexShader()
        : Shader()
    {
    }

    virtual ShaderType getShaderType() const
    {
        return cShaderType_Vertex;
    }

#if RIO_IS_CAFE
    GX2VertexShader* getBinary()
    {
        return static_cast<GX2VertexShader*>(Shader::getBinary());
    }

    const GX2VertexShader* getBinary() const
    {
        return static_cast<const GX2VertexShader*>(Shader::getBinary());
    }

    ShaderMode getShaderMode() const
    {
        return ShaderMode(getBinary()->mode);
    }
#endif // RIO_IS_CAFE
};

class FragmentShader : public Shader
{
public:
    FragmentShader()
        : Shader()
    {
    }

    virtual ShaderType getShaderType() const
    {
        return cShaderType_Fragment;
    }

#if RIO_IS_CAFE
    GX2PixelShader* getBinary()
    {
        return static_cast<GX2PixelShader*>(Shader::getBinary());
    }

    const GX2PixelShader* getBinary() const
    {
        return static_cast<const GX2PixelShader*>(Shader::getBinary());
    }

    ShaderMode getShaderMode() const
    {
        return ShaderMode(getBinary()->mode);
    }
#endif // RIO_IS_CAFE
};

class GeometryShader : public Shader
{
public:
    GeometryShader()
        : Shader()
    {
    }

    virtual ShaderType getShaderType() const
    {
        return cShaderType_Geometry;
    }

#if RIO_IS_CAFE
    GX2GeometryShader* getBinary()
    {
        return static_cast<GX2GeometryShader*>(Shader::getBinary());
    }

    const GX2GeometryShader* getBinary() const
    {
        return static_cast<const GX2GeometryShader*>(Shader::getBinary());
    }

    ShaderMode getShaderMode() const
    {
        return ShaderMode(getBinary()->mode);
    }
#endif // RIO_IS_CAFE
};

}
