#pragma once

#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/GetObjectInfoImpl>

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

namespace osgearth_plugin {

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
