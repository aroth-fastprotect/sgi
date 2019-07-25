#pragma once

#include <sstream>

#include <osgEarth/optional>
#include <osgEarth/Units>
#include <osgEarth/GeoData>
#include <osgEarth/TileKey>

class QLineEdit;

namespace osgEarth {
    class Bounds;
    class GeoExtent;
    class TileKey;
    class Map;
    class MapNode;
    class Profile;
    class ImageLayer;
    class ElevationLayer;
    class ModelLayer;
    class MaskLayer;
    class MapOptions;
    class MapNodeOptions;
    class SpatialReference;
    class Config;
    class TerrainLayerOptions;
    class ImageLayerOptions;
    class ElevationLayerOptions;
    class TileSourceOptions;
    class ModelSourceOptions;
    class ModelLayerOptions;
    class ProxySettings;
    class URI;
    class URIContext;
    class GeoPoint;
    class Units;
    class DepthOffsetOptions;

    class ProfileOptions;
    class CacheOptions;
    class TerrainOptions;

    class Cache;
    class CachePolicy;

    namespace Symbology {
        class NumericExpression;
        class StringExpression;
        class Query;
        class Fill;
        class Stroke;
    }
    class FadeOptions;

    class Viewpoint;

    namespace Features {
        class Feature;
        class FeatureSourceIndexOptions;
    }

    struct ReadResult;
}

namespace sgi {
	namespace osg_helpers {
		template<typename T>
		std::string vec4fToHtmlColor(const T& c);
	} // namespace osg_helpers
} // namespace sgi

namespace std {

template<typename T>
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::optional<T> & v)
{
    if(!v.isSet())
        return os << "unset(" << v.defaultValue() << ")";
    else
        return os << v.value();
}

template<>
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::optional<bool> & v)
{
    if(!v.isSet())
        return os << "unset(" << (v.defaultValue()?"true":"false") << ")";
    else
        return os << (v.value()?"true":"false");
}

template<>
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::optional<osg::Vec4> & v)
{
	if (!v.isSet())
		return os << "unset(" << sgi::osg_helpers::vec4fToHtmlColor(v.defaultValue()) << ")";
	else
		return os << sgi::osg_helpers::vec4fToHtmlColor(v.value());
}

template<typename T>
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::qualified_double<T> & d)
{
    const osgEarth::Units &  u = d.getUnits();
    double v = d.as(u);
    os << v << u.getAbbr();
    return os;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ProxySettings & v);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::TileKey & key);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::URI & uri);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::URIContext& urictx);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Bounds & bounds);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoExtent & extent);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Viewpoint & viewpoint);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ReadResult & result);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::CachePolicy & policy);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoPoint & pt);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::NumericExpression & expr);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::StringExpression & expr);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Query& query);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Units& t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Fill& t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Stroke& t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::FadeOptions & opts);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::DepthOffsetOptions& opts);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Features::FeatureSourceIndexOptions & opts);

} // namespace std


namespace sgi {
	namespace osgearth_plugin {

struct CoordinateResult {
	osgEarth::TileKey tileKey;
	osgEarth::GeoPoint geoPoint;
	std::string errorText;
};

double parseCoordinate(const QString & txt, const char positiveChar, const char negativeChar, bool * ok);
double parseLatitude(const QString & txt, bool * ok);
double parseLongitude(const QString & txt, bool * ok);
double parseElevation(const QString & txt, bool * ok);
osgEarth::GeoPoint geoPointFromString(const QString & str, bool * ok);
osgEarth::TileKey tileKeyFromString(const QString & input, const osgEarth::Profile * profile, int inputLod, bool * ok);
CoordinateResult coordinateFromString(const std::string & input, const osgEarth::Profile * profile, int inputLod, bool * ok);
CoordinateResult coordinateFromString(const QString & input, const osgEarth::Profile * profile, int inputLod, bool * ok);
CoordinateResult coordinateFromString(QLineEdit * lineEdit, const osgEarth::Profile * profile, int inputLod, bool * ok);

	} // namespace osgearth_plugin
} // namespace sgi
