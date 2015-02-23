#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGTerrain.h"
#include "SGIItemOsg"

#include <osg/io_utils>

// osgTerrain headers
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgTerrain/Layer>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::Layer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::ImageLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::ContourLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::HeightFieldLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::ProxyLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::CompositeLayer)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::Terrain)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgTerrain::TerrainTile)

bool writePrettyHTMLImpl<osgTerrain::Layer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::Layer * object = static_cast<osgTerrain::Layer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
            callNextHandler(os);

            os << "<tr><td>set name</td><td>" << object->getSetName() << "</td></tr>" << std::endl;
            os << "<tr><td>file name</td><td>" << object->getFileName() << "</td></tr>" << std::endl;
            os << "<tr><td>compound name</td><td>" << object->getCompoundName() << "</td></tr>" << std::endl;

            os << "<tr><td>min level</td><td>" << object->getMinLevel() << "</td></tr>" << std::endl;
            os << "<tr><td>max level</td><td>" << object->getMaxLevel() << "</td></tr>" << std::endl;

            os << "<tr><td>columns</td><td>" << object->getNumColumns() << "</td></tr>" << std::endl;
            os << "<tr><td>rows</td><td>" << object->getNumRows() << "</td></tr>" << std::endl;
            os << "<tr><td>default value</td><td>" << object->getDefaultValue() << "</td></tr>" << std::endl;

            os << "<tr><td>min filter</td><td>" << object->getMinFilter() << "</td></tr>" << std::endl;
            os << "<tr><td>mag filter</td><td>" << object->getMagFilter() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgTerrain::ImageLayer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::ImageLayer * object = static_cast<osgTerrain::ImageLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
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

bool writePrettyHTMLImpl<osgTerrain::ContourLayer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::ContourLayer * object = static_cast<osgTerrain::ContourLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
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

bool writePrettyHTMLImpl<osgTerrain::HeightFieldLayer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::HeightFieldLayer * object = static_cast<osgTerrain::HeightFieldLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
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

bool writePrettyHTMLImpl<osgTerrain::ProxyLayer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::ProxyLayer * object = static_cast<osgTerrain::ProxyLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
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

bool writePrettyHTMLImpl<osgTerrain::CompositeLayer>::process(std::basic_ostream<char>& os)
{
    osgTerrain::CompositeLayer * object = static_cast<osgTerrain::CompositeLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgTerrain::Layer properties first
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

bool writePrettyHTMLImpl<osgTerrain::Terrain>::process(std::basic_ostream<char>& os)
{
    osgTerrain::Terrain * object = static_cast<osgTerrain::Terrain*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::CoordinateSystemNode properties first
            callNextHandler(os);

            os << "<tr><td>sample ratio</td><td>" << object->getSampleRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>vert scale</td><td>" << object->getVerticalScale() << "</td></tr>" << std::endl;
            os << "<tr><td>equalize boundaries</td><td>" << (object->getEqualizeBoundaries()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>blending policy</td><td>" << object->getBlendingPolicy() << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgTerrain::TileID & id)
{
    os << "{level=" << id.level
        << ";x=" << id.x
        << ";y=" << id.y
        << "}";
    return os;
}

bool writePrettyHTMLImpl<osgTerrain::TerrainTile>::process(std::basic_ostream<char>& os)
{
    osgTerrain::TerrainTile * object = static_cast<osgTerrain::TerrainTile*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::Group properties first
            callNextHandler(os);

            os << "<tr><td>tile key</td><td>" << object->getTileID() << "</td></tr>" << std::endl;
            os << "<tr><td>elevation layer</td><td>" << (object->getElevationLayer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>num color layers</td><td>" << object->getNumColorLayers() << "</td></tr>" << std::endl;

            os << "<tr><td>terrain technique</td><td>" << object->getTerrainTechnique() << "</td></tr>" << std::endl;
            os << "<tr><td>locator</td><td>" << object->getLocator() << "</td></tr>" << std::endl;
            os << "<tr><td>requires normals</td><td>" << (object->getRequiresNormals()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>TreatBoundariesToValidDataAsDefaultValue</td><td>" << (object->getTreatBoundariesToValidDataAsDefaultValue()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>blending policy</td><td>" << object->getBlendingPolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>dirty mask</td><td>" << object->getDirtyMask() << "</td></tr>" << std::endl;

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

} // namespace osg_plugin
} // namespace sgi

