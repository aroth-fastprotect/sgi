#pragma once

#include <sstream>

#include <osgEarth/optional>
#include <osgEarth/Units>

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
    class GeoPoint;

    class ProfileOptions;
    class CacheOptions;
    class TerrainOptions;

    class Cache;
    class CachePolicy;

    namespace Symbology {
        class NumericExpression;
        class StringExpression;
    }
    class FadeOptions;

    class Viewpoint;

    namespace Features {
        class Feature;
        class FeatureSourceIndexOptions;
    }

    struct ReadResult;
}

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
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Bounds & bounds);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoExtent & extent);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Viewpoint & viewpoint);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ReadResult & result);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::CachePolicy & policy);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoPoint & pt);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::NumericExpression & expr);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::StringExpression & expr);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::FadeOptions & opts);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Features::FeatureSourceIndexOptions & opts);

} // namespace std
