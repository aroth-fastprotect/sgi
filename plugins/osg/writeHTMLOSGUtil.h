#pragma once

namespace osgUtil {
    class RenderBin;
    class RenderStage;
    class RenderLeaf;
	class IncrementalCompileOperation;
}

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgUtil::RenderBin)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgUtil::RenderStage)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgUtil::RenderLeaf)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgUtil::IncrementalCompileOperation)

} // namespace osg_plugin
} // namespace sgi
