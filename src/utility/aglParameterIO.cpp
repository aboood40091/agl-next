//#include <gfx/seadGraphics.h>
#include <utility/aglParameterIO.h>

namespace agl { namespace utl {

IParameterIO::IParameterIO(const char* type, u32 version)
    : IParameterList()
    , mpDelegate(nullptr)
    , _d4("")
{
    mType = type;
    mVersion = version;
    setParameterListName_("param_root");
}

bool IParameterIO::save(const char*, u32) const
{
    // Deleted from NSMBU
    // Uses sead::XmlDocument to write this parameter to an XML file
    return false;
}

void IParameterIO::applyResParameterArchive(ResParameterArchive arc)
{
  //sead::Graphics::instance()->lockDrawContext();
    {
        RIO_ASSERT(arc.isValid());

        if (mVersion != arc.ref().mTypeVersion)
            callbackInvalidVersion_(arc);

        applyResParameterList(arc.getResParameterList());
    }
  //sead::Graphics::instance()->unlockDrawContext();
}

void IParameterIO::applyResParameterArchiveLerp(ResParameterArchive arc_a, ResParameterArchive arc_b, f32 t)
{
    // Deleted from NSMBU
    // Implementation seems to be different from newer games
}

} }
