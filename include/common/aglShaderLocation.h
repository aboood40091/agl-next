#pragma once

#include <gfx/rio_Color.h>
#include <math/rio_MathTypes.h>
#include <misc/Namable.h>

namespace agl {

class ShaderLocation
{
public:
    ShaderLocation()
        : mVS(-1)
        , mFS(-1)
        , mGS(-1)
#if RIO_IS_DESKTOP
        , mBinary(false)
#endif // RIO_IS_DESKTOP
    {
    }

    bool isValid() const
    {
        return getVertexLocation()   != -1 ||
               getFragmentLocation() != -1 ||
               getGeometryLocation() != -1;
    }

    s32 getVertexLocation() const { return mVS; }
    s32 getFragmentLocation() const { return mFS; }
    s32 getGeometryLocation() const { return mGS; }

protected:
    s16 mVS;
    s16 mFS;
    s16 mGS;
#if RIO_IS_DESKTOP
    bool mBinary;
#endif // RIO_IS_DESKTOP
};
static_assert(
    sizeof(ShaderLocation) ==
#if RIO_IS_DESKTOP
        8,
#else
        6,
#endif
    "agl::ShaderLocation size mismatch"
);

class ShaderProgram;

class UniformLocation : public INamable, public ShaderLocation
{
public:
    UniformLocation()
        : INamable("Undefined")
    {
    }

    explicit UniformLocation(const char* name)
        : INamable(name)
    {
    }

    UniformLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    UniformLocation& operator=(const UniformLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);

public:
    void setBool(bool value) const;

    void setInt(s32 value) const;
    void setUInt(u32 value) const;
    void setFloat(f32 value) const;

    void setIVec2(const rio::BaseVec2i& value) const;
    void setUVec2(const rio::BaseVec2u& value) const;
    void setVec2(const rio::BaseVec2f& value) const;

    void setIVec3(const rio::BaseVec3i& value) const;
    void setUVec3(const rio::BaseVec3u& value) const;
    void setVec3(const rio::BaseVec3f& value) const;

    void setIVec4(const rio::BaseVec4i& value) const;
    void setUVec4(const rio::BaseVec4u& value) const;
    void setVec4(const rio::BaseVec4f& value) const;

    void setVec4(const rio::Color4f& value) const;

    void setVec4Array(const rio::BaseMtx34f& value) const;  // vec4[3]
    void setVec4Array(const rio::BaseMtx44f& value) const;  // vec4[4]

    void setMtx43(const f32* value) const;
    void setMtx44(const f32* value) const;
};
static_assert(sizeof(UniformLocation) == 0x10, "agl::UniformLocation size mismatch");

class UniformBlockLocation : public INamable, public ShaderLocation
{
public:
    UniformBlockLocation()
        : INamable("Undefined")
    {
    }

    explicit UniformBlockLocation(const char* name)
        : INamable(name)
    {
    }

    UniformBlockLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    UniformBlockLocation& operator=(const UniformBlockLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);
};
static_assert(sizeof(UniformBlockLocation) == 0x10, "agl::UniformBlockLocation size mismatch");

class SamplerLocation : public INamable, public ShaderLocation
{
public:
    SamplerLocation()
        : INamable("Undefined")
    {
    }

    explicit SamplerLocation(const char* name)
        : INamable(name)
    {
    }

    SamplerLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
    {
        search(program);
    }

    SamplerLocation& operator=(const SamplerLocation& rhs)
    {
        setName(rhs.getName());
        ShaderLocation::operator=(static_cast<const ShaderLocation&>(rhs));
        return *this;
    }

    void search(const ShaderProgram& program);
};
static_assert(sizeof(SamplerLocation) == 0x10, "agl::SamplerLocation size mismatch");

class AttributeLocation : public INamable
{
public:
    AttributeLocation()
        : INamable("Undefined")
        , mVS() // Nintendo mistakingly didn't set this to -1
#if RIO_IS_DESKTOP
        , mBinary(false)
#endif // RIO_IS_DESKTOP
    {
    }

    explicit AttributeLocation(const char* name)
        : INamable(name)
        , mVS() // I don't actually know if this is set here
#if RIO_IS_DESKTOP
        , mBinary(false)
#endif // RIO_IS_DESKTOP
    {
    }

    AttributeLocation(const char* name, const ShaderProgram& program)
        : INamable(name)
      //, mVS() // Set by search()
#if RIO_IS_DESKTOP
      //, mBinary(false) // Set by search()
#endif // RIO_IS_DESKTOP
    {
        search(program);
    }

    bool isValid() const
    {
        return getVertexLocation() != -1;
    }

    s32 getVertexLocation() const { return mVS; }

    void search(const ShaderProgram& program);

private:
    s16 mVS;
#if RIO_IS_DESKTOP
    bool mBinary;
#endif // RIO_IS_DESKTOP
};
static_assert(sizeof(AttributeLocation) == 0xC, "agl::AttributeLocation size mismatch");

}

#ifdef __cplusplus

#include <common/aglShaderLocation.hpp>

#endif // __cplusplus
