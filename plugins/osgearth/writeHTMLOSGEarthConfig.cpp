#include <ostream>
#include "writeHTMLOSGEarth.h"
#include "writeHTMLOSGEarthConfig.h"
#include "../osg/SGIItemOsg"
#include "SGIItemOsgEarth"

#include "string_helpers.h"

#include <osgEarth/Version>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/HTTPClient>
#include <osgEarth/GeoData>
#include <osgEarth/Cache>
#include <osgEarth/CacheBin>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/OverlayDecorator>

#include <osgEarth/ImageLayer>
#include <osgEarth/ElevationLayer>
#include <osgEarth/ModelLayer>
#include <osgEarth/MaskLayer>

#include <osgEarthFeatures/FeatureModelSource>
#include <osgEarthUtil/LatLongFormatter>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/model_feature_stencil/FeatureStencilModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/gdal/GDALOptions>

#include "osgearth_accessor.h"
#include <sgi/helpers/rtti>

namespace std {

template<>
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::optional<osg::Texture::FilterMode> & v)
{
    if(!v.isSet())
    {
        return os << "unset(" << sgi::castToEnumValueString<sgi::GLConstant>(v.defaultValue()) << ")";
    }
    else
        return os << sgi::castToEnumValueString<sgi::GLConstant>(v.value());
}
} // namespace std

namespace sgi {
namespace osgearth_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Config)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ConfigOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::DriverConfigOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::MapOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::MapNodeOptions)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ProfileOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TerrainOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TerrainLayerOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ImageLayerOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ElevationLayerOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ModelLayerOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::MaskLayerOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TileSourceOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ModelSourceOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::CacheOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Features::FeatureModelSourceOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Features::GeometryCompilerOptions)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::TMSOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::WMSOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::VPBOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::ArcGISOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::FileSystemCacheOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::SimpleModelOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::FeatureGeomModelOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::FeatureStencilModelOptions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Drivers::OGRFeatureOptions)


void writePrettyHTMLImplForDriverOptions(SGIPluginHostInterface * hostInterface, std::basic_ostream<char>& os, const osgEarth::DriverConfigOptions & opts)
{
    osgEarth::DriverConfigOptions * actualOpts = const_cast<osgEarth::DriverConfigOptions *>(&opts);
    const std::string driver = opts.getDriver();
    if(driver == "feature_geom")
        actualOpts = new osgEarth::Drivers::FeatureGeomModelOptions(opts);
    else if(driver == "feature_stencil")
        actualOpts = new osgEarth::Drivers::FeatureStencilModelOptions(opts);
    else if(driver == "simple")
        actualOpts = new osgEarth::Drivers::SimpleModelOptions(opts);
    else if(driver == "tms")
        actualOpts = new osgEarth::Drivers::TMSOptions(opts);
    else if(driver == "wms")
        actualOpts = new osgEarth::Drivers::WMSOptions(opts);
    else if(driver == "vpb")
        actualOpts = new osgEarth::Drivers::VPBOptions(opts);
    else if(driver == "arcgis")
        actualOpts = new osgEarth::Drivers::ArcGISOptions(opts);
    else if(driver == "gdal")
        actualOpts = new osgEarth::Drivers::GDALOptions(opts);
    else if(driver == "ogr")
        actualOpts = new osgEarth::Drivers::OGRFeatureOptions(opts);
    SGIHostItemOsgEarthConfigOptions featureOptions((actualOpts==&opts)?SGIHostItemOsgEarthConfigOptions::NO_DELETE:SGIHostItemOsgEarthConfigOptions::USE_NEW_DELETE, actualOpts);
    if(featureOptions.hasObject())
        hostInterface->writePrettyHTML(os, &featureOptions);
}

template<typename T>
void writePrettyHTMLImplForDriverOptions(SGIPluginHostInterface * hostInterface, std::basic_ostream<char>& os, const osgEarth::optional<T> & opts)
{
    if(!opts.isSet())
        writePrettyHTMLImplForDriverOptions(hostInterface, os, opts.defaultValue());
    else
        writePrettyHTMLImplForDriverOptions(hostInterface, os, opts.value());
}

