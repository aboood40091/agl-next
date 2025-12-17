#pragma once

#include <misc/rio_Types.h>

class INamable
{
public:
    INamable()
        : mINamableName(nullptr)
    {
    }

    INamable(const char* name)
        : mINamableName(name)
    {
    }

    void setName(const char* name)
    {
        mINamableName = name;
    }

    const char* getName() const
    {
        return mINamableName;
    }

private:
    const char* mINamableName;
};
