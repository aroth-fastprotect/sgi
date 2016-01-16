#include "stdafx.h"
#include <sgi/plugins/SGIItemBase.h>
#include "geo_helpers.h"
#include "string_helpers.h"
#include <osgEarth/TileSource>
#include <osgEarth/URI>

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
