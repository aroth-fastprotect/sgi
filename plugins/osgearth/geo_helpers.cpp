#include "stdafx.h"
#include <sgi/plugins/SGIItemBase.h>
#include "geo_helpers.h"
#include "string_helpers.h"
#include <osgEarth/TileSource>
#include <osgEarth/URI>
#include <osgEarth/Registry>

namespace sgi {
namespace osgearth_plugin {

namespace {
    std::string geoPointToString(const osgEarth::GeoPoint & pt)
    {
        std::stringstream buf;
        buf << std::setprecision(12) << pt.x() << ',' << pt.y();
        std::string ssStr;
        ssStr = buf.str();
        return ssStr;

    }
    template<typename T>
    inline std::string vec4fToMapColor( const T& c )
    {
        std::stringstream buf;
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.r()*255.0f);
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.g()*255.0f);
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.b()*255.0f);
        if ( c.a() < 1.0f )
            buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.a()*255.0f);
        std::string ssStr;
        ssStr = buf.str();
        return ssStr;
    }

}

TileKeySet::TileKeySet()
{
}

void TileKeySet::addTileKeyChilds(const osgEarth::TileKey & tilekey)
{
    if (tilekey.getLevelOfDetail() >= 24 || size() > 200)
        return;
    for (unsigned q = 0; q < 4; ++q)
    {
        osgEarth::TileKey qchild = tilekey.createChildKey(q);
        this->insert(qchild);
        this->addTileKeyChilds(qchild);
    }
}

TileKeyList::TileKeyList()
{
}

void TileKeyList::addTileKeyAndNeighbors(const osgEarth::TileKey & tilekey, NUM_NEIGHBORS numNeighbors)
{
    TileKeySet set;
    switch (numNeighbors)
    {
    case NUM_NEIGHBORS_NONE:
        set.insert(tilekey);
        break;
    case NUM_NEIGHBORS_CROSS:
        set.insert(tilekey);
        set.insert(tilekey.createNeighborKey(-1, 0));
        set.insert(tilekey.createNeighborKey(0, -1));
        set.insert(tilekey.createNeighborKey(1, 0));
        set.insert(tilekey.createNeighborKey(0, 1));
        break;
    case NUM_NEIGHBORS_IMMEDIATE:
        set.insert(tilekey);
        set.insert(tilekey.createNeighborKey(-1, 0));
        set.insert(tilekey.createNeighborKey(-1, -1));
        set.insert(tilekey.createNeighborKey(0, -1));
        set.insert(tilekey.createNeighborKey(1, -1));
        set.insert(tilekey.createNeighborKey(1, 0));
        set.insert(tilekey.createNeighborKey(1, 1));
        set.insert(tilekey.createNeighborKey(0, 1));
        set.insert(tilekey.createNeighborKey(-1, 1));
        break;
    case NUM_NEIGHBORS_PARENTAL:
    {
        set.insert(tilekey);
        osgEarth::TileKey t = tilekey.createParentKey();
        while (t.valid())
        {
            set.insert(t);
            t = t.createParentKey();
        }
    }
    break;
    case NUM_NEIGHBORS_PARENTAL_AND_CHILDS:
    {
        set.insert(tilekey);
        osgEarth::TileKey t = tilekey.createParentKey();
        while (t.valid())
        {
            set.insert(t);
            t = t.createParentKey();
        }
        set.addTileKeyChilds(tilekey);
    }
    break;
    case NUM_NEIGHBORS_CHILDS:
    {
        set.insert(tilekey);
        osgEarth::TileKey t = tilekey.createParentKey();
        while (t.valid())
        {
            set.insert(t);
            t = t.createParentKey();
        }
        set.addTileKeyChilds(tilekey);
    }
    break;
    }
    for (TileKeySet::const_iterator it = set.begin(); it != set.end(); it++)
        this->push_back(*it);
}

void TileKeyList::addTileForGeoPoint(const osgEarth::GeoPoint & pt, const osgEarth::Profile * profile, int selected_lod, NUM_NEIGHBORS numNeighbors)
{
    Q_ASSERT(profile != nullptr);
    osgEarth::TileKey tilekey = profile->createTileKey(pt.x(), pt.y(), selected_lod);
    addTileKeyAndNeighbors(tilekey, (numNeighbors == NUM_NEIGHBORS_PARENTAL || numNeighbors == NUM_NEIGHBORS_PARENTAL_AND_CHILDS) ? NUM_NEIGHBORS_NONE : numNeighbors);
}

