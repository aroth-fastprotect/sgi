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
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::TerrainLayer)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::ModelLayer)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::MaskLayer)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::Registry)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::Config)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::ConfigOptions)
GET_OBJECT_NAME_IMPL_DECLARE(osgEarth::LevelDBDatabase)
GET_OBJECT_NAME_IMPL_DECLARE(TileKeyReferenced)
GET_OBJECT_NAME_IMPL_DECLARE(TileSourceTileKey)

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE(osgEarth::Config)
GET_OBJECT_TYPE_IMPL_DECLARE(osgEarth::ConfigOptions)

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_IMPL_TEMPLATE()
GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(osgEarth::Map)
GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(osgEarth::MapNode)

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_TEMPLATE()
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(osgEarth::Map)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(osgEarth::MapNode)

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------
GET_OBJECT_FILENAME_FILTERS_IMPL_TEMPLATE()
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(osgEarth::Map)
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(osgEarth::MapNode)

//--------------------------------------------------------------------------------
// writeObjectFileImpl
//--------------------------------------------------------------------------------
WRITE_OBJECT_FILE_IMPL_TEMPLATE()
WRITE_OBJECT_FILE_IMPL_DECLARE(osgEarth::Map)
WRITE_OBJECT_FILE_IMPL_DECLARE(osgEarth::MapNode)

} // namespace osgearth_plugin
} // namespace sgi
