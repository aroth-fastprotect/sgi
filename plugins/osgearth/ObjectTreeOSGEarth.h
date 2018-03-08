#pragma once

#include <sgi/plugins/ObjectTreeImpl>

namespace osgEarth {
    class ElevationQuery;
    class TileKey;
} // namespace osgEarth

namespace sgi {
namespace osgearth_plugin {

typedef ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;
class TileSourceInfo;

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