void TileKeyList::addTilesForGeoExtent(const osgEarth::GeoExtent & ext, const osgEarth::Profile * profile, int selected_lod, NUM_NEIGHBORS numNeighbors)
{
    osgEarth::GeoPoint nw(profile->getSRS(), ext.xMin(), ext.yMin());
    osgEarth::GeoPoint ne(profile->getSRS(), ext.xMin(), ext.yMax());
    osgEarth::GeoPoint sw(profile->getSRS(), ext.xMax(), ext.yMin());
    osgEarth::GeoPoint se(profile->getSRS(), ext.xMax(), ext.yMax());

    osgEarth::GeoPoint center;
    ext.getCentroid(center);

    if (selected_lod < 0)
    {
        for (unsigned lod = 0; lod < 21; lod++)
        {
            addTileForGeoPoint(center, profile, lod, numNeighbors);
            addTileForGeoPoint(nw, profile, lod, numNeighbors);
            addTileForGeoPoint(ne, profile, lod, numNeighbors);
            addTileForGeoPoint(sw, profile, lod, numNeighbors);
            addTileForGeoPoint(se, profile, lod, numNeighbors);
        }
    }
    else
    {
        addTileForGeoPoint(center, profile, selected_lod, numNeighbors);
        addTileForGeoPoint(nw, profile, selected_lod, numNeighbors);
        addTileForGeoPoint(ne, profile, selected_lod, numNeighbors);
        addTileForGeoPoint(sw, profile, selected_lod, numNeighbors);
        addTileForGeoPoint(se, profile, selected_lod, numNeighbors);
    }
}


bool TileKeyList::fromLineEdit(QLineEdit * le, const osgEarth::Profile * profile, int selectedLod, NUM_NEIGHBORS numNeighbors)
{
    bool ret = false;
    CoordinateResult coordResult = coordinateFromString(le, profile, selectedLod, &ret);
    if (ret)
        ret = fromCoordinateResult(coordResult, profile, selectedLod, numNeighbors);
    return ret;
}

bool TileKeyList::fromString(const std::string & s, const osgEarth::Profile * profile, int selectedLod, NUM_NEIGHBORS numNeighbors)
{
    bool ret = false;
    CoordinateResult coordResult = coordinateFromString(s, profile, selectedLod, &ret);
    if (ret)
        ret = fromCoordinateResult(coordResult, profile, selectedLod, numNeighbors);
    return ret;
}

bool TileKeyList::fromCoordinateResult(const CoordinateResult & result, const osgEarth::Profile * profile, int selectedLod, NUM_NEIGHBORS numNeighbors)
{
    bool ret = false;
    const osgEarth::Profile * used_profile = profile ? profile : osgEarth::Registry::instance()->getGlobalGeodeticProfile();
    const unsigned maximum_lod = 21;
    if (result.geoPoint.isValid())
    {
        osgEarth::GeoPoint geoptProfile = result.geoPoint.transform(used_profile->getSRS());

        if (selectedLod == -1)
        {
            for (unsigned lod = 0; lod < maximum_lod; lod++)
            {
                osgEarth::TileKey tilekey = used_profile->createTileKey(geoptProfile.x(), geoptProfile.y(), lod);
                addTileKeyAndNeighbors(tilekey, (numNeighbors == NUM_NEIGHBORS_PARENTAL || numNeighbors == NUM_NEIGHBORS_PARENTAL_AND_CHILDS) ? NUM_NEIGHBORS_NONE : numNeighbors);
            }
        }
        else
        {
            osgEarth::TileKey tilekey = used_profile->createTileKey(geoptProfile.x(), geoptProfile.y(), selectedLod);
            addTileKeyAndNeighbors(tilekey, numNeighbors);
        }
        ret = true;
    }
    else if (result.tileKey.valid())
    {
        addTileKeyAndNeighbors(result.tileKey, numNeighbors);
        ret = true;
    }
    return ret;
}

bool TileKeyList::fromText(const std::string & s, const osgEarth::Profile * profile, int selectedLod, NUM_NEIGHBORS numNeighbors)
{
    bool ret = false;
    osgEarth::StringVector lines;
    osgEarth::StringTokenizer(s, lines);
    for (const std::string & line : lines)
    {
        if (fromString(line, profile, selectedLod, numNeighbors))
            ret = true;
    }
    return ret;
}

class MapDownload::MapDownloadPrivate
{
public:
    struct CoordinateSet {
        EntityType type;
        NamedGeoPointList coordinates;
    };

    typedef std::vector<CoordinateSet> CoordinateSetList;

