#pragma once

#include <sgi/plugins/ObjectTreeImpl>

namespace osgEarth {
    class ElevationQuery;
    class TileKey;
} // namespace osgEarth

namespace sgi {
namespace osgearth_plugin {

typedef details::ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef details::ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef details::ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;
class TileSourceInfo;

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
