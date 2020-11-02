#include <QtCore/QCoreApplication>
#include <iostream>

#include <sgi_version.h>

#if SGI_PLUGIN_OSG
#include <osg/Version>
#endif

#if SGI_PLUGIN_OSGEARTH
#include <osgEarth/Version>
#endif

#define OUT_VAR(v) \
    std::cout << #v ": " << v << std::endl

#define OUT_VAR_BOOL(v) \
    std::cout << #v ": " << (v?"true":"false") << std::endl

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    std::cout << "QT_VERSION: " << qVersion() << std::endl;
    OUT_VAR(SGI_QT_PLUGIN_DIR);
    OUT_VAR(SGI_VERSION_MAJOR);
    OUT_VAR(SGI_VERSION_MINOR);
    OUT_VAR(SGI_VERSION_PATCH);
    OUT_VAR(SGI_VERSION_BUILD);
    OUT_VAR(SGI_SOVERSION);
    OUT_VAR(SGI_PLUGIN_PREFIX);
    OUT_VAR(SGI_COPYRIGHT_TEXT);
    OUT_VAR(SGI_VERSION);

    OUT_VAR_BOOL(SGI_BUILD_GLIMAGEVIEW);
    OUT_VAR_BOOL(SGI_BUILD_GLIMAGEVIEW);
    OUT_VAR_BOOL(SGI_GENERATE_QT_CONF);
    OUT_VAR_BOOL(SGI_PLUGIN_LOG4CPLUS);
    OUT_VAR_BOOL(SGI_PLUGIN_OSG);
    OUT_VAR_BOOL(SGI_PLUGIN_OSGEARTH);
    OUT_VAR_BOOL(SGI_PLUGIN_QT);
    OUT_VAR_BOOL(SGI_PLUGIN_QTQUICK);
    OUT_VAR_BOOL(SGI_PLUGIN_QT_WITH_QTOPENGL);

#if SGI_PLUGIN_OSG
    std::cout << "OSG version: " << osgGetVersion() << std::endl;
    std::cout << "OSG SO version: " << osgGetSOVersion() << std::endl;
    std::cout << "OSG library name: " << osgGetLibraryName() << std::endl;
#endif

#if SGI_PLUGIN_OSGEARTH
    std::cout << "osgEarth version: " << osgEarthGetVersion() << std::endl;
    std::cout << "osgEarth SO version: " << osgEarthGetSOVersion() << std::endl;
    std::cout << "osgEarth library name: " << osgEarthGetLibraryName() << std::endl;
#endif


    return 0;
}