    osg::ref_ptr<osgEarth::SpatialReference>    srs;
    CoordinateSetList       coordinateSets;
    int                     outputWidth;
    int                     outputHeight;
    MapType                 outputType;
    bool                    flagSaveURL;

    osg::BoundingBoxd       coordinateBox;

    static osg::Vec4        colors[];

public:
    void                    calculateCoordinateBox();
    osgEarth::GeoPoint      getCenter() const;
    int                     getGoogleZoomLevel() const;
};

osg::Vec4 MapDownload::MapDownloadPrivate::colors[] = {
    osg::Vec4(1,0,0,1),
    osg::Vec4(0,1,0,1),
    osg::Vec4(0,0,1,1),
    osg::Vec4(1,1,0,1),
    osg::Vec4(0,1,1,1),
    osg::Vec4(1,0,1,1),
    osg::Vec4(1,1,1,1),
    osg::Vec4(0,0,0,1),
    osg::Vec4(0.5,0.5,0.5,1)
};

void MapDownload::MapDownloadPrivate::calculateCoordinateBox()
{
    coordinateBox = osg::BoundingBoxd();
    if(!coordinateSets.empty())
    {
        const CoordinateSet & set = coordinateSets.front();
        if(!set.coordinates.empty())
        {
            const osgEarth::GeoPoint & coordinate = set.coordinates.front().second;
            coordinateBox = osg::BoundingBoxd(coordinate.vec3d(), coordinate.vec3d());
        }
    }
    for( CoordinateSetList::const_iterator itSets = coordinateSets.begin(); itSets != coordinateSets.end(); itSets++)
    {
        const CoordinateSet & set = *itSets;
        for(MapDownload::NamedGeoPointList::const_iterator it = set.coordinates.begin(); it != set.coordinates.end(); it++)
        {
            const MapDownload::NamedGeoPoint & namedCoord = *it;
            //const std::string & name = namedCoord.first;
            const osgEarth::GeoPoint & coordinate = namedCoord.second;
            if(coordinate.isValid())
            {
                coordinateBox.expandBy(coordinate.vec3d());
            }
        }
    }
}

osgEarth::GeoPoint MapDownload::MapDownloadPrivate::getCenter() const
{
    return osgEarth::GeoPoint(srs, coordinateBox.center(), osgEarth::ALTMODE_ABSOLUTE);
}

int MapDownload::MapDownloadPrivate::getGoogleZoomLevel() const
{
    int ret = 0;
    osg::Vec3d boxSize = coordinateBox._max - coordinateBox._min;
    float lonSize = 360;
    float latSize = 180;
    for(int i = 0; i < 21; i++, lonSize/=2, latSize/=2, ret = i)
    {
        if(boxSize.x() > lonSize || boxSize.y() > latSize)
            break;
    }
    return ret;
}

MapDownload::MapDownload(MapType type, int width, int height)
    : d(new MapDownloadPrivate)
{
    d->outputType = type;
    d->flagSaveURL = true;
    d->outputWidth = width;
    d->outputHeight = height;
}

MapDownload::~MapDownload()
{
    delete d;
}

void MapDownload::addCoordinates(const NamedGeoPointList & points, EntityType type)
{
    MapDownload::MapDownloadPrivate::CoordinateSet s;
    s.coordinates = points;
    s.type = type;
    d->coordinateSets.push_back(s);
}

void MapDownload::addCoordinates(const GeoPointList & points, EntityType type)
{
    MapDownload::MapDownloadPrivate::CoordinateSet s;
    unsigned num_pt = 0;
    for(GeoPointList::const_iterator it = points.begin(); it != points.end(); it++)
    {
        NamedGeoPoint namedCoord(std::string(), *it);
        s.coordinates.push_back(namedCoord);
    }
    s.type = type;
    d->coordinateSets.push_back(s);
}

void MapDownload::addCoordinates(const TileKeyList & points)
{
    for(const osgEarth::TileKey & tk : points)
    {
        MapDownload::MapDownloadPrivate::CoordinateSet s;
        const osgEarth::GeoExtent & ge = tk.getExtent();

        NamedGeoPoint nw(tk.str(), osgEarth::GeoPoint(ge.getSRS(), ge.north(), ge.west()));
        NamedGeoPoint ne(tk.str(), osgEarth::GeoPoint(ge.getSRS(), ge.north(), ge.east()));
        NamedGeoPoint sw(tk.str(), osgEarth::GeoPoint(ge.getSRS(), ge.south(), ge.west()));
        NamedGeoPoint se(tk.str(), osgEarth::GeoPoint(ge.getSRS(), ge.south(), ge.east()));
        s.coordinates.push_back(nw);
        s.coordinates.push_back(ne);
        s.coordinates.push_back(se);
        s.coordinates.push_back(sw);
        s.coordinates.push_back(nw);
        s.type = Polygon;
        d->coordinateSets.push_back(s);
    }
}

