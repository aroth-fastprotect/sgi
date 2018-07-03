#include "item_osgearth_unittest.h"

#include <QDebug>

#if defined(__linux__)
#include <dlfcn.h>
#include <osgDB/Version>
#endif

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>
#include <sgi/WritePrettyHTML>
#include <sgi/SceneGraphDialog>
#include <sgi/ContextMenu>
#include <sgi/ImagePreviewDialog>
#include <sgi/Shutdown>
#include <sgi/AutoLoadOsg>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>

#include <osgEarth/Map>
#include <osgEarth/MapNode>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}

#ifdef __linux__
namespace {
    std::string getOSGDBModuleFilename()
    {
        std::string ret;
        Dl_info info;
        const char* (* addr) () = osgDBGetVersion;
        if(dladdr(reinterpret_cast<const void*>(addr), &info) != 0)
        {
            ret = info.dli_fname;
        }
        return ret;
    }
    std::string getOSGDBModuleDirectory()
    {
        std::string modulefilename = getOSGDBModuleFilename();
        return std::string(modulefilename, 0, modulefilename.find_last_of('/'));
    }
}
#endif

void item_osgearth_unittest::initTestCase()
{
#ifdef __linux__
    osgDB::Registry * registry = osgDB::Registry::instance();
    registry->initLibraryFilePathList();
    osgDB::FilePathList libdirs = registry->getLibraryFilePathList();
    libdirs.push_back(getOSGDBModuleDirectory());
    registry->setLibraryFilePathList(libdirs);
#endif
}

void item_osgearth_unittest::cleanupTestCase()
{
}

void item_osgearth_unittest::checkRefItems()
{
    osgEarth::Map * m = new osgEarth::Map();

    SGIItemBasePtr item;
    SGIHostItemOsg hostItem(m);
    sgi::generateItem<sgi::autoload::Osg>(item, &hostItem);
    QVERIFY(item.valid());

    std::stringstream ss;
    sgi::writePrettyHTML<sgi::autoload::Osg>(ss, item.get());
    QVERIFY(!ss.str().empty());

}

void item_osgearth_unittest::checkConfigItems()
{
    osgEarth::Map * m = new osgEarth::Map();
    const auto & o = m->getInitialMapOptions();

    osgEarth::Config cfg = o.getConfig();
    SGIHostItemOsgEarthConfig hostItem(cfg);
    QVERIFY(hostItem.hasObject());
    QCOMPARE(hostItem.object(), &cfg);

    SGIItemBasePtr item;
    sgi::generateItem<sgi::autoload::Osg>(item, &hostItem);
    QVERIFY(item.valid());

    std::stringstream ss;
    sgi::writePrettyHTML<sgi::autoload::Osg>(ss, item.get());
    QVERIFY(!ss.str().empty());
}

void item_osgearth_unittest::checkConfigOptionsItems()
{
    osgEarth::Map * m = new osgEarth::Map();
    const auto & o = m->getInitialMapOptions();

    SGIHostItemOsgEarthConfigOptions hostItem(o);
    QVERIFY(hostItem.hasObject());
    QCOMPARE(hostItem.object(), &o);

    SGIItemBasePtr item;
    sgi::generateItem<sgi::autoload::Osg>(item, &hostItem);
    QVERIFY(item.valid());

    std::stringstream ss;
    sgi::writePrettyHTML<sgi::autoload::Osg>(ss, item.get());
    QVERIFY(!ss.str().empty());

}

QTEST_MAIN(item_osgearth_unittest)
