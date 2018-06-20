#pragma once

#include <sgi/plugins/ContextMenuImpl>
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
    class TileKey;
    class VirtualProgram;
    class ElevationQuery;
	class LevelDBDatabase;

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

typedef details::ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef details::ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef details::ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;

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

} // namespace osgearth_plugin
} // namespace sgi