void MapDownload::addCoordinates(const DataExtentList & points)
{
    for(const osgEarth::DataExtent & de : points)
    {
        MapDownload::MapDownloadPrivate::CoordinateSet s;

        NamedGeoPoint nw("NW", osgEarth::GeoPoint(de.getSRS(), de.north(), de.west()));
        NamedGeoPoint ne("NE", osgEarth::GeoPoint(de.getSRS(), de.north(), de.east()));
        NamedGeoPoint sw("SW", osgEarth::GeoPoint(de.getSRS(), de.south(), de.west()));
        NamedGeoPoint se("SE", osgEarth::GeoPoint(de.getSRS(), de.south(), de.east()));
        s.coordinates.push_back(nw);
        s.coordinates.push_back(ne);
        s.coordinates.push_back(se);
        s.coordinates.push_back(sw);
        s.coordinates.push_back(nw);
        s.type = Polygon;
        d->coordinateSets.push_back(s);
    }
}

std::string MapDownload::getUrl() const
{
    std::string ret;
    std::string baseUrl;
    std::stringstream ss;

    d->calculateCoordinateBox();

    switch(d->outputType)
    {
    case GoogleSatelite:
        baseUrl = std::string("http://maps.google.com/maps/api/staticmap?maptype=satellite&sensor=false");
        break;
    case GoogleRoadmap:
        baseUrl = std::string("http://maps.google.com/maps/api/staticmap?maptype=roadmap&sensor=false");
        break;
    case GoogleTerrrain:
        baseUrl = std::string("http://maps.google.com/maps/api/staticmap?maptype=terrain&sensor=false");
        break;
    case GoogleHybrid:
        baseUrl = std::string("http://maps.google.com/maps/api/staticmap?maptype=hybrid&sensor=false");
        break;
    }
    ss << baseUrl;

    for( MapDownload::MapDownloadPrivate::CoordinateSetList::const_iterator itSets = d->coordinateSets.begin(); itSets != d->coordinateSets.end(); itSets++)
    {
        std::string csetStr;
        int ptNum = 0;
        const MapDownload::MapDownloadPrivate::CoordinateSet & set = *itSets;
        switch(set.type)
        {
        case Point:
            for(MapDownload::NamedGeoPointList::const_iterator it = set.coordinates.begin(); it != set.coordinates.end(); it++)
            {
                const NamedGeoPoint & namedCoord = *it;
                const std::string & name = namedCoord.first;
                const osgEarth::GeoPoint & coordinate = namedCoord.second;

                std::string pointStr = geoPointToString(coordinate);
                std::string label = name.size() > 0 ? std::string(1, name.at(0)) : std::string();
                unsigned colorIndex = ptNum % (sizeof(MapDownload::MapDownloadPrivate::colors)/sizeof(MapDownload::MapDownloadPrivate::colors[0]));
                std::string color = vec4fToMapColor(MapDownload::MapDownloadPrivate::colors[colorIndex]);
                csetStr += "&markers=color:0x" + color + "|label:" + label + "|" + pointStr;
                ptNum++;
            }
            break;
        case Line:
        case Polygon:
            csetStr += "&path=color:0x0000ff|weight:5";
            for(MapDownload::NamedGeoPointList::const_iterator it = set.coordinates.begin(); it != set.coordinates.end(); it++)
            {
                const NamedGeoPoint & namedCoord = *it;
                const std::string & name = namedCoord.first;
                const osgEarth::GeoPoint & coordinate = namedCoord.second;
                std::string pointStr = geoPointToString(coordinate);
                csetStr += "|" + pointStr;
                ptNum++;
            }
            break;
        }
        ss << csetStr;
    }

    //const osgEarth::GeoPoint center = d->getCenter();
    // = "&center=" + std::string::number(center.latitude()) + "," + std::string::number(center.longitude());
    switch(d->outputType)
    {
    case GoogleSatelite:
    case GoogleRoadmap:
    case GoogleTerrrain:
    case GoogleHybrid:
        {
            int zoom = d->getGoogleZoomLevel();
            ss << "&zoom=" << zoom;
            ss << "&size=" << d->outputWidth << 'x'  << d->outputHeight;
        }
        break;
    }

    ret = ss.str();
    return ret;
}

