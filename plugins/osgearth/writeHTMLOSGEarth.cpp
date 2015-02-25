#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>
#include <sgi/helpers/osg>

#include "string_helpers.h"

#include <osgEarth/Version>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/StateSetCache>
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
#include <osgEarth/ElevationQuery>

#include <osgEarthUtil/LatLongFormatter>

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#include <osgEarthFeatures/FeatureModelSource>

#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/ModelNode>
#include <osgEarthAnnotation/TrackNode>


#include "osgearth_accessor.h"
#include "ElevationQueryReferenced"
#include "geo_helpers.h"

namespace sgi {
namespace osgearth_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Registry)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Capabilities)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::StateSetCache)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Cache)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::CacheBin)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Layer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TerrainLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ImageLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ElevationLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ModelLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::MaskLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Map)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::MapNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TerrainDecorator)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::OverlayDecorator)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::SpatialReference)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Profile)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TileSource)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::ModelSource)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::VirtualProgram)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::TileBlacklist)
WRITE_PRETTY_HTML_IMPL_REGISTER(ElevationQueryReferenced)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::ControlCanvas)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::Control)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::ControlEventHandler)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::Container)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::ControlNodeBin)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::ControlNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::LabelControl)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::Controls::ImageControl)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::SkyNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Util::AutoClipPlaneCullCallback)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Features::FeatureModelSource)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::AnnotationNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::PositionedAnnotationNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::FeatureNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::LocalizedNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::OrthoNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::PlaceNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::LabelNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::TrackNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::CircleNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::EllipseNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::RectangleNode)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgEarth::Annotation::ModelNode)

using namespace osg_helpers;

