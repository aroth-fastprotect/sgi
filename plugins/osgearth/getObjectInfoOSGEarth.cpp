#include "stdafx.h"
#include "getObjectInfoOSGEarth.h"
#include "string_helpers.h"

#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <sgi/plugins/SGIItemOsg>
#include "SGIItemOsgEarth"

#include <osgEarth/Registry>
#include <osgEarth/Version>
#include <osgEarth/Map>
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarth/MapFrame>
#endif
#include <osgEarth/MapNode>
#include <osgEarth/ModelLayer>
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarth/MaskLayer>
#else
#include <osgEarth/TileSource>
#endif
#include <osgEarth/XmlUtils>
#include <sgi/helpers/rtti>

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarthSymbology/Style>
#include <osgEarthSymbology/StyleSelector>
#else
#include <osgEarth/Style>
#include <osgEarth/StyleSelector>
#endif

namespace sgi {

namespace osgearth_plugin {

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainLayer)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::MaskLayer)
#else
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::TileLayer)
#endif
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelLayer)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Registry)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Config)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::ConfigOptions)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(TileKeyReferenced)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(TileSourceTileKey)

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Symbology::Style)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Symbology::StyleSelector)
#else
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Style)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(osgEarth::StyleSelector)
#endif

GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(osgEarth::Config)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(osgEarth::ConfigOptions)

GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
WRITE_OBJECT_FILE_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
WRITE_OBJECT_FILE_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
std::string getObjectNameImpl<osgEarth::TerrainLayer>::process()
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    return object->getName();
}
#else
std::string getObjectNameImpl<osgEarth::TileLayer>::process()
{
    osgEarth::TileLayer * object = static_cast<osgEarth::TileLayer*>(item<SGIItemOsg>()->object());
    return object->getName();
}
#endif

std::string getObjectNameImpl<osgEarth::ModelLayer>::process()
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    return object->getName();
}

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
std::string getObjectNameImpl<osgEarth::MaskLayer>::process()
{
    osgEarth::MaskLayer * object = static_cast<osgEarth::MaskLayer*>(item<SGIItemOsg>()->object());
    return object->getName();
}
#endif

std::string getObjectNameImpl<osgEarth::Config>::process()
{
    osgEarth::Config * object = static_cast<osgEarth::Config*>(item<SGIItemEarthConfig>()->object());
    return object->key();
}

std::string getObjectNameImpl<osgEarth::ConfigOptions>::process()
{
    osgEarth::ConfigOptions * object = static_cast<osgEarth::ConfigOptions*>(item<SGIItemEarthConfigOptions>()->object());
    return object->getConfig().key();
}

std::string getObjectNameImpl<osgEarth::Registry>::process()
{
    return "osgEarth::Registry";
}


#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
std::string getObjectNameImpl<osgEarth::Symbology::Style>::process()
{
    osgEarth::Symbology::Style* object = static_cast<osgEarth::Symbology::Style*>(item<SGIItemEarthStyle>()->object());
    return object->getName();
}

std::string getObjectNameImpl<osgEarth::Symbology::StyleSelector>::process()
{
    osgEarth::Symbology::StyleSelector* object = static_cast<osgEarth::Symbology::StyleSelector*>(item<SGIItemEarthStyleSelector>()->object());
    return object->name();
}
#else
std::string getObjectNameImpl<osgEarth::Style>::process()
{
    osgEarth::Style* object = static_cast<osgEarth::Style*>(item<SGIItemEarthStyle>()->object());
    return object->getName();
}

std::string getObjectNameImpl<osgEarth::StyleSelector>::process()
{
    osgEarth::StyleSelector* object = static_cast<osgEarth::StyleSelector*>(item<SGIItemEarthStyleSelector>()->object());
    return object->name().value();
}
#endif

std::string getObjectNameImpl<TileKeyReferenced>::process()
{
    TileKeyReferenced * object_ptr = static_cast<TileKeyReferenced*>(item<SGIItemOsg>()->object());
    const osgEarth::TileKey object = object_ptr->data();
    std::stringstream ss;
    ss << object.str();
    if (object.getProfile())
        ss << '_' << object.getProfile()->getHorizSignature();
    return ss.str();
}

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const TileSourceTileKeyData::Status & t);

