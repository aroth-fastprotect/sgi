#pragma once

namespace osgTerrain {
    class Layer;
    class ImageLayer;
    class ContourLayer;
    class HeightFieldLayer;
    class ProxyLayer;
    class CompositeLayer;
    class Terrain;
    class TerrainTile;
}

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::Layer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::ImageLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::ContourLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::HeightFieldLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::ProxyLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::CompositeLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::Terrain)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgTerrain::TerrainTile)

} // namespace osg_plugin
} // namespace sgi
