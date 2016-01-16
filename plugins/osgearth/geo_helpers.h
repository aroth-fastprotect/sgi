#pragma once

#include <osgEarth/GeoData>
#include <osgEarth/TileKey>

namespace osgEarth {
    class TileSource;
    class DateTime;
}

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
    typedef std::vector<osgEarth::TileKey> TileKeyList;
    typedef std::vector<osgEarth::DataExtent> DataExtentList;

public:
                            MapDownload(MapType type=MapTypeDefault, int width=640, int height=480);
                            ~MapDownload();

public:
    void                    addCoordinates(const NamedGeoPointList & points, EntityType type=Point);
    void                    addCoordinates(const GeoPointList & points, EntityType type=Point);
    void                    addCoordinates(const TileKeyList & points);
    void                    addCoordinates(const DataExtentList & points);

    std::string                 getUrl() const;

public:
    static std::string          getUrl(const GeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);
    static std::string          getUrl(const NamedGeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);
    static std::string          getUrl(const TileKeyList & points, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::TileKey & tk, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const DataExtentList & points, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::DataExtent & de, bool satelite=true, int width=640, int height=480);

private:
    class MapDownloadPrivate;
    MapDownloadPrivate * d;
};

class TileSourceInfo : public osg::Referenced
{
public:
    TileSourceInfo(const osgEarth::TileSource * ts);
    ~TileSourceInfo();

    void refresh();

    const std::string & driver() const;
    const osgEarth::URI & url() const;
    const osgEarth::URI & path() const;
    const osgEarth::URI & infoURI() const;
    const osgEarth::Config & rawData() const;

    const osgEarth::ConfigSet & changesets() const;
    const osgEarth::DateTime & creationTime() const;
    const osgEarth::DateTime & modificationTime() const;

private:
    class TileSourceInfoPrivate;
    TileSourceInfoPrivate * d;
};


} // namespace osgearth_plugin
} // namespace sgi
