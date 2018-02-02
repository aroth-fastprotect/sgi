#include <osgEarth/Version>

#include <osgEarth/Map>
#include <osgEarth/MapNodeOptions>
#include <osgEarth/Extension>

#define MAPNODE_ACCESS_HACK
#include "osgearth_accessor.h"
#include <osgEarthQt/TerrainProfileWidget>

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

bool MapNodeAccess::hasMapInspector() const
{
    const auto & extensions = getExtensions();
    for (const auto & ext : extensions)
    {
        if (ext->getName() == "mapinspector")
            return true;
    }
    return false;
}

void MapNodeAccess::toggleMapInspector()
{
    const auto & extensions = getExtensions();
    for (const auto & ext : extensions)
    {
        if (ext->getName() == "mapinspector")
        {
            removeExtension(ext);
            return;
        }
    }

    osgEarth::ConfigOptions options;
    osgEarth::Extension * extension = osgEarth::Extension::create("mapinspector", options);
    if (extension)
    {
        extension->setName("mapinspector");
        addExtension(extension);
    }
}


bool MapNodeAccess::isTerrainProfileActive() const
{
    bool ret = false;
    const osg::Camera * camera = osgEarth::findRelativeNodeOfType<osg::Camera>(const_cast<MapNodeAccess*>(this));
    if (camera)
    {
        const osgViewer::View * view = dynamic_cast<const osgViewer::View *>(camera->getView());
        if (view)
        {
            for (const osg::ref_ptr<osgGA::EventHandler> & handler : view->getEventHandlers())
            {
                const osgEarth::QtGui::TerrainProfileMouseHandler * mousehandler = dynamic_cast<const osgEarth::QtGui::TerrainProfileMouseHandler*>(handler.get());
                if (mousehandler)
                {
                    ret = mousehandler->_profileWidget->isVisible();
                    break;
                }
            }
        }
    }
    return ret;
}

void MapNodeAccess::toggleTerrainProfile(QWidget * parent)
{
    bool active = false;
    osg::Camera * camera = osgEarth::findRelativeNodeOfType<osg::Camera>(this);
    if (camera)
    {
        osgViewer::View * view = dynamic_cast<osgViewer::View *>(camera->getView());
        if (view)
        {
            for (osg::ref_ptr<osgGA::EventHandler> & handler : view->getEventHandlers())
            {
                osgEarth::QtGui::TerrainProfileMouseHandler * mousehandler = dynamic_cast<osgEarth::QtGui::TerrainProfileMouseHandler*>(handler.get());
                if (mousehandler)
                {
                    if (mousehandler->_profileWidget)
                    {
                        if (mousehandler->_profileWidget->isVisible())
                        {
                            mousehandler->_profileWidget->close();
                            delete mousehandler->_profileWidget;
                            view->removeEventHandler(mousehandler);
                        }
                        else
                        {
                            mousehandler->_profileWidget->show();
                        }
                    }
                    active = true;
                    break;
                }
            }
            if (!active)
            {
                osgEarth::QtGui::TerrainProfileWidget * profileWidget = new osgEarth::QtGui::TerrainProfileWidget(camera, this, parent);
                profileWidget->setActiveView(view);
                profileWidget->setWindowTitle(QObject::tr("Terrain profile"));
                profileWidget->show();
            }
        }
    }

}

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
bool MapNodeAccess::getCullDataCameras(osg::NodeList & cameras)
{
    osgEarth::Threading::ScopedReadLock lock(_cullDataMutex);
    for (auto it = _cullData.begin(); it != _cullData.end(); ++it)
        cameras.push_back(it->first);
    return !_cullData.empty();
}

const osgEarth::MapNodeCullData * MapNodeAccess::getCullDataForCamera(osg::Camera* camera)
{
    osgEarth::Threading::ScopedReadLock lock(_cullDataMutex);
    CullDataMap::const_iterator i = _cullData.find(camera);
    if (i != _cullData.end())
        return i->second.get();
    else
        return NULL;
}
#endif

} // namespace osgearth_plugin
} // namespace sgi
