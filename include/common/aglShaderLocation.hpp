#pragma once

// TODO: Move to the proper header
#define SEAD_MACRO_UTIL_ROUNDUP(x, y) (((x) + ((y) - 1)) & ~((y) - 1))

#if RIO_IS_CAFE
#include <cafe/gx2/gx2Shaders.h>
#elif RIO_IS_DESKTOP
#include <misc/gl/rio_GL.h>
void GX2SetVertexUniformReg(u32 offset, u32 count, const void* values);
void GX2SetPixelUniformReg(u32 offset, u32 count, const void* values);
#endif

namespace agl {

namespace {

inline void
setUniform(const UniformLocation& location, u32 size, const void* buffer)
{
    if (location.getVertexLocation() != -1)
        GX2SetVertexUniformReg(location.getVertexLocation(), SEAD_MACRO_UTIL_ROUNDUP(size, sizeof(rio::BaseVec4u)) / sizeof(u32), buffer);

    if (location.getFragmentLocation() != -1)
        GX2SetPixelUniformReg(location.getFragmentLocation(), SEAD_MACRO_UTIL_ROUNDUP(size, sizeof(rio::BaseVec4u)) / sizeof(u32), buffer);
}

}

inline void
UniformLocation::setBool(bool value) const
{
    setInt(value);
}

inline void
UniformLocation::setInt(s32 value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(s32), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1i(location, value));

    }
#endif
}

inline void
UniformLocation::setUInt(u32 value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(u32), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1ui(location, value));

    }
#endif
}

inline void
UniformLocation::setFloat(f32 value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(f32), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform1f(location, value));

    }
#endif
}

inline void
UniformLocation::setIVec2(const rio::BaseVec2i& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec2i), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2iv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setUVec2(const rio::BaseVec2u& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec2u), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2uiv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setVec2(const rio::BaseVec2f& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec2f), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform2fv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setIVec3(const rio::BaseVec3i& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec3i), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3iv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setUVec3(const rio::BaseVec3u& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec3u), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3uiv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setVec3(const rio::BaseVec3f& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec3f), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform3fv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setIVec4(const rio::BaseVec4i& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec4i), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4iv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setUVec4(const rio::BaseVec4u& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec4u), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4uiv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setVec4(const rio::BaseVec4f& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseVec4f), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4fv(location, 1, &value.x));

    }
#endif
}

inline void
UniformLocation::setVec4(const rio::Color4f& value) const
{
    setVec4(value.v);
}

inline void
UniformLocation::setVec4Array(const rio::BaseMtx34f& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseMtx34f), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4fv(location, 3, &value.v[0].x));

    }
#endif
}

inline void
UniformLocation::setVec4Array(const rio::BaseMtx44f& value) const
{
    RIO_ASSERT(mGS == -1);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseMtx44f), &value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniform4fv(location, 4, &value.v[0].x));

    }
#endif
}

inline void
UniformLocation::setMtx43(const f32* value) const
{
    RIO_ASSERT(mGS == -1);

    RIO_ASSERT(value);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseMtx34f), value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix4x3fv(location, 1, GL_FALSE, value));

    }
#endif
}

inline void
UniformLocation::setMtx44(const f32* value) const
{
    RIO_ASSERT(mGS == -1);

    RIO_ASSERT(value);

#if RIO_IS_DESKTOP
    if (mBinary) {
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE || RIO_IS_DESKTOP

    setUniform(*this, sizeof(rio::BaseMtx44f), value);

#endif // RIO_IS_CAFE || RIO_IS_DESKTOP

#if RIO_IS_DESKTOP
    } else {

    s32 location;

    if (mVS != -1)
    {
        if (mFS != -1)
            RIO_ASSERT(mVS == mFS);

        location = mVS;
    }
    else
    {
        if (mFS == -1)
            return;

        location = mFS;
    }

    RIO_GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, value));

    }
#endif
}

}
