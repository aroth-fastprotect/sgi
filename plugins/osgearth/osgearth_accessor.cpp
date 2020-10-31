#include "stdafx.h"
#include <osgEarth/Version>
#include <sstream>

#ifdef _MSC_VER
#define _ALLOW_KEYWORD_MACROS
#endif
#define MAPNODE_ACCESS_HACK
#define ELEVATIONQUERY_ACCESS_HACK
#define VIRTUALPROGRAMM_ACCESS_HACK
#include "osgearth_accessor.h"

#include <osgEarth/MapNodeOptions>
#include <osgEarth/Extension>

#define PREALLOCATE_APPLY_VARS

#define USE_PROGRAM_REPO

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

#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
const osgEarth::MapInfo& ElevationQueryAccess::getMapInfo() const 
{ 
    return ((DataAccess*)this)->_mapf.getMapInfo();
}

const osgEarth::Profile* ElevationQueryAccess::getProfile() const 
{ 
    return ((DataAccess*)this)->_mapf.getProfile();
}
#else
const osgEarth::Map* ElevationQueryAccess::getMap() const
{
    return _map.get();
}
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
void ElevationPoolAccess::getTiles(Tiles & tiles)
{
    osgEarth::Threading::ScopedMutexLock lock(_tilesMutex);
    reinterpret_cast<ElevationPool::Tiles&>(tiles) = _tiles;
}

void ElevationPoolAccess::getMRU(MRU & mru)
{
    osgEarth::Threading::ScopedMutexLock lock(_tilesMutex);
    reinterpret_cast<ElevationPool::MRU&>(mru) = _mru;
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

void PolyShaderAccessor::resetShaders()
{
    _nominalShader = nullptr;
    _geomShader = nullptr;
    _tessevalShader = nullptr;
    _dirty = true;
}


#if 0
void VirtualProgramAccessor::getProgramCache(ProgramMap & programCache)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_programCacheMutex);
    programCache = _programCache;
}
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
void VirtualProgramAccessor::getGLSLExtensions(ExtensionsSet & extensions)
{
    _dataModelMutex.lock();
    extensions = _globalExtensions;
    _dataModelMutex.unlock();
}
#endif

class ProgramRepoAccessor : public osgEarth::ProgramRepo
{
public:
    void remove(const osgEarth::ProgramKey & key)
    {
        osgEarth::ScopedMutexLock lock(_m);
        ProgramMap::iterator i = _db.find(key);
        if (i != _db.end())
        {
            _db.erase(i);
        }
    }
};

void VirtualProgramAccessor::dirty(unsigned contextID)
{
#ifdef USE_PROGRAM_REPO
    ProgramRepoAccessor & programRepo = static_cast<ProgramRepoAccessor &>(osgEarth::Registry::programRepo());

    // Access the resuable shader map for this context. Bypasses reallocation overhead.
    ApplyVars& local = _apply[contextID];
    programRepo.remove(local.programKey);
#endif
}

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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
const osgEarth::Status& TileSourceAccessor::getStatus() const {
	return _status;
}
#endif




} // namespace osgearth_plugin
} // namespace sgi
