#pragma once

#include <container/Buffer.h>
#include <filedevice/rio_FileDevice.h>

namespace agl { namespace detail {

class FileIOMgr
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static FileIOMgr* instance() { return sInstance; }

private:
    static FileIOMgr* sInstance;

    FileIOMgr();
    ~FileIOMgr();

    FileIOMgr(const FileIOMgr&);
    FileIOMgr& operator=(const FileIOMgr&);

public:
    struct DialogArg
    {
        DialogArg();

        std::string mDefaultFilter;
        std::string mDefaultFilename;
        std::string mDefaultID;
        std::string mPath;
        std::string* mOutPath;
        s32 mLoadAlignment;
    };
  //static_assert(sizeof(DialogArg) == 0x28, "agl::detail::FileIOMgr::DialogArg size mismatch");

    struct File // No idea what the actual name is
    {
        u8* mpData;
        u32 mSize;
    };
    // static_assert(sizeof(File) == 8, "agl::detail::FileIOMgr::File size mismatch");

public:
    bool save(const void* p_buf, u32 size, const DialogArg& arg);

    s32 load(const DialogArg& arg);
    void close(s32 index);

    s32 getFileNum() const
    {
        return mBuffer.size();
    }

    const File& getFile(s32 index) const
    {
        return mBuffer[index];
    }

private:
    rio::FileDevice* mFileDevice; // CafeHostIOFileDevice*
    Buffer<File> mBuffer;
};
//static_assert(sizeof(FileIOMgr) == 0x5C, "agl::detail::FileIOMgr size mismatch");

} }
