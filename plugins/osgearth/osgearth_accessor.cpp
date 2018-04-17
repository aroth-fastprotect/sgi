#include <osgEarth/Version>
#include <sstream>

#ifdef _MSC_VER
#define _ALLOW_KEYWORD_MACROS
#endif
#define MAPNODE_ACCESS_HACK
#include "osgearth_accessor.h"

#include <osgEarth/MapNodeOptions>
#include <osgEarth/Extension>

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

void MapAccess::getMapCallbacks(osgEarth::MapCallbackList & mapCallbacks) const
{
    mapCallbacks = _mapCallbacks;
}

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
#if OSGEARTH_VERSION_LESS_THAN(2,8,0) && 0
                const osgEarth::QtGui::TerrainProfileMouseHandler * mousehandler = dynamic_cast<const osgEarth::QtGui::TerrainProfileMouseHandler*>(handler.get());
                if (mousehandler)
                {
                    ret = mousehandler->_profileWidget->isVisible();
                    break;
                }
#endif
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
#if OSGEARTH_VERSION_LESS_THAN(2,8,0) && 0
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
#endif
            }
            if (!active)
            {
#if OSGEARTH_VERSION_LESS_THAN(2,8,0) && 0
                osgEarth::QtGui::TerrainProfileWidget * profileWidget = new osgEarth::QtGui::TerrainProfileWidget(camera, this, parent);
                profileWidget->setActiveView(view);
                profileWidget->setWindowTitle(QObject::tr("Terrain profile"));
                profileWidget->show();
#endif
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
void LayerAccessor::getLayerCallbacks(LayerCallbackList & callbacks) const
{
    callbacks = _callbacks;
}
#endif


void RTTPickerAccess::getPickContexts(PickContexts & contexts) const
{
    contexts = _pickContexts;
}

void VirtualProgramAccessor::getProgramCache(ProgramMap & programCache)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_programCacheMutex);
    programCache = _programCache;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
void VirtualProgramAccessor::getGLSLExtensions(ExtensionsSet & extensions)
{
    _dataModelMutex.lock();
    extensions = _globalExtensions;
    _dataModelMutex.unlock();
}
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
osgEarth::CacheSettings * LayerAccessor::getCacheSettings() const 
{
    const osgEarth::TerrainLayer* tl = dynamic_cast<const osgEarth::TerrainLayer*>(static_cast<const osgEarth::Layer*>(this));
    if (tl)
        return tl->getCacheSettings();
    else
        return _cacheSettings.get();
}
#endif


} // namespace osgearth_plugin
} // namespace sgi
