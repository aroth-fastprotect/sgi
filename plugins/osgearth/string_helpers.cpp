#include "stdafx.h"
#include <osg/Object>
#include <osgEarth/Version>
#include <osgEarth/Viewpoint>
#include <osgEarth/URI>
#include <osgEarth/TileKey>
#include <osgEarth/HTTPClient>
#include <osgEarth/TerrainOptions>
#include <osgEarth/FadeEffect>
#include <osgEarth/Registry>
#include <osgEarth/Profile>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/Expression>
#include <osgEarth/FeatureSourceIndexNode>
#else
#include <osgEarthSymbology/Expression>
#include <osgEarthFeatures/FeatureSourceIndexNode>
#endif
#include <osg/io_utils>
#include <sgi/plugins/SGIItemBase.h>

#include <QLineEdit>

#include "string_helpers.h"

namespace std {

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::URI & uri)
{
    return os << uri.full();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::URIContext& urictx)
{
    return os << urictx.referrer();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::TileKey & key)
{
    return os << key.str();
}

#if OSGEARTH_VERSION_LESS_THAN(3,3,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Bounds & bounds)
{
    return os << bounds.toString();
}
#endif

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::GeoExtent & extent)
{
    return os << extent.toString();
}

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
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
#endif

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
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
	const osgEarth::SpatialReference * srs = viewpoint.getSRS();
    os << "{this=" << &viewpoint
        << ";name=" << viewpoint.getName()
        << ";focalPoint=" << viewpoint.getFocalPoint()
        << ";heading=" << viewpoint.getHeading()
        << ";pitch=" << viewpoint.getPitch()
        << ";range=" << viewpoint.getRange()
        << ";srs=" << ((srs)?srs->getName():"null")
        << "}";
#else
	os << "{this=" << &viewpoint
		<< ";name=" << viewpoint.name()
		<< ";focalPoint=" << viewpoint.focalPoint()
		<< ";heading=" << viewpoint.heading()
		<< ";pitch=" << viewpoint.pitch()
		<< ";range=" << viewpoint.range()
		<< "}";
#endif
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::NumericExpression & expr)
{
    return os << expr.expr();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::StringExpression & expr)
{
    return os << expr.expr();
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,3,0)
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingBoxd & b)
{
    return output << std::setprecision(12) << '[' << b._min << ',' << b._max << ']';
}
#endif

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Query& query)
{
    os << query.expression();
    if(query.orderby().isSet())
        os << " ORDER BY " << query.orderby().value();
    if (query.bounds().isSet())
        os << " BOUNDS " << query.bounds().value();
    if (query.tileKey().isSet())
        os << " TILEKEY " << query.tileKey().value();
    if (query.limit().isSet())
        os << " LIMIT " << query.limit().value();
    return os;
}
#else
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::NumericExpression & expr)
{
    return os << expr.expr();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::StringExpression & expr)
{
    return os << expr.expr();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Query& query)
{
    os << query.expression();
    if(query.orderby().isSet())
        os << " ORDER BY " << query.orderby().value();
    if (query.bounds().isSet())
        os << " BOUNDS " << query.bounds().value();
    if (query.tileKey().isSet())
        os << " TILEKEY " << query.tileKey().value();
    if (query.limit().isSet())
        os << " LIMIT " << query.limit().value();
    return os;
}
#endif

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Units& t)
{
    return os << t.getAbbr();
}
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Fill& t)
{
    return os << t.color();
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Stroke& t)
{
    /// @todo add remaining stroke parameters
    return os << '{' << t.color() << ',' << t.width() << t.widthUnits() << '}';
}
#else // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Fill& t)
{
    return os << t.color();
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Symbology::Stroke& t)
{
    /// @todo add remaining stroke parameters 
    return os << '{' << t.color() << ',' << t.width() << t.widthUnits() << '}';
}
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::DepthOffsetOptions& opts)
{
    return os << opts.enabled() << " auto=" << opts.automatic() << " bias=" << opts.minBias() << '/' << opts.maxBias() << " range=" << opts.minRange() << '/' << opts.maxRange();
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::FadeOptions & opts)
{
    return os << std::setprecision(12) << "duration=" << opts.duration() << " maxRange=" << opts.maxRange() << " attenuationDistance=" << opts.attenuationDistance();;
}

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::Features::FeatureSourceIndexOptions & opts)
{
    os << "embedFeatures=" << opts.embedFeatures();
    return os;
}
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)

} // namespace std

