#pragma once

#include <sgi/plugins/GetObjectInfoImpl>
#include <sgi/plugins/ConvertToImageImpl>
#include <sgi/plugins/SGIHostItemOsg.h>

namespace osg {
    class Object;
    class Node;
    class Group;
    class Camera;
    class StateSet;
    class LOD;
    class PagedLOD;
    class HeightField;
    class GraphicsContext;
    class Operation;
}
namespace osgDB {
    class Registry;
    class BaseSerializer;
}

namespace osgViewer {
    class View;
}

namespace osgGA {
    class CameraManipulator;
}

namespace OpenThreads {
    class Thread;
}

namespace sgi {

namespace osg_plugin {

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_NAME_IMPL_TEMPLATE()
GET_OBJECT_NAME_IMPL_DECLARE(osg::Referenced)
GET_OBJECT_NAME_IMPL_DECLARE(osg::Object)
GET_OBJECT_NAME_IMPL_DECLARE(osg::Node)
GET_OBJECT_NAME_IMPL_DECLARE(osg::Group)
GET_OBJECT_NAME_IMPL_DECLARE(osg::LOD)
GET_OBJECT_NAME_IMPL_DECLARE(osg::PagedLOD)
GET_OBJECT_NAME_IMPL_DECLARE(osg::Operation)
GET_OBJECT_NAME_IMPL_DECLARE(osg::Image)
GET_OBJECT_NAME_IMPL_DECLARE(osg::GraphicsContext)
GET_OBJECT_NAME_IMPL_DECLARE(osgDB::Registry)
GET_OBJECT_NAME_IMPL_DECLARE(osgDB::BaseSerializer)

//--------------------------------------------------------------------------------
// getObjectDisplayNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_DISPLAYNAME_IMPL_TEMPLATE()
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(osg::Referenced)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(osg::Object)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(osg::Operation)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(osgDB::Registry)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE(osgDB::BaseSerializer)

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE(osg::Referenced)
GET_OBJECT_TYPE_IMPL_DECLARE(osg::Object)

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_IMPL_TEMPLATE()
GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(osg::Object)
GET_OBJECT_SUGGESTED_FILENAME_IMPL_DECLARE(osg::Image)

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_TEMPLATE()
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(osg::Object)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(osg::Image)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_DECLARE(osg::HeightField)

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------
GET_OBJECT_FILENAME_FILTERS_IMPL_TEMPLATE()
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(osg::Object)
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(osg::Image)
GET_OBJECT_FILENAME_FILTERS_IMPL_DECLARE(osg::HeightField)

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------
GET_OBJECT_PATH_IMPL_TEMPLATE()
GET_OBJECT_PATH_IMPL_DECLARE(osg::Node)

//--------------------------------------------------------------------------------
// hasCallbackImpl
//--------------------------------------------------------------------------------
HAS_CALLBACK_IMPL_TEMPLATE()
HAS_CALLBACK_IMPL_DECLARE(osg::Node)
HAS_CALLBACK_IMPL_DECLARE(osg::Camera)
HAS_CALLBACK_IMPL_DECLARE(osg::StateSet)
HAS_CALLBACK_IMPL_DECLARE(osg::GraphicsContext)
HAS_CALLBACK_IMPL_DECLARE(osgGA::CameraManipulator)

//--------------------------------------------------------------------------------
// writeObjectFileImpl
//--------------------------------------------------------------------------------
WRITE_OBJECT_FILE_IMPL_TEMPLATE()
WRITE_OBJECT_FILE_IMPL_DECLARE(osg::Object)
WRITE_OBJECT_FILE_IMPL_DECLARE(osg::Node)
WRITE_OBJECT_FILE_IMPL_DECLARE(osg::Image)
WRITE_OBJECT_FILE_IMPL_DECLARE(osg::HeightField)

//--------------------------------------------------------------------------------
// convertToImageConvertImpl
//--------------------------------------------------------------------------------
CONVERT_TO_IMAGE_CONVERT_IMPL_TEMPLATE()

} // namespace osg_plugin
} // namespace sgi
