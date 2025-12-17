#pragma once

#include <container/rio_LinkList.h>

namespace agl { namespace utl {

class IParameterObj;
class ResParameter;

class ParameterBase
{
public:
    enum ParameterType
    {
        cType_bool      = 0,    // bool
        cType_f32       = 1,    // f32
        cType_int       = 2,    // s32
        cType_vec2      = 3,    // rio::Vector2f
        cType_vec3      = 4,    // rio::Vector3f
        cType_vec4      = 5,    // rio::Vector4f
        cType_color     = 6,    // rio::Color4f
        cType_string32  = 7,    // FixedSafeString<32>
        cType_string64  = 8,    // FixedSafeString<64>          (Unused in NSMBU)
        cType_curve1    = 9,    // agl::utl::ParameterCurve1
        cType_curve2    = 10,   // agl::utl::ParameterCurve2    (Unused in NSMBU)
        cType_curve3    = 11,   // agl::utl::ParameterCurve3    (Unused in NSMBU)
        cType_curve4    = 12    // agl::utl::ParameterCurve4    (Unused in NSMBU)
    };

public:
    ParameterBase();
    virtual ~ParameterBase() { }

    void applyResource(ResParameter res);
    void applyResource(ResParameter res, f32 t);

    virtual bool copy(const ParameterBase& parameter);
    virtual void copyUnsafe(const ParameterBase& parameter);
    virtual bool copyLerp(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t);

private:
    template <typename T>
    void copyLerp_(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t);

public:
    virtual ParameterType getParameterType() const = 0;
    virtual const void* ptr() const = 0;
    virtual void* ptr() = 0;
    virtual const void* typePtr() const = 0;
    virtual void* typePtr() = 0;
    virtual size_t size() const = 0;
    virtual void clone(IParameterObj*) const = 0;

protected:
    virtual void postApplyResource_(const void*, size_t) { }

public:
    void initializeListNode(const char* name, const char* label, const char* meta, IParameterObj* p_obj);

    u32 getNameHash() const { return mHash; }
    static u32 calcHash(const char* s);

protected:
    u32 mHash;
    rio::LinkListNode mListNode;

    friend class IParameterObj;
};
// static_assert(sizeof(ParameterBase) == 0x10, "agl::utl::ParameterBase size mismatch");

template <typename T>
class Parameter : public ParameterBase
{
public:
    Parameter(const T& value, const char* name, const char* label, IParameterObj* p_obj)
        : ParameterBase()
    {
        initializeListNode(name, label, "", p_obj);
        mValue = value;
    }

    ParameterType getParameterType() const;
    const void* ptr() const { return &mValue; }
    void* ptr() { return &mValue; }
    const void* typePtr() const { return &mValue; }
    void* typePtr() { return &mValue; }
    virtual size_t size() const { return sizeof(mValue); }
    void clone(IParameterObj*) const { /* Deleted in NSMBU */ }

    void setValue(const T& value) { mValue = value; }
    const T& getValue() const { return mValue; }

    T& operator*() { return mValue; }
    const T& operator*() const { return mValue; }

    T* operator->() { return &mValue; }
    const T* operator->() const { return &mValue; }

private:
    T mValue;
};

} }

#ifdef __cplusplus

#include <utility/aglParameter.hpp>

#endif // __cplusplus
