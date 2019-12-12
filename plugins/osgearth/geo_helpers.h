#pragma once

#include <osgEarth/GeoData>
#include <osgEarth/TileKey>

namespace osgEarth {
    class TileSource;
    class DateTime;
    class URI;
}

class QLineEdit;

namespace sgi {
namespace osgearth_plugin {

struct CoordinateResult;

class TileKeySet : public std::set<osgEarth::TileKey>
{
public:
    TileKeySet();

    void addTileKeyChilds(const osgEarth::TileKey & tilekey);
};

class TileKeyList : public std::list<osgEarth::TileKey>
{
public:
    TileKeyList();

public:
	enum {
		MINIMUM_LOD_LEVEL = 0,
		MAXIMUM_LOD_LEVEL = 23
	};
    enum NUM_NEIGHBORS
    {
        NUM_NEIGHBORS_NONE = 0,
        NUM_NEIGHBORS_CROSS,
        NUM_NEIGHBORS_IMMEDIATE,
        NUM_NEIGHBORS_PARENTAL,
        NUM_NEIGHBORS_PARENTAL_AND_CHILDS,
        NUM_NEIGHBORS_CHILDS,
    };

    void addTileKeyAndNeighbors(const osgEarth::TileKey & tilekey, NUM_NEIGHBORS numNeighbors);
    void addTileForGeoPoint(const osgEarth::GeoPoint & pt, const osgEarth::Profile * profile, int selected_lod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);
    void addTilesForGeoExtent(const osgEarth::GeoExtent & ext, const osgEarth::Profile * profile, int selected_lod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);

    bool fromLineEdit(QLineEdit * le, const osgEarth::Profile * profile, int selectedLod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);
    bool fromString(const std::string & s, const osgEarth::Profile * profile, int selectedLod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);
    bool fromCoordinateResult(const CoordinateResult & result, const osgEarth::Profile * profile, int selectedLod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);
    bool fromText(const std::string & s, const osgEarth::Profile * profile, int selectedLod = -1, NUM_NEIGHBORS numNeighbors = NUM_NEIGHBORS_NONE);

	void setMinimumLOD(int n) { _minimum_lod_level = n; }
	int minimumLOD() const { return _minimum_lod_level; }
	void setMaximumLOD(int n) { _maximum_lod_level = n; }
	int maximumLOD() const { return _maximum_lod_level; }

protected:
	int _maximum_lod_level;
	int _minimum_lod_level;

};

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
    void                    addCoordinates(const osgEarth::DataExtent & de, const TileKeyList & points);

    std::string                 getUrl() const;

public:
    static std::string          getUrl(const GeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);
    static std::string          getUrl(const NamedGeoPointList & points, bool satelite=true, bool markers=true, int width=640, int height=480);
    static std::string          getUrl(const TileKeyList & points, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::TileKey & tk, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const DataExtentList & points, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::DataExtent & de, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::DataExtent & de, const TileKeyList & points, bool satelite=true, int width=640, int height=480);
    static std::string          getUrl(const osgEarth::DataExtent & de, const osgEarth::TileKey & tk, bool satelite=true, int width=640, int height=480);

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
