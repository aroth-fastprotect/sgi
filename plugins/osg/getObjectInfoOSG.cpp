#include "stdafx.h"
#include "getObjectInfoOSG.h"
#include <sgi/helpers/osg>
#include <sgi/helpers/string>
#include <sgi/plugins/SGIImage.h>

#include "SGIItemOsg"

#include <osg/PagedLOD>
#include <osg/Image>
#include <osg/Camera>
#include <osg/Shape>
#include <osg/GraphicsContext>
#include <osg/OperationThread>
#include <osgViewer/View>
#include <osgDB/FileNameUtils>
#include <osgDB/WriteFile>

#include <sgi/helpers/rtti>
#include <sgi/helpers/html>

#include "ObjectLoggerOSG.h"

namespace sgi {

namespace osg_plugin {

GET_OBJECT_NAME_IMPL_REGISTER(osg::Referenced)
GET_OBJECT_NAME_IMPL_REGISTER(osg::Object)
GET_OBJECT_NAME_IMPL_REGISTER(osg::Node)
GET_OBJECT_NAME_IMPL_REGISTER(osg::Group)
GET_OBJECT_NAME_IMPL_REGISTER(osg::LOD)
GET_OBJECT_NAME_IMPL_REGISTER(osg::PagedLOD)
GET_OBJECT_NAME_IMPL_REGISTER(osg::Operation)
GET_OBJECT_NAME_IMPL_REGISTER(osg::Image)
GET_OBJECT_NAME_IMPL_REGISTER(osg::GraphicsContext)
GET_OBJECT_NAME_IMPL_REGISTER(osgDB::Registry)
GET_OBJECT_NAME_IMPL_REGISTER(osgDB::BaseSerializer)

GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(CullingNodeInfo)

GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(osg::Referenced)
GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(osg::Object)
GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(osg::Operation)
GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(osgDB::Registry)
GET_OBJECT_DISPLAYNAME_IMPL_REGISTER(osgDB::BaseSerializer)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(CullingNodeInfo)

GET_OBJECT_TYPE_IMPL_REGISTER(osg::Referenced)
GET_OBJECT_TYPE_IMPL_REGISTER(osg::Object)
GET_OBJECT_SUGGESTED_FILENAME_IMPL_REGISTER(osg::Object)
GET_OBJECT_SUGGESTED_FILENAME_IMPL_REGISTER(osg::Image)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_REGISTER(osg::Object)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_REGISTER(osg::Image)
GET_OBJECT_SUGGESTED_FILENAME_EXTENSION_IMPL_REGISTER(osg::HeightField)
GET_OBJECT_FILENAME_FILTERS_IMPL_REGISTER(osg::Object)
GET_OBJECT_FILENAME_FILTERS_IMPL_REGISTER(osg::Image)
GET_OBJECT_FILENAME_FILTERS_IMPL_REGISTER(osg::HeightField)
GET_OBJECT_PATH_IMPL_REGISTER(osg::Node)
HAS_CALLBACK_IMPL_REGISTER(osg::Node)
HAS_CALLBACK_IMPL_REGISTER(osg::Camera)
HAS_CALLBACK_IMPL_REGISTER(osg::StateSet)
HAS_CALLBACK_IMPL_REGISTER(osg::GraphicsContext)
HAS_CALLBACK_IMPL_REGISTER(osgGA::CameraManipulator)
WRITE_OBJECT_FILE_IMPL_REGISTER(osg::Object)
WRITE_OBJECT_FILE_IMPL_REGISTER(osg::Node)
WRITE_OBJECT_FILE_IMPL_REGISTER(osg::Image)
WRITE_OBJECT_FILE_IMPL_REGISTER(osg::HeightField)

CONVERT_TO_IMAGE_CONVERT_IMPL_DECLARE_AND_REGISTER(osg::Image)
CONVERT_TO_IMAGE_CONVERT_IMPL_DECLARE_AND_REGISTER(osg::HeightField)

using namespace sgi::osg_helpers;

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
std::string getObjectNameImpl<osg::Referenced>::process()
{
    osg::Referenced * object = getObject<osg::Referenced, SGIItemOsg>();
    return helpers::getRTTIObjectNameAndType(object);
}

std::string getObjectNameImpl<osg::Object>::process()
{
    osg::Object * object = static_cast<osg::Object*>(item<SGIItemOsg>()->object());
	std::string classname = getObjectTypename(object);
	std::string name = object->getName();
	std::string ret;
	if (name.empty())
	{
		std::stringstream ss;
		ss << helpers::getRTTITypenameShort(object) << "(" << (void*)object << ")";
		name = ss.str();
	}
	if (classname.empty())
		ret = name;
	else
		ret = classname + "/" + name;
	return ret;
}

std::string getObjectNameImpl<osg::Node>::process()
{
    getObjectNameImpl<osg::Object> f(_hostInterface, _item);
    return f.process();
}

std::string getObjectNameImpl<osg::Group>::process()
{
    getObjectNameImpl<osg::Node> f(_hostInterface, _item);
    return f.process();
}

std::string getObjectNameImpl<osg::LOD>::process()
{
    getObjectNameImpl<osg::Group> f(_hostInterface, _item);
    return f.process();
}

std::string getObjectNameImpl<osg::PagedLOD>::process()
{
    osg::PagedLOD * object = static_cast<osg::PagedLOD*>(item<SGIItemOsg>()->object());

    std::string classname = getObjectTypename(object);
    std::string name = object->getName();
    if(name.empty())
    {
        std::stringstream ss;
        ss << helpers::getRTTITypenameShort(object) << "(" << (void*)object << ")";
        name = ss.str();
    }

    std::stringstream ss;
    unsigned numFiles = object->getNumFileNames();
    if(numFiles > 2)
        ss << "PagedLOD(" << name << ";" << numFiles << " childs)";
    else if(numFiles == 2)
    {
        const std::string & filename0 = object->getFileName(0);
        const std::string & filename1 = object->getFileName(1);
        ss << "PagedLOD(" << name << ";" << filename0 << "," << filename1 << ")";
    }
    else if(numFiles == 1)
    {
        const std::string & filename0 = object->getFileName(0);
        ss << "PagedLOD(" << name << ";" << filename0 << ")";
    }
    else
        ss << "PagedLOD(" << name << ";empty)";
    return classname + "/" + ss.str();
}

std::string getObjectNameImpl<osg::Operation>::process()
{
    osg::Operation * object = dynamic_cast<osg::Operation*>(item<SGIItemOsg>()->object());
    std::string ret = object->getName();
	if (!ret.empty())
		return ret;
	getObjectNameImpl<osg::Referenced> f(_hostInterface, _item);
	return f.process();
}

std::string getObjectNameImpl<osg::Image>::process()
{
	osg::Image * object = dynamic_cast<osg::Image*>(item<SGIItemOsg>()->object());
	std::string ret = object->getName();
	if (ret.empty())
		ret = object->getFileName();
	if (!ret.empty())
		return ret;
	getObjectNameImpl<osg::Object> f(_hostInterface, _item);
	return f.process();
}

std::string getObjectNameImpl<osg::GraphicsContext>::process()
{
	osg::GraphicsContext * object = dynamic_cast<osg::GraphicsContext*>(item<SGIItemOsg>()->object());
	std::string ret = object->getName();
	if (ret.empty())
	{
		osg::State * state = object->getState();
		if (state)
		{
			std::stringstream ss;
			ss << "Ctx" << state->getContextID() << " " << helpers::getRTTITypenameShort(object) << "(" << (void*)object << ")";
			ret = ss.str();
		}
	}
	if (!ret.empty())
		return ret;
	getObjectNameImpl<osg::Object> f(_hostInterface, _item);
	return f.process();
}

std::string getObjectNameImpl<osgDB::Registry>::process()
{
    return "osgDB::Registry";
}

std::string getObjectNameImpl<osgDB::BaseSerializer>::process()
{
    osgDB::BaseSerializer * object = dynamic_cast<osgDB::BaseSerializer*>(item<SGIItemOsg>()->object());
    return object->getName();
}

std::string getObjectNameImpl<CullingNodeInfo>::process()
{
    CullingNodeInfo* object = static_cast<CullingNodeInfo*>(item<SGIItemOsg>()->object());
    SGIHostItemOsg node(object->node);
    std::string ret;
    _hostInterface->getObjectName(ret, &node);
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectDisplayNameImpl
//--------------------------------------------------------------------------------
std::string getObjectDisplayNameImpl<osg::Referenced>::process()
{
    osg::Referenced * object = getObject<osg::Referenced, SGIItemOsg>();
    return helpers::getRTTIObjectNameAndType(object);
}

std::string getObjectDisplayNameImpl<osg::Object>::process()
{
    osg::Object * object = static_cast<osg::Object*>(item<SGIItemOsg>()->object());

    std::string name = object->getName();
    std::string ret;
    if(name.empty())
    {
        std::string classname = getObjectTypename(object);
        std::stringstream ss;
        ss << classname << "(" << (void*)object << ")";
        ret = ss.str();
    }
    else
        ret = name;
    return ret;
}

std::string getObjectDisplayNameImpl<osg::Operation>::process()
{
    osg::Operation * object = dynamic_cast<osg::Operation*>(item<SGIItemOsg>()->object());
    return object->getName();
}

std::string getObjectDisplayNameImpl<osgDB::Registry>::process()
{
    return "osgDB::Registry";
}

std::string getObjectDisplayNameImpl<osgDB::BaseSerializer>::process()
{
    osgDB::BaseSerializer * object = dynamic_cast<osgDB::BaseSerializer*>(item<SGIItemOsg>()->object());
    return object->getName();
}

std::string getObjectDisplayNameImpl<CullingNodeInfo>::process()
{
    CullingNodeInfo* object = static_cast<CullingNodeInfo*>(item<SGIItemOsg>()->object());
    SGIHostItemOsg node(object->node);
    std::string ret;
    _hostInterface->getObjectDisplayName(ret, &node);
    return ret;
}
//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
std::string getObjectTypeImpl<osg::Referenced>::process()
{
    osg::Referenced * object = getObject<osg::Referenced, SGIItemOsg>();
    return helpers::getRTTITypename(object);
}

std::string getObjectTypeImpl<osg::Object>::process()
{
    osg::Object * object = static_cast<osg::Object*>(item<SGIItemOsg>()->object());
    std::string ret;
    std::string classname = getObjectTypename(object);
    std::string rtticlass = helpers::getRTTITypename(object);

    if(helpers::is_same_class(classname, rtticlass))
        ret = classname;
    else
        ret = classname + '(' + rtticlass + ')';

    return ret;
}


//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------

std::string getObjectSuggestedFilenameImpl<osg::Object>::process()
{
    osg::Object * object = static_cast<osg::Object*>(item<SGIItemOsg>()->object());
    std::string ret;

    getObjectNameImpl<osg::Object> objectNameFunc(_hostInterface, _item);
    ret = objectNameFunc.process();
    helpers::replaceIn(ret, "::", "_");
    helpers::replaceIn(ret, "/", "_");
    helpers::replaceIn(ret, ":", "_");
    helpers::replaceIn(ret, " ", "_");
    return ret;
}

std::string getObjectSuggestedFilenameImpl<osg::Image>::process()
{
    osg::Image * object = static_cast<osg::Image*>(item<SGIItemOsg>()->object());
    std::string ret;

    const std::string & filename = object->getFileName();
    ret = osgDB::getSimpleFileName(filename);
    if(ret.empty())
    {
        getObjectSuggestedFilenameImpl<osg::Object> objectNameFunc(_hostInterface, _item);
        ret = objectNameFunc.process();
    }
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------

std::string getObjectSuggestedFilenameExtensionImpl<osg::Object>::process()
{
    return "osgb";
}

std::string getObjectSuggestedFilenameExtensionImpl<osg::Image>::process()
{
    osg::Image * object = getObject<osg::Image,SGIItemOsg>();
    std::string ret;

    const std::string & filename = object->getFileName();
    if(filename.empty())
        ret = "dds";
    else
    {
        ret = osgDB::getFileExtension(filename);
        if(ret.empty())
            ret = "png";
    }
    return ret;
}

std::string getObjectSuggestedFilenameExtensionImpl<osg::HeightField>::process()
{
//    return "tiff";
    return "osgb";
}

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------
std::vector<std::string> getObjectFilenameFiltersImpl<osg::Object>::process()
{
    std::vector<std::string> ret;
    ret.push_back("OSG Files (*.osgb *.osgx *.osgt *.osg *.ive)");
    return ret;
}

std::vector<std::string> getObjectFilenameFiltersImpl<osg::Image>::process()
{
    osg::Image * object = static_cast<osg::Image*>(item<SGIItemOsg>()->object());
    std::vector<std::string> ret;
    ret.push_back("Image Files (*.png *.jpg *.bmp *.gif *.tiff *.dds)");
    return ret;
}

std::vector<std::string> getObjectFilenameFiltersImpl<osg::HeightField>::process()
{
    std::vector<std::string> ret;
    ret.push_back("OSG Files (*.osgb *.osgx *.osgt *.osg *.ive)");
    ret.push_back("Height Field Files (*.tif *.tiff)");
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------

SGIItemBasePtrPath getObjectPathImpl<osg::Node>::process()
{
    osg::Node* object = static_cast<osg::Node*>(item<SGIItemOsg>()->object());
    SGIItemBasePtrPath ret;
    osg::Node* firstParent = nullptr;
    osg::NodePathList parentPaths = object->getParentalNodePaths();
    if(!parentPaths.empty())
    {
        const osg::NodePath & firstParentalPath = parentPaths.front();
        for(osg::NodePath::const_iterator it = firstParentalPath.begin(); it != firstParentalPath.end(); it++)
        {
            SGIHostItemOsg pathHostItem(*it);
            SGIItemBasePtr pathItem;
            if(_hostInterface->generateItem(pathItem, &pathHostItem))
                ret.push_back(pathItem);
        }
    }
    return ret;
}

//--------------------------------------------------------------------------------
// hasCallbackImpl
//--------------------------------------------------------------------------------

bool hasCallbackImpl<osg::Node>::process()
{
    osg::Node* object = getObject<osg::Node,SGIItemOsg>();
    bool ret = callNextHandler();
    if(!ret && object->getUpdateCallback())
        ret = true;
    if(!ret && object->getEventCallback())
        ret = true;
    if(!ret && object->getCullCallback())
        ret = true;
    return ret;
}

bool hasCallbackImpl<osg::Camera>::process()
{
    osg::Camera* object = getObject<osg::Camera,SGIItemOsg>();
    bool ret = callNextHandler();
    if(!ret && object->getInitialDrawCallback())
        ret = true;
    if(!ret && object->getPreDrawCallback())
        ret = true;
    if(!ret && object->getPostDrawCallback())
        ret = true;
    if(!ret && object->getFinalDrawCallback())
        ret = true;
    // from CullSettings
    if(!ret && object->getClampProjectionMatrixCallback())
        ret = true;
    return ret;
}

bool hasCallbackImpl<osg::StateSet>::process()
{
    osg::StateSet* object = getObject<osg::StateSet,SGIItemOsg>();
    bool ret = callNextHandler();
    if(!ret && object->getUpdateCallback())
        ret = true;
    if(!ret && object->getEventCallback())
        ret = true;
    return ret;
}

bool hasCallbackImpl<osg::GraphicsContext>::process()
{
    osg::GraphicsContext* object = getObject<osg::GraphicsContext,SGIItemOsg>();
    bool ret = callNextHandler();
    if(!ret && object->getSwapCallback())
        ret = true;
    if(!ret && object->getResizedCallback())
        ret = true;
    return ret;
}

bool hasCallbackImpl<osgGA::CameraManipulator>::process()
{
    osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator,SGIItemOsg,DynamicCaster>();
    bool ret = callNextHandler();
    if(!ret && object->getCoordinateFrameCallback())
        ret = true;
    return ret;
}

//--------------------------------------------------------------------------------
// writeObjectFileImpl
//--------------------------------------------------------------------------------

bool writeObjectFileImpl<osg::Object>::process(const std::string& filename, const SGIItemBase* options)
{
    osg::Object* object = getObject<osg::Object,SGIItemOsg>();
    bool ret = false;

    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const SGIItemOsg* optionsOsg = dynamic_cast<const SGIItemOsg*>(options);
            const osgDB::Options * inputOptions = dynamic_cast<const osgDB::Options*>(optionsOsg->object());
            const bool noOptimizer = true;
            osg::ref_ptr<osgDB::Options> cloned_opts =
                inputOptions ? static_cast<osgDB::Options*>(inputOptions->clone(osg::CopyOp::SHALLOW_COPY)) :
                osgDB::Registry::instance()->getOptions();

            if(!cloned_opts.valid())
                cloned_opts = new osgDB::Options;

            cloned_opts->setPluginStringData("noOptimizer",(noOptimizer==true)?"true":"false");
            cloned_opts->setPluginStringData("WriteImageHint", "IncludeData");

            ret = osgDB::writeObjectFile(*object, filename, cloned_opts);
        }
        break;
    }
    return ret;
}

bool writeObjectFileImpl<osg::Node>::process(const std::string& filename, const SGIItemBase* options)
{
    osg::Node* object = getObject<osg::Node,SGIItemOsg>();
    bool ret = false;
    const bool noOptimizer = true;
    const SGIItemOsg* optionsOsg = dynamic_cast<const SGIItemOsg*>(options);
    const osgDB::Options * inputOptions = dynamic_cast<const osgDB::Options*>(optionsOsg->object());
    osg::ref_ptr<osgDB::Options> cloned_opts =
        inputOptions ? static_cast<osgDB::Options*>(inputOptions->clone(osg::CopyOp::SHALLOW_COPY)) :
        osgDB::Registry::instance()->getOptions();

    if(!cloned_opts.valid())
        cloned_opts = new osgDB::Options;

    cloned_opts->setPluginStringData("noOptimizer",(noOptimizer==true)?"true":"false");

    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = osgDB::writeNodeFile(*object, filename, cloned_opts);
        break;
    }
    return ret;
}

bool writeObjectFileImpl<osg::Image>::process(const std::string& filename, const SGIItemBase* options)
{
    osg::Image* object = getObject<osg::Image,SGIItemOsg>();
    bool ret = false;
    const SGIItemOsg* optionsOsg = dynamic_cast<const SGIItemOsg*>(options);
    const osgDB::Options * inputOptions = dynamic_cast<const osgDB::Options*>(optionsOsg->object());
    const bool noOptimizer = true;
    osg::ref_ptr<osgDB::Options> cloned_opts =
        inputOptions ? static_cast<osgDB::Options*>(inputOptions->clone(osg::CopyOp::SHALLOW_COPY)) :
        osgDB::Registry::instance()->getOptions();

    if(!cloned_opts.valid())
        cloned_opts = new osgDB::Options;

    cloned_opts->setPluginStringData("noOptimizer",(noOptimizer==true)?"true":"false");

    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = osgDB::writeImageFile(*object, filename, cloned_opts);
        break;
    }
    return ret;
}

bool writeObjectFileImpl<osg::HeightField>::process(const std::string& filename, const SGIItemBase* options)
{
    osg::HeightField* object = getObject<osg::HeightField, SGIItemOsg>();
    bool ret = false;
    const SGIItemOsg* optionsOsg = dynamic_cast<const SGIItemOsg*>(options);
    const osgDB::Options * inputOptions = dynamic_cast<const osgDB::Options*>(optionsOsg->object());
    const bool noOptimizer = true;
    osg::ref_ptr<osgDB::Options> cloned_opts =
        inputOptions ? static_cast<osgDB::Options*>(inputOptions->clone(osg::CopyOp::SHALLOW_COPY)) :
        osgDB::Registry::instance()->getOptions();

    if (!cloned_opts.valid())
        cloned_opts = new osgDB::Options;

    cloned_opts->setPluginStringData("noOptimizer", (noOptimizer == true) ? "true" : "false");

    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = osgDB::writeHeightFieldFile(*object, filename, cloned_opts);
        if(!ret)
            ret = osgDB::writeObjectFile(*object, filename, cloned_opts);
        break;
    }
    return ret;
}
//--------------------------------------------------------------------------------
// convertToImageConvertImpl
//--------------------------------------------------------------------------------
bool convertToImageConvertImpl<osg::Image>::convert()
{
    osg::Image* object = getObject<osg::Image,SGIItemOsg>();

    *_image = const_cast<sgi::Image*>(osg_helpers::convertImage(object));
    return true;
}

bool convertToImageConvertImpl<osg::HeightField>::convert()
{
    osg::HeightField* object = getObject<osg::HeightField, SGIItemOsg>();

    *_image = const_cast<sgi::Image*>(osg_helpers::convertImage(object));
    return true;
}

} // namespace osg_plugin
} // namespace sgi
