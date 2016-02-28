#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemOsg.h>

namespace osgEarth {
    class Map;
    class MapNode;
    class Registry;
    class TerrainLayer;
    class ModelLayer;
    class MaskLayer;
    class ElevationQuery;
    class TileKey;

    class Config;
    class ConfigOptions;
	class LevelDBDatabase;
}

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

typedef ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_NAME_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------
GET_OBJECT_FILENAME_FILTERS_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// writeObjectFileImpl
//--------------------------------------------------------------------------------
WRITE_OBJECT_FILE_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
