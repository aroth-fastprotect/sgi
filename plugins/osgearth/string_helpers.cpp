#include "stdafx.h"
#include <osg/Object>
#include <osgEarth/Viewpoint>
#include <osgEarth/URI>
#include <osgEarth/TileKey>
#include <osgEarth/HTTPClient>
#include <osgEarth/TerrainOptions>
#include <osgEarth/FadeEffect>
#include <osgEarthSymbology/Expression>
#include <osgEarthFeatures/FeatureSourceIndexNode>
#include <osg/io_utils>
#include <sgi/plugins/SGIItemBase.h>
#include "string_helpers.h"

namespace std {

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::URI & uri)
{
    return os << uri.full();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::TileKey & key)
{
    return os << key.str();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Bounds & bounds)
{
    return os << bounds.toString();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoExtent & extent)
{
    return os << extent.toString();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ElevationInterpolation & v)
{
    switch(v)
    {
    case osgEarth::INTERP_AVERAGE: os << "average"; break;
    case osgEarth::INTERP_NEAREST: os << "nearest"; break;
    case osgEarth::INTERP_BILINEAR: os << "bilinear"; break;
    case osgEarth::INTERP_TRIANGULATE: os << "triangulate"; break;
    default: os << (int)v; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ElevationSamplePolicy & v)
{
    switch(v)
    {
    case osgEarth::SAMPLE_FIRST_VALID: os << "first valid"; break;
    case osgEarth::SAMPLE_HIGHEST: os << "highest"; break;
    case osgEarth::SAMPLE_LOWEST: os << "lowest"; break;
    case osgEarth::SAMPLE_AVERAGE: os << "average"; break;
    default: os << (int)v; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ProxySettings & v)
{
    if(!v.hostName().empty())
    {
        std::string proxy_auth;
        if(!v.userName().empty())
            proxy_auth = v.userName() + ':' + v.password() + '@';

        os << proxy_auth << v.hostName() << ':' << v.port();
    }
    else
        os << "none";
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Viewpoint & viewpoint)
{
    const osgEarth::SpatialReference * srs = viewpoint.getSRS();
    os << "{this=" << &viewpoint
        << ";name=" << viewpoint.getName()
        << ";focalPoint=" << viewpoint.getFocalPoint()
        << ";heading=" << viewpoint.getHeading()
        << ";pitch=" << viewpoint.getPitch()
        << ";range=" << viewpoint.getRange()
        << ";srs=" << ((srs)?srs->getName():"null")
        << "}";
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ReadResult & result)
{
    switch(result.code())
    {
    case osgEarth::ReadResult::RESULT_OK: os << "ok"; break;
    case osgEarth::ReadResult::RESULT_CANCELED: os << "canceled"; break;
    case osgEarth::ReadResult::RESULT_NOT_FOUND: os << "notFound"; break;
    case osgEarth::ReadResult::RESULT_SERVER_ERROR: os << "serverError"; break;
    case osgEarth::ReadResult::RESULT_TIMEOUT: os << "timeout"; break;
    case osgEarth::ReadResult::RESULT_NO_READER: os << "noReader"; break;
    case osgEarth::ReadResult::RESULT_READER_ERROR: os << "readerError"; break;
    case osgEarth::ReadResult::RESULT_UNKNOWN_ERROR: os << "unknownError"; break;
    default: os << (int)result.code(); break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::CachePolicy & policy)
{
    return os << policy.usageString()
        << ";maxAge=" << policy.maxAge()
        << ";minTime=" << policy.minTime();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::AltitudeMode & t)
{
    switch(t)
    {
    case osgEarth::ALTMODE_ABSOLUTE: os << "absolute"; break;
    case osgEarth::ALTMODE_RELATIVE: os << "relative"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoPoint & pt)
{
    return os << std::setprecision(12) << "{x=" << pt.x() << ";y=" << pt.y() << ";z=" << pt.z()
                << ";altMode=" << pt.altitudeMode()
                << ";srs=" << (pt.getSRS()?pt.getSRS()->getName():"none")
                << "}";
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::NumericExpression & expr)
{
    return os << expr.expr();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::StringExpression & expr)
{
    return os << expr.expr();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::FadeOptions & opts)
{
    return os << std::setprecision(12) << "duration=" << opts.duration() << " maxRange=" << opts.maxRange() << " attenuationDistance=" << opts.attenuationDistance();;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Features::FeatureSourceIndexOptions & opts)
{
    os << "embedFeatures=" << opts.embedFeatures();
    return os;
}

} // namespace std
