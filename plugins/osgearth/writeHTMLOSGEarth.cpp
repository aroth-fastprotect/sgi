#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>
#include <sgi/helpers/osg>
#include <sgi/helpers/string>

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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarth/LandCoverLayer>
#include <osgEarth/PatchLayer>
#include <osgEarth/VideoLayer>
#include <osgEarthUtil/FlatteningLayer>
#include <osgEarthUtil/FractalElevationLayer>
#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/GARSGraticule>
#include <osgEarthUtil/MGRSGraticule>
#include <osgEarthUtil/UTMGraticule>
#include <osgEarthUtil/MultiElevationLayer>
#include <osgEarthUtil/SimpleOceanLayer>
#endif

#include <osgEarth/ElevationQuery>
#include <osgEarth/ImageUtils>
#ifdef SGI_USE_OSGEARTH_FAST
#include <osgEarth/LevelDBFactory>
#endif
#include <osgEarth/TraversalData>

#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/RTTPicker>

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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarthAnnotation/GeoPositionNode>
#include <osgEarth/TimeControl>
#endif

#include "SGIItemOsgEarth"

#include "osgearth_accessor.h"
#include "ElevationQueryReferenced"
#include "geo_helpers.h"

namespace sgi {
namespace osgearth_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Registry)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Capabilities)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::StateSetCache)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Cache)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::CacheBin)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Layer)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::VisibleLayer)
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ImageLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::MaskLayer)

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::LandCoverLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::PatchLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::VideoLayer)
//WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::FlatteningLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::FractalElevationLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::GeodeticGraticule)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::GARSGraticule)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::MGRSGraticule)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::UTMGraticule)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::MultiElevationLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::SimpleOceanLayer)
#endif

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Extension)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationPool)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationEnvelope)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::PolyShader)
#endif
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Terrain)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainEngineNode)
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainDecorator)
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::OverlayDecorator)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::SpatialReference)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Profile)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::TileSource)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(TileSourceInfo)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelSource)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::VirtualProgram)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::TileBlacklist)
#ifdef SGI_USE_OSGEARTH_FAST
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::LevelDBDatabase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::LODScaleOverrideNode)
#endif

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ElevationQueryReferenced)


WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(TileKeyReferenced)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(TileSourceTileKey)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlCanvas)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Control)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlEventHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Container)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlNodeBin)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::LabelControl)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ImageControl)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::SkyNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::AutoClipPlaneCullCallback)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Picker)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::RTTPicker)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureProfile)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureModelSource)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureSource)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::AnnotationNode)
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::PositionedAnnotationNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::LocalizedNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::OrthoNode)
#endif
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::FeatureNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::PlaceNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::LabelNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::TrackNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::CircleNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::EllipseNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::RectangleNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::ModelNode)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Image)

using namespace osg_helpers;