bool writePrettyHTMLImpl<osgEarth::Config>::process(std::basic_ostream<char>& os)
{
    osgEarth::Config * object = getObject<osgEarth::Config,SGIItemEarthConfig>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            os << "<ul>";

            if(!object->key().empty() || !object->value().empty())
            {
                os << "<li>" << object->key() << "=" << object->value() << "</li>";
            }
            if(!object->referrer().empty())
                os << "<li>referrer=" << object->referrer() << "</li>";

            const osgEarth::ConfigSet& childs = object->children();
            if(!childs.empty())
                os << "<li>childs=" << childs.size() << "</li>";

            os << "</ul><hr/>" << std::endl;
            if(object->key().empty())
                os << "<pre>" << object->toJSON(true) << "</pre>" << std::endl;
            else
                os << "<pre>&quot;" << object->key() << "&quot; = " << object->toJSON(true) << "</pre>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            os << "<ul>";
            const osgEarth::ConfigSet& childs = object->children();
            for(osgEarth::ConfigSet::const_iterator it = childs.begin(); it != childs.end(); it++)
            {
                const osgEarth::Config & child = *it;
                os << "<li>" << child.key() << "=" << child.value();
                if(!child.referrer().empty() && child.referrer() != object->referrer())
                    os << " [" << child.referrer() << "]";
                os << "</li>";
            }
            os << "</ul>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        // there's no way to get the parents of a Config item
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ConfigOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ConfigOptions * object = getObject<osgEarth::ConfigOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>class</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
            os << "<tr><td>referrer</td><td>" << object->referrer() << "</td></tr>" << std::endl;
            /*
            if(!brief)
            {
                os << "<tr><td>config</td><td>";
                writePrettyHTML(os, object->getConfig(), object->referrer(), true);
                os << "</td></tr>" << std::endl;
            }
            */

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        {
            osg::ref_ptr<SGIHostItemOsgEarthConfig> hostitem = new SGIHostItemOsgEarthConfig(object->getConfig());
            _hostInterface->writePrettyHTML(os, hostitem.get());
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::DriverConfigOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::DriverConfigOptions * object = getObject<osgEarth::DriverConfigOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>driver</td><td>" << object->getDriver() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::MapOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::MapOptions * object = getObject<osgEarth::MapOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->name() << "</td></tr>" << std::endl;
            os << "<tr><td>coordSysType</td><td>" << object->coordSysType() << "</td></tr>" << std::endl;
            os << "<tr><td>elevationInterpolation</td><td>" << object->elevationInterpolation() << "</td></tr>" << std::endl;
            os << "<tr><td>elevationTileSize</td><td>" << object->elevationTileSize() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            SGIHostItemOsgEarthConfigOptions profile(object->profile());
            if(profile.hasObject())
                _hostInterface->writePrettyHTML(os, &profile);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>cache</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->cache());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>cachePolicy</td><td>" << object->cachePolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>referenceURI</td><td>" << object->referenceURI() << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::MapNodeOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::MapNodeOptions * object = getObject<osgEarth::MapNodeOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>proxy</td><td>" << object->proxySettings() << "</td></tr>" << std::endl;
            os << "<tr><td>cacheOnly</td><td>" << object->cacheOnly() << "</td></tr>" << std::endl;
            os << "<tr><td>lighting</td><td>" << object->enableLighting() << "</td></tr>" << std::endl;
            os << "<tr><td>terrainOptions</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->getTerrainOptions());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ProfileOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ProfileOptions * object = getObject<osgEarth::ProfileOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->namedProfile() << "</td></tr>" << std::endl;
            os << "<tr><td>srsString</td><td>" << object->srsString() << "</td></tr>" << std::endl;
            os << "<tr><td>vsrsString</td><td>" << object->vsrsString() << "</td></tr>" << std::endl;
            os << "<tr><td>bounds</td><td>" << object->bounds() << "</td></tr>" << std::endl;
            os << "<tr><td>numTilesWideAtLod0</td><td>" << object->numTilesWideAtLod0() << "</td></tr>" << std::endl;
            os << "<tr><td>numTilesHighAtLod0</td><td>" << object->numTilesHighAtLod0() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::TerrainOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::TerrainOptions * object = getObject<osgEarth::TerrainOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>vertScale</td><td>" << object->verticalScale() << "</td></tr>" << std::endl;
            os << "<tr><td>sampleRatio</td><td>" << object->heightFieldSampleRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>minTileRangeFactor</td><td>" << object->minTileRangeFactor() << "</td></tr>" << std::endl;
            os << "<tr><td>maxLOD</td><td>" << object->maxLOD() << "</td></tr>" << std::endl;
            os << "<tr><td>lighting</td><td>" << object->enableLighting() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::TerrainLayerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::TerrainLayerOptions * object = getObject<osgEarth::TerrainLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->name() << "</td></tr>" << std::endl;

            os << "<tr><td>driver</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->driver());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>verticalDatum</td><td>" << object->verticalDatum() << "</td></tr>" << std::endl;

            os << "<tr><td>minLevel</td><td>" << object->minLevel() << "</td></tr>" << std::endl;
            os << "<tr><td>minResolution</td><td>" << object->minResolution() << "</td></tr>" << std::endl;
            os << "<tr><td>maxLevel</td><td>" << object->maxLevel() << "</td></tr>" << std::endl;
            os << "<tr><td>maxResolution</td><td>" << object->maxResolution() << "</td></tr>" << std::endl;

            os << "<tr><td>enabled</td><td>" << object->enabled() << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << object->visible() << "</td></tr>" << std::endl;
            os << "<tr><td>exactCropping</td><td>" << object->exactCropping() << "</td></tr>" << std::endl;
            os << "<tr><td>reprojectedTileSize</td><td>" << object->reprojectedTileSize() << "</td></tr>" << std::endl;
            os << "<tr><td>cacheId</td><td>" << object->cacheId() << "</td></tr>" << std::endl;

            os << "<tr><td>cacheFormat</td><td>" << object->cacheFormat() << "</td></tr>" << std::endl;
            os << "<tr><td>cachePolicy</td><td>" << object->cachePolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>loadingWeight</td><td>" << object->loadingWeight() << "</td></tr>" << std::endl;
            os << "<tr><td>edgeBufferRatio</td><td>" << object->edgeBufferRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>proxySettings</td><td>" << object->proxySettings() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ImageLayerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ImageLayerOptions * object = getObject<osgEarth::ImageLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>opacity</td><td>" << object->opacity() << "</td></tr>" << std::endl;
            os << "<tr><td>minVisibleRange</td><td>" << object->minVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>maxVisibleRange</td><td>" << object->maxVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>noDataImageFilename</td><td>" << object->noDataImageFilename() << "</td></tr>" << std::endl;
            os << "<tr><td>transparentColor</td><td>" << object->transparentColor() << "</td></tr>" << std::endl;
            os << "<tr><td>lodBlending</td><td>" << object->lodBlending() << "</td></tr>" << std::endl;
            os << "<tr><td>colorFilters</td><td><ol>";
            const osgEarth::ColorFilterChain & colorFilters = object->colorFilters();
            for(auto it = colorFilters.begin(); it != colorFilters.end(); it++)
            {
                os << "<li>" << getObjectNameAndType((*it).get()) << "</li>";

            }
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>shared</td><td>" << object->shared() << "</td></tr>" << std::endl;
            os << "<tr><td>featherPixels</td><td>" << object->featherPixels() << "</td></tr>" << std::endl;

            os << "<tr><td>minFilter</td><td>" << object->minFilter() << "</td></tr>" << std::endl;
            os << "<tr><td>magFilter</td><td>" << object->magFilter() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>textureCompression</td><td>" << object->textureCompression() << "</td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ElevationLayerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ElevationLayerOptions * object = getObject<osgEarth::ElevationLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>offset</td><td>" << object->offset() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ModelLayerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ModelLayerOptions * object = getObject<osgEarth::ModelLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->name() << "</td></tr>" << std::endl;
            os << "<tr><td>enabled</td><td>" << object->enabled() << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << object->visible() << "</td></tr>" << std::endl;
            os << "<tr><td>lightingEnabled</td><td>" << object->lightingEnabled() << "</td></tr>" << std::endl;
            os << "<tr><td>opacity</td><td>" << object->opacity() << "</td></tr>" << std::endl;
            os << "<tr><td>driver</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->driver());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::MaskLayerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::MaskLayerOptions * object = getObject<osgEarth::MaskLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->name() << "</td></tr>" << std::endl;
            os << "<tr><td>driver</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->driver());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::TileSourceOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::TileSourceOptions * object = getObject<osgEarth::TileSourceOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>tileSize</td><td>" << object->tileSize() << "</td></tr>" << std::endl;
            os << "<tr><td>noDataValue</td><td>" << object->noDataValue() << "</td></tr>" << std::endl;
            os << "<tr><td>noDataMinValue</td><td>" << object->noDataMinValue() << "</td></tr>" << std::endl;
            os << "<tr><td>noDataMaxValue</td><td>" << object->noDataMaxValue() << "</td></tr>" << std::endl;
            os << "<tr><td>blacklistFilename</td><td>" << object->blacklistFilename() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            SGIHostItemOsgEarthConfigOptions profile(object->profile());
            if(profile.hasObject())
                _hostInterface->writePrettyHTML(os, &profile);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>L2CacheSize</td><td>" << object->L2CacheSize() << "</td></tr>" << std::endl;
            os << "<tr><td>bilinearReprojection</td><td>" << object->bilinearReprojection() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ModelSourceOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::ModelSourceOptions * object = getObject<osgEarth::ModelSourceOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>minRange</td><td>" << object->minRange() << "</td></tr>" << std::endl;
            os << "<tr><td>maxRange</td><td>" << object->maxRange() << "</td></tr>" << std::endl;
            os << "<tr><td>renderOrder</td><td>" << object->renderOrder() << "</td></tr>" << std::endl;
            os << "<tr><td>depthTestEnabled</td><td>" << object->depthTestEnabled() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::CacheOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::CacheOptions * object = getObject<osgEarth::CacheOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Features::FeatureModelSourceOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Features::FeatureModelSourceOptions * object = getObject<osgEarth::Features::FeatureModelSourceOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>featureOptions</td><td>";
            writePrettyHTMLImplForDriverOptions(_hostInterface, os, object->featureOptions());
            os << "</td></tr>" << std::endl;

            os << "<tr><td>maxGranularity</td><td>" << object->maxGranularity() << "</td></tr>" << std::endl;
            os << "<tr><td>mergeGeometry</td><td>" << object->mergeGeometry() << "</td></tr>" << std::endl;
            os << "<tr><td>enableLighting</td><td>" << object->maxGranularity() << "</td></tr>" << std::endl;
            os << "<tr><td>clusterCulling</td><td>" << object->clusterCulling() << "</td></tr>" << std::endl;
            os << "<tr><td>featureName</td><td>" << object->featureName() << "</td></tr>" << std::endl;
            os << "<tr><td>backfaceCulling</td><td>" << object->backfaceCulling() << "</td></tr>" << std::endl;
            os << "<tr><td>alphaBlending</td><td>" << object->alphaBlending() << "</td></tr>" << std::endl;
            os << "<tr><td>cachePolicy</td><td>" << object->cachePolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>fading</td><td>" << object->fading() << "</td></tr>" << std::endl;
            os << "<tr><td>featureIndexing</td><td>" << object->featureIndexing() << "</td></tr>" << std::endl;

#if 0

        osg::ref_ptr<StyleSheet>& styles() { return _styles; }
        const osg::ref_ptr<StyleSheet>& styles() const { return _styles; }

        optional<FeatureDisplayLayout>& layout() { return _layout; }
        const optional<FeatureDisplayLayout>& layout() const { return _layout; }

        optional<StringExpression>& featureName() { return _featureNameExpr; }
        const optional<StringExpression>& featureName() const { return _featureNameExpr; }
#endif // 0

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Features::GeometryCompilerOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Features::GeometryCompilerOptions * object = getObject<osgEarth::Features::GeometryCompilerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>maxGranularity</td><td>" << object->maxGranularity() << "</td></tr>" << std::endl;
            os << "<tr><td>geoInterp</td><td>" << object->geoInterp() << "</td></tr>" << std::endl;
            os << "<tr><td>mergeGeometry</td><td>" << object->mergeGeometry() << "</td></tr>" << std::endl;
            os << "<tr><td>featureName</td><td>" << object->featureName() << "</td></tr>" << std::endl;
            os << "<tr><td>clustering</td><td>" << object->clustering() << "</td></tr>" << std::endl;
            os << "<tr><td>instancing</td><td>" << object->instancing() << "</td></tr>" << std::endl;
            os << "<tr><td>ignoreAltitudeSymbol</td><td>" << object->ignoreAltitudeSymbol() << "</td></tr>" << std::endl;
            os << "<tr><td>resampleMode</td><td>" << object->resampleMode() << "</td></tr>" << std::endl;
            os << "<tr><td>resampleMaxLength</td><td>" << object->resampleMaxLength() << "</td></tr>" << std::endl;
            os << "<tr><td>useVertexBufferObjects</td><td>" << object->useVertexBufferObjects() << "</td></tr>" << std::endl;
            os << "<tr><td>shaderPolicy</td><td>" << object->shaderPolicy() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::TMSOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::TMSOptions * object = getObject<osgEarth::Drivers::TMSOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>tmsType</td><td>" << object->tmsType() << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::WMSOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::WMSOptions * object = getObject<osgEarth::Drivers::WMSOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>capabilitiesUrl</td><td>" << object->capabilitiesUrl() << "</td></tr>" << std::endl;
            os << "<tr><td>tileServiceUrl</td><td>" << object->tileServiceUrl() << "</td></tr>" << std::endl;
            os << "<tr><td>layers</td><td>" << object->layers() << "</td></tr>" << std::endl;
            os << "<tr><td>style</td><td>" << object->style() << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
            os << "<tr><td>wmsFormat</td><td>" << object->wmsFormat() << "</td></tr>" << std::endl;
            os << "<tr><td>wmsVersion</td><td>" << object->wmsVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>elevationUnit</td><td>" << object->elevationUnit() << "</td></tr>" << std::endl;
            os << "<tr><td>srs</td><td>" << object->srs() << "</td></tr>" << std::endl;
            os << "<tr><td>crs</td><td>" << object->crs() << "</td></tr>" << std::endl;
            os << "<tr><td>transparent</td><td>" << object->transparent() << "</td></tr>" << std::endl;
            os << "<tr><td>times</td><td>" << object->times() << "</td></tr>" << std::endl;
            os << "<tr><td>secondsPerFrame</td><td>" << object->secondsPerFrame() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::VPBOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::VPBOptions * object = getObject<osgEarth::Drivers::VPBOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>primarySplitLevel</td><td>" << object->primarySplitLevel() << "</td></tr>" << std::endl;
            os << "<tr><td>secondarySplitLevel</td><td>" << object->secondarySplitLevel() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>maxDataLevelOverride</td><td>" << object->maxDataLevelOverride() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>directoryStructure</td><td>" << object->directoryStructure() << "</td></tr>" << std::endl;
            os << "<tr><td>layer</td><td>" << object->layer() << "</td></tr>" << std::endl;
            os << "<tr><td>layerSetName</td><td>" << object->layerSetName() << "</td></tr>" << std::endl;
            os << "<tr><td>numTilesWideAtLod0</td><td>" << object->numTilesWideAtLod0() << "</td></tr>" << std::endl;
            os << "<tr><td>numTilesHighAtLod0</td><td>" << object->numTilesHighAtLod0() << "</td></tr>" << std::endl;
            os << "<tr><td>baseName</td><td>" << object->baseName() << "</td></tr>" << std::endl;
            os << "<tr><td>terrainTileCacheSize</td><td>" << object->terrainTileCacheSize() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::ArcGISOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::ArcGISOptions * object = getObject<osgEarth::Drivers::ArcGISOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>token</td><td>" << object->token() << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::FileSystemCacheOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::FileSystemCacheOptions * object = getObject<osgEarth::Drivers::FileSystemCacheOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>rootPath</td><td>" << object->rootPath() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::SimpleModelOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::SimpleModelOptions * object = getObject<osgEarth::Drivers::SimpleModelOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>lodScale</td><td>" << object->lodScale() << "</td></tr>" << std::endl;
            os << "<tr><td>location</td><td>" << object->location() << "</td></tr>" << std::endl;
            os << "<tr><td>orientation</td><td>" << object->orientation() << "</td></tr>" << std::endl;
            os << "<tr><td>shaderPolicy</td><td>" << object->shaderPolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>loadingPriorityScale</td><td>" << object->loadingPriorityScale() << "</td></tr>" << std::endl;
            os << "<tr><td>loadingPriorityOffset</td><td>" << object->loadingPriorityOffset() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::FeatureStencilModelOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::FeatureStencilModelOptions * object = getObject<osgEarth::Drivers::FeatureStencilModelOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>extrusionDistance</td><td>" << object->extrusionDistance() << "</td></tr>" << std::endl;
            os << "<tr><td>densificationThreshold</td><td>" << object->densificationThreshold() << "</td></tr>" << std::endl;
            os << "<tr><td>inverted</td><td>" << object->inverted() << "</td></tr>" << std::endl;
            os << "<tr><td>mask</td><td>" << object->mask() << "</td></tr>" << std::endl;
            os << "<tr><td>showVolumes</td><td>" << object->showVolumes() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::OGRFeatureOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::OGRFeatureOptions * object = getObject<osgEarth::Drivers::OGRFeatureOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>connection</td><td>" << object->connection() << "</td></tr>" << std::endl;
            os << "<tr><td>ogrDriver</td><td>" << object->ogrDriver() << "</td></tr>" << std::endl;
            os << "<tr><td>buildSpatialIndex</td><td>" << object->buildSpatialIndex() << "</td></tr>" << std::endl;
            os << "<tr><td>geometryUrl</td><td>" << object->geometryUrl() << "</td></tr>" << std::endl;
            os << "<tr><td>layer</td><td>" << object->layer() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Drivers::FeatureGeomModelOptions>::process(std::basic_ostream<char>& os)
{
    osgEarth::Drivers::FeatureGeomModelOptions * object = getObject<osgEarth::Drivers::FeatureGeomModelOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>compilerOptions</td><td>";
            SGIHostItemOsgEarthConfigOptions compilerOptions(object->compilerOptions());
            if(compilerOptions.hasObject())
                _hostInterface->writePrettyHTML(os, &compilerOptions);
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

} // namespace osgearth_plugin
} // namespace sgi
