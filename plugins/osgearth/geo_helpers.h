#pragma once

#include <osgEarth/GeoData>

namespace sgi {
namespace osgearth_plugin {

class MapDownload
{
public:
    enum MapType
    {
        MapTypeInvalid = -1,
        GoogleSatelite = 0,
        GoogleRoadmap,
        GoogleTerrrain,
        GoogleHybrid,
        MapTypeDefault = GoogleSatelite,
        MapTypeMax = GoogleHybrid
    };

    enum EntityType
    {
        Point = 0,
        Line,
        Polygon
    };

    typedef std::pair<std::string, osgEarth::GeoPoint>   NamedGeoPoint;
    typedef std::vector<NamedGeoPoint> NamedGeoPointList;
    typedef std::vector<osgEarth::GeoPoint> GeoPointList;

public:
                            MapDownload(MapType type=MapTypeDefault, int width=640, int height=480);
                            ~MapDownload();

public:
    void                    addCoordinates(const NamedGeoPointList & points, EntityType type=Point);
    void                    addCoordinates(const GeoPointList & points, EntityType type=Point);

    std::string                 getUrl() const;

public:
    static std::string          getUrl(const GeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);
    static std::string          getUrl(const NamedGeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);

private:
    class MapDownloadPrivate;
    MapDownloadPrivate * d;
};

} // namespace osgearth_plugin
} // namespace sgi