extern void writePrettyHTMLImplForDriverOptions(SGIPluginHostInterface * hostInterface, std::basic_ostream<char>& os, const osgEarth::DriverConfigOptions & opts);

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t);

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Status & s)
{
    return os << s.toString();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Layer::RenderType & t)
{
    switch(t)
    {
    case osgEarth::Layer::RENDERTYPE_NONE: os << "none"; break;
#if OSGEARTH_VERSION_GREATER_THAN(2,9,0)
    case osgEarth::Layer::RENDERTYPE_TERRAIN_SURFACE: os << "surface"; break;
    case osgEarth::Layer::RENDERTYPE_TERRAIN_PATCH: os << "patch"; break;
#else
    case osgEarth::Layer::RENDERTYPE_TILE: os << "tile"; break;
    case osgEarth::Layer::RENDERTYPE_PATCH: os << "patch"; break;
#endif
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osgEarth::Registry>::process(std::basic_ostream<char>& os)
{
    osgEarth::Registry * object = static_cast<osgEarth::Registry*>(item<SGIItemOsg>()->object());
	RegistryAccess * access = static_cast<RegistryAccess*>(object);
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
            os << "<tr><td>overrideCachePolicy</td><td>" << object->overrideCachePolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>defaultCachePolicy</td><td>" << object->defaultCachePolicy() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            os << "<tr><td>defaultCacheDriverName</td><td>" << object->getDefaultCacheDriverName() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>defaultTerrainEngineDriverName</td><td>" << object->getDefaultTerrainEngineDriverName() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            os << "<tr><td>overrideTerrainEngineDriverName</td><td>" << object->overrideTerrainEngineDriverName() << "</td></tr>" << std::endl;
            os << "<tr><td>unRefImageDataAfterApply</td><td>" << object->unRefImageDataAfterApply() << "</td></tr>" << std::endl;
            {
                os << "<tr><td>offLimitsTextureImageUnits</td><td>";
                const auto & s = object->getOffLimitsTextureImageUnits();
                if(s.empty())
                    os << "<i>empty</i>";
                else
                {
                    os << "<ul>";
                    for(const int & unit : s)
                        os << "<li>" << unit << "</li>";
                    os << "</ul>";
                }
                os << "</td></tr>" << std::endl;
            }
            os << "<tr><td>devicePixelRatio</td><td>" << object->getDevicePixelRatio() << "</td></tr>" << std::endl;
#endif

            os << "<tr><td>HTTP user agent</td><td>" << osgEarth::HTTPClient::getUserAgent() << "</td></tr>" << std::endl;
            os << "<tr><td>HTTP timeout</td><td>" << osgEarth::HTTPClient::getTimeout() << "</td></tr>" << std::endl;
            os << "<tr><td>HTTP connect timeout</td><td>" << osgEarth::HTTPClient::getConnectTimeout() << "</td></tr>" << std::endl;
#ifdef SGI_USE_OSGEARTH_FAST
            os << "<tr><td>HTTP proxy</td><td>" << osgEarth::HTTPClient::getProxySettings() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>HTTP URL rewriter</td><td>" << osgEarth::HTTPClient::getURLRewriter() << "</td></tr>" << std::endl;

			os << "<tr><td>unRefImageDataAfterApply</td><td>" << object->unRefImageDataAfterApply() << "</td></tr>" << std::endl;
			os << "<tr><td>blacklist filenames</td><td>" << access->numBlacklistFilenames() << " entries</td></tr>" << std::endl;
			os << "<tr><td>activities</td><td>";

#ifdef SGI_USE_OSGEARTH_FAST
			{
				std::vector<osgEarth::LevelDBDatabasePtr> databases;
				osgEarth::LevelDBFactory::getActiveDatabases(databases);
				os << "<tr><td>LevelDB databases</td><td>" << databases.size() << " entries</td></tr>" << std::endl;
			}
#endif
			
			std::set<std::string> activities;
			object->getActivities(activities);
			if (activities.empty())
				os << "<i>none</i>";
			else
			{
				os << "<ul>";
				for (const std::string & activity : activities)
				{
					os << "<li>" << activity << "</li>";
				}
				os << "</ul>";
			}
			os << "</td></tr>" << std::endl;
			

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
	case SGIItemTypeBlacklist:
		{
			std::set<std::string> filenames;
			access->getBlacklistFilenames(filenames);
			os << filenames.size() << " entries<br/>" << std::endl;
			os << "<ul>";
			for (const std::string & filename : filenames)
			{
				os << "<li>" << filename << "</li>" << std::endl;
			}
			os << "</ul>" << std::endl;

			ret = true;
		}
		break;
#ifdef SGI_USE_OSGEARTH_FAST
    case SGIItemTypeDatabases:
		{
            osgEarth::LevelDBDatabasePairList databases;
			osgEarth::LevelDBFactory::getDatabases(databases, false);
			os << databases.size() << " databases<br/>" << std::endl;
			os << "<ul>";
			for (const osgEarth::LevelDBDatabasePair & pair : databases)
			{
                os << "<li>" << pair.first << ": ";
                if (pair.second.valid())
                {
                    os << (void*)pair.second->_db << " " << pair.second->created();
                }
                else
                {
                    os << "NULL";
                }
                os << "</li>" << std::endl;
			}
			os << "</ul>" << std::endl;

			ret = true;
        }
		break;
#endif
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
            os << "<tr><td>ellipsoid</td><td>" << getObjectNameAndType(object->getEllipsoid()) << "</td></tr>" << std::endl;

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
        os << "(null)";
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
        os << "(null)";
    os << "</td></tr>" << std::endl;
    os << std::setprecision(12);
    os << "<tr><td>north</td><td>" << object.north() << "</td></tr>" << std::endl;
    os << "<tr><td>south</td><td>" << object.south() << "</td></tr>" << std::endl;
    os << "<tr><td>east</td><td>" << object.east() << "</td></tr>" << std::endl;
    os << "<tr><td>west</td><td>" << object.west() << "</td></tr>" << std::endl;
    os << "<tr><td>width</td><td>" << object.width() << "</td></tr>" << std::endl;
    os << "<tr><td>height</td><td>" << object.height() << "</td></tr>" << std::endl;
    os << "<tr><td>geocirle</td><td>";
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    writePrettyHTML(os, object.getBoundingGeoCircle());
#else
    writePrettyHTML(os, object.computeBoundingGeoCircle());
#endif
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
                os << "(null)";
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
    LayerAccessor * object = static_cast<LayerAccessor*>(getObject<osgEarth::Layer,SGIItemOsg>());
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
            os << "<tr><td>uid</td><td>" << object->getUID() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>typeName</td><td>" << object->getTypeName() << "</td></tr>" << std::endl;
            os << "<tr><td>status</td><td>" << object->getStatus() << "</td></tr>" << std::endl;
            os << "<tr><td>readOptions</td><td>" << getObjectNameAndType(object->getReadOptions()) << "</td></tr>" << std::endl;
            os << "<tr><td>cacheSettings</td><td>" << getObjectNameAndType(object->getCacheSettings()) << "</td></tr>" << std::endl;
            os << "<tr><td>sequenceControl</td><td>" << object->getSequenceControl() << "</td></tr>" << std::endl;
            os << "<tr><td>extent</td><td>" << object->getExtent() << "</td></tr>" << std::endl;
            os << "<tr><td>cacheId</td><td>" << object->getCacheID() << "</td></tr>" << std::endl;
            os << "<tr><td>stateSet</td><td>" << getObjectNameAndType(object->getStateSet()) << "</td></tr>" << std::endl;
            os << "<tr><td>renderType</td><td>" << object->getRenderType() << "</td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>layer callbacks</td><td><ul>" << std::endl;
            LayerAccessor::LayerCallbackList callbacks;
            object->getLayerCallbacks(callbacks);
            for (LayerAccessor::LayerCallbackList::const_iterator it = callbacks.begin(); it != callbacks.end(); it++)
            {
                const osg::ref_ptr<osgEarth::LayerCallback> & callback = *it;
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
bool writePrettyHTMLImpl<osgEarth::VisibleLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::VisibleLayer * object = getObject<osgEarth::VisibleLayer,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            os << "<tr><td>visible</td><td>" << (object->getVisible()?"true":"false") << "</td></tr>" << std::endl;
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
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)


bool writePrettyHTMLImpl<osgEarth::TerrainLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::TerrainLayer * object = getObject<osgEarth::TerrainLayer,SGIItemOsg>();
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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            os << "<tr><td>tileSourceInitAttempted</td><td>" << (access->tileSourceInitAttempted()?"true":"false") << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            os << "<tr><td>tileSourceInitFailed</td><td>" << (access->tileSourceInitFailed()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>tileSize</td><td>" << access->tileSize() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>visible</td><td>" << (object->getVisible()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>profile</td><td>";
            if(profile)
                os << profile->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>targetProfileHint</td><td>";
            if(targetProfileHint)
                os << targetProfileHint->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>dynamic</td><td>" << (object->isDynamic()?"true":"false") << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>cacheOnly</td><td>" <<  (object->isCacheOnly()?"true":"false") << "</td></tr>" << std::endl;
#else
            os << "<tr><td>dataExtents</td><td>" << object->getDataExtents().size() << " entries" << "</td></tr>" << std::endl;
            os << "<tr><td>dataExtentsUnion</td><td>" << object->getDataExtentsUnion() << "</td></tr>" << std::endl;
            os << "<tr><td>nodata value</td><td>" << object->getNoDataValue() << "</td></tr>" << std::endl;
            os << "<tr><td>min valid value</td><td>" << object->getMinValidValue() << "</td></tr>" << std::endl;
            os << "<tr><td>max valid value</td><td>" << object->getMaxValidValue() << "</td></tr>" << std::endl;
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

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            os << "<tr><td>LOD blending</td><td>" <<  (object->isLODBlendingEnabled()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>minVisibleRange</td><td>" << object->getMinVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>maxVisibleRange</td><td>" << object->getMaxVisibleRange() << "</td></tr>" << std::endl;
            os << "<tr><td>isShared</td><td>" <<  (object->isShared()?"true":"false") << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>isCoverage</td><td>" <<  (object->isCoverage()?"true":"false") << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>shareImageUnit</td><td>" <<  object->shareImageUnit() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,8,0)
            os << "<tr><td>shareTexUniformName</td><td>" <<  object->shareTexUniformName() << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>shareTexMatUniformName</td><td>" <<  object->shareTexMatUniformName() << "</td></tr>" << std::endl;
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>suggestCacheFormat</td><td>" << access->suggestCacheFormatStr() << "</td></tr>" << std::endl;
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>driver</td><td>" <<  object->getModelLayerOptions().driver().value().getDriver() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>visible</td><td>" << (object->getVisible()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>lightingEnabled</td><td>" << (object->isLightingEnabled()?"true":"false") << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && OSGEARTH_VERSION_LESS_THAN(2,10,0)
            os << "<tr><td>isTerrainPatch</td><td>" << (object->isTerrainPatch()?"true":"false") << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            os << "<tr><td>maskMinLevel</td><td>" << object->getMaskMinLevel() << "</td></tr>" << std::endl;
#endif
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
    osgEarth::MaskLayer * object = getObject<osgEarth::MaskLayer,SGIItemOsg>();
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)

bool writePrettyHTMLImpl<osgEarth::PatchLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::LandCoverLayer * object = getObject<osgEarth::LandCoverLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::LandCoverLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::LandCoverLayer * object = getObject<osgEarth::LandCoverLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::VideoLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::VideoLayer * object = getObject<osgEarth::VideoLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            os << "<tr><td>texture</td><td>" << getObjectNameAndType(object->getTexture()) << "</td></tr>" << std::endl;

            if (_table)
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
bool writePrettyHTMLImpl<osgEarth::Util::FlatteningLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::FlatteningLayer * object = getObject<osgEarth::Util::FlatteningLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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
#endif

bool writePrettyHTMLImpl<osgEarth::Util::FractalElevationLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::FractalElevationLayer * object = getObject<osgEarth::Util::FractalElevationLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::GeodeticGraticule>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::GeodeticGraticule * object = getObject<osgEarth::Util::GeodeticGraticule, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::GARSGraticule>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::GARSGraticule * object = getObject<osgEarth::Util::GARSGraticule, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::MGRSGraticule>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::MGRSGraticule * object = getObject<osgEarth::Util::MGRSGraticule, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::UTMGraticule>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::UTMGraticule * object = getObject<osgEarth::Util::UTMGraticule, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::MultiElevationLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::MultiElevationLayer * object = getObject<osgEarth::Util::MultiElevationLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::Util::SimpleOceanLayer>::process(std::basic_ostream<char>& os)
{
    osgEarth::Util::SimpleOceanLayer * object = getObject<osgEarth::Util::SimpleOceanLayer, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add terrain layer properties first
            callNextHandler(os);

            os << "<tr><td>color</td><td>" << object->getColor() << "</td></tr>" << std::endl;
            os << "<tr><td>maxAltitude</td><td>" << object->getMaxAltitude() << "</td></tr>" << std::endl;

            if (_table)
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
#endif

bool writePrettyHTMLImpl<osgEarth::Map>::process(std::basic_ostream<char>& os)
{
    MapAccess * object = static_cast<MapAccess*>(getObject<osgEarth::Map,SGIItemOsg>());
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
#else
            os << "<tr><td>map name</td><td>" << object->getMapName() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>UID</td><td>" << object->getUID() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            if(object->getProfile())
                os << object->getProfile()->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>profile no vdatum</td><td>";
            if(object->getProfileNoVDatum())
                os << object->getProfileNoVDatum()->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>geocentric</td><td>" << (object->isGeocentric()?"true":"false") << "</td></tr>" << std::endl;

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            os << "<tr><td>SRS</td><td>" << getObjectNameAndType(object->getSRS()) << "</td></tr>" << std::endl;
            os << "<tr><td>World SRS</td><td>" << getObjectNameAndType(object->getWorldSRS())  << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>cache</td><td>" << getObjectNameAndType(object->getCache(), true) << "</td></tr>" << std::endl;

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#else
            {
                osgEarth::LayerVector layers;
                osgEarth::Revision rev = object->getLayers(layers);
                os << "<tr><td>revision</td><td>" << rev << "</td></tr>" << std::endl;
                os << "<tr><td>layers</td><td><ol>";
                for(osgEarth::LayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
                {
                    const osgEarth::Layer * layer = *it;
                    os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
                }
                os << "</ol></td></tr>" << std::endl;
            }

            os << "<tr><td>global options</td><td>" << getObjectNameAndType(object->getGlobalOptions()) << "</td></tr>" << std::endl;
            os << "<tr><td>read options</td><td>" << getObjectNameAndType(object->getReadOptions()) << "</td></tr>" << std::endl;
            os << "<tr><td>elevation pool</td><td>" << getObjectNameAndType(object->getElevationPool()) << "</td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeLayers:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            osgEarth::LayerVector layers;
            osgEarth::Revision rev = object->getLayers(layers);
            os << "Revision " << rev << "<br/><ol>";
            for(osgEarth::LayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::Layer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
            os << "</ol>";
#endif
            ret = true;
        }
        break;
    case SGIItemTypeImageLayers:
        {
            os << "<ol>";
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::IMAGE_LAYERS);
            const osgEarth::ImageLayerVector & layers = mapframe.imageLayers();
            for(osgEarth::ImageLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ImageLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
#endif
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeElevationLayers:
        {
            os << "<ol>";
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ELEVATION_LAYERS);
            const osgEarth::ElevationLayerVector & layers = mapframe.elevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ElevationLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
#endif
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeModelLayers:
        {
            os << "<ol>";
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MODEL_LAYERS);
            const osgEarth::ModelLayerVector & layers = mapframe.modelLayers();
            for(osgEarth::ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::ModelLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
#endif
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeMaskLayers:
        {
            os << "<ol>";
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MASK_LAYERS);
            const osgEarth::MaskLayerVector & layers = mapframe.terrainMaskLayers();
            for(osgEarth::MaskLayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osgEarth::MaskLayer * layer = *it;
                os << "<li>" << layer->getName() << "&lt;&gt;" << getObjectNameAndType(layer) << "</li>" << std::endl;
            }
#endif
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
            object->getMapCallbacks(callbacks);
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
    MapNodeAccess * object = static_cast<MapNodeAccess *>(getObject<osgEarth::MapNode, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            os << "<tr><td>map</td><td>" << getObjectNameAndType(const_cast<MapNodeAccess*>(object)->getMap()) << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>terrain</td><td>" << getObjectNameAndType(object->terrain()) << "</td></tr>" << std::endl;
            os << "<tr><td>terrain engine initialized</td><td>" << object->terrainEngineInitialized() << "</td></tr>" << std::endl;
			os << "<tr><td>terrain engine</td><td>" << getObjectNameAndType(object->terrainEngineNode(), true) << "</td></tr>" << std::endl;
			os << "<tr><td>terrain engine container</td><td>" << getObjectNameAndType(object->terrainEngineContainer(), true) << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
            os << "<tr><td>modelLayerGroup</td><td>" << getObjectNameAndType(object->getModelLayerGroup(), true) << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>overlayDecorator</td><td>" << getObjectNameAndType(const_cast<MapNodeAccess*>(object)->getOverlayDecorator(), true) << "</td></tr>" << std::endl;

			os << "<tr><td>extensions</td><td>";
			const auto & extensions = object->getExtensions();
			if (extensions.empty())
				os << "<i>none</i>";
			else
			{
				os << extensions.size() << " extensions<br/><ul>";
				for (const auto & ext : extensions)
				{
					os << "<li>" << getObjectNameAndType(ext, true) << "</li>";
				}
				os << "</ul>";
			}
			os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
	case SGIItemTypeExtensions:
		{
			const auto & extensions = object->getExtensions();
			if (extensions.empty())
				os << "<i>none</i>";
			else
			{
				os << extensions.size() << " extensions<br/><ul>";
				for (const auto & ext : extensions)
				{
					os << "<li>" << getObjectNameAndType(ext, true) << "</li>";
				}
				os << "</ul>";
			}
			ret = true;
		}
		break;
    case SGIItemTypeCullData:
        {
            osg::NodeList cameras;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            object->getCullDataCameras(cameras);
            if (itemNumber() == ~0u)
            {
                if (cameras.empty())
                    os << "<i>none</i>";
                else
                {
                    os << cameras.size() << " extensions<br/><ul>";
                    for (const auto & camera : cameras)
                    {
                        os << "<li>" << getObjectNameAndType(camera, true) << "</li>";
                    }
                    os << "</ul>";
                }
            }
            else
            {
                bool found = false;
                unsigned index = 0;
                for (const auto & camera : cameras)
                {
                    if (index == itemNumber())
                    {
                        const osgEarth::MapNodeCullData * data = object->getCullDataForCamera(static_cast<osg::Camera*>(camera.get()));
                        if (data)
                        {
                            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                            os << "<tr><td>stateSet</td><td>" << getObjectNameAndType(data->_stateSet.get()) << "</td></tr>" << std::endl;
                            os << "<tr><td>windowMatrixUniform</td><td>" << getObjectNameAndType(data->_windowMatrixUniform.get()) << "</td></tr>" << std::endl;
                            os << "<tr><td>cameraAltitude</td><td>" << data->_cameraAltitude << "</td></tr>" << std::endl;
                            os << "<tr><td>cameraAltitudeUniform</td><td>" << getObjectNameAndType(data->_cameraAltitudeUniform.get()) << "</td></tr>" << std::endl;
                            os << "</table>" << std::endl;
                            found = true;
                        }
                        break;
                    }
                    index++;
                }
                if(!found)
                    os << "<i>N/A</i>";
            }
#endif
            ret = true;
        }
        break;
	default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Extension>::process(std::basic_ostream<char>& os)
{
    osgEarth::Extension * object = getObject<osgEarth::Extension,SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        {
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
bool writePrettyHTMLImpl<osgEarth::ElevationPool>::process(std::basic_ostream<char>& os)
{
    osgEarth::ElevationPool * object = getObject<osgEarth::ElevationPool,SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>tileSize</td><td>" << object->getTileSize() << "</td></tr>" << std::endl;
            os << "<tr><td>maxEntries</td><td>" << object->getMaxEntries() << "</td></tr>" << std::endl;

            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::ElevationEnvelope>::process(std::basic_ostream<char>& os)
{
    osgEarth::ElevationEnvelope * object = getObject<osgEarth::ElevationEnvelope,SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>SRS</td><td>" << getObjectNameAndType(object->getSRS()) << "</td></tr>" << std::endl;
            os << "<tr><td>LOD</td><td>" << object->getLOD() << "</td></tr>" << std::endl;


            if (_table)
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

bool writePrettyHTMLImpl<osgEarth::PolyShader>::process(std::basic_ostream<char>& os)
{
    osgEarth::PolyShader * object = getObject<osgEarth::PolyShader,SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>location</td><td>" << object->getLocation() << "</td></tr>" << std::endl;
            os << "<tr><td>source</td><td><pre>" << object->getShaderSource() << "</pre></td></tr>" << std::endl;

            if (_table)
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

#endif

bool writePrettyHTMLImpl<osgEarth::Terrain>::process(std::basic_ostream<char>& os)
{
	osgEarth::Terrain * object = getObject<osgEarth::Terrain,SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		// add group properties first
		callNextHandler(os);

		// add remaining Terrain properties
		os << "<tr><td>profile</td><td>" << getObjectNameAndType(object->getProfile()) << "</td></tr>" << std::endl;
		os << "<tr><td>SRS</td><td>" << getObjectNameAndType(object->getSRS()) << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
		os << "<tr><td>isGeocentric</td><td>" << (object->isGeocentric() ? "true" : "false") << "</td></tr>" << std::endl;
#endif
		os << "<tr><td>graph</td><td>" << getObjectNameAndType(object->getGraph()) << "</td></tr>" << std::endl;

		if (_table)
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

bool writePrettyHTMLImpl<osgEarth::TerrainEngineNode>::process(std::basic_ostream<char>& os)
{
	osgEarth::TerrainEngineNode * object = static_cast<osgEarth::TerrainEngineNode*>(item<SGIItemOsg>()->object());
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		// add group properties first
		callNextHandler(os);

		// add remaining TerrainEngineNode properties
		os << "<tr><td>terrain</td><td>" << getObjectNameAndType(object->getTerrain()) << "</td></tr>" << std::endl;
		os << "<tr><td>resources</td><td>" << getObjectNameAndType(object->getResources()) << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
		os << "<tr><td>terrain stateset</td><td>" << getObjectNameAndType(object->getTerrainStateSet()) << "</td></tr>" << std::endl;
		os << "<tr><td>payload stateset</td><td>" << getObjectNameAndType(object->getPayloadStateSet()) << "</td></tr>" << std::endl;
#else
        os << "<tr><td>surface stateset</td><td>" << getObjectNameAndType(object->getSurfaceStateSet()) << "</td></tr>" << std::endl;
#endif
		os << "<tr><td>normalTexturesRequired</td><td>" << (object->normalTexturesRequired() ? "true" : "false") << "</td></tr>" << std::endl;
		os << "<tr><td>elevationTexturesRequired</td><td>" << (object->elevationTexturesRequired() ? "true" : "false") << "</td></tr>" << std::endl;
		os << "<tr><td>parentTexturesRequired</td><td>" << (object->parentTexturesRequired() ? "true" : "false") << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
		os << "<tr><td>verticalScale</td><td>" << object->getVerticalScale() << "</td></tr>" << std::endl;
#else
        os << "<tr><td>elevationBorderRequired</td><td>" << (object->elevationBorderRequired() ? "true" : "false") << "</td></tr>" << std::endl;
        os << "<tr><td>fullDataAtFirstLodRequired</td><td>" << (object->fullDataAtFirstLodRequired() ? "true" : "false") << "</td></tr>" << std::endl;
#endif

		if (_table)
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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)

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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::TileSource::Status & status)
{
    if(status.isOK())
        return os << "OK";
    else
        return os << status.message();
}
#endif

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::DataExtent & ext, bool table)
{
    std::string mapUrl = MapDownload::getUrl(ext);
    if(table)
    {
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
        os << "<tr><td>description</td><td>" << ext.description() << "</td></tr>" << std::endl;
#endif
        os << "<tr><td>min level</td><td>" << ext.minLevel() << "</td></tr>" << std::endl;
        os << "<tr><td>max level</td><td>" << ext.maxLevel() << "</td></tr>" << std::endl;
        os << "<tr><td>north</td><td>" << ext.north() << "</td></tr>" << std::endl;
        os << "<tr><td>south</td><td>" << ext.south() << "</td></tr>" << std::endl;
        os << "<tr><td>east</td><td>" << ext.east() << "</td></tr>" << std::endl;
        os << "<tr><td>west</td><td>" << ext.west() << "</td></tr>" << std::endl;
        os << "<tr><td>preview</td><td>";
        os << " <a href=\"" << mapUrl << "\">preview</a>";
        os << "</td></tr>" << std::endl;
        os << "</table>" << std::endl;
    }
    else
    {
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
    }
}

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::DataExtentList & object, unsigned number)
{
    if(number == ~0u)
    {
        os << "<ul>" << std::endl;

        os << std::setprecision(12);
        for(osgEarth::DataExtentList::const_iterator it = object.begin(); it != object.end(); it++)
        {
            const osgEarth::DataExtent & ext = *it;
            os << "<li>" << std::endl;
            writePrettyHTML(os, ext, false);
            os << "</li>" << std::endl;
        }

        os << "</ul>" << std::endl;
    }
    else
    {
        if(number >= object.size())
            os << "element #" << number << " not available";
        else
        {
            const osgEarth::DataExtent & ext = object[number];
            writePrettyHTML(os, ext, true);
        }
    }
}

void tileSourceMode(std::basic_ostream<char>& os, const osgEarth::TileSource::Mode & mode)
{
	if(mode == osgEarth::TileSource::MODE_READ)
		os << "read";
	else if(mode == osgEarth::TileSource::MODE_WRITE)
		os << "write";
	else if(mode == osgEarth::TileSource::MODE_CREATE)
		os << "create";
	else
		os << "unknown " << (int)mode;
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

			os << "<tr><td>mode</td><td>";
			tileSourceMode(os, object->getMode());
			os << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>status</td><td>" << object->getStatus() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>isOk</td><td>" << (object->isOK()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>pixels/tile</td><td>" << object->getPixelsPerTile() << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            if(profile)
                os << profile->toString();
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>nodata value</td><td>" << object->getNoDataValue() << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            os << "<tr><td>nodata min value</td><td>" << object->getNoDataMinValue() << "</td></tr>" << std::endl;
            os << "<tr><td>nodata max value</td><td>" << object->getNoDataMaxValue() << "</td></tr>" << std::endl;
#else
			os << "<tr><td>min valid value</td><td>" << object->getMinValidValue() << "</td></tr>" << std::endl;
			os << "<tr><td>max valid value</td><td>" << object->getMaxValidValue() << "</td></tr>" << std::endl;
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
            os << "<tr><td>extension</td><td>" << object->getExtension() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>isDynamic</td><td>" << (object->isDynamic()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>cachePolicyHint</td><td>" << object->getCachePolicyHint(profile) << "</td></tr>" << std::endl;
            os << "<tr><td>dataExtents</td><td>" << object->getDataExtents().size() << " entries" << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
			os << "<tr><td>dataExtentsUnion</td><td>" << object->getDataExtentsUnion() << "</td></tr>" << std::endl;
#endif
			os << "<tr><td>lastModifiedTime</td><td>" << object->getLastModifiedTime() << "</td></tr>" << std::endl;
			

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDataExtents:
		{
			writePrettyHTML(os, object->getDataExtents(), _item->number());
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
			os << "<p>union: " << object->getDataExtentsUnion() << "</p>" << std::endl;
#endif
			ret = true;
		}
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<TileSourceInfo>::process(std::basic_ostream<char>& os)
{
    TileSourceInfo * object = dynamic_cast<TileSourceInfo*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            os << "<tr><td>driver</td><td>" << object->driver() << "</td></tr>" << std::endl;
            os << "<tr><td>url</td><td>" << object->url() << "</td></tr>" << std::endl;
            os << "<tr><td>path</td><td>" << object->path() << "</td></tr>" << std::endl;
            os << "<tr><td>infoURI</td><td>" << object->infoURI() << "</td></tr>" << std::endl;
            os << "<tr><td>creationTime</td><td>" << object->creationTime() << "</td></tr>" << std::endl;
            os << "<tr><td>modificationTime</td><td>" << object->modificationTime() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeInfo:
        {
            if(object->rawData().empty())
                os << "<i>empty</i>";
            else
                os << "<pre>" << object->rawData().toJSON(true) << "</pre>";
            ret = true;
        }
        break;
    case SGIItemTypeChangeset:
        {
            const osgEarth::ConfigSet & changesets = object->changesets();
            if(_item->number() == ~0u)
            {
                os << "<table border=\'1\' align=\'left\'>" << std::endl;

                unsigned num = 0;
                for(osgEarth::ConfigSet::const_iterator it = changesets.begin(); it != changesets.end(); ++it, ++num)
                {
                    const osgEarth::Config & cfg = *it;
                    std::string name = (cfg.key().empty())?helpers::str_plus_count("Changeset", num):cfg.key();
                    os << "<tr><td>" << name << "</td><td><pre>" << cfg.toJSON(true) << "</pre></td></tr>";
                }
                os << "</table>" << std::endl;
            }
            else
            {
                unsigned num = 0;
                for(osgEarth::ConfigSet::const_iterator it = changesets.begin(); it != changesets.end(); ++it, ++num)
                {
                    const osgEarth::Config & cfg = *it;
                    if(num == _item->number())
                    {
                        std::string name = (cfg.key().empty())?helpers::str_plus_count("Changeset", num):cfg.key();
                        os << "<b>" << name << "</b><br/><pre>" << cfg.toJSON(true) << "</pre>";
                        break;
                    }
                }
            }
            ret = true;
        }
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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
        writePrettyHTML(os, object->getDataExtents(), _item->number());
#endif
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#ifdef SGI_USE_OSGEARTH_FAST
bool writePrettyHTMLImpl<osgEarth::LevelDBDatabase>::process(std::basic_ostream<char>& os)
{
	osgEarth::LevelDBDatabase * object = getObject<osgEarth::LevelDBDatabase, SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		{
			if (_table)
				os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

			// add Object properties first
			callNextHandler(os);

			os << "<tr><td>rootPath</td><td>" << object->rootPath() << "</td></tr>" << std::endl;
			os << "<tr><td>isDynamic</td><td>" << (object->created() ? "true" : "false") << "</td></tr>" << std::endl;
			os << "<tr><td>database ptr</td><td>" << (void*)object->getDB() << "</td></tr>" << std::endl;

			if (_table)
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
#endif // SGI_USE_OSGEARTH_FAST

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t)
{
    switch(t)
    {
    case osgEarth::ShaderComp::LOCATION_VERTEX_MODEL: os << "VERTEX_MODEL"; break;
    case osgEarth::ShaderComp::LOCATION_VERTEX_VIEW: os << "VERTEX_VIEW"; break;
    case osgEarth::ShaderComp::LOCATION_VERTEX_CLIP: os << "VERTEX_CLIP"; break;
    case osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING: os << "FRAGMENT_COLORING"; break;
    case osgEarth::ShaderComp::LOCATION_FRAGMENT_LIGHTING: os << "FRAGMENT_LIGHTING"; break;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
    case osgEarth::ShaderComp::LOCATION_FRAGMENT_OUTPUT: os << "FRAGMENT_OUTPUT"; break;
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    case osgEarth::ShaderComp::LOCATION_TESS_CONTROL: os << "TESS_CONTROL"; break;
    case osgEarth::ShaderComp::LOCATION_TESS_EVALUATION: os << "TESS_EVALUATION"; break;
    case osgEarth::ShaderComp::LOCATION_GEOMETRY: os << "GEOMETRY"; break;
    case osgEarth::ShaderComp::LOCATION_UNDEFINED: os << "UNDEFINED"; break;
#endif
    default: os << (int)t; break;
    }
    return os;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::Function & f)
{
    os << f._name << " min=" << f._minRange << " max=" << f._maxRange << " accept=" << getObjectNameAndType(f._accept.get());
    return os;
}
#endif

bool writePrettyHTMLImpl<osgEarth::VirtualProgram>::process(std::basic_ostream<char>& os)
{
    VirtualProgramAccessor * object = static_cast<VirtualProgramAccessor*>(getObject<osgEarth::VirtualProgram,SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            os << "<tr><td>mask</td><td>0x" << std::hex << object->mask() << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>inherit</td><td>";
            if(!object->inheritSet())
                os << "unspecified";
            else
                os << (object->inherit()?"true":"false");
            os << "</td></tr>" << std::endl;
            os << "<tr><td>acceptCallbacksVaryPerFrame</td><td>" << (object->getAcceptCallbacksVaryPerFrame() ? "true" : "false") << "</td></tr>" << std::endl;

            os << "<tr><td>abstract</td><td>" << (object->getIsAbstract() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>logging</td><td>" << (object->getShaderLogging() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>logfile</td><td>" << object->getShaderLogFile() << "</td></tr>" << std::endl;
            os << "<tr><td>template</td><td>" << getObjectNameAndType(object->getTemplate(), true) << "</td></tr>";

            const osgEarth::VirtualProgram::AttribBindingList & attribBindingList = object->getAttribBindingList();
            os << "<tr><td>attribBindingList</td><td>";
            if (attribBindingList.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for (osgEarth::VirtualProgram::AttribBindingList::const_iterator it = attribBindingList.begin(); it != attribBindingList.end(); it++)
                {
                    const std::string & name = it->first;
                    const GLuint & bind = it->second;
                    //os << "<li>" << name << ":" << osg_plugin::sgi::castToEnumValueString<sgi::GLConstant>(bind) << "</li>";
                    os << "<li>" << name << ":" << bind << "</li>";
                }
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;

            osgEarth::VirtualProgram::ProgramMap programCache;
            object->getProgramCache(programCache);
            os << "<tr><td>programCache</td><td>";
            if (programCache.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for (auto it = programCache.begin(); it != programCache.end(); it++)
                {
                    os << "<li>";
                    bool firstKey = true;
                    const osgEarth::ProgramKey & keys = it->first;
                    for (const auto & k : keys)
                    {
                        if (!firstKey)
                            os << ",";
                        os << getObjectNameAndType(k.get());
                        firstKey = false;
                    }
                    const osgEarth::VirtualProgram::ProgramEntry & entry = it->second;
                    os << "=>";
                    os << getObjectNameAndType(entry._program.get()) << " fno=" << entry._frameLastUsed;
                    os << "</li>";
                }
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;

            osgEarth::VirtualProgram::ExtensionsSet extensions;
            object->getGLSLExtensions(extensions);
            os << "<tr><td>glsl extensions</td><td>";
            if (extensions.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for (auto it = extensions.begin(); it != extensions.end(); it++)
                    os << "<li>" << *it << "</li>";
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;

            osgEarth::VirtualProgram::ShaderMap shaderMap;
            object->getShaderMap(shaderMap);
            os << "<tr><td>shader map</td><td>";
            if(shaderMap.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for(osgEarth::VirtualProgram::ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); it++)
                {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
					const std::string & name = it->first;
					const osgEarth::VirtualProgram::ShaderEntry & entry = it->second;
#else
					const auto & id = it->key();
					const auto & entry = it->data();
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
                    const auto & shader = entry._shader;
                    const osg::StateAttribute::OverrideValue & overrideValue = entry._overrideValue;
#else
                    const osg::ref_ptr<osg::Shader> & shader = entry.first;
                    const osg::StateAttribute::OverrideValue & overrideValue = entry.second;
#endif
                    os << "<li>";
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
                    os << name;
#else
                    os << "0x" << std::hex << id;
#endif
                    os << '(' << glOverrideValueName(overrideValue) << ')';
                    os << "</li>";
                }
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;

            osgEarth::ShaderComp::FunctionLocationMap functions;
            object->getFunctions(functions);

            os << "<tr><td>functions</td><td>";
            if(functions.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for(osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.begin(); it != functions.end(); it++)
                {
                    const osgEarth::ShaderComp::FunctionLocation & location = it->first;
                    const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                    os << "<li>" << location << "<ul>";
                    for(osgEarth::ShaderComp::OrderedFunctionMap::const_iterator it = orderedFunctions.begin(); it != orderedFunctions.end(); it++)
                    {
                        const float & order = it->first;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                        const osgEarth::ShaderComp::Function & function = it->second;
                        os << "<li>order " << order << " function " << function._name;
                        if (function._minRange.isSet())
                            os << " min=" << function._minRange.value();
                        if (function._maxRange.isSet())
                            os << " max=" << function._maxRange.value();
                        if (function._accept.valid())
                            os << " accept=" << getObjectNameAndType(function._accept.get());
                        os << "</li>";
#elif OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
                        const osgEarth::ShaderComp::Function & function = it->second;
                        os << "<li>order " << order << " function " << function._name << "</li>";
#else
                        const std::string & function = it->second;
                        os << "<li>order " << order << " function " << function << "</li>";
#endif
                    }
                    os << "</ul></li>";
                }
                os << "</ul>" << std::endl;
            }
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramShaderMap:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            os << "<table border=\'1\' align=\'left\'><tr><th>ShaderID</th><th>Value</th></tr>" << std::endl;
#else
            os << "<table border=\'1\' align=\'left\'><tr><th>Shader</th><th>Value</th></tr>" << std::endl;
#endif
            osgEarth::VirtualProgram::ShaderMap shaderMap;
            object->getShaderMap(shaderMap);
            for(osgEarth::VirtualProgram::ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); it++)
            {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
				const std::string & name = it->first;
				const osgEarth::VirtualProgram::ShaderEntry & entry = it->second;
				os << "<tr><td>" << name << "</td><td>";
#else
				const auto & id = it->key();
				const auto & entry = it->data();
				os << "<tr><td>0x" << std::hex << id << "</td><td>";
#endif
                os << "<table border=\'1\' align=\'left\'>";
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                os << "<tr><td>shader</td><td>";
                if (entry._shader.valid())
                    os << entry._shader->getName();
                else
                    os << "<i>null</i>";
                os << "</td></tr>";
#else
                os << "<tr><td>shader</td><td>" << glOverrideValueName(entry._shader.get()) << "</td></tr>";
#endif
                os << "<tr><td>override</td><td>" << glOverrideValueName(entry._overrideValue) << "</td></tr>";
                os << "<tr><td>accept</td><td>" << getObjectNameAndType(entry._accept.get()) << "</td></tr>";
#else
                os << "<tr><td>shader</td><td>" << getObjectNameAndType(entry.first.get(), true) << "</td></tr>";
                os << "<tr><td>override</td><td>" << glOverrideValueName(entry.second) << "</td></tr>";
#endif
                os << "</table>";
                os << "</td></tr>";
            }
            os << "</table>" << std::endl;

            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramFunctions:
        {
            osgEarth::ShaderComp::FunctionLocationMap functions;
            object->getFunctions(functions);
            if (itemNumber() == ~0u)
            {
                os << "<ul>";
                for (osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.begin(); it != functions.end(); it++)
                {
                    const osgEarth::ShaderComp::FunctionLocation & location = it->first;
                    const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                    os << "<li>";
                    os << location << "<ul>";
                    for (osgEarth::ShaderComp::OrderedFunctionMap::const_iterator it = orderedFunctions.begin(); it != orderedFunctions.end(); it++)
                    {
                        const float & order = it->first;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
                        const osgEarth::ShaderComp::Function & function = it->second;
                        os << "<li>order=" << order << "<br/>" << function << "</li>";
#else
                        os << "<li>order=" << order << "<br/>" << it->second << "</li>";
#endif
                    }
                    os << "</ul></li>";
                }
                os << "</ul>" << std::endl;
            }
            else
            {
                osgEarth::ShaderComp::FunctionLocation location = (osgEarth::ShaderComp::FunctionLocation)itemNumber();
                osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.find(location);
                if (it != functions.end())
                {
                    os << location << "<br/><ul>";
                    const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                    for (osgEarth::ShaderComp::OrderedFunctionMap::const_iterator it = orderedFunctions.begin(); it != orderedFunctions.end(); it++)
                    {
                        const float & order = it->first;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
                        const osgEarth::ShaderComp::Function & function = it->second;
                        os << "<li>order=" << order << "<br/>" << function << "</li>";
#else
                        os << "<li>order=" << order << "<br/>" << it->second << "</li>";
#endif
                    }
                    os << "</ul>";
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramEffectiveProgram:
        {
            os << "<i>Not yet implemented</i>";
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
    TileBlacklistAccess * access = static_cast<TileBlacklistAccess*>(object);
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;

#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            os << "<tr><td>tiles</td><td>";
            TileBlacklistAccess::TileKeySet tiles;
            access->getTileKeySet(tiles);

            const int max_level = 35;
            MapDownload::TileKeyList tiles_per_level[max_level];
            for(const osgEarth::TileKey & tilekey : tiles)
            {
                if(tilekey.valid())
                    tiles_per_level[tilekey.getLOD()].push_back(tilekey);
            }

            for(unsigned level = 0; level < max_level; ++level)
            {
                if(!tiles_per_level[level].empty())
                {
                    os << "<a href=\"" << MapDownload::getUrl(tiles_per_level[level]) << "\">Level " << level << " Preview</a><br/>";
                }
            }

            os << "<ul>";
            for(const osgEarth::TileKey & tilekey : tiles)
            {
                os << "<li>";
                if (tilekey.valid())
                    os << tilekey << "&nbsp;<a href=\"" << MapDownload::getUrl(tilekey) << "\">Preview</a>";
                else
                    os << "invalid " << tilekey.getLOD() << '/' << tilekey.getTileX() << '/' << tilekey.getTileY();
                os << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

#else
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
#endif
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>maxTilesToCache</td><td>" << object->getMaxTilesToCache() << "</td></tr>" << std::endl;
            os << "<tr><td>maxLevelOverride</td><td>" << object->getMaxLevelOverride() << "</td></tr>" << std::endl;
            os << "<tr><td>avg query time</td><td>" << object->getAverageQueryTime() << "</td></tr>" << std::endl;
#endif

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

bool writePrettyHTMLImpl<TileKeyReferenced>::process(std::basic_ostream<char>& os)
{
    TileKeyReferenced * object_ptr = getObject<TileKeyReferenced, SGIItemOsg>();
    const osgEarth::TileKey & object = object_ptr->data();
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
            os << "<tr><td>tileKey</td><td>" << object << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const TileSourceTileKeyData::Status & t)
{
    switch(t)
    {
    case TileSourceTileKeyData::StatusNotLoaded: os << "not loaded"; break;
    case TileSourceTileKeyData::StatusLoaded: os << "loaded"; break;
    case TileSourceTileKeyData::StatusLoadFailure: os << "load failure"; break;
    case TileSourceTileKeyData::StatusNoData: os << "no data"; break;
    default: os << (int)t; break;
    }
    return os;
}

namespace {

    osgEarth::DataExtentList findTileSourceDataExtents(const osgEarth::TileSource * tileSource, const osgEarth::TileKey & key)
    {
        osgEarth::DataExtentList ret;
        const osgEarth::GeoExtent& keyExtent = key.getExtent();

        unsigned int lod = key.getLevelOfDetail();

        const osgEarth::DataExtentList & _dataExtents = tileSource->getDataExtents();
        for (osgEarth::DataExtentList::const_iterator itr = _dataExtents.begin(); itr != _dataExtents.end(); ++itr)
        {
            if ((keyExtent.intersects( *itr )) &&
                (!itr->minLevel().isSet() || itr->minLevel() <= lod ) &&
                (!itr->maxLevel().isSet() || itr->maxLevel() >= lod ))
            {
                ret.push_back((*itr));
            }
        }

        return ret;
    }

}

bool writePrettyHTMLImpl<TileSourceTileKey>::process(std::basic_ostream<char>& os)
{
    TileSourceTileKey * object_ptr = getObject<TileSourceTileKey, SGIItemOsg>();
    const TileSourceTileKeyData & object = object_ptr->data();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Referenced properties first
            callNextHandler(os);

            // add remaining TileSourceTileKey properties
            os << "<tr><td>tileKey</td><td>" << object.tileKey << "</td></tr>" << std::endl;
			os << "<tr><td>tileKey extents</td><td>" << object.tileKey.getExtent() << "</td></tr>" << std::endl;
            os << "<tr><td>status</td><td>" << object.status << "</td></tr>" << std::endl;
            os << "<tr><td>tileSource</td><td>" << getObjectNameAndType(object.tileSource.get(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>tileData</td><td>" << getObjectNameAndType(object.tileData.get()) << "</td></tr>" << std::endl;
            if(object.tileSource.valid())
            {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
                os << "<tr><td>tileSource hasData</td><td>" << (object.tileSource->hasData(object.tileKey)?"true":"false") << "</td></tr>" << std::endl;
#endif
                os << "<tr><td>tileSource data extents</td><td>";
                osgEarth::DataExtentList dataExtents = findTileSourceDataExtents(object.tileSource, object.tileKey);
                if(dataExtents.empty())
                    os << "<i>none</i>";
                else
                {
                    os << "<ul>";
                    for(const osgEarth::DataExtent & ext : dataExtents)
                    {
                        // show the tilekey in relation to the data extent
                        std::string mapUrl = MapDownload::getUrl(ext, object.tileKey);
                        os << "<li>";

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
                        os << "</li>";
                    }
                    os << "</ul>";
                }
                os << "</td></tr>" << std::endl;
            }

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

#ifdef SGI_USE_OSGEARTH_FAST
bool writePrettyHTMLImpl<osgEarth::LODScaleOverrideNode>::process(std::basic_ostream<char>& os)
{
    osgEarth::LODScaleOverrideNode * object = static_cast<osgEarth::LODScaleOverrideNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
    {
        if (_table)
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

        // add Camera properties first
        callNextHandler(os);

        os << "<tr><td>lod scale</td><td>" << object->getLODScale() << "</td></tr>" << std::endl;

        if (_table)
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
#endif // SGI_USE_OSGEARTH_FAST

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

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Angular & a)
#else
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Angle & a)
#endif
{
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
	static osg::ref_ptr<osgEarth::Util::LatLongFormatter> s_formater(new osgEarth::Util::LatLongFormatter);
	return os << s_formater->format(a);
#else
	return os << a.asString();
#endif
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
            os << "<tr><td>backColor</td><td>" << object->backColor() << "</td></tr>" << std::endl;
            os << "<tr><td>activeColor</td><td>" << object->activeColor() << "</td></tr>" << std::endl;
            os << "<tr><td>borderColor</td><td>" << object->borderColor() << "</td></tr>" << std::endl;

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
                os << "<li>" << getObjectNameAndType(evthandler.get()) << "</li>" << std::endl;
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
            os << "<tr><td>text</td><td>" << object->text() << "</td></tr>" << std::endl;
            os << "<tr><td>encoding</td><td>" << object->encoding() << "</td></tr>" << std::endl;

            os << "<tr><td>font</td><td>" << getObjectNameAndType(object->font()) << "</td></tr>" << std::endl;
            os << "<tr><td>fontSize</td><td>" << object->fontSize() << "</td></tr>" << std::endl;
            os << "<tr><td>haloColor</td><td>" << object->haloColor() << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropType</td><td>" << object->getTextBackdropType() << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropImplementation</td><td>" << object->getTextBackdropImplementation() << "</td></tr>" << std::endl;
            os << "<tr><td>textBackdropOffset</td><td>" << object->getTextBackdropOffset() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgEarth::Picker>::process(std::basic_ostream<char>& os)
{
    osgEarth::Picker * object = getObject<osgEarth::Picker, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
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

bool writePrettyHTMLImpl<osgEarth::Util::RTTPicker>::process(std::basic_ostream<char>& os)
{
    RTTPickerAccess * object = static_cast<RTTPickerAccess*>(getObject<osgEarth::Util::RTTPicker, SGIItemOsg, DynamicCaster>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            os << "<tr><td>cullMask</td><td>" << castToEnumValueString<osgNodeMask>(object->getCullMask()) << "</td></tr>" << std::endl;
            os << "<tr><td>buffer</td><td>" << object->getBuffer() << "</td></tr>" << std::endl;
            os << "<tr><td>defaultCallback</td><td>" << getObjectNameAndType(object->getDefaultCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>group</td><td>" << getObjectNameAndType(object->getGroup()) << "</td></tr>" << std::endl;
            os << "<tr><td>pick contexts</td><td>";

            RTTPickerAccess::PickContexts contexts;
            object->getPickContexts(contexts);
            if(contexts.empty())
                os << "<i>empty</i>";
            else
            {
                os << "<ul>";
                for(const auto & context : contexts)
                {
                    os << "<li><table border=\'1\' align=\'left\'>";
                    os << "<tr><td>view</td><td>" << getObjectNameAndType(context._view.get()) << "</td></tr>" << std::endl;
                    os << "<tr><td>camera</td><td>" << getObjectNameAndType(context._pickCamera.get()) << "</td></tr>" << std::endl;
                    os << "<tr><td>image</td><td>" << getObjectNameAndType(context._image.get()) << "</td></tr>" << std::endl;
                    os << "<tr><td>tex</td><td>" << getObjectNameAndType(context._tex.get()) << "</td></tr>" << std::endl;
                    os << "<tr><td>numPicks</td><td>" << context._numPicks << "</td></tr>" << std::endl;
                    os << "</table></li>";
                }
                os << "</ul>";
            }

            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePickerContext:
    {
        RTTPickerAccess::PickContexts contexts;
        object->getPickContexts(contexts);
        unsigned i = 0;
        for (const auto & context : contexts)
        {
            if (i == itemNumber())
            {
                os << "<table border=\'1\' align=\'left\'>";
                os << "<tr><td>view</td><td>" << getObjectNameAndType(context._view.get()) << "</td></tr>" << std::endl;
                os << "<tr><td>camera</td><td>" << getObjectNameAndType(context._pickCamera.get()) << "</td></tr>" << std::endl;
                os << "<tr><td>image</td><td>" << getObjectNameAndType(context._image.get()) << "</td></tr>" << std::endl;
                os << "<tr><td>tex</td><td>" << getObjectNameAndType(context._tex.get()) << "</td></tr>" << std::endl;
                os << "<tr><td>numPicks</td><td>" << context._numPicks << "</td></tr>" << std::endl;
                os << "</table>";
                break;
            }
            ++i;
        }
        ret = true;
    }
    break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgEarth::Features::FeatureProfile>::process(std::basic_ostream<char>& os)
{
	osgEarth::Features::FeatureProfile * object = getObject<osgEarth::Features::FeatureProfile, SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		callNextHandler(os);

		os << "<tr><td>extent</td><td>" << object->getExtent() << "</td></tr>" << std::endl;
		os << "<tr><td>SRS</td><td>" << getObjectNameAndType(object->getSRS()) << "</td></tr>" << std::endl;
		os << "<tr><td>tiled</td><td>" << (object->getTiled() ? "true" : "false") << "</td></tr>" << std::endl;
		os << "<tr><td>profile</td><td>" << getObjectNameAndType(object->getProfile()) << "</td></tr>" << std::endl;
		os << "<tr><td>first level</td><td>" << object->getFirstLevel() << "</td></tr>" << std::endl;
		os << "<tr><td>max level</td><td>" << object->getMaxLevel() << "</td></tr>" << std::endl;
		os << "<tr><td>geometry interpolation</td><td>" << object->geoInterp() << "</td></tr>" << std::endl;

		if (_table)
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
bool writePrettyHTMLImpl<osgEarth::Features::FeatureSource>::process(std::basic_ostream<char>& os)
{
	osgEarth::Features::FeatureSource * object = getObject<osgEarth::Features::FeatureSource, SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		callNextHandler(os);

		os << "<tr><td>feature profile</td><td>" << getObjectNameAndType(object->getFeatureProfile()) << "</td></tr>" << std::endl;
		os << "<tr><td>isWritable</td><td>" << (object->isWritable() ? "true" : "false") << "</td></tr>" << std::endl;
		os << "<tr><td>feature count</td><td>" << object->getFeatureCount() << "</td></tr>" << std::endl;
		os << "<tr><td>supportsGetFeature</td><td>" << (object->supportsGetFeature() ? "true" : "false") << "</td></tr>" << std::endl;
		os << "<tr><td>geometry type</td><td>" << object->getGeometryType() << "</td></tr>" << std::endl;
		os << "<tr><td>hasEmbeddedStyles</td><td>" << (object->hasEmbeddedStyles() ? "true" : "false") << "</td></tr>" << std::endl;

		if (_table)
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

			os << "<tr><td>feature source</td><td>" << getObjectNameAndType(object->getFeatureSource()) << "</td></tr>" << std::endl;

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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            os << "<tr><td>decoration</td><td>" << object->getDecoration() << "</td></tr>" << std::endl;
#endif
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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#endif

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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#endif

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
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            os << "<tr><td>draped</td><td>" << (object->isDraped()?"true":"false") << "</td></tr>" << std::endl;
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


bool writePrettyHTMLImpl<osg::Image>::process(std::basic_ostream<char>& os)
{
    osg::Image * object = getObject<osg::Image, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>isCompressed</td><td>" << (osgEarth::ImageUtils::isCompressed(object)?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasAlphaChannel</td><td>" << (osgEarth::ImageUtils::hasAlphaChannel(object)?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasTransparency</td><td>" << (osgEarth::ImageUtils::hasTransparency(object)?"true":"false") << "</td></tr>" << std::endl;

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