std::string getObjectNameImpl<TileSourceTileKey>::process()
{
    TileSourceTileKey * object_ptr = static_cast<TileSourceTileKey*>(item<SGIItemOsg>()->object());
    const TileSourceTileKeyData object = object_ptr->data();
    std::stringstream ss;
    std::string name;
	if (object.tileSource.valid())
        name = object.tileSource->getName();
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
    else if(object.terrainLayer.valid())
        name = object.terrainLayer->getName();
#else
    else if(object.tileLayer.valid())
        name = object.tileLayer->getName();
#endif
    else if (object.cacheBin.valid())
        name = object.cacheBin->getID();
    if (!name.empty())
        ss << name << '/';
    ss << object.tileKey.str();
    if(object.tileKey.getProfile())
        ss << '_' << object.tileKey.getProfile()->getHorizSignature();
    ss << '(' << object.status << ')';

    return ss.str();
}

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------

std::string getObjectTypeImpl<osgEarth::Config>::process()
{
    osgEarth::Config * object = static_cast<osgEarth::Config*>(item<SGIItemEarthConfig>()->object());
    std::string ret;
    std::string classname = "osgEarth::Config";
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';

    return ret;
}

std::string getObjectTypeImpl<osgEarth::ConfigOptions>::process()
{
    osgEarth::ConfigOptions * object = static_cast<osgEarth::ConfigOptions*>(item<SGIItemEarthConfigOptions>()->object());
    std::string ret = helpers::getRTTITypename(object);
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------

std::string getObjectSuggestedFilenameImpl<osgEarth::Map>::process()
{
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    std::string ret;
    ret = object->getName() + ".earth";
    return ret;
}

std::string getObjectSuggestedFilenameImpl<osgEarth::MapNode>::process()
{
    osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
    std::string ret;
    ret = object->getName() + ".earth";
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------

std::string getObjectSuggestedFilenameExtensionImpl<osgEarth::Map>::process()
{
    return "earth";
}

std::string getObjectSuggestedFilenameExtensionImpl<osgEarth::MapNode>::process()
{
    return "earth";
}

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------

std::vector<std::string> getObjectFilenameFiltersImpl<osgEarth::Map>::process()
{
    std::vector<std::string> ret;
    ret.push_back("osgEarth Files (*.earth)");
    return ret;
}

std::vector<std::string> getObjectFilenameFiltersImpl<osgEarth::MapNode>::process()
{
    std::vector<std::string> ret;
    ret.push_back("osgEarth Files (*.earth)");
    return ret;
}

//--------------------------------------------------------------------------------
// writeObjectFileImpl
//--------------------------------------------------------------------------------

namespace {

osgEarth::Config serializeMapNode( const osgEarth::MapNode* input, const osgEarth::Config & mergeExternal )
{
    osgEarth::Config mapConf("map");
    mapConf.set("version", "2");

    if ( !input || !input->getMap() )
        return mapConf;

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    mapConf = input->getConfig();
#else
    const osgEarth::Map* map = input->getMap();
    osgEarth::MapFrame mapf( map, osgEarth::Map::ENTIRE_MODEL );

    // the map and node options:
    osgEarth::Config optionsConf = map->getInitialMapOptions().getConfig();
    optionsConf.merge( input->getMapNodeOptions().getConfig() );
    mapConf.add( "options", optionsConf );

    // the layers

    for( osgEarth::ImageLayerVector::const_iterator i = mapf.imageLayers().begin(); i != mapf.imageLayers().end(); ++i )
    {
        osgEarth::ImageLayer* layer = i->get();
        //Config layerConf = layer->getInitialOptions().getConfig();
        osgEarth::Config layerConf = layer->getImageLayerOptions().getConfig();
        layerConf.set("name", layer->getName());
        layerConf.set("driver", layer->getInitialOptions().driver()->getDriver());
        layerConf.remove("default_tile_size");
        mapConf.add( "image", layerConf );
    }

    for( osgEarth::ElevationLayerVector::const_iterator i = mapf.elevationLayers().begin(); i != mapf.elevationLayers().end(); ++i )
    {
        osgEarth::ElevationLayer* layer = i->get();
        //Config layerConf = layer->getInitialOptions().getConfig();
        osgEarth::Config layerConf = layer->getElevationLayerOptions().getConfig();
        layerConf.set("name", layer->getName());
        layerConf.set("driver", layer->getInitialOptions().driver()->getDriver());
        layerConf.remove("default_tile_size");
        mapConf.add( "elevation", layerConf );
    }

    for( osgEarth::ModelLayerVector::const_iterator i = mapf.modelLayers().begin(); i != mapf.modelLayers().end(); ++i )
    {
        osgEarth::ModelLayer* layer = i->get();
        osgEarth::Config layerConf = layer->getModelLayerOptions().getConfig();
        layerConf.set("name", layer->getName());
        layerConf.set("driver", layer->getModelLayerOptions().driver()->getDriver());
        mapConf.add( "model", layerConf );
    }
#endif

    osgEarth::Config ext = input->externalConfig();
    if ( !ext.empty() || !mergeExternal.empty())
    {
        ext.merge(mergeExternal);
        ext.key() = "external";
        mapConf.add( ext );
    }

    return mapConf;
}

class osgEarthConfigAccessor : public osgEarth::Config
{
public:
    void setDefaultValue(const std::string & value)
    {
        _defaultValue = value;
    }
};

void transformConfigUri(osgEarth::Config & conf)
{
    const osgEarth::ConfigSet& childs_ = conf.children();
    osgEarth::ConfigSet& childs = const_cast<osgEarth::ConfigSet& >(childs_);
    for(osgEarth::ConfigSet::iterator it = childs.begin(); it != childs.end(); it++)
    {
        if( it->key() == "uri" || it->key() == "url")
        {
            osgEarth::URI olduri = osgEarth::URI( it->value(), it->referrer() );
            ((osgEarthConfigAccessor&)*it).setDefaultValue(olduri.full());
        }
        else
        {
            transformConfigUri(*it);
        }
    }
}

bool writeMapNode( const osgEarth::MapNode* node, std::ostream& out, const std::string & baseuri, const osgEarth::Config & mergeExternal=osgEarth::Config() )
{
    osgEarth::Config conf = serializeMapNode(node, mergeExternal);

    // transform all URI/URL keys to full/absolute URL
    transformConfigUri(conf);

    // dump that Config out as XML.
    osg::ref_ptr<osgEarth::XmlDocument> xml = new osgEarth::XmlDocument( conf );
    xml->store( out );
    return out.good();
}

bool writeMapNode(const osgEarth::MapNode* node, const std::string& fileName, const std::string & baseuri, const osgEarth::Config & mergeExternal=osgEarth::Config() )
{
    std::ofstream out( fileName.c_str());
    if ( out.is_open() )
        return writeMapNode( node, out, baseuri, mergeExternal );
    else
        return false;
}

bool writeMapNode(const osgEarth::MapNode* node, const std::string& fileName, const osgEarth::Config & mergeExternal=osgEarth::Config() )
{
    std::string baseuri;
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::Config cfg = node->getMapNodeOptions().getConfig();
#else
    osgEarth::Config cfg = node->getConfig();
#endif
    baseuri = cfg.referrer();
    return writeMapNode(node, fileName, baseuri, mergeExternal);
}

}

bool writeObjectFileImpl<osgEarth::Map>::process(const std::string& filename, const SGIItemBase* options)
{
    osgEarth::Map* object = getObject<osgEarth::Map,SGIItemOsg>();
    bool ret = false;
    const osgDB::Options * dbOpts = NULL;
    if(options)
        dbOpts = dynamic_cast<const osgDB::Options*>(static_cast<const SGIItemOsg*>(options));
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = false;
        break;
    }
    return ret;
}

bool writeObjectFileImpl<osgEarth::MapNode>::process(const std::string& filename, const SGIItemBase* options)
{
    osgEarth::MapNode* object = getObject<osgEarth::MapNode,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = writeMapNode(object, filename);
        break;
    }
    return ret;
}

} // namespace osgearth_plugin
} // namespace sgi