namespace sgi {
	namespace osgearth_plugin {

double parseCoordinate(const QString & txt, const char positiveChar, const char negativeChar, bool * ok)
{
	double result = 0.0;

	bool is_negative = false;
	QString tmp = txt.trimmed();
	if (tmp.at(0) == '-')
		is_negative = true;

	QStringList elems = tmp.split(' ');
	int size = elems.size();
	if (size == 1)
	{
		bool degreesOk = false;
		double degrees = elems[0].toDouble(&degreesOk);
		if (degreesOk)
		{
			if (ok) *ok = true;
			result = degrees;
		}
		else
		{
			if (ok) *ok = false;
			result = 0.0;
		}
	}
	else if (size == 2)
	{
		bool degreesOk = false;
		bool minutesOk = false;
		double degrees = elems[0].toDouble(&degreesOk);
		double minutes = elems[1].toDouble(&minutesOk);

		if (degreesOk && minutesOk)
		{
			if (ok) *ok = true;
			result = degrees + (minutes / 60.0);
		}
		else if (degreesOk)
		{
			QString s = elems[1].toUpper().trimmed();
			if (s[0] == positiveChar && s.length() == 1)
			{
				if (ok) *ok = true;
				result = std::abs(degrees);
			}
			else if (s[0] == negativeChar && s.length() == 1)
			{
				if (ok) *ok = true;
				result = -1.0 * std::abs(degrees);
			}
			else if (s.length() > 0)
			{
				if (ok) *ok = false;
				result = 0;
			}
		}
		else
			result = 0.0;
	}
	else if (size > 2)
	{
		bool degreesOk = false;
		bool minutesOk = false;
		bool secondsOk = false;
		double degrees = elems[0].toDouble(&degreesOk);
		double minutes = elems[1].toDouble(&minutesOk);
		double seconds = elems[2].toDouble(&secondsOk);

		if (degreesOk && minutesOk && secondsOk)
		{
			result = degrees + (minutes / 60.0) + (seconds / 3600.0);
			if (size > 3) {
				QString s = elems[3].toUpper().trimmed();
				if (s[0] == positiveChar && s.length() == 1)
				{
					if (ok) *ok = true;
					result = std::abs(degrees);
				}
				else if (s[0] == negativeChar && s.length() == 1)
				{
					if (ok) *ok = true;
					result = -1.0 * std::abs(degrees);
				}
				else if (s.length() > 0)
				{
					if (ok) *ok = false;
					result = 0;
				}
			}
			else {
				if (ok) *ok = true;
			}

		}
		else if (degreesOk && minutesOk)
		{
			result = degrees + (minutes / 60.0);
			QString s = elems[2].toUpper().trimmed();
			if (s[0] == positiveChar && s.length() == 1)
			{
				if (ok) *ok = true;
				result = std::abs(degrees);
			}
			else if (s[0] == negativeChar && s.length() == 1)
			{
				if (ok) *ok = true;
				result = -1.0 * std::abs(degrees);
			}
			else if (s.length() > 0)
			{
				if (ok) *ok = false;
				result = 0;
			}
		}
	}

	if (is_negative)
		result = -std::abs(result);

	return result;
}

double parseLatitude(const QString & txt, bool * ok)
{
	return parseCoordinate(txt, 'N', 'S', ok);
}

double parseLongitude(const QString & txt, bool * ok)
{
	return parseCoordinate(txt, 'E', 'W', ok);
}

double parseElevation(const QString & txt, bool * ok)
{
	double ret;
	ret = txt.toDouble(ok);
	return ret;
}

osgEarth::GeoPoint geoPointFromString(const QString & str, bool * ok)
{
	osgEarth::GeoPoint ret;

	bool latOk = false, lonOk = false, elevOk = false;

	if (!str.isEmpty())
	{
		double lat = 0.0, lon = 0.0, elev = 0.0;
		QStringList parts;
		int firstComma = str.indexOf(',');
		int firstSpace = str.indexOf(' ');
		if (firstComma > 0)
		{
            if(firstSpace > 0 && firstSpace > firstComma + 1)
                parts = str.split(' ', Qt::SkipEmptyParts);
			else
                parts = str.split(',', Qt::SkipEmptyParts);
		}
		else if (firstSpace > 0)
            parts = str.split(' ', Qt::SkipEmptyParts);
		if (parts.size() == 2)
		{
			lat = parseLatitude(parts[0], &latOk);
			lon = parseLongitude(parts[1], &lonOk);
			elevOk = true;
			elev = 0;
		}
		else if (parts.size() >= 3)
		{
			lat = parseLatitude(parts[0], &latOk);
			lon = parseLongitude(parts[1], &lonOk);
			elev = parseElevation(parts[2], &elevOk);
		}
		if (latOk && lonOk && elevOk)
			ret.set(osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS(), osg::Vec3d(lon, lat, elev), osgEarth::ALTMODE_ABSOLUTE);
	}
	if (ok)
		*ok = (latOk && lonOk && elevOk);
	return ret;
}

osgEarth::TileKey tileKeyFromString(const QString & input, const osgEarth::Profile * profile, int inputLod, bool * ok)
{
	osgEarth::TileKey ret;
	unsigned lod = 0;
	unsigned x = 0;
	unsigned y = 0;
	int slash_char = input.indexOf('/');
	int underscore_char = input.indexOf('_');
	QStringList elems;
	if (slash_char > 0)
		elems = input.split('/');
	else if (underscore_char > 0)
		elems = input.split('_');
	if (elems.size() == 3)
	{
		lod = elems[0].toUInt();
		x = elems[1].toUInt();
		y = elems[2].toUInt();
		ret = osgEarth::TileKey(lod, x, y, profile);
	}
	else if (elems.size() == 2)
	{
		if (inputLod >= 0)
		{
			x = elems[0].toUInt();
			y = elems[1].toUInt();
			ret = osgEarth::TileKey(inputLod, x, y, profile);
		}
	}
	if (ok)
		*ok = ret.valid();
	return ret;
}

CoordinateResult coordinateFromString(const std::string & input, const osgEarth::Profile * profile, int inputLod, bool * ok)
{
    return coordinateFromString(QString::fromStdString(input), profile, inputLod, ok);
}

CoordinateResult coordinateFromString(const QString & input, const osgEarth::Profile * profile, int inputLod, bool * ret_ok)
{
	CoordinateResult ret;
	bool ok = false;
	osgEarth::GeoPoint geopt;
	osgEarth::TileKey tilekey;
	bool hasGeoPoint = false;
	bool hasTileKey = false;
	const unsigned maximum_lod = 21;
	unsigned lod = maximum_lod;
	bool hasLOD = false;
	int at_char = input.indexOf('@');
	if (at_char > 0)
	{
		QString inputgps = input.left(at_char);
		QString inputlod = input.mid(at_char + 1);
		lod = inputlod.toUInt(&ok);
		if (ok)
			hasLOD = true;
		if (inputgps.indexOf('/') > 0 || inputgps.indexOf('_') > 0)
		{
			tilekey = tileKeyFromString(inputgps, profile, (hasLOD) ? lod : -1, &ok);
			if (ok)
				hasTileKey = true;
		}
		else
		{
			geopt = geoPointFromString(inputgps, &ok);
			if (ok)
				hasGeoPoint = true;
		}
	}
	else
	{
		if (input.indexOf('/') > 0 || input.indexOf('_') > 0)
		{
			tilekey = tileKeyFromString(input, profile, -1, &ok);
			if (ok)
				hasTileKey = true;
		}
		else
		{
			geopt = geoPointFromString(input, &ok);
			if (ok)
				hasGeoPoint = true;
		}
	}

	if (ok)
	{
		if (hasGeoPoint)
			ret.geoPoint = geopt;
		if (hasTileKey)
			ret.tileKey = tilekey;
	}

	if (ret_ok)
		*ret_ok = ok;
	return ret;
}

CoordinateResult coordinateFromString(QLineEdit * lineEdit, const osgEarth::Profile * profile, int inputLod, bool * ok)
{
	return coordinateFromString(lineEdit->text(), profile, inputLod, ok);
}

	} // namespace osgearth_plugin
} // namespace sgi