std::string MapDownload::getUrl(const GeoPointList & points, bool satelite, bool markers, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    dl.addCoordinates(points, (markers)?Point:Line);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const NamedGeoPointList & points, bool satelite, bool markers, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    dl.addCoordinates(points, (markers)?Point:Line);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const TileKeyList & points, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    dl.addCoordinates(points);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const osgEarth::TileKey & tk, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    TileKeyList points(1);
    points[0] = tk;
    dl.addCoordinates(points);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const DataExtentList & points, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    dl.addCoordinates(points);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const osgEarth::DataExtent & de, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    DataExtentList points;
    points.push_back(de);
    dl.addCoordinates(points);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const osgEarth::DataExtent & de, const TileKeyList & points, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    DataExtentList delist;
    delist.push_back(de);
    dl.addCoordinates(delist);
    dl.addCoordinates(points);
    return dl.getUrl();
}

std::string MapDownload::getUrl(const osgEarth::DataExtent & de, const osgEarth::TileKey & tk, bool satelite, int width, int height)
{
    MapDownload dl(satelite?GoogleSatelite:GoogleRoadmap, width, height);
    DataExtentList delist;
    delist.push_back(de);
    dl.addCoordinates(delist);
    TileKeyList points;
    points.push_back(tk);
    dl.addCoordinates(points);
    return dl.getUrl();
}


class TileSourceInfo::TileSourceInfoPrivate
{
public:
    TileSourceInfoPrivate(const osgEarth::TileSource * ts)
        : tileSource(ts)
        , creationTime(static_cast<osgEarth::TimeStamp>(0))
        , modificationTime(static_cast<osgEarth::TimeStamp>(0))
        {
            const osgEarth::TileSourceOptions & opts = ts->getOptions();
            driver = opts.getDriver();
            osgEarth::Config optsCfg = opts.getConfig();
            if(optsCfg.hasValue("url"))
                url = osgEarth::URI(optsCfg.value("url"), osgEarth::URIContext(optsCfg.referrer()));
            if(optsCfg.hasValue("path"))
                path = osgEarth::URI(optsCfg.value("path"), osgEarth::URIContext(optsCfg.referrer()));

            if(driver == "tms")
            {
                osgEarth::URIContext context(url.full());
                infoURI = osgEarth::URI("info", context);
            }
        }

    const osgEarth::TileSource * tileSource;
    std::string driver;
    osgEarth::URI url;
    osgEarth::URI path;
    osgEarth::Config rawData;
    osgEarth::URI infoURI;
    osgEarth::ConfigSet changesets;
    osgEarth::DateTime creationTime;
    osgEarth::DateTime modificationTime;
};

TileSourceInfo::TileSourceInfo(const osgEarth::TileSource * ts)
    : d(new TileSourceInfoPrivate(ts))
{
    refresh();
}

TileSourceInfo::~TileSourceInfo()
{
    delete d;
}

const std::string & TileSourceInfo::driver() const
{
    return d->driver;
}

const osgEarth::Config & TileSourceInfo::rawData() const
{
    return d->rawData;
}

const osgEarth::URI & TileSourceInfo::url() const
{
    return d->url;
}

const osgEarth::URI & TileSourceInfo::path() const
{
    return d->path;
}

const osgEarth::URI & TileSourceInfo::infoURI() const
{
    return d->infoURI;
}

const osgEarth::ConfigSet & TileSourceInfo::changesets() const
{
    return d->changesets;
}

const osgEarth::DateTime & TileSourceInfo::creationTime() const
{
    return d->creationTime;
}

const osgEarth::DateTime & TileSourceInfo::modificationTime() const
{
    return d->modificationTime;
}

void TileSourceInfo::refresh()
{
    std::string info = d->infoURI.getString();
    if(!info.empty())
        d->rawData.fromJSON(info);
    d->changesets = d->rawData.child("changesets").children();

    if(d->rawData.hasValue("creation_time"))
        d->creationTime = osgEarth::DateTime(d->rawData.value("creation_time"));

    if(d->rawData.hasValue("modification_time"))
        d->modificationTime = osgEarth::DateTime(d->rawData.value("modification_time"));
    else if(d->rawData.hasValue("last_update_time"))
        d->modificationTime = osgEarth::DateTime(d->rawData.value("last_update_time"));

}

} // namespace osgearth_plugin
} // namespace sgi