bool writePrettyHTMLImpl<osgEarth::Registry>::process(std::basic_ostream<char>& os)
{
    osgEarth::Registry * object = static_cast<osgEarth::Registry*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining Registry properties
            os << "<tr><td>overrideCachePolicy</td><td>" << (object?object->overrideCachePolicy():osgEarth::optional<osgEarth::CachePolicy>()) << "</td></tr>" << std::endl;
            os << "<tr><td>defaultCachePolicy</td><td>" << (object?object->defaultCachePolicy():osgEarth::optional<osgEarth::CachePolicy>()) << "</td></tr>" << std::endl;
            os << "<tr><td>defaultTerrainEngineDriverName</td><td>" << ((object)?object->getDefaultTerrainEngineDriverName():"&lt;null&gt;") << "</td></tr>" << std::endl;

            os << "<tr><td>HTTP user agent</td><td>" << osgEarth::HTTPClient::getUserAgent() << "</td></tr>" << std::endl;
            os << "<tr><td>HTTP timeout</td><td>" << osgEarth::HTTPClient::getTimeout() << "</td></tr>" << std::endl;
            os << "<tr><td>HTTP connect timeout</td><td>" << osgEarth::HTTPClient::getConnectTimeout() << "</td></tr>" << std::endl;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            os << "<tr><td>HTTP proxy</td><td>" << osgEarth::HTTPClient::getProxySettings() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>HTTP URL rewriter</td><td>" << osgEarth::HTTPClient::getURLRewriter() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Capabilities>::process(std::basic_ostream<char>& os)
{
    osgEarth::Capabilities * object = static_cast<osgEarth::Capabilities*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining Capabilities properties
            os << "<tr><td>MaxFFPTextureUnits</td><td>" << object->getMaxFFPTextureUnits() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxGPUTextureUnits</td><td>" << object->getMaxGPUTextureUnits() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxGPUTextureCoordSets</td><td>" << object->getMaxGPUTextureCoordSets() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxGPUAttribs</td><td>" << object->getMaxGPUAttribs() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxTextureSize</td><td>" << object->getMaxTextureSize() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxFastTextureSize</td><td>" << object->getMaxFastTextureSize() << "</td></tr>" << std::endl;
            os << "<tr><td>MaxLights</td><td>" << object->getMaxLights() << "</td></tr>" << std::endl;
            os << "<tr><td>DepthBufferBits</td><td>" << object->getDepthBufferBits() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsGLSL</td><td>" << (object->supportsGLSL()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>GLSL version</td><td>" << object->getGLSLVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>vendor</td><td>" << object->getVendor() << "</td></tr>" << std::endl;
            os << "<tr><td>renderer</td><td>" << object->getRenderer() << "</td></tr>" << std::endl;
            os << "<tr><td>version</td><td>" << object->getVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsTextureArrays</td><td>" << (object->supportsTextureArrays()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsTexture3D</td><td>" << (object->supportsTexture3D()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsMultiTexture</td><td>" << (object->supportsMultiTexture()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsStencilWrap</td><td>" << (object->supportsStencilWrap()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsTwoSidedStencil</td><td>" << (object->supportsTwoSidedStencil()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsTexture2DLod</td><td>" << (object->supportsTexture2DLod()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsMipmappedTextureUpdates</td><td>" << (object->supportsMipmappedTextureUpdates()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsDepthPackedStencilBuffer</td><td>" << (object->supportsDepthPackedStencilBuffer()?"true":"false") << "</td></tr>" << std::endl;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            os << "<tr><td>supportsQuadBufferStereo</td><td>" << (object->supportsQuadBufferStereo()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>supportsOcclusionQuery</td><td>" << (object->supportsOcclusionQuery()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsDrawInstanced</td><td>" << (object->supportsDrawInstanced()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsUniformBufferObjects</td><td>" << (object->supportsUniformBufferObjects()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsNonPowerOfTwoTextures</td><td>" << (object->supportsNonPowerOfTwoTextures()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>maxUniformBlockSize</td><td>" << object->getMaxUniformBlockSize() << "</td></tr>" << std::endl;
            os << "<tr><td>preferDisplayListsForStaticGeometry</td><td>" << (object->preferDisplayListsForStaticGeometry()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>numProcessors</td><td>" << object->getNumProcessors() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsFragDepthWrite</td><td>" << (object->supportsFragDepthWrite()?"true":"false") << "</td></tr>" << std::endl;

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

namespace {
    class StateSetCacheHTML : public osgEarth::StateSetCache
    {
    public:
        void writeHTML(std::basic_ostream<char>& os) const
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            osgEarth::Threading::ScopedMutexLock lock( _mutex );

            os << "<tr><td>size</td><td>" << size() << "</td></tr>" << std::endl;
            os << "<tr><td>attr attempts</td><td>" << _attrShareAttempts << "</td></tr>" << std::endl;
            os << "<tr><td>ineligibles attrs</td><td>" << _attrsIneligible << "</td></tr>" << std::endl;
            os << "<tr><td>attr share hits</td><td>" << _attrShareHits << "</td></tr>" << std::endl;
            os << "<tr><td>attr share misses</td><td>" << _attrShareMisses << "</td></tr>" << std::endl;
#endif
        }
    };
}

bool writePrettyHTMLImpl<osgEarth::StateSetCache>::process(std::basic_ostream<char>& os)
{
    osgEarth::StateSetCache * object = static_cast<osgEarth::StateSetCache*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining StateSetCache properties
            const StateSetCacheHTML * access = (const StateSetCacheHTML*)object;
            access->writeHTML(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStateAttibutes:
        ret = true;
        break;
    case SGIItemTypeStateSets:
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::CacheBin>::process(std::basic_ostream<char>& os)
{
    osgEarth::CacheBin * object = static_cast<osgEarth::CacheBin*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>id</td><td>" << object->getID() << "</td></tr>" << std::endl;
            os << "<tr><td>metadata</td><td>";
            osgEarth::Config config = object->readMetadata();
            os << "<pre>" << config.toJSON(true) << "</pre>";
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        {
            osgEarth::Config config = object->readMetadata();
            os << "<pre>" << config.toJSON(true) << "</pre>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Cache>::process(std::basic_ostream<char>& os)
{
    osgEarth::Cache * object = static_cast<osgEarth::Cache*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            CacheAccess * access = (CacheAccess *)object;

            os << "<tr><td>ok</td><td>" << (object->isOK()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>options</td><td>";
            /*
            if(access->getCacheOptions().getDriver() == "filesystem")
            {
                osgEarth::Drivers::FileSystemCacheOptions fsopt(access->getCacheOptions());
                writePrettyHTML(os, fsopt);
            }
            else
                writePrettyHTML(os, access->getCacheOptions());
            */
            os << "</td></tr>" << std::endl;
            os << "<tr><td>default cache bin</td><td>" << getObjectNameAndType(access->getDefaultBin()) << "</td></tr>" << std::endl;
            os << "<tr><td>cache bins</td><td><ul>";
            const ThreadSafeCacheBinMapAccessor::MAP & cacheBinMap = ((const ThreadSafeCacheBinMapAccessor&)access->getCacheBinMap())._data;
            for(ThreadSafeCacheBinMapAccessor::MAP::const_iterator it = cacheBinMap.begin(); it != cacheBinMap.end(); it++)
            {
                const std::string & name = it->first;
                const osg::ref_ptr<osgEarth::CacheBin> & cachebin = it->second;
                os << "<li>" << getObjectNameAndType(cachebin.get()) << "</li>" << std::endl;
            }
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>approx. size</td><td>" << object->getApproximateSize() << "</td></tr>" << std::endl;
#endif
            os << "</ul></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::SpatialReference>::process(std::basic_ostream<char>& os)
{
    osgEarth::SpatialReference * object = static_cast<osgEarth::SpatialReference*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>wkt</td><td>" << object->getWKT() << "</td></tr>" << std::endl;
            os << "<tr><td>initType</td><td>" << object->getInitType() << "</td></tr>" << std::endl;
            os << "<tr><td>horizontal init</td><td>" << object->getHorizInitString() << "</td></tr>" << std::endl;
            os << "<tr><td>vertical init</td><td>" << object->getVertInitString() << "</td></tr>" << std::endl;
            os << "<tr><td>datum name</td><td>" << object->getDatumName() << "</td></tr>" << std::endl;
            os << "<tr><td>ellipsoid</td><td>" << object->getEllipsoid() << "</td></tr>" << std::endl;

            os << "<tr><td>isGeographic</td><td>" << (object->isGeographic()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isGeodetic</td><td>" << (object->isGeodetic()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isProjected</td><td>" << (object->isProjected()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isMercator</td><td>" << (object->isMercator()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isSphericalMercator</td><td>" << (object->isSphericalMercator()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isNorthPolar</td><td>" << (object->isNorthPolar()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isSouthPolar</td><td>" << (object->isSouthPolar()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isUserDefined</td><td>" << (object->isUserDefined()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isContiguous</td><td>" << (object->isContiguous()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isCube</td><td>" << (object->isCube()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isLTP</td><td>" << (object->isLTP()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isPlateCarre</td><td>" << (object->isPlateCarre()?"true":"false") << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Profile::ProfileType & t)
{
    switch(t)
    {
    case osgEarth::Profile::TYPE_UNKNOWN: os << "unknown"; break;
    case osgEarth::Profile::TYPE_GEODETIC: os << "geodetic"; break;
    case osgEarth::Profile::TYPE_MERCATOR: os << "mercator"; break;
    case osgEarth::Profile::TYPE_LOCAL: os << "local"; break;
    default: os << (int)t; break;
    }
    return os;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::GeoCircle & object)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>srs</td><td>";
    const osgEarth::SpatialReference * srs = object.getSRS();
    if(srs)
        os << srs->getName();
    else
        os << "&lt;null&gt;";
    os << "</td></tr>" << std::endl;
    os << std::setprecision(12);
    os << "<tr><td>radius</td><td>" << object.getRadius() << "</td></tr>" << std::endl;
    os << "<tr><td>center</td><td>" << object.getCenter() << "</td></tr>" << std::endl;
    os << "</table>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::GeoExtent & object)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>srs</td><td>";
    const osgEarth::SpatialReference * srs = object.getSRS();
    if(srs)
        os << srs->getName();
    else
        os << "&lt;null&gt;";
    os << "</td></tr>" << std::endl;
    os << std::setprecision(12);
    os << "<tr><td>north</td><td>" << object.north() << "</td></tr>" << std::endl;
    os << "<tr><td>south</td><td>" << object.south() << "</td></tr>" << std::endl;
    os << "<tr><td>east</td><td>" << object.east() << "</td></tr>" << std::endl;
    os << "<tr><td>west</td><td>" << object.west() << "</td></tr>" << std::endl;
    os << "<tr><td>width</td><td>" << object.width() << "</td></tr>" << std::endl;
    os << "<tr><td>height</td><td>" << object.height() << "</td></tr>" << std::endl;
    os << "<tr><td>geocirle</td><td>";
    writePrettyHTML(os, object.getBoundingGeoCircle());
    os << "</td></tr>" << std::endl;
    os << "</table>" << std::endl;
}

bool writePrettyHTMLImpl<osgEarth::Profile>::process(std::basic_ostream<char>& os)
{
    osgEarth::Profile * object = static_cast<osgEarth::Profile*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>isOk</td><td>" << (object->isOK()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>extent</td><td>";
            writePrettyHTML(os, object->getExtent());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>lat/lon extent</td><td>";
            writePrettyHTML(os, object->getLatLongExtent());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>profileType</td><td>" << object->getProfileType() << "</td></tr>" << std::endl;
            os << "<tr><td>full signature</td><td>" << object->getFullSignature() << "</td></tr>" << std::endl;
            os << "<tr><td>horiz signature</td><td>" << object->getHorizSignature() << "</td></tr>" << std::endl;
            os << "<tr><td>srs</td><td>";
            const osgEarth::SpatialReference * srs = object->getSRS();
            if(srs)
                os << srs->getName();
            else
                os << "&lt;null&gt;";
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

bool writePrettyHTMLImpl<osgEarth::Layer>::process(std::basic_ostream<char>& os)
{
    osgEarth::Layer * object = static_cast<osgEarth::Layer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add terrain layer properties
            os << "<tr><td>uid</td><td>" << (object?object->getUID():0) << "</td></tr>" << std::endl;

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
/*
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::TerrainLayer::CacheBinMetadata & object, bool table)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

    os << "<tr><td>empty</td><td>" << (object._empty?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>cacheBinId</td><td>" << object._cacheBinId << "</td></tr>" << std::endl;
    os << "<tr><td>sourceName</td><td>" << object._sourceName << "</td></tr>" << std::endl;
    os << "<tr><td>sourceDriver</td><td>" << object._sourceDriver << "</td></tr>" << std::endl;
    os << "<tr><td>sourceProfile</td><td>";
    if(object._sourceProfile.isSet())
        writePrettyHTML(os, object._sourceProfile.value(), true);
    os << "</td></tr>" << std::endl;
    os << "<tr><td>cacheProfile</td><td>";
    if(object._cacheProfile.isSet())
        writePrettyHTML(os, object._cacheProfile.value(), true);
    os << "</td></tr>" << std::endl;

    if(_table)
        os << "</table>" << std::endl;
}
*/

bool writePrettyHTMLImpl<osgEarth::TerrainLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add layer properties first
            callNextHandler(os);

            // add terrain layer properties
            TerrainLayerAccessor * access = (TerrainLayerAccessor*)object;
            const osgEarth::Profile * profile = access->profileNoInit();
            const osgEarth::Profile * targetProfileHint = access->targetProfileHintNoInit();

            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>tileSourceInitAttempted</td><td>" << (access->tileSourceInitAttempted()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>tileSourceInitFailed</td><td>" << (access->tileSourceInitFailed()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>tileSize</td><td>" << access->tileSize() << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << (object->getVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            if(profile)
                os << profile->toString();
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>targetProfileHint</td><td>";
            if(targetProfileHint)
                os << targetProfileHint->toString();
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>dynamic</td><td>" << (object->isDynamic()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>cacheOnly</td><td>" <<  (object->isCacheOnly()?"true":"false") << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::ImageLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            // add image layer properties
            os << "<tr><td>colorFilters</td><td>";
            const osgEarth::ColorFilterChain & colorFilters = object->getColorFilters();
            if(colorFilters.empty())
                os << "<i>none</i>";
            else
            {
                os << "<ol>";
                for(auto it = colorFilters.begin(); it != colorFilters.end(); it++)
                {
                    os << "<li>" << getObjectNameAndType((*it).get()) << "</li>";
                }
                os << "</ol>";
            }
            os << "</td></tr>" << std::endl;
            os << "<tr><td>opacity</td><td>" << object->getOpacity() << "</td></tr>" << std::endl;

            os << "<tr><td>LOD blending</td><td>" <<  (object->isLODBlendingEnabled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>minVisibleRange</td><td>" << object->getMinVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>maxVisibleRange</td><td>" << object->getMaxVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>isShared</td><td>" <<  (object->isShared()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isCoverage</td><td>" <<  (object->isCoverage()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>shareImageUnit</td><td>" <<  object->shareImageUnit() << "</td></tr>" << std::endl;
            os << "<tr><td>shareTexMatUniformName</td><td>" <<  object->shareTexMatUniformName() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::ElevationLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::ElevationLayer * object = static_cast<osgEarth::ElevationLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            // add elevation layer properties
            ElevationLayerAccessor * access = (ElevationLayerAccessor*)object;

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>isOffset</td><td>" << (access->isOffset()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>suggestCacheFormat</td><td>" << access->suggestCacheFormatStr() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::ModelLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            // add model layer properties
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>driver</td><td>" <<  object->getModelLayerOptions().driver().value().getDriver() << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << (object->getVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>lightingEnabled</td><td>" << (object->isLightingEnabled()?"true":"false") << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>isTerrainPatch</td><td>" << (object->isTerrainPatch()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>maskMinLevel</td><td>" << object->getMaskMinLevel() << "</td></tr>" << std::endl;
            os << "<tr><td>opacity</td><td>" << object->getOpacity() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::MaskLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::MaskLayer * object = static_cast<osgEarth::MaskLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            // add mask layer properties
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>minLevel</td><td>" << object->getMinLevel() << "</td></tr>" << std::endl;
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

bool writePrettyHTMLImpl<osgEarth::Map>::process(std::basic_ostream<char>& os)
{
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining map properties
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            if(object->getProfile())
                os << object->getProfile()->toString();
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>profile no vdatum</td><td>";
            if(object->getProfileNoVDatum())
                os << object->getProfileNoVDatum()->toString();
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>geocentric</td><td>" << (object->isGeocentric()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>cache</td><td>" << object->getCache() << "</td></tr>" << std::endl;
            
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ENTIRE_MODEL);

            {
                os << "<tr><td>image layers</td><td><ol>";
                const osgEarth::ImageLayerVector & layers = mapframe.imageLayers();
                for(osgEarth::ImageLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
                {
                    const osgEarth::ImageLayer * layer = *it;
                    os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
                }
                os << "</ol></td></tr>" << std::endl;
            }
            {
                os << "<tr><td>elevation layers</td><td><ol>";
                const osgEarth::ElevationLayerVector & layers = mapframe.elevationLayers();
                for(osgEarth::ElevationLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
                {
                    const osgEarth::ElevationLayer * layer = *it;
                    os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
                }
                os << "</ol></td></tr>" << std::endl;
            }
            {
                os << "<tr><td>model layers</td><td><ol>";
                const osgEarth::ModelLayerVector & layers = mapframe.modelLayers();
                for(osgEarth::ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
                {
                    const osgEarth::ModelLayer * layer = *it;
                    os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
                }
                os << "</ol></td></tr>" << std::endl;
            }
            {
                os << "<tr><td>terrain mask layers</td><td><ol>";
                const osgEarth::MaskLayerVector & layers = mapframe.terrainMaskLayers();
                for(osgEarth::MaskLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
                {
                    const osgEarth::MaskLayer * layer = *it;
                    os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
                }
                os << "</ol></td></tr>" << std::endl;
            }
            os << "<tr><td>database options</td><td>" << getObjectNameAndType(object->getDBOptions()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeImageLayers:
        {
            os << "<ol>";
            osgEarth::MapFrame mapframe(object, osgEarth::Map::IMAGE_LAYERS);
            const osgEarth::ImageLayerVector & layers = mapframe.imageLayers();
            for(osgEarth::ImageLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ImageLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeElevationLayers:
        {
            os << "<ol>";
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ELEVATION_LAYERS);
            const osgEarth::ElevationLayerVector & layers = mapframe.elevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ElevationLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeModelLayers:
        {
            os << "<ol>";
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MODEL_LAYERS);
            const osgEarth::ModelLayerVector & layers = mapframe.modelLayers();
            for(osgEarth::ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ModelLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeMaskLayers:
        {
            os << "<ol>";
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MASK_LAYERS);
            const osgEarth::MaskLayerVector & layers = mapframe.terrainMaskLayers();
            for(osgEarth::MaskLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::MaskLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>map callbacks</td><td><ul>" << std::endl;
            osgEarth::MapCallbackList callbacks;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            object->getMapCallbacks(callbacks);
#endif
            for(osgEarth::MapCallbackList::const_iterator it = callbacks.begin(); it != callbacks.end(); it++)
            {
                const osg::ref_ptr<osgEarth::MapCallback> & callback = *it;
                os << "<li>" << getObjectNameAndType(callback.get()) << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::MapNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            os << "<tr><td>map</td><td>" << getObjectNameAndType(const_cast<osgEarth::MapNode *>(object)->getMap()) << "</td></tr>" << std::endl;
            os << "<tr><td>terrain</td><td>" << getObjectNameAndType(const_cast<osgEarth::MapNode *>(object)->getTerrain()) << "</td></tr>" << std::endl;
            os << "<tr><td>terrain engine</td><td>" << getObjectNameAndType(const_cast<osgEarth::MapNode *>(object)->getTerrainEngine()) << "</td></tr>" << std::endl;
            os << "<tr><td>modelLayerGroup</td><td>" << getObjectNameAndType(object->getModelLayerGroup()) << "</td></tr>" << std::endl;
            os << "<tr><td>overlayDecorator</td><td>" << getObjectNameAndType(const_cast<osgEarth::MapNode *>(object)->getOverlayDecorator()) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::TerrainDecorator>::process(std::basic_ostream<char>& os)
{
    osgEarth::TerrainDecorator * object = static_cast<osgEarth::TerrainDecorator*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            // add remaining TerrainDecorator properties

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

bool writePrettyHTMLImpl<osgEarth::OverlayDecorator>::process(std::basic_ostream<char>& os)
{
    osgEarth::OverlayDecorator * object = static_cast<osgEarth::OverlayDecorator*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add TerrainDecorator properties first
            callNextHandler(os);

            // add remaining OverlayDecorator properties
            os << "<tr><td>max horiz distance</td><td>" << object->getMaxHorizonDistance() << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::TileSource::Status & status)
{
    if(status.isOK())
        return os << "OK";
    else
        return os << status.message();
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::DataExtentList & object)
{
    os << "<ul>" << std::endl;

    os << std::setprecision(12);
    for(osgEarth::DataExtentList::const_iterator it = object.begin(); it != object.end(); it++)
    {
        const osgEarth::DataExtent & ext = *it;

        MapDownload::NamedGeoPointList coords;
        coords.push_back(MapDownload::NamedGeoPoint("A", osgEarth::GeoPoint(ext.getSRS(), osg::Vec3d(ext.north(), ext.west(), 0), osgEarth::ALTMODE_ABSOLUTE)));
        coords.push_back(MapDownload::NamedGeoPoint("B", osgEarth::GeoPoint(ext.getSRS(), osg::Vec3d(ext.north(), ext.east(), 0), osgEarth::ALTMODE_ABSOLUTE)));
        coords.push_back(MapDownload::NamedGeoPoint("C", osgEarth::GeoPoint(ext.getSRS(), osg::Vec3d(ext.south(), ext.east(), 0), osgEarth::ALTMODE_ABSOLUTE)));
        coords.push_back(MapDownload::NamedGeoPoint("D", osgEarth::GeoPoint(ext.getSRS(), osg::Vec3d(ext.south(), ext.west(), 0), osgEarth::ALTMODE_ABSOLUTE)));

        std::string mapUrl = MapDownload::getUrl(coords);

        os << "<li>" << std::endl;

#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
        if(ext.description().isSet())
            os << ext.description() << " ";
#endif

        if(ext.minLevel().isSet() || ext.maxLevel().isSet())
            os << "min=" << ext.minLevel() << " max=" << ext.maxLevel();

        os << "<br/>&emsp;";
        os << " north=" << ext.north();
        os << " south=" << ext.south();
        os << " east=" << ext.east();
        os << " west=" << ext.west();
        os << " <a href=\"" << mapUrl << "\">preview</a>";
        os << "</li>" << std::endl;
    }

    os << "</ul>" << std::endl;
}

bool writePrettyHTMLImpl<osgEarth::TileSource>::process(std::basic_ostream<char>& os)
{
    osgEarth::TileSource * object = dynamic_cast<osgEarth::TileSource*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            // add remaining TileSource properties
            const osgEarth::Profile * profile = object->getProfile();

            os << "<tr><td>status</td><td>" << object->getStatus() << "</td></tr>" << std::endl;
            os << "<tr><td>isOk</td><td>" << (object->isOK()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>pixels/tile</td><td>" << object->getPixelsPerTile() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            if(profile)
                os << profile->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>nodata value</td><td>" << object->getNoDataValue() << "</td></tr>" << std::endl;
            os << "<tr><td>nodata min value</td><td>" << object->getNoDataMinValue() << "</td></tr>" << std::endl;
            os << "<tr><td>nodata max value</td><td>" << object->getNoDataMaxValue() << "</td></tr>" << std::endl;

            os << "<tr><td>extension</td><td>" << object->getExtension() << "</td></tr>" << std::endl;
            os << "<tr><td>isDynamic</td><td>" << (object->isDynamic()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>cachePolicyHint</td><td>" << object->getCachePolicyHint(profile) << "</td></tr>" << std::endl;
            os << "<tr><td>dataExtents</td><td>" << object->getDataExtents().size() << " entries" << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDataExtents:
        writePrettyHTML(os, object->getDataExtents());
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::ModelSource>::process(std::basic_ostream<char>& os)
{
    osgEarth::ModelSource * object = getObject<osgEarth::ModelSource,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            // add remaining ModelSource properties
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            const osgEarth::NodeOperationVector& preMergeOperations = object->preMergeOperations();
            os << "<tr><td>preMergeOperations</td><td><ul>";
            for(osgEarth::NodeOperationVector::const_iterator it = preMergeOperations.begin(); it != preMergeOperations.end(); it++)
            {
                const osg::ref_ptr<osgEarth::NodeOperation> & nodeop = *it;
                os << "<li>" << getObjectNameAndType(nodeop.get()) << "</li>" << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;

            const osgEarth::NodeOperationVector& postMergeOperations = object->postMergeOperations();
            os << "<tr><td>postMergeOperations</td><td><ul>";
            for(osgEarth::NodeOperationVector::const_iterator it = postMergeOperations.begin(); it != postMergeOperations.end(); it++)
            {
                const osg::ref_ptr<osgEarth::NodeOperation> & nodeop = *it;
                os << "<li>" << getObjectNameAndType(nodeop.get()) << "</li>" << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;
            os << "<tr><td>dataExtents</td><td>" << object->getDataExtents().size() << " entries" << "</td></tr>" << std::endl;
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDataExtents:
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
        writePrettyHTML(os, object->getDataExtents());
#endif
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t)
{
    switch(t)
    {
    case osgEarth::ShaderComp::LOCATION_VERTEX_MODEL: os << "VERTEX_MODEL"; break;
    case osgEarth::ShaderComp::LOCATION_VERTEX_VIEW: os << "VERTEX_VIEW"; break;
    case osgEarth::ShaderComp::LOCATION_VERTEX_CLIP: os << "VERTEX_CLIP"; break;
    case osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING: os << "FRAGMENT_COLORING"; break;
    case osgEarth::ShaderComp::LOCATION_FRAGMENT_LIGHTING: os << "FRAGMENT_LIGHTING"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osgEarth::VirtualProgram>::process(std::basic_ostream<char>& os)
{
    osgEarth::VirtualProgram * object = static_cast<osgEarth::VirtualProgram*>(item<SGIItemOsg>()->object());
    const VirtualProgramAccessor * access = (const VirtualProgramAccessor*)object;
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            os << "<tr><td>mask</td><td>0x" << std::hex << access->mask() << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>inherit</td><td>";
            if(!access->inheritSet())
                os << "unspecified";
            else
                os << (access->inherit()?"true":"false");
            os << "</td></tr>" << std::endl;

            const osgEarth::VirtualProgram::AttribBindingList & attribBindingList = object->getAttribBindingList();
            os << "<tr><td>attribBindingList</td><td><ul>";
            for(osgEarth::VirtualProgram::AttribBindingList::const_iterator it = attribBindingList.begin(); it != attribBindingList.end(); it++)
            {
                const std::string & name = it->first;
                const GLuint & bind = it->second;
                //os << "<li>" << name << ":" << osg_plugin::sgi::castToEnumValueString<sgi::GLConstant>(bind) << "</li>";
                os << "<li>" << name << ":" << bind << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

            osgEarth::VirtualProgram::ShaderMap shaderMap;
            access->getShaderMap(shaderMap);
            os << "<tr><td>shader map</td><td>" << shaderMap.size() << " shaders</td></tr>" << std::endl;

            osgEarth::ShaderComp::FunctionLocationMap functions;
            access->getFunctions(functions);
            os << "<tr><td>functions</td><td>" << functions.size() << " functions</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramShaderMap:
        {
            os << "<ul>";
            osgEarth::VirtualProgram::ShaderMap shaderMap;
            access->getShaderMap(shaderMap);
            for(osgEarth::VirtualProgram::ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); it++)
            {
                const std::string & name = it->first;
                const osgEarth::VirtualProgram::ShaderEntry & entry = it->second;
                os << "<li>" << name << "</li>";
            }
            os << "</ul>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramFunctions:
        {
            osgEarth::ShaderComp::FunctionLocationMap functions;
            access->getFunctions(functions);
            os << "<ul>";
            for(osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.begin(); it != functions.end(); it++)
            {
                const osgEarth::ShaderComp::FunctionLocation & location = it->first;
                const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                os << "<li>";
                os << location << "<ul>";
                for(osgEarth::ShaderComp::OrderedFunctionMap::const_iterator it = orderedFunctions.begin(); it != orderedFunctions.end(); it++)
                {
                    const float & order = it->first;
                    /*
                    const std::string & function = it->second;
                    os << "<li>Order=" << order << " function=" << function << "</li>";
                    */
                }
                os << "</ul></li>";
            }
            os << "</ul>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::TileBlacklist>::process(std::basic_ostream<char>& os)
{
    osgEarth::TileBlacklist * object = dynamic_cast<osgEarth::TileBlacklist*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining TileSource properties
            os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;
            os << "<tr><td>tiles</td><td><ul>";
            std::stringstream oss;
            object->write(oss);
            std::string tilelist = oss.str();
            std::string::iterator it = tilelist.begin();
            std::string::iterator lastpos = it;
            while(it != tilelist.end())
            {
                if(*it == '\n')
                {
                    os << "<li>" << std::string(lastpos, it) << "</li>";
                    lastpos = it;
                }
                it++;
            }
            if(lastpos != tilelist.end())
                os << "<li>" << std::string(lastpos, tilelist.end()) << "</li>";
            os << "</ul></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<ElevationQueryReferenced>::process(std::basic_ostream<char>& os)
{
    ElevationQueryReferenced * query_ref = getObject<ElevationQueryReferenced, SGIItemOsg>();
    osgEarth::ElevationQuery* object = query_ref->get();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining ElevationQuery properties
            os << "<tr><td>maxTilesToCache</td><td>" << object->getMaxTilesToCache() << "</td></tr>" << std::endl;
            os << "<tr><td>maxLevelOverride</td><td>" << object->getMaxLevelOverride() << "</td></tr>" << std::endl;
            os << "<tr><td>avg query time</td><td>" << object->getAverageQueryTime() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeTileCache:
        {
            ElevationQueryAccess * access = (ElevationQueryAccess*)object;
            const ElevationQueryAccess::TileCache & tileCache = access->tileCache();
            const osgEarth::CacheStats stats = tileCache.getStats();

            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>entries</td><td>" << stats._entries << "</td></tr>" << std::endl;
            os << "<tr><td>maxEntries</td><td>" << stats._maxEntries << "</td></tr>" << std::endl;
            os << "<tr><td>hitRatio</td><td>" << stats._hitRatio<< "</td></tr>" << std::endl;
            os << "<tr><td>numQueries</td><td>" << stats._queries << "</td></tr>" << std::endl;
            os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeTileCacheMap:
        {
            ElevationQueryAccess * access = (ElevationQueryAccess*)object;
            const ElevationQueryAccess::TileCache & tileCache = access->tileCache();

            os << "<ul>";
            for(ElevationQueryAccess::TileCache::map_const_iter it = tileCache.map().begin(); it != tileCache.map().end(); it++)
            {
                const osgEarth::TileKey & tilekey = it->first;
                const osg::ref_ptr<osg::HeightField> & hf = it->second.first;
                os << "<li>" << tilekey << ": " << (void*)hf.get() << "</li>" << std::endl;
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeTileCacheLRU:
        {
            ElevationQueryAccess * access = (ElevationQueryAccess*)object;
            const ElevationQueryAccess::TileCache & tileCache = access->tileCache();

            os << "<ol>";
            for(ElevationQueryAccess::TileCache::lru_const_iter it = tileCache.lru().begin(); it != tileCache.lru().end(); it++)
            {
                const osgEarth::TileKey & tilekey = *it;
                os << "<li>" << tilekey << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Util::Controls::ControlContext & object)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>view</td><td>" << (void*)object._view << "</td></tr>" << std::endl;
    os << "<tr><td>viewport</td><td>" << (void*)object._vp.get() << "</td></tr>" << std::endl;
    os << "<tr><td>viewContextID</td><td>" << object._viewContextID << "</td></tr>" << std::endl;
    os << "<tr><td>frameStamp</td><td>" << object._frameStamp << "</td></tr>" << std::endl;

    os << "<tr><td>active</td><td>" << object._active.size() << " controls</td></tr>" << std::endl;
    //typedef std::queue< osg::ref_ptr<osgEarth::Util::Controls::Control> > ControlQueue;
    os << "</table>" << std::endl;
    return os;
}

bool writePrettyHTMLImpl<osgEarth::Util::Controls::ControlCanvas>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::ControlCanvas * object = static_cast<osgEarth::Util::Controls::ControlCanvas*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Camera properties first
            callNextHandler(os);

            ControlCanvasAccess* access = (ControlCanvasAccess*)object;
            // add remaining ControlCanvas properties
            os << "<tr><td>controlContext</td><td>";
            os << access->controlContext();
            os << "</td></tr>" << std::endl;

            os << "<tr><td>isContextDirty</td><td>" << (access->isContextDirty()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isUpdatePending</td><td>" << (access->isUpdatePending()?"true":"false") << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Util::Controls::Control::Side & t)
{
    switch(t)
    {
    case osgEarth::Util::Controls::Control::SIDE_TOP: os << "top"; break;
    case osgEarth::Util::Controls::Control::SIDE_BOTTOM: os << "bottom"; break;
    case osgEarth::Util::Controls::Control::SIDE_LEFT: os << "left"; break;
    case osgEarth::Util::Controls::Control::SIDE_RIGHT: os << "right"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Util::Controls::Control::Dock & t)
{
    switch(t)
    {
    case osgEarth::Util::Controls::Control::DOCK_NONE: os << "none"; break;
    case osgEarth::Util::Controls::Control::DOCK_TOP: os << "top"; break;
    case osgEarth::Util::Controls::Control::DOCK_BOTTOM: os << "bottom"; break;
    case osgEarth::Util::Controls::Control::DOCK_LEFT: os << "left"; break;
    case osgEarth::Util::Controls::Control::DOCK_RIGHT: os << "right"; break;
    case osgEarth::Util::Controls::Control::DOCK_FILL: os << "fill"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Util::Controls::Control::Alignment & t)
{
    switch(t)
    {
    case osgEarth::Util::Controls::Control::ALIGN_NONE: os << "none"; break;
    case osgEarth::Util::Controls::Control::ALIGN_TOP: os << "top"; break;
    case osgEarth::Util::Controls::Control::ALIGN_BOTTOM: os << "bottom"; break;
    case osgEarth::Util::Controls::Control::ALIGN_LEFT: os << "left"; break;
    case osgEarth::Util::Controls::Control::ALIGN_RIGHT: os << "right"; break;
    case osgEarth::Util::Controls::Control::ALIGN_CENTER: os << "center"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Util::Controls::Gutter & t)
{
    return os << "{left=" << t.left() << ";top=" << t.top() << ";right=" << t.right() << ";bottom=" << t.bottom() << "}";
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Angular & a)
{
    static osg::ref_ptr<osgEarth::Util::LatLongFormatter> s_formater(new osgEarth::Util::LatLongFormatter);
    return os << s_formater->format(a);
}

bool writePrettyHTMLImpl<osgEarth::Util::Controls::Control>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::Control * object = static_cast<osgEarth::Util::Controls::Control*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            // add remaining Control properties
            os << "<tr><td>dirty</td><td>" << (object->isDirty()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>renderPos</td><td>" << object->renderPos() << "</td></tr>" << std::endl;
            os << "<tr><td>renderSize</td><td>" << object->renderSize() << "</td></tr>" << std::endl;

            os << "<tr><td>x</td><td>" << object->x() << "</td></tr>" << std::endl;
            os << "<tr><td>y</td><td>" << object->y() << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << object->width() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->height() << "</td></tr>" << std::endl;

            os << "<tr><td>margin</td><td>" << object->margin() << "</td></tr>" << std::endl;
            os << "<tr><td>padding</td><td>" << object->padding() << "</td></tr>" << std::endl;
            os << "<tr><td>borderWidth</td><td>" << object->borderWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>opacity</td><td>" << object->getOpacity() << "</td></tr>" << std::endl;

            os << "<tr><td>vertAlign</td><td>" << object->vertAlign() << "</td></tr>" << std::endl;
            os << "<tr><td>horizAlign</td><td>" << object->horizAlign() << "</td></tr>" << std::endl;

            os << "<tr><td>horizFill</td><td>" << (object->horizFill()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>vertFill</td><td>" << (object->horizFill()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>visible</td><td>" << (object->visible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>parentIsVisible</td><td>" << (object->parentIsVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>active</td><td>" << (object->getActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>absorbEvents</td><td>" << (object->getAbsorbEvents()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>foreColor</td><td>" << object->foreColor() << "</td></tr>" << std::endl;
            os << "<tr><td>backColor</td><td>" << object->foreColor() << "</td></tr>" << std::endl;
            os << "<tr><td>activeColor</td><td>" << object->foreColor() << "</td></tr>" << std::endl;
            os << "<tr><td>borderColor</td><td>" << object->foreColor() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeEventHandlers:
        {
            ControlAccess * access = (ControlAccess *)object;
            const osgEarth::Util::Controls::ControlEventHandlerList & eventHandlers = access->eventHandlers();
            os << "<ul>";
            for(osgEarth::Util::Controls::ControlEventHandlerList::const_iterator it = eventHandlers.begin(); it != eventHandlers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::Util::Controls::ControlEventHandler> evthandler = *it;
                os << "<li>" << getObjectName(evthandler.get()) << " (" << getObjectTypename(evthandler.get()) << ")</li>" << std::endl;
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Util::Controls::ControlEventHandler>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::ControlEventHandler * object = getObject<osgEarth::Util::Controls::ControlEventHandler,SGIItemOsg>();
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

bool writePrettyHTMLImpl<osgEarth::Util::Controls::Container>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::Container * object = static_cast<osgEarth::Util::Controls::Container*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Control properties first
            callNextHandler(os);

            // add remaining Container properties
            os << "<tr><td>childSpacing</td><td>" << object->childSpacing() << "</td></tr>" << std::endl;
            os << "<tr><td>childHorizAlign</td><td>" << object->childHorizAlign() << "</td></tr>" << std::endl;
            os << "<tr><td>childVertAlign</td><td>" << object->childVertAlign() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Util::Controls::LabelControl>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::LabelControl * object = static_cast<osgEarth::Util::Controls::LabelControl*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Control properties first
            callNextHandler(os);

            // add remaining LabelControl properties
            os << "<tr><td>text</td><td>" << ((object)?object->text():"&lt;null&gt;") << "</td></tr>" << std::endl;
            os << "<tr><td>encoding</td><td>" << ((object)?object->encoding():osgText::String::ENCODING_UNDEFINED) << "</td></tr>" << std::endl;

            os << "<tr><td>font</td><td>" << getObjectNameAndType(object->font()) << "</td></tr>" << std::endl;
            os << "<tr><td>fontSize</td><td>" << ((object)?object->fontSize():0.0f) << "</td></tr>" << std::endl;
            os << "<tr><td>haloColor</td><td>" << ((object)?object->haloColor():osgEarth::optional<osg::Vec4f>()) << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropType</td><td>" << ((object)?object->getTextBackdropType():osgText::Text::NONE) << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropImplementation</td><td>" << ((object)?object->getTextBackdropImplementation():osgText::Text::POLYGON_OFFSET) << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropOffset</td><td>" << ((object)?object->getTextBackdropOffset():0.0f) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Util::Controls::ImageControl>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::ImageControl * object = static_cast<osgEarth::Util::Controls::ImageControl*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Control properties first
            callNextHandler(os);

            // add remaining ImageControl properties
            os << "<tr><td>rotation</td><td>" << object->getRotation() << "</td></tr>" << std::endl;
            os << "<tr><td>image</td><td>" << getObjectNameAndType(object->getImage()) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Util::Controls::ControlNodeBin>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::ControlNodeBin * object = static_cast<osgEarth::Util::Controls::ControlNodeBin*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            // add remaining ControlNodeBin properties

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

bool writePrettyHTMLImpl<osgEarth::Util::Controls::ControlNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::Controls::ControlNode * object = static_cast<osgEarth::Util::Controls::ControlNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::Node properties first
            callNextHandler(os);

            // add remaining ControlNode properties
            os << "<tr><td>priority</td><td>" << object->getPriority() << "</td></tr>" << std::endl;
            os << "<tr><td>anchorPoint</td><td>" << object->anchorPoint() << "</td></tr>" << std::endl;

            os << "<tr><td>views</td><td>";
            ((ControlNodeAccess*)object)->appendAllViews(os);
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

bool writePrettyHTMLImpl<osgEarth::Util::SkyNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::SkyNode * object = static_cast<osgEarth::Util::SkyNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            // add remaining SkyNode properties
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>referencePoint</td><td>" << object->getReferencePoint() << "</td></tr>" << std::endl;
            os << "<tr><td>lighting</td><td>" << object->getLighting() << "</td></tr>" << std::endl;
            os << "<tr><td>dateTime</td><td>" << object->getDateTime() << "</td></tr>" << std::endl;
            os << "<tr><td>sunVisible</td><td>" << (object->getSunVisible()?"true":"false") << "</td></tr>" << std::endl;
#else
            osgEarth::DateTime t;
            object->getDateTime(t);
            os << "<tr><td>dateTime</td><td>" << t << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>moonVisible</td><td>" << (object->getMoonVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>starsVisible</td><td>" << (object->getStarsVisible()?"true":"false") << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Util::AutoClipPlaneCullCallback>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            AutoClipPlaneCullCallbackAccessor * access = (AutoClipPlaneCullCallbackAccessor*)object;

            os << std::setprecision(12) << std::fixed;
            os << "<tr><td>active</td><td>" << (access->isActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>minNearFarRatio</td><td>"  << object->getMinNearFarRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>maxNearFarRatio</td><td>" << object->getMaxNearFarRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>heightThreshold</td><td>" << object->getHeightThreshold() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumRadius</td><td>" << access->minimumRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>clampFarClipPlane</td><td>" << (object->getClampFarClipPlane()?"true":"false") << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Features::FeatureModelSource>::process(std::basic_ostream<char>& os)
{
    osgEarth::Features::FeatureModelSource * object = getObject<osgEarth::Features::FeatureModelSource, SGIItemOsg>();
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


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Annotation::Style& style)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>name</td><td>" << style.getName() << "</td></tr>" << std::endl;
    os << "<tr><td>empty</td><td>" << (style.empty()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>config</td><td><pre>" << style.getConfig().toJSON(true) << "</pre></td></tr>" << std::endl;
    os << "</table>" << std::endl;
    return os;
}

bool writePrettyHTMLImpl<osgEarth::Annotation::AnnotationNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::AnnotationNode * object = getObject<osgEarth::Annotation::AnnotationNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>decoration</td><td>" << object->getDecoration() << "</td></tr>" << std::endl;
            os << "<tr><td>style</td><td>" << object->getStyle() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::PositionedAnnotationNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::PositionedAnnotationNode * object = getObject<osgEarth::Annotation::PositionedAnnotationNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>position</td><td>" << object->getPosition() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::FeatureNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::FeatureNode * object = getObject<osgEarth::Annotation::FeatureNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            //os << "<tr><td>feature</td><td>" << object->getFeature() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::LocalizedNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::LocalizedNode * object = getObject<osgEarth::Annotation::LocalizedNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>local offset</td><td>" << object->getLocalOffset() << "</td></tr>" << std::endl;
            os << "<tr><td>local rotation</td><td>" << object->getLocalRotation() << "</td></tr>" << std::endl;
            os << "<tr><td>scale</td><td>" << object->getScale() << "</td></tr>" << std::endl;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            os << "<tr><td>horizon culling</td><td>" << (object->getHorizonCulling()?"true":"false") << "</td></tr>" << std::endl;
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

bool writePrettyHTMLImpl<osgEarth::Annotation::OrthoNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::OrthoNode * object = getObject<osgEarth::Annotation::OrthoNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>local offset</td><td>" << object->getLocalOffset() << "</td></tr>" << std::endl;
            os << "<tr><td>horizon culling</td><td>" << (object->getHorizonCulling()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>occlusion culling</td><td>" << (object->getOcclusionCulling()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>occlusion culling max alt</td><td>" << object->getOcclusionCullingMaxAltitude() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::PlaceNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::PlaceNode * object = getObject<osgEarth::Annotation::PlaceNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>text</td><td>" << object->getText() << "</td></tr>" << std::endl;
            os << "<tr><td>icon image</td><td>" << getObjectNameAndType(object->getIconImage()) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::LabelNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::LabelNode * object = getObject<osgEarth::Annotation::LabelNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>text</td><td>" << object->text() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::TrackNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::TrackNode * object = getObject<osgEarth::Annotation::TrackNode, SGIItemOsg>();
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

bool writePrettyHTMLImpl<osgEarth::Annotation::CircleNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::CircleNode * object = getObject<osgEarth::Annotation::CircleNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>arc start</td><td>" << object->getArcStart() << "</td></tr>" << std::endl;
            os << "<tr><td>arc end</td><td>" << object->getArcEnd() << "</td></tr>" << std::endl;
            os << "<tr><td>num segments</td><td>" << object->getNumSegments() << "</td></tr>" << std::endl;
            os << "<tr><td>pie</td><td>" << (object->getPie()?"true":"false") << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::EllipseNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::EllipseNode * object = getObject<osgEarth::Annotation::EllipseNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>radius major</td><td>" << object->getRadiusMajor() << "</td></tr>" << std::endl;
            os << "<tr><td>radius minor</td><td>" << object->getRadiusMinor() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation angle</td><td>" << object->getRotationAngle() << "</td></tr>" << std::endl;
            os << "<tr><td>arc start</td><td>" << object->getArcStart() << "</td></tr>" << std::endl;
            os << "<tr><td>arc end</td><td>" << object->getArcEnd() << "</td></tr>" << std::endl;
            os << "<tr><td>num segments</td><td>" << object->getNumSegments() << "</td></tr>" << std::endl;
            os << "<tr><td>pie</td><td>" << (object->getPie()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>draped</td><td>" << (object->isDraped()?"true":"false") << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::RectangleNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::RectangleNode * object = getObject<osgEarth::Annotation::RectangleNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>width</td><td>" << object->getWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->getHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>upper left</td><td>" << object->getUpperLeft() << "</td></tr>" << std::endl;
            os << "<tr><td>upper right</td><td>" << object->getUpperRight() << "</td></tr>" << std::endl;
            os << "<tr><td>lower left</td><td>" << object->getLowerLeft() << "</td></tr>" << std::endl;
            os << "<tr><td>lower right</td><td>" << object->getLowerRight() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Annotation::ModelNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::Annotation::ModelNode * object = getObject<osgEarth::Annotation::ModelNode, SGIItemOsg>();
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


#if 0

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Config & cfg, const std::string & parentReferrer, bool table)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

    os << "<tr><td>" << cfg.key() << "</td><td>";
    os << cfg.value();
    if(!cfg.referrer().empty() && cfg.referrer() != parentReferrer)
        os << " [" << cfg.referrer() << "]";

    const osgEarth::ConfigSet& childs = cfg.children();
    if(!childs.empty())
    {
        os << "<br/><table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
        for(osgEarth::ConfigSet::const_iterator it = childs.begin(); it != childs.end(); it++)
        {
            const osgEarth::Config & child = *it;
            writePrettyHTML(os, child, cfg.referrer(), false);
        }
        os << "</table>" << std::endl;
    }
    os << "</td></tr>" << std::endl;

    if(_table)
        os << "</table>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::CachePolicy & opts, bool table)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    writePrettyHTML(os, (const osgEarth::DriverConfigOptions&)opts, false);

    os << "<tr><td>usage</td><td>" << opts.usageString() << "</td></tr>" << std::endl;
    os << "<tr><td>maxAge</td><td>" << opts.maxAge() << "</td></tr>" << std::endl;
    os << "<tr><td>minTime</td><td>" << opts.minTime() << "</td></tr>" << std::endl;

    if(_table)
        os << "</table>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const ElevationQueryRef * object, bool table)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

    // add Referenced properties first
    writePrettyHTML(os, (const osg::Referenced*)object, false);

    // add remaining ElevationQueryRef properties
    os << "<tr><td>maxTilesToCache</td><td>" << ((object && object->valid())?(*object)->getMaxTilesToCache():0) << "</td></tr>" << std::endl;
    os << "<tr><td>maxLevelOverride</td><td>" << ((object && object->valid())?(*object)->getMaxLevelOverride():0) << "</td></tr>" << std::endl;

    if(_table)
        os << "</table>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Picker::Hit & hit, bool table, bool brief)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

    os << "<tr><td>ratio</td><td>" << hit.ratio << "</td></tr>" << std::endl;
    os << "<tr><td>nodePath</td><td>";
    if(brief)
        os << hit.nodePath.size() << " elements";
    else
        writePrettyHTML(os, hit.nodePath);
    os << "</td></tr>" << std::endl;

    os << "<tr><td>drawable</td><td>";
    if(brief)
    {
        QString name, nodetype;
        sgi_getObjectDisplayName(hit.drawable.get(), name);
        sgi_getObjectTypename(hit.drawable.get(), nodetype, true);
        os << name << " (" << nodetype << ")" << std::endl;
    }
    else
        writePrettyHTML(os, hit.drawable.get());
    os << "</td></tr>" << std::endl;
    os << "<tr><td>matrix</td><td>";
    if(!brief)
    {
        if(hit.matrix.valid())
            writePrettyHTML(os, *hit.matrix.get(), MatrixUsageTypeGeneric, hit.drawable.get());
        else
            os << "&lt;none&gt;";
    }
    os << "</td></tr>" << std::endl;
    os << "<tr><td>localIntersectionPoint</td><td>" << hit.getLocalIntersectPoint() << "</td></tr>" << std::endl;
    os << "<tr><td>worldIntersectionPoint</td><td>";
    writePrettyPositionHTML(os, hit.getWorldIntersectPoint(), hit.drawable.get());
    os << "</td></tr>" << std::endl;
    os << "<tr><td>localIntersectionNormal</td><td>" << hit.getLocalIntersectNormal() << "</td></tr>" << std::endl;
    os << "<tr><td>worldIntersectionNormal</td><td>" << hit.getWorldIntersectNormal() << "</td></tr>" << std::endl;

    os << "<tr><td>indexList</td><td>";
    if(!brief)
    {
        os << "<ol>";
        for(osgEarth::Picker::Hit::IndexList::const_iterator it = hit.indexList.begin(); it != hit.indexList.end(); it++)
        {
            os << "<li>" << *it << "</li>";
        }
        os << "</ol>";
    }
    os << "</td></tr>" << std::endl;
    os << "<tr><td>ratioList</td><td>";
    if(!brief)
    {
        os << "<ol>";
        for(osgEarth::Picker::Hit::RatioList::const_iterator it = hit.ratioList.begin(); it != hit.ratioList.end(); it++)
        {
            os << "<li>" << *it << "</li>";
        }
        os << "</ol>";
    }
    os << "</td></tr>" << std::endl;
    os << "<tr><td>primitiveIndex</td><td>" << hit.primitiveIndex << "</td></tr>" << std::endl;

    if(_table)
        os << "</table>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Picker::Hits & hits, bool table, bool brief)
{
    if(_table)
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

    unsigned num = 0;
    for(osgEarth::Picker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, num++)
    {
        const osgEarth::Picker::Hit & hit = *it;
        os << "<tr><td>" << num << "</td><td>";
        writePrettyHTML(os, hit, true, brief);
        os << "</td></tr>" << std::endl;
    }

    if(_table)
        os << "</table>" << std::endl;
}


#endif // 0

} // namespace osgearth_plugin
} // namespace sgi
