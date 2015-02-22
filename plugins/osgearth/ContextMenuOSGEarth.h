#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemOsg.h>

#include <osgEarth/Viewpoint>

namespace osgEarth {
    class Registry;
    class Map;
    class MapNode;
    class Layer;
    class TerrainLayer;
    class ImageLayer;
    class ElevationLayer;
    class ModelLayer;
    class MaskLayer;
    class TileSource;
    class TileBlacklist;
    class VirtualProgram;
    class ElevationQuery;

    namespace Util {
        class SkyNode;
        class AutoClipPlaneCullCallback;
        namespace Controls {
            class Control;
            class Container;
            class ControlCanvas;
        }
    }
}

namespace sgi {
namespace osgearth_plugin {

typedef ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;

class NamedViewpoint : public osgEarth::Viewpoint
{
public:
    NamedViewpoint()
        : osgEarth::Viewpoint()
        , _group()
        {
        }
    NamedViewpoint(const osgEarth::Viewpoint & vp)
        : osgEarth::Viewpoint(vp)
        , _group()
        {
        }
    NamedViewpoint(const NamedViewpoint & vp)
        : osgEarth::Viewpoint(vp)
        , _group(vp._group)
        {
        }

    void                            setGroup(const std::string& group) { _group = group; }
    const std::string&              getGroup() const { return _group; }

    NamedViewpoint & operator=(const osgEarth::Viewpoint & vp)
    {
        osgEarth::Viewpoint::operator=(vp);
        _group.clear();
        return *this;
    }

    NamedViewpoint & operator=(const NamedViewpoint & vp)
    {
        osgEarth::Viewpoint::operator=(vp);
        _group = vp._group;
        return *this;
    }

private:
    std::string _group;
};

typedef ReferencedDataT<NamedViewpoint> ReferencedNamedViewpoint;

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Node)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Image)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Registry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Map)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::MapNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Layer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::TerrainLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::ImageLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::ElevationLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::ModelLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::MaskLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Util::SkyNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Util::AutoClipPlaneCullCallback)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Util::Controls::Control)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Util::Controls::Container)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::Util::Controls::ControlCanvas)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::TileSource)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::TileBlacklist)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgEarth::VirtualProgram)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(ElevationQueryReferenced)

} // namespace osgearth_plugin
} // namespace sgi
