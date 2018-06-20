#pragma once

#include <sgi/plugins/WritePrettyHTMLImpl>

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

WRITE_PRETTY_HTML_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
