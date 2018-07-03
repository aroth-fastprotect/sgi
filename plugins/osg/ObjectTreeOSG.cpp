#include "stdafx.h"
#include "ObjectTreeOSG.h"
#include <sgi/helpers/osg>
#include "SGIItemOsg"
#include "getObjectInfoOSG.h"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/string>
#include <sgi/helpers/osg_helper_nodes>

#include <osg/UserDataContainer>
#include <osg/ProxyNode>
#include <osg/PagedLOD>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/ShapeDrawable>
#include <osg/ValueObject>

#include <osgDB/Registry>
#define protected public
#include <osgDB/DatabasePager>
#undef protected
#include <osgDB/ImagePager>
#include <osgDB/ObjectCache>

#include <osgViewer/View>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#include <osgQt/QObjectWrapper>
#include <sgi/plugins/SGIItemQt>
#endif

#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgTerrain/Layer>

#include <osgText/Font>
#include <osgText/Text>

#include <osgAnimation/Animation>
#include <osgAnimation/AnimationManagerBase>

#include <sgi/ReferencedPicker>

#include "osgdb_accessor.h"
#include "DrawableHelper.h"
#include "ObjectLoggerOSG.h"
#include "osg_accessor.h"
#include "osganimation_accessor.h"
#include "osgviewer_accessor.h"
#include "osgtext_accessor.h"
#include <sys/stat.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace sgi {

namespace osg_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Referenced)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::ObserverSet)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Object)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::UserDataContainer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Node)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Group)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::StateSet)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Geode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Drawable)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::ShapeDrawable)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Geometry)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Transform)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Camera)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::View)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Program)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Program::PerContextProgram)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Shader)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Uniform)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::GraphicsContext)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::NodeVisitor)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Stats)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::ProxyNode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::PagedLOD)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::StateAttribute)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObject)
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::TextureObjectSet)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::TextureObjectManager)
#else
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObjectSet)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObjectManager)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture2D)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Texture3D)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Image)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::HeightField)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::BufferData)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::Array)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::BufferObject)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::PrimitiveSet)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::DrawElements)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::State)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::ShaderComposer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osg::ShaderComponent)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(CullingNodeInfo)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(CullingInfo)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(CullingInfoForCamera)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(CullingInfoRegistry)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::Registry)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::Options)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::ReaderWriter)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::ImagePager)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabaseRevision)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabaseRevisions)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::FileCache)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectCache)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::FileList)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectWrapper)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectWrapperManager)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::DatabaseRequest)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::RequestQueue)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::ReadQueue)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::ViewerBase)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::View)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::GraphicsWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::Scene)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::Renderer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::HelpHandler)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgViewer::StatsHandler)

#ifdef SGI_USE_OSGQT
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgQt::GraphicsWindowQt)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgQt::QObjectWrapper)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgQt::GLWidget)
#endif

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgGA::GUIEventHandler)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgGA::GUIEventAdapter)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgGA::CameraManipulator)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgUtil::RenderBin)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgUtil::RenderStage)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgUtil::StateGraph)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgUtil::RenderLeaf)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgUtil::SceneView)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::Terrain)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::TerrainTile)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::Layer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::ImageLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::ContourLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::HeightFieldLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::ProxyLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgTerrain::CompositeLayer)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgText::Font)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgText::TextBase)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgText::Text)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgAnimation::Animation)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgAnimation::AnimationManagerBase)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(RenderInfoDrawable)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(RenderInfoGeometry)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(RenderInfoDrawCallback)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedPickerBase)
#ifdef SGI_USE_OSGEARTH
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedPicker)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedLinePicker)

using namespace sgi::osg_helpers;

extern bool objectInfo_hasCallback(SGIPluginHostInterface * hostInterface, bool & result, const SGIItemBase * object);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::Type & t);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::BufferComponent t);

bool objectTreeBuildImpl<osg::Referenced>::build(IObjectTreeItem * treeItem)
{
    osg::Referenced * object = getObject<osg::Referenced,SGIItemOsg>();

    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            SGIHostItemOsg observerSet(object->getObserverSet());
            if(observerSet.hasObject())
                treeItem->addChild("ObserverSet", &observerSet);
        }
        break;
    }
    return true;
}

bool objectTreeBuildImpl<osg::ObserverSet>::build(IObjectTreeItem * treeItem)
{
    bool ret = false;
    osg::ObserverSet * object = getObject<osg::ObserverSet,SGIItemOsg>();

    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg observerdObject(object->getObserverdObject());
            treeItem->addChild(std::string(), &observerdObject);

            const osg::ObserverSet::Observers& observers = object->getObservers();
            if(!observers.empty())
                treeItem->addChild(helpers::str_plus_count("Observers", observers.size()), _item->clone<SGIItemOsg>(SGIItemTypeObservers));
        }
        break;
    case SGIItemTypeObservers:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Object>::build(IObjectTreeItem * treeItem)
{
    osg::Object * object = getObject<osg::Object,SGIItemOsg>();
    bool ret;

    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg userData(object->getUserData());
            if(userData.hasObject())
                treeItem->addChild("User data", &userData);

            SGIHostItemOsg userDataContainer(object->getUserDataContainer());
            if(userDataContainer.hasObject())
                treeItem->addChild("User data container", &userDataContainer);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::UserDataContainer>::build(IObjectTreeItem * treeItem)
{
    osg::UserDataContainer * object = getObject<osg::UserDataContainer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numUserObjects = object->getNumUserObjects();
            for(unsigned n = 0; n < numUserObjects; n++)
            {
                osg::Object * uobj = object->getUserObject(n);
                if (uobj)
                {
                    SGIHostItemOsg userDataObject(uobj);
                    if(uobj->getName().empty())
                        treeItem->addChild(helpers::str_plus_count("UserDataObject", n), &userDataObject);
                    else
                        treeItem->addChild(helpers::str_plus_info("UserDataObject", uobj->getName()), &userDataObject);
                }
            }
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Node>::build(IObjectTreeItem * treeItem)
{
    osg::Node * object = getObject<osg::Node,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numParents = object->getNumParents();
            if(numParents)
            {
                treeItem->addChild(helpers::str_plus_count("Parents", numParents), cloneItem<SGIItemOsg>(SGIItemTypeParents));
                treeItem->addChild("ParentNodePaths", cloneItem<SGIItemOsg>(SGIItemTypeParentalNodePath, ~0u));
            }

            if (object->getEventCallback() ||
                object->getUpdateCallback() ||
                object->getCullCallback() ||
                object->getComputeBoundingSphereCallback() )
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

            SGIHostItemOsg stateSet(object->getStateSet());
            if(stateSet.hasObject())
                treeItem->addChild("StateSet", &stateSet);

            treeItem->addChild("Statistics", cloneItem<SGIItemOsg>(SGIItemTypeStatistics));
        }
        break;
    case SGIItemTypeParents:
        {
            unsigned numParents = object->getNumParents();
            for(unsigned i = 0; i < numParents; i++)
            {
                SGIHostItemOsg parentItem(object->getParent(i));
                treeItem->addChild(std::string(), &parentItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeParentalNodePath:
        {
            unsigned numPath = _item->number();
            const osg::NodePathList parentalNodePaths = object->getParentalNodePaths();
            unsigned parentalNodePathNum = 0;
            if(numPath == ~0u)
            {
                for(osg::NodePathList::const_iterator itPathList = parentalNodePaths.begin(); itPathList != parentalNodePaths.end(); itPathList++, parentalNodePathNum++)
                    treeItem->addChild(helpers::str_plus_number("Path", parentalNodePathNum), cloneItem<SGIItemOsg>(SGIItemTypeParentalNodePath, parentalNodePathNum));
            }
            else
            {
                if(numPath >= parentalNodePaths.size())
                    treeItem->addChild(helpers::str_plus_number("N/A", numPath), (SGIItemBase*)NULL);
                else
                {
                    for(osg::NodePathList::const_iterator itPathList = parentalNodePaths.begin(); itPathList != parentalNodePaths.end(); itPathList++, parentalNodePathNum++)
                    {
                        if(parentalNodePathNum == numPath)
                        {
                            const osg::NodePath & nodePath = *itPathList;
                            for(osg::NodePath::const_iterator it = nodePath.begin(); it != nodePath.end(); it++)
                            {
                                const osg::ref_ptr<osg::Node> & node = *it;
                                SGIHostItemOsg item(node.get());
                                treeItem->addChild(std::string(), &item);
                            }
                            break;
                        }
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStatistics:
        {
            // nothing to be done here
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            callNextHandler(treeItem);
            // add osg::Node callbacks now
#if OSG_VERSION_GREATER_THAN(3,3,1)
            const osg::Callback * cb;
#else
            const osg::NodeCallback * cb;
#endif
            cb = object->getEventCallback();
            while(cb)
            {
                SGIHostItemOsg callbackItem(cb, new SGIRefPtrOsg(object));
                treeItem->addChild("EventCallback", &callbackItem);
                cb = cb->getNestedCallback();
            }
            cb = object->getUpdateCallback();
            while(cb)
            {
                SGIHostItemOsg callbackItem(cb, new SGIRefPtrOsg(object));
                treeItem->addChild("UpdateCallback", &callbackItem);
                cb = cb->getNestedCallback();
            }
            cb = object->getCullCallback();
            while(cb)
            {
                SGIHostItemOsg callbackItem(cb, new SGIRefPtrOsg(object));
                treeItem->addChild("CullCallback", &callbackItem);
                cb = cb->getNestedCallback();
            }

            SGIHostItemOsg computeBoundingSphereCallback(object->getComputeBoundingSphereCallback(), new SGIRefPtrOsg(object));
            if(computeBoundingSphereCallback.hasObject())
                treeItem->addChild("ComputeBoundingSphereCallback", &computeBoundingSphereCallback);
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Group>::build(IObjectTreeItem * treeItem)
{
    osg::Group * object = getObject<osg::Group,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osg::LOD * lod = dynamic_cast<const osg::LOD*>(object);
            const osg::PagedLOD * plod = dynamic_cast<const osg::PagedLOD*>(object);
            const osg::ProxyNode * proxynode = dynamic_cast<osg::ProxyNode*>(object);
            unsigned numChildren = object->getNumChildren();
            unsigned numRanges = lod?lod->getNumRanges():0;
            unsigned numFileNames = plod?plod->getNumFileNames():(proxynode?proxynode->getNumFileNames():0);
            unsigned numItems = std::max(numChildren, std::max(numRanges, numFileNames) );
            if(numItems)
                treeItem->addChild(helpers::str_plus_count("Childs", numItems), cloneItem<SGIItemOsg>(SGIItemTypeChilds));
        }
        break;
    case SGIItemTypeChilds:
        {
            const osg::LOD * lod = dynamic_cast<const osg::LOD*>(object);
            const osg::PagedLOD * plod = dynamic_cast<const osg::PagedLOD*>(object);
            const osg::ProxyNode * proxynode = dynamic_cast<osg::ProxyNode*>(object);
            unsigned numChildren = object->getNumChildren();
            unsigned numRanges = lod?lod->getNumRanges():0;
            unsigned numFileNames = plod?plod->getNumFileNames():(proxynode?proxynode->getNumFileNames():0);
            unsigned numItems = std::max(numChildren, std::max(numRanges, numFileNames) );
            for(unsigned i = 0; i < numItems; i++)
            {
                if(i < numChildren)
                {
                    osg::Node * child = object->getChild(i);
                    SGIHostItemOsg childItem(child);
                    treeItem->addChild(std::string(), &childItem);
                }
                else
                {
                    treeItem->addChild(helpers::str_plus_number("Child", i), cloneItem<SGIItemOsg>(SGIItemTypePendingChild, i));
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypePendingChild:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }

    return ret;
}

bool objectTreeBuildImpl<osg::Geode>::build(IObjectTreeItem * treeItem)
{
    osg::Geode * object = getObject<osg::Geode,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSG_VERSION_LESS_THAN(3,4,0)
            unsigned numDrawables = object->getNumDrawables();
            if(numDrawables)
                treeItem->addChild(helpers::str_plus_count("Drawables", numDrawables), cloneItem<SGIItemOsg>(SGIItemTypeChilds));
#endif
        }
        break;
#if OSG_VERSION_LESS_THAN(3,4,0)
	case SGIItemTypeChilds:
        {
			unsigned numDrawables = object->getNumDrawables();
            for(unsigned i = 0; i < numDrawables; i++)
            {
                osg::Drawable * drawable = object->getDrawable(i);
                SGIHostItemOsg drawableItem(drawable);
                treeItem->addChild(std::string(), &drawableItem);
            }
            ret = true;
        }
        break;
#endif
	default:
        ret = callNextHandler(treeItem);
        break;
    }

    return ret;
}

bool objectTreeBuildImpl<osg::Drawable>::build(IObjectTreeItem * treeItem)
{
    osg::Drawable * object = getObject<osg::Drawable,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSG_VERSION_LESS_THAN(3,4,0)
            unsigned numParents = object->getNumParents();
            if(numParents)
                treeItem->addChild(helpers::str_plus_count("Parents", numParents), cloneItem<SGIItemOsg>(SGIItemTypeParents));
#endif // OSG_VERSION_LESS_THAN(3,4,0)

#if OSG_VERSION_LESS_THAN(3,4,0)
            SGIHostItemOsg stateSet(object->getStateSet());
            if(stateSet.hasObject())
                treeItem->addChild("StateSet", &stateSet);
#endif // OSG_VERSION_LESS_THAN(3,4,0)

            if(object->getComputeBoundingBoxCallback() ||
                object->getEventCallback() ||
                object->getUpdateCallback() ||
                object->getCullCallback() ||
                object->getDrawCallback()
            )
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

        }
        break;
#if OSG_VERSION_LESS_THAN(3,4,0)
    case SGIItemTypeParents:
        {
            unsigned numParents = object->getNumParents();
            for(unsigned i = 0; i < numParents; i++)
            {
                SGIHostItemOsg parentItem(object->getParent(i));
                treeItem->addChild(std::string(), &parentItem);
            }
            ret = true;
        }
        break;
#endif // OSG_VERSION_LESS_THAN(3,4,0)
    case SGIItemTypeCallbacks:
        {
            callNextHandler(treeItem);

            SGIHostItemOsg computeBoundingBoxCallback(object->getComputeBoundingBoxCallback());
            if(computeBoundingBoxCallback.hasObject())
                treeItem->addChild("ComputeBoundingBoxCallback", &computeBoundingBoxCallback);
            SGIHostItemOsg eventCallback(object->getEventCallback());
            if(eventCallback.hasObject())
                treeItem->addChild("EventCallback", &eventCallback);
            SGIHostItemOsg updateCallback(object->getUpdateCallback());
            if(updateCallback.hasObject())
                treeItem->addChild("UpdateCallback", &updateCallback);
            SGIHostItemOsg cullCallback(object->getCullCallback());
            if(cullCallback.hasObject())
                treeItem->addChild("CullCallback", &cullCallback);
            SGIHostItemOsg drawCallback(object->getDrawCallback());
            if(drawCallback.hasObject())
                treeItem->addChild("DrawCallback", &drawCallback);
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::ShapeDrawable>::build(IObjectTreeItem * treeItem)
{
    osg::ShapeDrawable * object = getObject<osg::ShapeDrawable,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg tessellationHints(object->getTessellationHints());
            if(tessellationHints.hasObject())
                treeItem->addChild("TessellationHints", &tessellationHints);

            SGIHostItemOsg shape(object->getShape());
            if(shape.hasObject())
                treeItem->addChild("Shape", &shape);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Geometry>::build(IObjectTreeItem * treeItem)
{
    osg::Geometry * object = getObject<osg::Geometry,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            bool isImageGeode = false;
            if(!object->getUserValue<bool>("sgi_tree_imagegeode", isImageGeode))
                isImageGeode = false;

            if(isImageGeode)
            {
                osg::StateSet* stateSet = object->getStateSet();
                if(stateSet)
                {
                    osg::StateAttribute * sa = stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
                    osg::Texture * texture = sa ? sa->asTexture() : NULL;
                    if(texture)
                    {
                        std::string itemname;
                        object->getUserValue<std::string>("sgi_tree_itemname", itemname);

                        SGIHostItemOsg image(texture->getImage(0));
                        if(image.hasObject())
                            treeItem->addChild(itemname, &image);
                    }
                }
            }

            SGIHostItemOsg vertexArray(object->getVertexArray());
            if(vertexArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("Vertices", ((const osg::Array * )vertexArray.object())->getNumElements()), &vertexArray);

            SGIHostItemOsg normalArray(object->getNormalArray());
            if(normalArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("Normals", ((const osg::Array * )normalArray.object())->getNumElements()), &normalArray);

            SGIHostItemOsg colorArray(object->getColorArray());
            if(colorArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("Colors", ((const osg::Array * )colorArray.object())->getNumElements()), &colorArray);

            SGIHostItemOsg secondaryColorArray(object->getSecondaryColorArray());
            if(secondaryColorArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("Secondary colors", ((const osg::Array * )secondaryColorArray.object())->getNumElements()), &secondaryColorArray);

            unsigned numVertexAttribArrays = object->getNumVertexAttribArrays();
            if (numVertexAttribArrays)
                treeItem->addChild(helpers::str_plus_count("VertexAttribArrays", numVertexAttribArrays), cloneItem<SGIItemOsg>(SGIItemTypeDrawableVertexAttribArrays));

            unsigned numTexCoordArrays = object->getNumTexCoordArrays();
            if(numTexCoordArrays)
                treeItem->addChild(helpers::str_plus_count("Texture Coordinates", numTexCoordArrays), cloneItem<SGIItemOsg>(SGIItemTypeDrawableTexCoordsList));

            

            unsigned numPrimitiveSets = object->getNumPrimitiveSets();
            if(numPrimitiveSets)
                treeItem->addChild(helpers::str_plus_count("PrimitiveSets", numPrimitiveSets), cloneItem<SGIItemOsg>(SGIItemTypePrimitiveSetList));
        }
        break;
    case SGIItemTypeDrawableVertexAttribArrays:
        {
            unsigned numVertexAttribArrays = object->getNumVertexAttribArrays();
            for(unsigned i = 0; i < numVertexAttribArrays; ++i)
            {
                SGIHostItemOsg vertexAttribArray(object->getVertexAttribArray(i));
                if(vertexAttribArray.hasObject())
                    treeItem->addChild(helpers::str_plus_count(helpers::str_plus_number("VertexAttribArray", i).c_str(), ((const osg::Array * )vertexAttribArray.object())->getNumElements()), &vertexAttribArray);
            }
            ret = true;
        }
        break;
    case SGIItemTypeDrawableTexCoordsList:
        {
            unsigned numTexCoordArrays = object->getNumTexCoordArrays();
            for(unsigned textureUnit = 0; textureUnit < numTexCoordArrays; textureUnit++)
            {
                SGIHostItemOsg texCoordArray(object->getTexCoordArray(textureUnit));
                if(texCoordArray.hasObject())
                    treeItem->addChild(helpers::str_plus_count(helpers::str_plus_number("Texture", textureUnit).c_str(), ((const osg::Array * )texCoordArray.object())->getNumElements()), &texCoordArray);
            }
            ret = true;
        }
        break;
    case SGIItemTypePrimitiveSetList:
        {
            unsigned numPrimitiveSets = object->getNumPrimitiveSets();
            for(unsigned i = 0; i < numPrimitiveSets; i++)
            {
                osg::ref_ptr<osg::PrimitiveSet> primset = object->getPrimitiveSet(i);
                SGIHostItemOsg primitiveSet(primset);
                if(primitiveSet.hasObject())
                    treeItem->addChild(helpers::str_plus_number("PrimitiveSet", i), &primitiveSet);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::StateSet>::build(IObjectTreeItem * treeItem)
{
    osg::StateSet * object = getObject<osg::StateSet,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numParents = object->getNumParents();
            if(numParents)
                treeItem->addChild(helpers::str_plus_count("Parents", numParents), cloneItem<SGIItemOsg>(SGIItemTypeParents));

            const osg::StateSet::ModeList & modelist = object->getModeList();
            if(!modelist.empty())
                treeItem->addChild(helpers::str_plus_count("Modes", modelist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetModeList, ~0u));

            const osg::StateSet::AttributeList & attributelist = object->getAttributeList();
            if(!attributelist.empty())
                treeItem->addChild(helpers::str_plus_count("Attributes", attributelist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetAttributeList, ~0u));

            if(object->getNumTextureModeLists() != 0)
                treeItem->addChild(helpers::str_plus_count("Texture modes", object->getNumTextureModeLists()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetTextureModeList, ~0u));

            if(object->getNumTextureAttributeLists() != 0)
                treeItem->addChild(helpers::str_plus_count("Texture attributes", object->getNumTextureAttributeLists()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetTextureAttributeLists, ~0u));

            const osg::StateSet::UniformList & uniformlist = object->getUniformList();
            if(!uniformlist.empty())
                treeItem->addChild(helpers::str_plus_count("Uniforms", uniformlist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetUniformList, ~0u));

            const osg::StateSet::DefineList & definelist = object->getDefineList();
            if(!definelist.empty())
                treeItem->addChild(helpers::str_plus_count("Defines", definelist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetDefineList, ~0u));
        }
        break;
    case SGIItemTypeParents:
        {
            unsigned numParents = object->getNumParents();
            for(unsigned i = 0; i < numParents; i++)
            {
                SGIHostItemOsg parentItem(object->getParent(i));
                treeItem->addChild(std::string(), &parentItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetModeList:
        {
            unsigned itemNumber = _item->number();
            const osg::StateSet::TextureModeList & textureModeList = object->getTextureModeList();
            if (!textureModeList.empty())
            {
                unsigned count = 0;

                for (osg::StateSet::TextureModeList::const_iterator it = textureModeList.begin(); it != textureModeList.end(); it++, count++)
                {
                    if (itemNumber == ~0u)
                    {
                        const osg::StateSet::ModeList & modeList = *it;
                        if (modeList.empty())
                        {
                            SGIItemBasePtr modeItem = cloneItem<SGIItemOsg>(SGIItemTypeStateSetModeList, count);
                            std::stringstream ss;
                            ss << "Unit #" << count << ": empty";
                            treeItem->addChild(ss.str(), modeItem.get());
                        }
                        else
                        {
                            for (osg::StateSet::ModeList::const_iterator it2 = modeList.begin(); it2 != modeList.end(); it2++)
                            {
                                const osg::StateAttribute::GLMode & mode = it2->first;
                                const osg::StateAttribute::GLModeValue & value = it2->second;

                                SGIItemBasePtr modeItem = cloneItem<SGIItemOsg>(SGIItemTypeStateSetModeList, count);
                                std::stringstream ss;
                                ss << "Unit #" << count << ": " << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode) << "=" << glValueName(value);
                                treeItem->addChild(ss.str(), modeItem.get());
                            }
                        }
                    }

                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetTextureModeList:
        {
            unsigned itemNumber = _item->number();
            const osg::StateSet::TextureModeList & textureModeList = object->getTextureModeList();
            unsigned textureUnit = 0;
            if (itemNumber == ~0u)
            {
                for (osg::StateSet::TextureModeList::const_iterator it = textureModeList.begin(); it != textureModeList.end(); it++, textureUnit++)
                {
                    const osg::StateSet::ModeList & modeList = *it;
                    for (osg::StateSet::ModeList::const_iterator it2 = modeList.begin(); it2 != modeList.end(); it2++)
                    {
                        const osg::StateAttribute::GLMode & mode = it2->first;
                        const osg::StateAttribute::GLModeValue & value = it2->second;

                        SGIItemBasePtr modeItem = cloneItem<SGIItemOsg>(SGIItemTypeStateSetTextureModeList, textureUnit);

                        std::stringstream ss;
                        ss << helpers::str_plus_number("Texture", textureUnit) << '/' << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode);
                        treeItem->addChild(ss.str(), modeItem.get());
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetUniformList:
        {
            unsigned childNo = 0;
            const osg::StateSet::UniformList & list = object->getUniformList();
            for(osg::StateSet::UniformList::const_iterator it = list.begin(); it != list.end(); it++, childNo++)
            {
                const std::string & name = it->first;
                const osg::StateSet::RefUniformPair & unipair = it->second;
                const osg::ref_ptr<osg::Uniform> & uniform = unipair.first;
                SGIHostItemOsg uniformItem(uniform.get());
                std::stringstream ss;
                ss << childNo << ':' << osg::Uniform::getTypename(uniform->getType()) << ' ' << uniform->getName();
                treeItem->addChild(ss.str(), &uniformItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetDefineList:
        ret = true;
        break;
    case SGIItemTypeStateSetAttributeList:
        {
            unsigned childNo = 0;
            const osg::StateSet::AttributeList & list = object->getAttributeList();
            for(osg::StateSet::AttributeList::const_iterator it = list.begin(); it != list.end(); it++, childNo++)
            {
                const osg::StateAttribute::TypeMemberPair & typepair = it->first;
                const osg::StateAttribute::Type & type = typepair.first;
                const unsigned & member = typepair.second;
                const osg::StateSet::RefAttributePair & attrpair = it->second;
                const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                SGIHostItemOsg attrItem(attr.get());
                std::stringstream ss;
                ss << '#' << member << ':' << type << ' ' << attr->getName();
                treeItem->addChild(ss.str(), &attrItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetTextureAttributeLists:
        {
            unsigned itemNumber = _item->number();
            unsigned textureUnit = 0;
            const osg::StateSet::TextureAttributeList & textureAttributes = object->getTextureAttributeList();
            for(osg::StateSet::TextureAttributeList::const_iterator it = textureAttributes.begin(); it != textureAttributes.end(); it++, textureUnit++)
            {
                if(itemNumber == ~0u || textureUnit == itemNumber)
                {
                    unsigned attrChildNo = 0;
                    const osg::StateSet::AttributeList & attributes = *it;
                    for(osg::StateSet::AttributeList::const_iterator it2 = attributes.begin(); it2 != attributes.end(); it2++, attrChildNo++)
                    {
                        const osg::StateAttribute::TypeMemberPair typepair = it2->first;
                        const osg::StateAttribute::Type & type = typepair.first;
                        const unsigned & member = typepair.second;
                        const osg::StateSet::RefAttributePair & attrpair = it2->second;
                        const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                        //const osg::StateAttribute::OverrideValue & overrideValue = attrpair.second;

                        std::stringstream ss;
                        ss << helpers::str_plus_number("Texture", textureUnit) << '/' << member << ':' << type << ' ' << attr->getName();
                        SGIHostItemOsg attrItem(attr.get());
                        treeItem->addChild(ss.str(), &attrItem);
                    }
                }
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Transform>::build(IObjectTreeItem * treeItem)
{
    osg::Transform * object = getObject<osg::Transform,SGIItemOsg>();
    bool ret = callNextHandler(treeItem);
    return ret;
}

bool objectTreeBuildImpl<osg::Camera>::build(IObjectTreeItem * treeItem)
{
    osg::Camera * object = getObject<osg::Camera,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getInitialDrawCallback() ||
                object->getPreDrawCallback() ||
                object->getPostDrawCallback() ||
                object->getFinalDrawCallback() ||
                object->getClampProjectionMatrixCallback()
            )
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

            SGIHostItemOsg view(object->getView());
            if(view.hasObject())
                treeItem->addChild("View", &view);

            SGIHostItemOsg viewport(object->getViewport());
            if(viewport.hasObject())
                treeItem->addChild("Viewport", &viewport);

            SGIHostItemOsg graphicsContext(object->getGraphicsContext());
            if(graphicsContext.hasObject())
                treeItem->addChild("Graphics context", &graphicsContext);

            SGIHostItemOsg stats(object->getStats());
            if(stats.hasObject())
                treeItem->addChild("Stats", &stats);

            SGIHostItemOsg cameraThread(object->getCameraThread());
            if(cameraThread.hasObject())
                treeItem->addChild("Camera thread", &cameraThread);

            SGIHostItemOsg renderer(object->getRenderer());
            if(renderer.hasObject())
                treeItem->addChild("Renderer", &renderer);

            const osg::Camera::BufferAttachmentMap & bufferAttachmentMap = object->getBufferAttachmentMap();
            if(!bufferAttachmentMap.empty())
                treeItem->addChild(helpers::str_plus_count("Attachments", bufferAttachmentMap.size()), cloneItem<SGIItemOsg>(SGIItemTypeCameaBufferAttachments, ~0u));
        }
        break;
    case SGIItemTypeCallbacks:
        {
            callNextHandler(treeItem);
            SGIHostItemOsg initialDrawCallback(object->getInitialDrawCallback(), new SGIRefPtrOsg(object));
            if(initialDrawCallback.hasObject())
                treeItem->addChild("InitialDrawCallback", &initialDrawCallback);
            SGIHostItemOsg preDrawCallback(object->getPreDrawCallback(), new SGIRefPtrOsg(object));
            if(preDrawCallback.hasObject())
                treeItem->addChild("PreDrawCallback", &preDrawCallback);
            SGIHostItemOsg postDrawCallback(object->getPostDrawCallback(), new SGIRefPtrOsg(object));
            if(postDrawCallback.hasObject())
                treeItem->addChild("PostDrawCallback", &postDrawCallback);
            SGIHostItemOsg finalDrawCallback(object->getFinalDrawCallback(), new SGIRefPtrOsg(object));
            if(finalDrawCallback.hasObject())
                treeItem->addChild("FinalDrawCallback", &finalDrawCallback);
            SGIHostItemOsg clampProjectionMatrixCallback(object->getClampProjectionMatrixCallback(), new SGIRefPtrOsg(object));
            if(clampProjectionMatrixCallback.hasObject())
                treeItem->addChild("ClampProjectionMatrixCallback", &clampProjectionMatrixCallback);
            ret = true;
        }
        break;
    case SGIItemTypeCameaBufferAttachments:
        {
            const osg::Camera::BufferAttachmentMap & bufferAttachmentMap = object->getBufferAttachmentMap();
            if(_item->number() == ~0u)
            {
                for(auto it = bufferAttachmentMap.begin(); it != bufferAttachmentMap.end(); ++it)
                {
                    const osg::Camera::BufferComponent & buffercomponent = it->first;
                    std::stringstream ss;
                    ss << buffercomponent;
                    treeItem->addChild(ss.str(), cloneItem<SGIItemOsg>(SGIItemTypeCameaBufferAttachments, (unsigned)buffercomponent));
                }
            }
            else
            {
                const osg::Camera::BufferComponent buffercomponent = (osg::Camera::BufferComponent)_item->number();
                auto it = bufferAttachmentMap.find(buffercomponent);
                if(it != bufferAttachmentMap.end())
                {
                    const osg::Camera::Attachment & attachment = it->second;
                    SGIHostItemOsg image(attachment._image.get());
                    if(image.hasObject())
                        treeItem->addChild("Image", &image);
                    SGIHostItemOsg texture(attachment._texture.get());
                    if(texture.hasObject())
                        treeItem->addChild("Texture", &texture);
                }
            }
			ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::View>::build(IObjectTreeItem * treeItem)
{
    osg::View * object = getObject<osg::View,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg camera(object->getCamera());
            if(camera.hasObject())
                treeItem->addChild("Camera", &camera);

            SGIHostItemOsg stats(object->getStats());
            if(stats.hasObject())
                treeItem->addChild("Stats", &stats);

            SGIHostItemOsg light(object->getLight());
            if(light.hasObject())
                treeItem->addChild("Light", &light);

            if(object->getNumSlaves())
                treeItem->addChild(helpers::str_plus_count("Slaves", object->getNumSlaves()), cloneItem<SGIItemOsg>(SGIItemTypeSlaves));
        }
        break;
    case SGIItemTypeSlaves:
        {
            unsigned numSlaves = object->getNumSlaves();
            for(unsigned i = 0; i < numSlaves; i++)
            {
                const osg::View::Slave & slave = object->getSlave(i);
                SGIHostItemOsg slaveCamera(slave._camera);
                if(slaveCamera.hasObject())
                    treeItem->addChild(helpers::str_plus_number("SlaveCamera", i), &slaveCamera);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Shader>::build(IObjectTreeItem * treeItem)
{
    osg::Shader * object = getObject<osg::Shader,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Source", cloneItem<SGIItemOsg>(SGIItemTypeShaderSource));

            SGIHostItemOsg binary(object->getShaderBinary());
            if(binary.hasObject())
                treeItem->addChild("Binary", &binary);

            const osg::Shader::CodeInjectionMap & codeInjectionMap = object->getCodeInjectionMap();
            if(!codeInjectionMap.empty())
                treeItem->addChild(helpers::str_plus_number("Code injection map", codeInjectionMap.size()), cloneItem<SGIItemOsg>(SGIItemTypeShaderCodeInjectionMap));
        }
        break;
    case SGIItemTypeShaderSource:
        ret = true;
        break;
    case SGIItemTypeShaderCodeInjectionMap:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Uniform>::build(IObjectTreeItem * treeItem)
{
    osg::Uniform * object = getObject<osg::Uniform,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numParents = object->getNumParents();
            if(numParents)
                treeItem->addChild(helpers::str_plus_count("Parents", numParents), cloneItem<SGIItemOsg>(SGIItemTypeParents));

            if(object->getEventCallback() || object->getUpdateCallback())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            SGIHostItemOsg floatArray(object->getFloatArray());
            if(floatArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("FloatArray", ((const osg::Array * )floatArray.object())->getNumElements()), &floatArray);

            SGIHostItemOsg doubleArray(object->getDoubleArray());
            if(doubleArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("DoubleArray", ((const osg::Array * )doubleArray.object())->getNumElements()), &doubleArray);

            SGIHostItemOsg intArray(object->getIntArray());
            if(intArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("IntArray", ((const osg::Array * )intArray.object())->getNumElements()), &intArray);

            SGIHostItemOsg uintArray(object->getUIntArray());
            if(uintArray.hasObject())
                treeItem->addChild(helpers::str_plus_count("UIntArray", ((const osg::Array * )uintArray.object())->getNumElements()), &uintArray);

        }
        break;
    case SGIItemTypeParents:
        {
            unsigned numParents = object->getNumParents();
            for(unsigned i = 0; i < numParents; i++)
            {
                SGIHostItemOsg parentItem(object->getParent(i));
                treeItem->addChild(std::string(), &parentItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg eventCallback(object->getEventCallback(), new SGIRefPtrOsg(object));
            if(eventCallback.hasObject())
                treeItem->addChild("EventCallback", &eventCallback);

            SGIHostItemOsg updateCallback(object->getUpdateCallback(), new SGIRefPtrOsg(object));
            if(updateCallback.hasObject())
                treeItem->addChild("UpdateCallback", &updateCallback);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Program>::build(IObjectTreeItem * treeItem)
{
    osg::Program * object = getObject<osg::Program,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numShaders = object->getNumShaders();
            if(numShaders)
                treeItem->addChild(helpers::str_plus_count("Shaders", numShaders), cloneItem<SGIItemOsg>(SGIItemTypeShaders));

            SGIHostItemOsg programBinary(object->getProgramBinary());
            if(programBinary.hasObject())
                treeItem->addChild("Program Binary", &programBinary);
        }
        break;
    case SGIItemTypeShaders:
        {
            unsigned numShaders = object->getNumShaders();
            for(unsigned i = 0; i < numShaders; i++)
            {
                osg::Shader * shader = object->getShader(i);
                SGIHostItemOsg shaderItem(shader);
                std::stringstream ss;
                ss << i << ": " << shader->getName() << '(' << shader->getTypename() << ')';
                treeItem->addChild(ss.str(), &shaderItem);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

namespace {
    class PerContextProgramAccess : public osg::Program::PerContextProgram
    {
    public:
        const osg::Program * parent() const { return _program; }
    };
}

bool objectTreeBuildImpl<osg::Program::PerContextProgram>::build(IObjectTreeItem * treeItem)
{
    osg::Program::PerContextProgram * object = getObject<osg::Program::PerContextProgram,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            PerContextProgramAccess * access = (PerContextProgramAccess*)object;
            SGIHostItemOsg parent(access->parent());
            if(parent.hasObject())
                treeItem->addChild(helpers::str_plus_info("Parent", access->parent()->getName()), &parent);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::GraphicsContext>::build(IObjectTreeItem * treeItem)
{
    GraphicsContextAccess * object = static_cast<GraphicsContextAccess*>(getObject<osg::GraphicsContext, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg traits(object->getTraits());
            if(traits.hasObject())
                treeItem->addChild("Traits", &traits);

            SGIHostItemOsg state(object->getState());
            if(state.hasObject())
                treeItem->addChild("State", &state);

            SGIHostItemOsg graphicsThread(object->getGraphicsThread());
            if(graphicsThread.hasObject())
                treeItem->addChild("GraphicsThread", &graphicsThread);

#if OSG_VERSION_LESS_OR_EQUAL(3,4,0)
            SGIHostItemOsg txtobjmgr(osg::Texture::getTextureObjectManager(object->getContextID()));
            if (txtobjmgr.hasObject())
                treeItem->addChild("TextureObjectManager", &txtobjmgr);
#endif

            const osg::GraphicsContext::Cameras & cameras = object->getCameras();
            if(!cameras.empty())
                treeItem->addChild(helpers::str_plus_count("Cameras", cameras.size()), cloneItem<SGIItemOsg>(SGIItemTypeGraphicsContextCameras));

            const osg::GraphicsContext::GraphicsOperationQueue & operationQueue = object->getOperationsQueue();
            treeItem->addChild(helpers::str_plus_count("Operations", operationQueue.size()), cloneItem<SGIItemOsg>(SGIItemTypeGraphicsContextOperations));
            treeItem->addChild("GLExtensions", cloneItem<SGIItemOsg>(SGIItemTypeGraphicsContextGLExtensions));
        }
        break;
    case SGIItemTypeCallbacks:
        {
            callNextHandler(treeItem);

            SGIHostItemOsg swapcb(object->getSwapCallback());
            if(swapcb.hasObject())
                treeItem->addChild("SwapCallback", &swapcb);

            SGIHostItemOsg resizedcb(object->getResizedCallback());
            if(resizedcb.hasObject())
                treeItem->addChild("ResizedCallback", &resizedcb);
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextCameras:
        {
            unsigned childNo = 0;
            const osg::GraphicsContext::Cameras & cameras = object->getCameras();
            for(osg::GraphicsContext::Cameras::const_iterator it = cameras.begin(); it != cameras.end(); it++, childNo++)
            {
                SGIHostItemOsg camera(*it);
                if(camera.hasObject())
                    treeItem->addChild(helpers::str_plus_number("Camera", childNo), &camera);
            }
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextOperations:
        {
            const osg::GraphicsContext::GraphicsOperationQueue & queue = object->getOperationsQueue();
            unsigned operationNo = 0;
            for(osg::GraphicsContext::GraphicsOperationQueue::const_iterator it = queue.begin(); it != queue.end(); it++, operationNo++)
            {
                SGIHostItemOsg operation(*it);
                if(operation.hasObject())
                    treeItem->addChild(helpers::str_plus_number("Operation", operationNo), &operation);
            }
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextGLExtensions:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::NodeVisitor>::build(IObjectTreeItem * treeItem)
{
    osg::NodeVisitor * object = getObject<osg::NodeVisitor,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg framestamp(object->getFrameStamp());
            if(framestamp.hasObject())
                treeItem->addChild("FrameStamp", &framestamp);

            SGIHostItemOsg databaseRequestHandler(object->getDatabaseRequestHandler());
            if(databaseRequestHandler.hasObject())
                treeItem->addChild("DatabaseRequestHandler", &databaseRequestHandler);

            SGIHostItemOsg imageRequestHandler(object->getImageRequestHandler());
            if(imageRequestHandler.hasObject())
                treeItem->addChild("ImageRequestHandler", &imageRequestHandler);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Stats>::build(IObjectTreeItem * treeItem)
{
    osg::Stats * object = getObject<osg::Stats, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            for (unsigned frame = object->getEarliestFrameNumber(); frame <= object->getLatestFrameNumber(); ++frame)
            {
                treeItem->addChild(helpers::str_plus_number("Frame", frame), cloneItem<SGIItemOsg>(SGIItemTypeStatsFrame, frame));
            }
        }
        break;
    case SGIItemTypeStatsFrame:
        {
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::ProxyNode>::build(IObjectTreeItem * treeItem)
{
    osg::ProxyNode * object = getObject<osg::ProxyNode,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg dbOpts(object->getDatabaseOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("DatabaseOptions", &dbOpts);
        }
        break;
    case SGIItemTypePendingChild:
        {
            unsigned requestedChild = _item->number();
            if(requestedChild < object->getNumFileNames())
            {
                SGIHostItemOsg dbReq(object->getDatabaseRequest(requestedChild));
                if(dbReq.hasObject())
                    treeItem->addChild("DatabaseRequest", &dbReq);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::PagedLOD>::build(IObjectTreeItem * treeItem)
{
    osg::PagedLOD * object = getObject<osg::PagedLOD,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg dbOpts(object->getDatabaseOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("DatabaseOptions", &dbOpts);
        }
        break;
    case SGIItemTypePendingChild:
        {
            unsigned requestedChild = _item->number();
            if(requestedChild < object->getNumFileNames())
            {
                SGIHostItemOsg dbReq(object->getDatabaseRequest(requestedChild));
                if(dbReq.hasObject())
                    treeItem->addChild("DatabaseRequest", &dbReq);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::StateAttribute>::build(IObjectTreeItem * treeItem)
{
    osg::StateAttribute * object = getObject<osg::StateAttribute,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numParents = object->getNumParents();
            if(numParents)
                treeItem->addChild(helpers::str_plus_count("Parents", numParents), cloneItem<SGIItemOsg>(SGIItemTypeParents));

            if(object->getUpdateCallback() || object->getEventCallback())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            SGIHostItemOsg shaderComponent(object->getShaderComponent());
            if(shaderComponent.hasObject())
                treeItem->addChild("ShaderComponent", &shaderComponent);
        }
        break;
    case SGIItemTypeParents:
        {
            unsigned numParents = object->getNumParents();
            for(unsigned i = 0; i < numParents; i++)
            {
                SGIHostItemOsg parentItem(object->getParent(i));
                treeItem->addChild(std::string(), &parentItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg eventCallback(object->getEventCallback());
            if(eventCallback.hasObject())
                treeItem->addChild("EventCallback", &eventCallback);

            SGIHostItemOsg updateCallback(object->getUpdateCallback());
            if(updateCallback.hasObject())
                treeItem->addChild("UpdateCallback", &updateCallback);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Texture>::build(IObjectTreeItem * treeItem)
{
    TextureAccess * object = static_cast<TextureAccess*>(getObject<osg::Texture,SGIItemOsg,DynamicCaster>());
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {

            SGIHostItemOsg readPBuffer(object->getReadPBuffer());
            if(readPBuffer.hasObject())
                treeItem->addChild("ReadPBuffer", &readPBuffer);

            treeItem->addChild("TextureObjects", cloneItem<SGIItemOsg>(SGIItemTypeTextureObjects, ~0u));

            for (unsigned n = 0; n < object->getNumImages(); ++n)
            {
                SGIHostItemOsg image(object->getImage(n));
                if (image.hasObject())
                    treeItem->addChild(helpers::str_plus_number("Image", n), &image);
            }
        }
        break;
    case SGIItemTypeTextureObjects:
        {
            TextureAccess::TextureObjectList list = object->getTextureObjectList();
            if (itemNumber() == ~0u)
            {
                for (unsigned n = 0; n < list.size(); ++n)
                {
                    SGIHostItemOsg item(list[n]);
                    if (item.hasObject())
                        treeItem->addChild(helpers::str_plus_number("Context", n), &item);
                }
            }
            else
            {
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Texture::TextureObject>::build(IObjectTreeItem * treeItem)
{
    osg::Texture::TextureObject * object = getObject<osg::Texture::TextureObject, SGIItemOsg>();
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg texture(object->getTexture());
            if (texture.hasObject())
                treeItem->addChild("Texture", &texture);

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#if OSG_MIN_VERSION_REQUIRED(3,5,0)
bool objectTreeBuildImpl<osg::TextureObjectSet>::build(IObjectTreeItem * treeItem)
#else
bool objectTreeBuildImpl<osg::Texture::TextureObjectSet>::build(IObjectTreeItem * treeItem)
#endif
{
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    TextureObjectSetAccess * object = static_cast<TextureObjectSetAccess*>(getObject<osg::TextureObjectSet, SGIItemOsg>());
#else
    TextureObjectSetAccess * object = static_cast<TextureObjectSetAccess*>(getObject<osg::Texture::TextureObjectSet, SGIItemOsg>());
#endif
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg parent(object->getParent());
            if (parent.hasObject())
                treeItem->addChild("Parent", &parent);

            treeItem->addChild("TextureObjects", cloneItem<SGIItemOsg>(SGIItemTypeTextureObjects, ~0u));
        }
        break;
    case SGIItemTypeTextureObjects:
        {
            TextureObjectSetAccess::TextureObjectList list = object->getTextureObjects();
            for (auto to : list)
            {
                SGIHostItemOsg item(to.get());
                if (item.hasObject())
                    treeItem->addChild(std::string(), &item);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#if OSG_MIN_VERSION_REQUIRED(3,5,0)
bool objectTreeBuildImpl<osg::TextureObjectManager>::build(IObjectTreeItem * treeItem)
#else
bool objectTreeBuildImpl<osg::Texture::TextureObjectManager>::build(IObjectTreeItem * treeItem)
#endif
{
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    TextureObjectManagerAccess * object = static_cast<TextureObjectManagerAccess *>(getObject<osg::TextureObjectManager, SGIItemOsg>());
#else
    TextureObjectManagerAccess * object = static_cast<TextureObjectManagerAccess *>(getObject<osg::Texture::TextureObjectManager, SGIItemOsg>());
#endif
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild("Statistics", cloneItem<SGIItemOsg>(SGIItemTypeStatistics, ~0u));

            const TextureObjectManagerAccess::TextureSetMap & textureSetMap = object->getTextureSetMap();
            treeItem->addChild(helpers::str_plus_count("TextureSetMap", textureSetMap.size()), cloneItem<SGIItemOsg>(SGIItemTypeTextureSetMap, ~0u));
        }
        break;
    case SGIItemTypeStatistics:
        ret = true;
        break;
    case SGIItemTypeTextureSetMap:
        {
            const TextureObjectManagerAccess::TextureSetMap & textureSetMap = object->getTextureSetMap();
            if (itemNumber() == ~0u)
            {
                for (auto it = textureSetMap.begin(); it != textureSetMap.end(); ++it)
                {
                    const osg::Texture::TextureProfile & p = it->first;
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
                    const osg::ref_ptr<osg::TextureObjectSet> & s = it->second;
#else
                    const osg::ref_ptr<osg::Texture::TextureObjectSet> & s = it->second;
#endif
                    unsigned hash = TextureObjectManagerAccess::hash(p);
                    std::string name = TextureObjectManagerAccess::shortName(p);
                    SGIHostItemOsg item(s.get());
                    //treeItem->addChild(helpers::str_plus_count(name, s.valid() ? s->size() : 0), cloneItem<SGIItemOsg>(SGIItemTypeTextureSetMap, hash));
                    treeItem->addChild(helpers::str_plus_count(name, s.valid() ? s->size() : 0), &item);
                }
            }
            else
            {
                for (auto it = textureSetMap.begin(); it != textureSetMap.end(); ++it)
                {
                    const osg::Texture::TextureProfile & p = it->first;
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
                    const osg::ref_ptr<osg::TextureObjectSet> & s = it->second;
#else
                    const osg::ref_ptr<osg::Texture::TextureObjectSet> & s = it->second;
#endif
                    unsigned hash = TextureObjectManagerAccess::hash(p);
                    if (hash == itemNumber())
                    {

                        break;
                    }
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Texture2D>::build(IObjectTreeItem * treeItem)
{
    osg::Texture2D * object = getObject<osg::Texture2D,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            // texture is already added by osg::Texture
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Texture3D>::build(IObjectTreeItem * treeItem)
{
    osg::Texture3D * object = getObject<osg::Texture3D,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            // texture is already added by osg::Texture
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::Image>::build(IObjectTreeItem * treeItem)
{
    osg::Image * object = getObject<osg::Image,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg pbo(object->getPixelBufferObject());
            if(pbo.hasObject())
                treeItem->addChild("PBO", &pbo);

            if (object->getNumMipmapLevels() > 1)
            {
                treeItem->addChildIfNotExists(helpers::str_plus_count("MipMaps", object->getNumMipmapLevels()), cloneItem<SGIItemOsg>(SGIItemTypeImageMipMap, ~0u));
            }
        }
        break;
    case SGIItemTypeImageMipMap:
        {
            if (itemNumber() == ~0u)
            {
                for(unsigned i = 0; i < object->getNumMipmapLevels(); ++i)
                    treeItem->addChildIfNotExists(helpers::str_plus_number("Level", i), cloneItem<SGIItemOsg>(SGIItemTypeImageMipMap, i));
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::BufferData>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::BufferData * object = getObject<osg::BufferData, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getModifiedCallback())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            if(object->getDataPointer())
                treeItem->addChild(helpers::str_plus_number("Data", object->getTotalDataSize()), cloneItem<SGIItemOsg>(SGIItemTypeArrayData));

            SGIHostItemOsg bufObj(object->getBufferObject());
            if(bufObj.hasObject())
                treeItem->addChild("BufferObject", &bufObj);
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg modifiedCallback(object->getModifiedCallback());
            if(modifiedCallback.hasObject())
                treeItem->addChild(std::string(), &modifiedCallback);

            ret = true;
        }
        break;
    case SGIItemTypeArrayData:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::HeightField>::build(IObjectTreeItem * treeItem)
{
    osg::HeightField * object = getObject<osg::HeightField,SGIItemOsg,DynamicCaster>();
	bool ret;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
            SGIHostItemOsg array(object->getFloatArray());
            if (array.hasObject())
                treeItem->addChild("FloatArray", &array);
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osg::Array>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::Array * object = getObject<osg::Array, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            osg::VertexBufferObject * vbo = object->getVertexBufferObject();
            if (vbo && vbo != object->getBufferObject())
            {
                SGIHostItemOsg vboItem(vbo);
                treeItem->addChild("VertexBufferObject", &vboItem);
            }
        }
        break;
    case SGIItemTypeArrayData:
        {
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::BufferObject>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::BufferObject * object = getObject<osg::BufferObject, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numBufferData = object->getNumBufferData();
            if(numBufferData)
                treeItem->addChild(helpers::str_plus_count("BufferData", numBufferData), cloneItem<SGIItemOsg>(SGIItemTypeBufferDatas));
        }
        break;
    case SGIItemTypeBufferDatas:
        {
            unsigned numBufferData = object->getNumBufferData();
            for(unsigned n = 0; n < numBufferData; n++)
            {
                SGIHostItemOsg data(object->getBufferData(n));
                if(data.hasObject())
                    treeItem->addChild(helpers::str_plus_number("BufferData", n), &data);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::PrimitiveSet>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::PrimitiveSet * object = getObject<osg::PrimitiveSet, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            unsigned numIndices = object->getNumIndices();
            if (numIndices)
                treeItem->addChild(helpers::str_plus_count("Indices", numIndices), cloneItem<SGIItemOsg>(SGIItemTypeDrawElementsIndicies));
        }
        break;
    case SGIItemTypeDrawElementsIndicies:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::DrawElements>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::DrawElements * object = getObject<osg::DrawElements, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::State>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::State * object = getObject<osg::State, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg shaderComposer(object->getShaderComposer());
            if(shaderComposer.hasObject())
                treeItem->addChild("ShaderComposer", &shaderComposer);
            SGIHostItemOsg gfxctx(object->getGraphicsContext());
            if(gfxctx.hasObject())
                treeItem->addChild("GraphicsContext", &gfxctx);
            SGIHostItemOsg framestamp(object->getFrameStamp());
            if(framestamp.hasObject())
                treeItem->addChild("FrameStamp", &framestamp);
            SGIHostItemOsg displaySettings(object->getDisplaySettings());
            if(displaySettings.hasObject())
                treeItem->addChild("DisplaySettings", &displaySettings);

            SGIHostItemOsg gfxcostest(object->getGraphicsCostEstimator());
            if(gfxcostest.hasObject())
                treeItem->addChild("GraphicsCostEstimator", &gfxcostest);

            SGIHostItemOsg vbo(object->getCurrentVertexBufferObject());
            if (vbo.hasObject())
                treeItem->addChild("CurrentVBO", &vbo);

            SGIHostItemOsg ebo(object->getCurrentElementBufferObject());
            if (ebo.hasObject())
                treeItem->addChild("CurrentEBO", &ebo);

            SGIHostItemOsg pbo(object->getCurrentPixelBufferObject());
            if (pbo.hasObject())
                treeItem->addChild("CurrentPBO", &pbo);

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::ShaderComposer>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::ShaderComposer * object = getObject<osg::ShaderComposer, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osg::ShaderComponent>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osg::ShaderComponent * object = getObject<osg::ShaderComponent, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned numShaders = object->getNumShaders();
            if(numShaders)
                treeItem->addChild(helpers::str_plus_count("Shaders", numShaders), cloneItem<SGIItemOsg>(SGIItemTypeShaders));
        }
        break;
    case SGIItemTypeShaders:
        {
            unsigned numShaders = object->getNumShaders();
            for(unsigned i = 0; i < numShaders; i++)
            {
                osg::Shader * shader = object->getShader(i);
                SGIHostItemOsg shaderItem(shader);
                std::stringstream ss;
                ss << i << ": " << shader->getName() << '(' << shader->getTypename() << ')';
                treeItem->addChild(ss.str(), &shaderItem);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::ViewerBase>::build(IObjectTreeItem * treeItem)
{
    ViewerBaseAccess * object = static_cast<ViewerBaseAccess*>(getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg viewerstats(object->getViewerStats());
            if(viewerstats.hasObject())
                treeItem->addChild("ViewerStats", &viewerstats);

            SGIHostItemOsg eventvisitor(object->getEventVisitor());
            if(eventvisitor.hasObject())
                treeItem->addChild("EventVisitor", &eventvisitor);

            SGIHostItemOsg updatevisitor(object->getUpdateVisitor());
            if(updatevisitor.hasObject())
                treeItem->addChild("UpdateVisitor", &updatevisitor);

            SGIHostItemOsg updatequeue(object->getUpdateOperations());
            if(updatequeue.hasObject())
                treeItem->addChild("UpdateOperations", &updatequeue);

            SGIHostItemOsg realizeOp(object->getRealizeOperation());
            if(realizeOp.hasObject())
                treeItem->addChild("RealizeOperation", &realizeOp);

            SGIHostItemOsg incrCompileOp(object->getIncrementalCompileOperation());
            if(incrCompileOp.hasObject())
                treeItem->addChild("IncrementalCompileOperation", &incrCompileOp);

            SGIHostItemOsg framestamp(object->getViewerFrameStamp());
            if(framestamp.hasObject())
                treeItem->addChild("FrameStamp", &framestamp);

            treeItem->addChild("Scenes", cloneItem<SGIItemOsg>(SGIItemTypeViewerBaseScenes));
            treeItem->addChild("Views", cloneItem<SGIItemOsg>(SGIItemTypeViewerBaseViews));
            treeItem->addChild("Cameras", cloneItem<SGIItemOsg>(SGIItemTypeViewerBaseCameras));
            treeItem->addChild("Contexts", cloneItem<SGIItemOsg>(SGIItemTypeViewerBaseContexts));
        }
        break;
    case SGIItemTypeViewerBaseScenes:
        {
            osgViewer::ViewerBase::Scenes list;
            object->getScenes(list);
            for(auto it = list.begin(); it != list.end(); it++)
            {
                SGIHostItemOsg child(*it);
                if(child.hasObject())
                    treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseViews:
        {
            osgViewer::ViewerBase::Views list;
            object->getViews(list);
            for(auto it = list.begin(); it != list.end(); it++)
            {
                SGIHostItemOsg child(*it);
                if(child.hasObject())
                    treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseCameras:
        {
            osgViewer::ViewerBase::Cameras list;
            object->getCameras(list);
            for(auto it = list.begin(); it != list.end(); it++)
            {
                SGIHostItemOsg child(*it);
                if(child.hasObject())
                    treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeViewerBaseContexts:
        {
            osgViewer::ViewerBase::Contexts list;
            object->getContexts(list);
            for(auto it = list.begin(); it != list.end(); it++)
            {
                SGIHostItemOsg child(*it);
                if(child.hasObject())
                    treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::View>::build(IObjectTreeItem * treeItem)
{
    osgViewer::View * object = getObject<osgViewer::View,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg viewerbase(object->getViewerBase());
            if(viewerbase.hasObject())
                treeItem->addChild("ViewerBase", &viewerbase);

            SGIHostItemOsg manipulator(object->getCameraManipulator());
            if(manipulator.hasObject())
                treeItem->addChild("CameraManipulator", &manipulator);

            const osgViewer::View::Devices& devices = object->getDevices();
            if(!devices.empty())
                treeItem->addChild(helpers::str_plus_count("Devices", devices.size()), cloneItem<SGIItemOsg>(SGIItemTypeDevices));

            SGIHostItemOsg eventQueue(object->getEventQueue());
            if (eventQueue.hasObject())
                treeItem->addChild("Event Queue", &eventQueue);

            const osgViewer::View::EventHandlers& eventHandlers = object->getEventHandlers();
            if(!eventHandlers.empty())
                treeItem->addChild(helpers::str_plus_count("Event handlers", eventHandlers.size()), cloneItem<SGIItemOsg>(SGIItemTypeEventHandlers));

            SGIHostItemOsg displaySettings(object->getDisplaySettings());
            if(displaySettings.hasObject())
                treeItem->addChild("DisplaySettings", &displaySettings);

            SGIHostItemOsg scene(object->getScene());
            if(scene.hasObject())
                treeItem->addChild("Scene", &scene);
        }
        break;
    case SGIItemTypeDevices:
        {
            const osgViewer::View::Devices& devices = object->getDevices();
            for(osgViewer::View::Devices::const_iterator it = devices.begin(); it != devices.end(); it++)
            {
                SGIHostItemOsg device(*it);
                if(device.hasObject())
                    treeItem->addChild(std::string(), &device);
            }
            ret = true;
        }
        break;
    case SGIItemTypeEventHandlers:
        {
            const osgViewer::View::EventHandlers& eventHandlers = object->getEventHandlers();
            for(osgViewer::View::EventHandlers::const_iterator it = eventHandlers.begin(); it != eventHandlers.end(); it++)
            {
                SGIHostItemOsg eventHandler(*it);
                if(eventHandler.hasObject())
                    treeItem->addChild(std::string(), &eventHandler);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::Scene>::build(IObjectTreeItem * treeItem)
{
    osgViewer::Scene * object = getObject<osgViewer::Scene,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg sceneData(object->getSceneData());
            if(sceneData.hasObject())
                treeItem->addChild("SceneData", &sceneData);

            SGIHostItemOsg imagepager(object->getImagePager());
            if(imagepager.hasObject())
                treeItem->addChild("ImagePager", &imagepager);

            SGIHostItemOsg databasepager(object->getDatabasePager());
            if(databasepager.hasObject())
                treeItem->addChild("DatabasePager", &databasepager);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::Renderer>::build(IObjectTreeItem * treeItem)
{
    osgViewer::Renderer * object = getObject<osgViewer::Renderer,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg sceneView0(object->getSceneView(0));
            if(sceneView0.hasObject())
                treeItem->addChild("SceneView0", &sceneView0);

            SGIHostItemOsg sceneView1(object->getSceneView(1));
            if(sceneView1.hasObject())
                treeItem->addChild("SceneView1", &sceneView1);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::HelpHandler>::build(IObjectTreeItem * treeItem)
{
    osgViewer::HelpHandler * object = getObject<osgViewer::HelpHandler,SGIItemOsg, DynamicCaster>();
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg camera(object->getCamera());
            if (camera.hasObject())
                treeItem->addChild("Camera", &camera);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::StatsHandler>::build(IObjectTreeItem * treeItem)
{
    osgViewer::StatsHandler * object = getObject<osgViewer::StatsHandler, SGIItemOsg, DynamicCaster>();
    osgViewerStatsHandlerAccess * access = static_cast<osgViewerStatsHandlerAccess*>(object);
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg camera(object->getCamera());
            if (camera.hasObject())
                treeItem->addChild("Camera", &camera);
            SGIHostItemOsg sw(access->getSwitch());
            if (sw.hasObject())
                treeItem->addChild("Switch", &sw);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgViewer::GraphicsWindow>::build(IObjectTreeItem * treeItem)
{
    osgViewer::GraphicsWindow * object = getObject<osgViewer::GraphicsWindow,SGIItemOsg>();
    bool ret = callNextHandler(treeItem);
    return ret;
}

#ifdef SGI_USE_OSGQT
bool objectTreeBuildImpl<osgQt::GraphicsWindowQt>::build(IObjectTreeItem * treeItem)
{
    osgQt::GraphicsWindowQt * object = static_cast<osgQt::GraphicsWindowQt*>,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemQt widget(object->getGLWidget());
            if(widget.hasObject())
                treeItem->addChild("GL Widget", &widget);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgQt::QObjectWrapper>::build(IObjectTreeItem * treeItem)
{
    osgQt::QObjectWrapper * object = getObject<osgQt::QObjectWrapper, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemQt qobject(object->getQObject());
            if(qobject.hasObject())
                treeItem->addChild(std::string(), &qobject);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgQt::GLWidget>::build(IObjectTreeItem * treeItem)
{
    osgQt::GLWidget * object = getObject<osgQt::GLWidget, SGIItemQt>();
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg gw(object->getGraphicsWindow());
            if (gw.hasObject())
                treeItem->addChild(std::string(), &gw);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif // SGI_USE_OSGQT

bool objectTreeBuildImpl<osgGA::GUIEventHandler>::build(IObjectTreeItem * treeItem)
{
    osgGA::GUIEventHandler * object = getObject<osgGA::GUIEventHandler,SGIItemOsg,DynamicCaster>();
    bool ret = callNextHandler(treeItem);
    switch(itemType())
    {
    case SGIItemTypeObject:
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgGA::GUIEventAdapter>::build(IObjectTreeItem * treeItem)
{
    osgGA::GUIEventAdapter * object = getObject<osgGA::GUIEventAdapter,SGIItemOsg>();
    bool ret = callNextHandler(treeItem);
    switch(itemType())
    {
    case SGIItemTypeObject:
        if(ret)
        {
            SGIHostItemOsg context(object->getGraphicsContext());
            if (context.hasObject())
                treeItem->addChild("GraphicsContext", &context);

//             ReferencedPicker * picker = new ReferencedPicker(const_cast<osgGA::GUIEventAdapter*>(object));
//             buildTree(0, item, picker , SGIItemTypeObject, tr("osgEarth Picker"));
//
//             ReferencedLinePicker * linepicker = new ReferencedLinePicker(const_cast<osgGA::GUIEventAdapter*>(object));
//             buildTree(0, item, linepicker , SGIItemTypeObject, tr("LineSegment picker"));
//
//             ReferencedLinePickerOwn * linepickerOwn = new ReferencedLinePickerOwn(const_cast<osgGA::GUIEventAdapter*>(object));
//             buildTree(0, item, linepickerOwn , SGIItemTypeObject, tr("Own LineSegment picker"));
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgGA::CameraManipulator>::build(IObjectTreeItem * treeItem)
{
    osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator, SGIItemOsg, DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if (object->getCoordinateFrameCallback())
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

            SGIHostItemOsg node(object->getNode());
            if(node.hasObject())
                treeItem->addChild("Node", &node);
        }
        break;
    case SGIItemTypeCallbacks:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgGA::CameraManipulator::CoordinateFrameCallback * callback = object->getCoordinateFrameCallback();
            SGIHostItemOsg coordinateFrameCallback(object->getCoordinateFrameCallback());
            if(coordinateFrameCallback.hasObject())
                treeItem->addChild("CoordinateFrameCallback", &coordinateFrameCallback);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<CullingNodeInfo>::build(IObjectTreeItem * treeItem)
{
    CullingNodeInfo * object = getObject<CullingNodeInfo, SGIItemOsg>();
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg node(object->node.get());
            treeItem->addChild("Node", &node);
            treeItem->addChild("Before", cloneItem<SGIItemOsg>(SGIItemTypeCullingInfoBefore));
            treeItem->addChild("After", cloneItem<SGIItemOsg>(SGIItemTypeCullingInfoAfter));
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<CullingInfo>::build(IObjectTreeItem * treeItem)
{
    CullingInfo * object = getObject<CullingInfo, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg item(object->node());
            treeItem->addChild("Node", &item);

            unsigned pathnum = 0;
            const CullingInfo::CullingNodeInfoPathList & pathlist = object->pathlist();
            for (auto node : pathlist)
            {
                treeItem->addChild(helpers::str_plus_count("Path", pathnum++), cloneItem<SGIItemOsg>(SGIItemTypeChilds));
            }

        }
        break;
    case SGIItemTypeChilds:
        {
            unsigned pathnum = itemNumber();
            const CullingInfo::CullingNodeInfoPathList & pathlist = object->pathlist();
            if (pathnum < pathlist.size())
            {
                const CullingNodeInfoPath & path = pathlist[pathnum];
                for (const CullingNodeInfoPtr & cullinfo : path)
                {
                    SGIHostItemOsg item(cullinfo.get());
                    treeItem->addChild(std::string(), &item);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<CullingInfoForCamera>::build(IObjectTreeItem * treeItem)
{
    CullingInfoForCamera * object = getObject<CullingInfoForCamera, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg camera(object->camera());
            treeItem->addChild("Camera", &camera);

            const CullingInfoPtrList & activeNodes = object->activeNodes();
            for (auto node : activeNodes )
            {
                SGIHostItemOsg item(node.get());
                treeItem->addChild(std::string(), &item);
            }
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<CullingInfoRegistry>::build(IObjectTreeItem * treeItem)
{
    CullingInfoRegistry * object = getObject<CullingInfoRegistry, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const CullingInfoRegistry::CameraCullingInfoMap & map = object->map();
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const CullingInfoRegistry::CameraPtr & camera = it->first;
                const CullingInfoRegistry::CullingInfoForCameraPtr cull_info = it->second;
                osg::ref_ptr<osg::Camera> camera_ref;
                if (camera.lock(camera_ref))
                {
                    SGIHostItemOsg item(cull_info.get());
                    treeItem->addChild(std::string(), &item);
                }
            }
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::Registry>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osgDB::Registry * object = getObject<osgDB::Registry, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Build info", cloneItem<SGIItemOsg>(SGIItemTypeBuildInfo));

            if(object->getReadFileCallback() ||
                object->getWriteFileCallback() ||
                object->getFileLocationCallback() ||
                object->getFindFileCallback()
                )
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

            treeItem->addChild("Readers/Writers", cloneItem<SGIItemOsg>(SGIItemTypeReadersWriters));
            treeItem->addChild("Image processors", cloneItem<SGIItemOsg>(SGIItemTypeImageProcessors));
			treeItem->addChild("Graphics contexts", cloneItem<SGIItemOsg>(SGIItemTypeGraphicsContexts));

            SGIHostItemOsg fileCache(object->getFileCache());
            if(fileCache.hasObject())
                treeItem->addChild("FileCache", &fileCache);

            SGIHostItemOsg objectCache(object->getObjectCache());
            if (objectCache.hasObject())
                treeItem->addChild("ObjectCache", &objectCache);

            SGIHostItemOsg opts(object->getOptions());
            if(opts.hasObject())
                treeItem->addChild("Options", &opts);

            SGIHostItemOsg objectWrapperManager(object->getObjectWrapperManager());
            if(objectWrapperManager.hasObject())
                treeItem->addChild("ObjectWrapperManager", &objectWrapperManager);

            SGIHostItemOsg sharedStateManager(object->getSharedStateManager());
            if(sharedStateManager.hasObject())
                treeItem->addChild("SharedStateManager", &sharedStateManager);

            SGIHostItemOsg kdTreeBuilder(object->getKdTreeBuilder());
            if(kdTreeBuilder.hasObject())
                treeItem->addChild("KdTreeBuilder", &kdTreeBuilder);

            SGIHostItemOsg authenticationMap(object->getAuthenticationMap());
            if(authenticationMap.hasObject())
                treeItem->addChild("AuthenticationMap", &authenticationMap);
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg readFileCallback(object->getReadFileCallback());
            if(readFileCallback.hasObject())
                treeItem->addChild("ReadFileCallback", &readFileCallback);

            SGIHostItemOsg writeFileCallback(object->getWriteFileCallback());
            if(writeFileCallback.hasObject())
                treeItem->addChild("WriteFileCallback", &writeFileCallback);

            SGIHostItemOsg fileLocationCallback(object->getFileLocationCallback());
            if(fileLocationCallback.hasObject())
                treeItem->addChild("FileLocationCallback", &fileLocationCallback);

            SGIHostItemOsg findFileCallback(object->getFindFileCallback());
            if(findFileCallback.hasObject())
                treeItem->addChild("FindFileCallback", &findFileCallback);

            ret = true;
        }
        break;
    case SGIItemTypeReadersWriters:
        {
            const osgDB::Registry::ReaderWriterList& list = object->getReaderWriterList();
            for(osgDB::Registry::ReaderWriterList::const_iterator it = list.begin(); it != list.end(); it++)
            {
                SGIHostItemOsg rw((*it).get());
                if(rw.hasObject())
                    treeItem->addChild(std::string(), &rw);
            }
            ret = true;
        }
        break;
	case SGIItemTypeGraphicsContexts:
		{
			const osg::GraphicsContext::GraphicsContexts & list = osg::GraphicsContext::getAllRegisteredGraphicsContexts();
			for (osg::GraphicsContext::GraphicsContexts::const_iterator it = list.begin(); it != list.end(); it++)
			{
				SGIHostItemOsg ctx((*it));
				if (ctx.hasObject())
					treeItem->addChild(std::string(), &ctx);
			}
			ret = true;
		}
		break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::Options>::build(IObjectTreeItem * treeItem)
{
    bool ret;
    osgDB::Options * object = getObject<osgDB::Options, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getReadFileCallback() ||
                object->getWriteFileCallback() ||
                object->getFileLocationCallback() ||
                object->getFindFileCallback()
                )
            {
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
            }

            SGIHostItemOsg fileCache(object->getFileCache());
            if(fileCache.hasObject())
                treeItem->addChild("FileCache", &fileCache);

            SGIHostItemOsg authenticationMap(object->getAuthenticationMap());
            if(authenticationMap.hasObject())
                treeItem->addChild("AuthenticationMap", &authenticationMap);

            osg::ref_ptr<osg::Node> terrain;
            if(object->getTerrain().lock(terrain) && terrain.valid())
            {
                SGIHostItemOsg item(terrain.get());
                treeItem->addChild("Terrain", &item);
            }

            osg::ref_ptr<osg::Group> parentGroup;
            if(object->getParentGroup().lock(parentGroup) && parentGroup.valid())
            {
                SGIHostItemOsg item(parentGroup.get());
                treeItem->addChild("ParentGroup", &item);
            }

        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg readFileCallback(object->getReadFileCallback());
            if(readFileCallback.hasObject())
                treeItem->addChild("ReadFileCallback", &readFileCallback);

            SGIHostItemOsg writeFileCallback(object->getWriteFileCallback());
            if(writeFileCallback.hasObject())
                treeItem->addChild("WriteFileCallback", &writeFileCallback);

            SGIHostItemOsg fileLocationCallback(object->getFileLocationCallback());
            if(fileLocationCallback.hasObject())
                treeItem->addChild("FileLocationCallback", &fileLocationCallback);

            SGIHostItemOsg findFileCallback(object->getFindFileCallback());
            if(findFileCallback.hasObject())
                treeItem->addChild("FindFileCallback", &findFileCallback);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::ReaderWriter>::build(IObjectTreeItem * treeItem)
{
    osgDB::ReaderWriter * object = getObject<osgDB::ReaderWriter, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::DatabasePager>::build(IObjectTreeItem * treeItem)
{
    DatabasePagerAccessor * object = static_cast<DatabasePagerAccessor*>(getObject<osgDB::DatabasePager,SGIItemOsg,DynamicCaster>());
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            osgDB::DatabasePager::PagedLODList * activePagedLODList = object->activePagedLODList();
            SGIHostItemOsg activePagedLODListItem(activePagedLODList);
            if (activePagedLODListItem.hasObject())
            {
                unsigned numberOfPagedLODs = activePagedLODList->size();
                treeItem->addChild(helpers::str_plus_count("PagedLODs", numberOfPagedLODs), &activePagedLODListItem);
            }

            treeItem->addChild(helpers::str_plus_count("Threads", object->getNumDatabaseThreads()), cloneItem<SGIItemOsg>(SGIItemTypeThreads));

            treeItem->addChild(helpers::str_plus_count("File requests", object->getLocalFileRequestListSize()), cloneItem<SGIItemOsg>(SGIItemTypeDBPagerFileRequests));
            treeItem->addChild(helpers::str_plus_count("HTTP requests", object->getHttpRequestListSize()), cloneItem<SGIItemOsg>(SGIItemTypeDBPagerHttpRequests));
            treeItem->addChild(helpers::str_plus_count("Data to compile", object->getDataToCompileListSize()), cloneItem<SGIItemOsg>(SGIItemTypeDBPagerDataToCompile));
            treeItem->addChild(helpers::str_plus_count("Data to merge", object->getDataToMergeListSize()), cloneItem<SGIItemOsg>(SGIItemTypeDBPagerDataToMerge));
        }
        break;
    case SGIItemTypeThreads:
        {
            for(unsigned n = 0; n < object->getNumDatabaseThreads(); n++)
            {
                SGIHostItemOsg thread(object->getDatabaseThread(n));
                treeItem->addChild(helpers::str_plus_number("Thread", n), &thread);
            }
            ret = true;
        }
        break;
    case SGIItemTypeActivePagedLODs:
        {
            osgDB::DatabasePager::PagedLODList * activePagedLODList = object->activePagedLODList();
            const DatabasePagerAccessor::SetBasedPagedLODList * list = static_cast<const DatabasePagerAccessor::SetBasedPagedLODList *>(activePagedLODList);
            for(DatabasePagerAccessor::SetBasedPagedLODList::const_iterator it = list->begin(); it != list->end(); it++)
            {
                const osg::observer_ptr<osg::PagedLOD> & value = *it;
                osg::ref_ptr<osg::PagedLOD> node;
                if(value.lock(node))
                {
                    SGIHostItemOsg child(node);
                    if(child.hasObject())
                        treeItem->addChild(std::string(), &child);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerFileRequests:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyFileRequests(requestList);
            if (itemNumber() == ~0u)
            {
                for (auto it = requestList.begin(); it != requestList.end(); it++)
                {
                    const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                    SGIHostItemOsg child(req);
                    if (child.hasObject())
                        treeItem->addChild(std::string(), &child);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerHttpRequests:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyHttpRequests(requestList);
            if (itemNumber() == ~0u)
            {
                for (auto it = requestList.begin(); it != requestList.end(); it++)
                {
                    const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                    SGIHostItemOsg child(req);
                    if (child.hasObject())
                        treeItem->addChild(std::string(), &child);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToCompile:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyDataToCompile(requestList);
            if (itemNumber() == ~0u)
            {
                for (auto it = requestList.begin(); it != requestList.end(); it++)
                {
                    const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                    SGIHostItemOsg child(req);
                    if (child.hasObject())
                        treeItem->addChild(std::string(), &child);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToMerge:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyDataToMerge(requestList);
            if (itemNumber() == ~0u)
            {
                for (auto it = requestList.begin(); it != requestList.end(); it++)
                {
                    const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                    SGIHostItemOsg child(req);
                    if (child.hasObject())
                        treeItem->addChild(std::string(), &child);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::DatabaseThread)

bool objectTreeBuildImpl<osgDB::DatabasePager::DatabaseThread>::build(IObjectTreeItem * treeItem)
{
    osgDB::DatabasePager::DatabaseThread * object = getObject<osgDB::DatabasePager::DatabaseThread,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::ImagePager>::build(IObjectTreeItem * treeItem)
{
    osgDB::ImagePager * object = getObject<osgDB::ImagePager,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild(helpers::str_plus_count("Threads", object->getNumImageThreads()), cloneItem<SGIItemOsg>(SGIItemTypeThreads));
        }
        break;
    case SGIItemTypeThreads:
        {
            for(unsigned n = 0; n < object->getNumImageThreads(); n++)
            {
                SGIHostItemOsg thread(object->getImageThread(n));
                treeItem->addChild(helpers::str_plus_number("Thread", n), &thread);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::FileCache>::build(IObjectTreeItem * treeItem)
{
    osgDB::FileCache * object = getObject<osgDB::FileCache, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgDB::FileCache::DatabaseRevisionsList& revlist = object->getDatabaseRevisionsList();
            if(!revlist.empty())
                treeItem->addChild(helpers::str_plus_count("Revisions", revlist.size()), cloneItem<SGIItemOsg>(SGIItemTypeDatabaseRevisions));
        }
        break;
    case SGIItemTypeDatabaseRevisions:
        {
            const osgDB::FileCache::DatabaseRevisionsList& revlist = object->getDatabaseRevisionsList();
            for(osgDB::FileCache::DatabaseRevisionsList::const_iterator it = revlist.begin(); it != revlist.end(); it++)
            {
                SGIHostItemOsg rev((*it).get());
                treeItem->addChild(std::string(), &rev);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::FileList>::build(IObjectTreeItem * treeItem)
{
    osgDB::FileList * object = getObject<osgDB::FileList, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::ObjectCache>::build(IObjectTreeItem * treeItem)
{
    ObjectCacheAccess * object = static_cast<ObjectCacheAccess *>(getObject<osgDB::ObjectCache, SGIItemOsg>());
    bool ret;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            unsigned num = object->getNumItems();
            if (num)
                treeItem->addChild(helpers::str_plus_count("Objects", num), cloneItem<SGIItemOsg>(SGIItemTypeCachedObjects, ~0u));
        }
        break;
    case SGIItemTypeCachedObjects:
        {
            ObjectCacheAccess::ItemList items;
            object->getItems(items);
            if (itemNumber() == ~0u)
            {
                for (unsigned i = 0; i < items.size(); ++i)
                {
                    const auto & item = items[i];
                    treeItem->addChild(helpers::str_plus_info(item.name, i), cloneItem<SGIItemOsg>(SGIItemTypeCachedObjects, i));
                }
            }
            else if(itemNumber() < items.size())
            {
                const auto & item = items[itemNumber()];
                SGIHostItemOsg object(item.object);
                if(object.hasObject())
                    treeItem->addChild("Object", &object);
                SGIHostItemOsg options(item.options);
                if(options.hasObject())
                    treeItem->addChild("Options", &options);

            }
            ret = true;
        }
        break;

    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
bool objectTreeBuildImpl<osgDB::ObjectWrapper>::build(IObjectTreeItem * treeItem)
{
    osgDB::ObjectWrapper * object = getObject<osgDB::ObjectWrapper, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
            const osgDB::ObjectWrapper::SerializerList & serializers = object->getSerializerList();
            if(!serializers.empty())
                treeItem->addChild(helpers::str_plus_count("Serializers", serializers.size()), cloneItem<SGIItemOsg>(SGIItemTypeSerializers, ~0u));
#endif
        }
        break;
    case SGIItemTypeSerializers:
        {
            if (itemNumber() == ~0u)
            {
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
                const osgDB::ObjectWrapper::SerializerList & serializers = object->getSerializerList();
                for(auto it = serializers.begin(); it != serializers.end(); it++)
                {
                    SGIHostItemOsg serializer((*it).get());
                    treeItem->addChild(std::string(), &serializer);
                }
#endif
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::ObjectWrapperManager>::build(IObjectTreeItem * treeItem)
{
    osgDB::ObjectWrapperManager * object = getObject<osgDB::ObjectWrapperManager, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgDB::ObjectWrapperManager::WrapperMap & wrappers = object->getWrapperMap();
            if(!wrappers.empty())
                treeItem->addChild(helpers::str_plus_count("Wrappers", wrappers.size()), cloneItem<SGIItemOsg>(SGIItemTypeWrappers));

            const osgDB::ObjectWrapperManager::CompressorMap & compressors = object->getCompressorMap();
            if(!compressors.empty())
                treeItem->addChild(helpers::str_plus_count("Compressors", compressors.size()), cloneItem<SGIItemOsg>(SGIItemTypeCompressors));
        }
        break;
    case SGIItemTypeWrappers:
        {
            const osgDB::ObjectWrapperManager::WrapperMap & wrappers = object->getWrapperMap();
            for(auto it = wrappers.begin(); it != wrappers.end(); it++)
            {
                SGIHostItemOsg wrapper(it->second.get());
                treeItem->addChild(it->first, &wrapper);
            }
            ret = true;
        }
        break;
    case SGIItemTypeCompressors:
        {
            const osgDB::ObjectWrapperManager::CompressorMap & compressors = object->getCompressorMap();
            for(auto it = compressors.begin(); it != compressors.end(); it++)
            {
                SGIHostItemOsg compressor(it->second.get());
                treeItem->addChild(it->first, &compressor);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::DatabaseRevisions>::build(IObjectTreeItem * treeItem)
{
    osgDB::DatabaseRevisions * object = getObject<osgDB::DatabaseRevisions, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgDB::DatabaseRevisions::DatabaseRevisionList& revlist = object->getDatabaseRevisionList();
            if(!revlist.empty())
                treeItem->addChild(helpers::str_plus_count("Revisions", revlist.size()), cloneItem<SGIItemOsg>(SGIItemTypeDatabaseRevisions));
        }
        break;
    case SGIItemTypeDatabaseRevisions:
        {
            const osgDB::DatabaseRevisions::DatabaseRevisionList& revlist = object->getDatabaseRevisionList();
            for(osgDB::DatabaseRevisions::DatabaseRevisionList::const_iterator it = revlist.begin(); it != revlist.end(); it++)
            {
                SGIHostItemOsg rev((*it).get());
                treeItem->addChild(std::string(), &rev);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::DatabaseRevision>::build(IObjectTreeItem * treeItem)
{
    osgDB::DatabaseRevision * object = getObject<osgDB::DatabaseRevision, SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg filesAdded(object->getFilesAdded());
            if(filesAdded.hasObject())
                treeItem->addChild("Files added", &filesAdded);
            SGIHostItemOsg filesModified(object->getFilesModified());
            if(filesModified.hasObject())
                treeItem->addChild("Files modified", &filesModified);
            SGIHostItemOsg filesRemoved(object->getFilesRemoved());
            if(filesRemoved.hasObject())
                treeItem->addChild("Files removed", &filesRemoved);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgDB::DatabasePager::DatabaseRequest>::build(IObjectTreeItem * treeItem)
{
	osgDB::DatabasePager::DatabaseRequest * object = getObject<osgDB::DatabasePager::DatabaseRequest, SGIItemOsg>();
	bool ret;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			osg::ref_ptr<osg::Node> terrain;
			if (object->_terrain.lock(terrain))
			{
				SGIHostItemOsg item(terrain.get());
				if (item.hasObject())
					treeItem->addChild("Terrain", &item);
			}
			osg::ref_ptr<osg::Group> group;
			if (object->_group.lock(group))
			{
				SGIHostItemOsg item(group.get());
				if (item.hasObject())
					treeItem->addChild("Group", &item);
			}
			SGIHostItemOsg loadedModel(object->_loadedModel.get());
			if (loadedModel.hasObject())
				treeItem->addChild("LoadedModel", &loadedModel);

			SGIHostItemOsg options(object->_loadOptions.get());
			if (options.hasObject())
				treeItem->addChild("Options", &options);

			SGIHostItemOsg objcache(object->_objectCache.get());
			if (objcache.hasObject())
				treeItem->addChild("Object cache", &objcache);

			osg::ref_ptr<osgUtil::IncrementalCompileOperation::CompileSet> compileset;
			if (object->_compileSet.lock(compileset))
			{
				SGIHostItemOsg item(compileset.get());
				if (item.hasObject())
					treeItem->addChild("ICO compile set", &item);
			}
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgDB::DatabasePager::RequestQueue>::build(IObjectTreeItem * treeItem)
{
	osgDB::DatabasePager::RequestQueue * object = getObject<osgDB::DatabasePager::RequestQueue, SGIItemOsg>();
	bool ret;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgDB::DatabasePager::ReadQueue>::build(IObjectTreeItem * treeItem)
{
	osgDB::DatabasePager::ReadQueue * object = getObject<osgDB::DatabasePager::ReadQueue, SGIItemOsg>();
	bool ret;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}


bool objectTreeBuildImpl<osgUtil::RenderBin>::build(IObjectTreeItem * treeItem)
{
    osgUtil::RenderBin * object = getObject<osgUtil::RenderBin,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getSortCallback())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            SGIHostItemOsg parent(object->getParent());
            if(parent.hasObject())
                treeItem->addChild("Parent", &parent);
            if(object->getStage() != object)
            {
                SGIHostItemOsg stage(object->getStage());
                if(stage.hasObject())
                    treeItem->addChild("Stage", &stage);
            }
            SGIHostItemOsg stateset(object->getStateSet());
            if(stateset.hasObject())
                treeItem->addChild("StateSet", &stateset);
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg sortcallback(object->getSortCallback());
            if(sortcallback.hasObject())
                treeItem->addChild(std::string(), &sortcallback);

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgUtil::RenderStage>::build(IObjectTreeItem * treeItem)
{
    osgUtil::RenderStage * object = getObject<osgUtil::RenderStage,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg texture(object->getTexture());
            if(texture.hasObject())
                treeItem->addChild("Texture", &texture);
            SGIHostItemOsg image(object->getImage());
            if(image.hasObject())
                treeItem->addChild("Image", &image);
            
            SGIHostItemOsg fbo(object->getFrameBufferObject());
            if(fbo.hasObject())
                treeItem->addChild("FBO", &fbo);
            SGIHostItemOsg mutlisamplefbo(object->getMultisampleResolveFramebufferObject());
            if(mutlisamplefbo.hasObject())
                treeItem->addChild("Multisample FBO", &mutlisamplefbo);

            SGIHostItemOsg graphiccontext(object->getGraphicsContext());
            if(graphiccontext.hasObject())
                treeItem->addChild("GraphicsContext", &graphiccontext);

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgUtil::RenderLeaf>::build(IObjectTreeItem * treeItem)
{
    osgUtil::RenderLeaf * object = getObject<osgUtil::RenderLeaf,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->_parent)
            {
                SGIHostItemOsg parent(object->_parent);
                treeItem->addChild("Parent", &parent);
            }
            SGIHostItemOsg drawable(object->getDrawable());
            if(drawable.hasObject())
                treeItem->addChild("Drawable", &drawable);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgUtil::StateGraph>::build(IObjectTreeItem * treeItem)
{
    osgUtil::StateGraph * object = getObject<osgUtil::StateGraph,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(!object->_children.empty())
                treeItem->addChild(helpers::str_plus_count("Childs", object->_children.size()), cloneItem<SGIItemOsg>(SGIItemTypeChilds));
            if(!object->_leaves.empty())
                treeItem->addChild(helpers::str_plus_count("Leaves", object->_leaves.size()), cloneItem<SGIItemOsg>(SGIItemTypeLeaves));

            if(object->_parent)
            {
                osg::ref_ptr<osgUtil::StateGraph> safe_ptr(object->_parent);
                SGIHostItemOsg item(safe_ptr.get());
                treeItem->addChild("Parent", &item);
                safe_ptr.release();
            }
            {
                osg::ref_ptr<osg::StateSet> safe_ptr(const_cast<osg::StateSet*>(object->getStateSet()));
                SGIHostItemOsg item(safe_ptr.get());
                if(item.hasObject())
                    treeItem->addChild("StateSet", &item);
                safe_ptr.release();
            }

            SGIHostItemOsg userdata(object->getUserData());
            if(userdata.hasObject())
                treeItem->addChild("UserData", &userdata);
        }
        break;
    case SGIItemTypeChilds:
        {
            for(osgUtil::StateGraph::ChildList::const_iterator it = object->_children.begin(); it != object->_children.end(); it++)
            {
                SGIHostItemOsg child(it->second.get());
                treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeLeaves:
        {
            for(osgUtil::StateGraph::LeafList::const_iterator it = object->_leaves.begin(); it != object->_leaves.end(); it++)
            {
                SGIHostItemOsg child(*it);
                treeItem->addChild(std::string(), &child);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgUtil::SceneView>::build(IObjectTreeItem * treeItem)
{
    osgUtil::SceneView * object = getObject<osgUtil::SceneView,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg camera(object->getCamera());
            if(camera.hasObject())
                treeItem->addChild("Camera", &camera);
            
            SGIHostItemOsg scenedata(object->getSceneData());
            if(scenedata.hasObject())
                treeItem->addChild("SceneData", &scenedata);

            SGIHostItemOsg displaySettings(object->getDisplaySettings());
            if(displaySettings.hasObject())
                treeItem->addChild("DisplaySettings", &displaySettings);

            SGIHostItemOsg state(object->getState());
            if(state.hasObject())
                treeItem->addChild("State", &state);

            SGIHostItemOsg globalStateSet(object->getGlobalStateSet());
            if(globalStateSet.hasObject())
                treeItem->addChild("GlobalStateSet", &globalStateSet);

            SGIHostItemOsg secondaryStateSet(object->getSecondaryStateSet());
            if(secondaryStateSet.hasObject())
                treeItem->addChild("SecondaryStateSet", &secondaryStateSet);

            SGIHostItemOsg localStateSet(object->getLocalStateSet());
            if(localStateSet.hasObject())
                treeItem->addChild("LocalStateSet", &localStateSet);

            SGIHostItemOsg light(object->getLight());
            if(light.hasObject())
                treeItem->addChild("Light", &light);

            SGIHostItemOsg view(object->getView());
            if(view.hasObject())
                treeItem->addChild("View", &view);

            SGIHostItemOsg initVisitor(object->getInitVisitor());
            if(initVisitor.hasObject())
                treeItem->addChild("InitVisitor", &initVisitor);

            SGIHostItemOsg updateVisitor(object->getUpdateVisitor());
            if(updateVisitor.hasObject())
                treeItem->addChild("UpdateVisitor", &updateVisitor);

            SGIHostItemOsg cullVisitor(object->getCullVisitor());
            if(cullVisitor.hasObject())
                treeItem->addChild("CullVisitor", &cullVisitor);

            SGIHostItemOsg stateGraph(object->getStateGraph());
            if(stateGraph.hasObject())
                treeItem->addChild("StateGraph", &stateGraph);

            SGIHostItemOsg renderStage(object->getRenderStage());
            if(renderStage.hasObject())
                treeItem->addChild("RenderStage", &renderStage);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::Terrain>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::Terrain * object = getObject<osgTerrain::Terrain,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::TerrainTile>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::TerrainTile * object = getObject<osgTerrain::TerrainTile,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg terrain(object->getTerrain());
            if(terrain.hasObject())
                treeItem->addChild("Terrain", &terrain);

            SGIHostItemOsg locator(object->getLocator());
            if(locator.hasObject())
                treeItem->addChild("Locator", &locator);

            SGIHostItemOsg terrainTechnique(object->getTerrainTechnique());
            if(terrainTechnique.hasObject())
                treeItem->addChild("TerrainTechnique", &terrainTechnique);

            SGIHostItemOsg elevationLayer(object->getElevationLayer());
            if(elevationLayer.hasObject())
                treeItem->addChild("Elevation Layer", &elevationLayer);

            unsigned numColorLayers = object->getNumColorLayers();
            if(numColorLayers)
                treeItem->addChild(helpers::str_plus_count("ColorLayers", numColorLayers), cloneItem<SGIItemOsg>(SGIItemTypeColorLayers));

        }
        break;
    case SGIItemTypeColorLayers:
        {
            unsigned numColorLayers = object->getNumColorLayers();
            for(unsigned n = 0; n < numColorLayers; n++)
            {
                SGIHostItemOsg colorLayer(object->getColorLayer(n));
                if(colorLayer.hasObject())
                    treeItem->addChild(helpers::str_plus_number("Color Layer", n), &colorLayer);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::Layer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::Layer * object = getObject<osgTerrain::Layer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::ImageLayer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::ImageLayer * object = getObject<osgTerrain::ImageLayer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg image(object->getImage());
            if(image.hasObject())
                treeItem->addChild("Image", &image);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::ContourLayer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::ContourLayer * object = getObject<osgTerrain::ContourLayer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg image(object->getImage());
            if(image.hasObject())
                treeItem->addChild("Image", &image);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::HeightFieldLayer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::HeightFieldLayer * object = getObject<osgTerrain::HeightFieldLayer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg heightField(object->getHeightField());
            if(heightField.hasObject())
                treeItem->addChild("Height field", &heightField);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::ProxyLayer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::ProxyLayer * object = getObject<osgTerrain::ProxyLayer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg image(object->getImage());
            if(image.hasObject())
                treeItem->addChild("Image", &image);

            SGIHostItemOsg implementation(object->getImplementation());
            if(implementation.hasObject())
                treeItem->addChild("Implementation", &implementation);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgTerrain::CompositeLayer>::build(IObjectTreeItem * treeItem)
{
    osgTerrain::CompositeLayer * object = getObject<osgTerrain::CompositeLayer,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            unsigned num = object->getNumLayers();
            for(unsigned n = 0; n < num; n++)
            {
                SGIHostItemOsg layer(object->getLayer(n));
                if(layer.hasObject())
                    treeItem->addChild(helpers::str_plus_number("Layer", n), &layer);
            }
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgText::Font>::build(IObjectTreeItem * treeItem)
{
    osgText::Font * object = getObject<osgText::Font,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSG_VERSION_LESS_THAN(3,5,9)
            SGIHostItemOsg stateSet(object->getStateSet());
            if(stateSet.hasObject())
                treeItem->addChild("StateSet", &stateSet);

            SGIHostItemOsg texEnv(object->getTexEnv());
            if(texEnv.hasObject())
                treeItem->addChild("TexEnv", &texEnv);
#endif

            SGIHostItemOsg implementation(object->getImplementation());
            if(implementation.hasObject())
                treeItem->addChild("Implementation", &implementation);

			const osgText::Font::GlyphTextureList & textureList = object->getGlyphTextureList();
			if (!textureList.empty())
				treeItem->addChild(helpers::str_plus_count("TextureList", textureList.size()), cloneItem<SGIItemOsg>(SGIItemTypeFontTextureList));
        }
        break;
	case SGIItemTypeFontTextureList:
		{
			const osgText::Font::GlyphTextureList & textureList = object->getGlyphTextureList();
			for(const auto & texture : textureList)
			{
				SGIHostItemOsg item(texture.get());
				treeItem->addChild(std::string(), &item);
			}
			ret = true;
		}
		break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgText::TextBase>::build(IObjectTreeItem * treeItem)
{
    osgTextBaseAccess * object = static_cast<osgTextBaseAccess *>(getObject<osgText::TextBase, SGIItemOsg>());
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
			SGIHostItemOsg font(object->getFont());
			if (font.hasObject())
				treeItem->addChild("Font", &font);
			SGIHostItemOsg style(object->getStyle());
			if (style.hasObject())
				treeItem->addChild("Style", &style);

            const auto & primitives = object->getPrimitives();
            if (!primitives.empty())
                treeItem->addChild(helpers::str_plus_count("Primitives", primitives.size()), cloneItem<SGIItemOsg>(SGIItemTypePrimitiveSetList));

            SGIHostItemOsg vbo(object->getVBO());
            if (vbo.hasObject())
                treeItem->addChild("VBO", &vbo);
            SGIHostItemOsg ebo(object->getEBO());
            if (ebo.hasObject())
                treeItem->addChild("EBO", &ebo);
		}
        break;
    case SGIItemTypePrimitiveSetList:
        {
            for (auto prim : object->getPrimitives())
            {
                SGIHostItemOsg item(prim);
                if (item.hasObject())
                    treeItem->addChild(std::string(), &item);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgText::Text>::build(IObjectTreeItem * treeItem)
{
    osgText::Text * object = getObject<osgText::Text,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgAnimation::Animation>::build(IObjectTreeItem * treeItem)
{
    osgAnimation::Animation * object = getObject<osgAnimation::Animation,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgAnimation::ChannelList& channels = object->getChannels();
            if(!channels.empty())
                treeItem->addChild(helpers::str_plus_count("Channels", channels.size()), cloneItem<SGIItemOsg>(SGIItemTypeChannels));
        }
        break;
    case SGIItemTypeChannels:
        {
            const osgAnimation::ChannelList & channels = object->getChannels();
            for(osgAnimation::ChannelList::const_iterator it = channels.begin(); it != channels.end(); it++)
            {
                SGIHostItemOsg channel((*it).get());
                if(channel.hasObject())
                    treeItem->addChild(std::string(), &channel);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgAnimation::AnimationManagerBase>::build(IObjectTreeItem * treeItem)
{
    osgAnimation::AnimationManagerBase * object = getObject<osgAnimation::AnimationManagerBase,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            AnimationManagerBaseAccess * access = (AnimationManagerBaseAccess*)object;
            SGIHostItemOsg linkVisitor(access->getLinkVisitor());
            if(linkVisitor.hasObject())
                treeItem->addChild("LinkVisitor", &linkVisitor);

            const osgAnimation::AnimationList& animations = object->getAnimationList();
            if(!animations.empty())
                treeItem->addChild(helpers::str_plus_count("Animations", animations.size()), cloneItem<SGIItemOsg>(SGIItemTypeAnimations));
        }
        break;
    case SGIItemTypeAnimations:
        {
            const osgAnimation::AnimationList & animations = object->getAnimationList();
            for(osgAnimation::AnimationList::const_iterator it = animations.begin(); it != animations.end(); it++)
            {
                SGIHostItemOsg animation((*it).get());
                if(animation.hasObject())
                    treeItem->addChild(std::string(), &animation);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl_RenderInfoData(SGIPluginHostInterface * hostInterface, IObjectTreeItem * treeItem, SGIItemBase * item, const RenderInfoData & data)
{
    bool ret = false;
    SGIItemBase * itemNext = item->nextBase();
    switch (item->type())
    {
    case SGIItemTypeObject:
        ret = hostInterface->objectTreeBuildTree(treeItem, itemNext);
        if (ret)
        {
            const RenderInfoData::HashedState & hashedState = data.hashedState();
            treeItem->addChild(helpers::str_plus_count("States", hashedState.size()), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoState, ~0u));
        }
        break;
    case SGIItemTypeRenderInfoState:
    {
        const RenderInfoData::HashedState & hashedState = data.hashedState();
        if (item->number() == ~0u)
        {
            for (RenderInfoData::HashedState::const_iterator it = hashedState.begin(); it != hashedState.end(); it++)
            {
                const RenderInfoData::HashedStateId & hash = it->first;
                const RenderInfoData::State & state = it->second;
                unsigned contextID = (state.state.valid() ? state.state->getContextID() : ~0u);
                treeItem->addChild(helpers::str_plus_hex("State", contextID, hash), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoState, hash));
            }
        }
        else
        {
            RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
            if (it != hashedState.end())
            {
                const RenderInfoData::State & state = it->second;

                SGIHostItemOsg osgstate(state.state);
                treeItem->addChild("State", &osgstate);

                SGIHostItemOsg view(state.view);
                treeItem->addChild("View", &view);

                SGIHostItemOsg userData(state.userData);
                if(userData.hasObject())
                    treeItem->addChild("UserData", &userData);

                SGIHostItemOsg capturedStateSet(state.capturedStateSet);
                treeItem->addChild("CapturedStateSet", &capturedStateSet);

                SGIHostItemOsg combinedStateSet(state.combinedStateSet);
                treeItem->addChild("CombinedStateSet", &combinedStateSet);

                treeItem->addChild(helpers::str_plus_count("StateSetStack", state.stateSetStack.size()), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoStateSetStack, item->number()));
                treeItem->addChild(helpers::str_plus_count("RenderBinStack", state.renderBinStack.size()), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoRenderBinStack, item->number()));
                treeItem->addChild(helpers::str_plus_count("CameraStack", state.cameraStack.size()), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoCameraStack, item->number()));
                treeItem->addChild(helpers::str_plus_count("AppliedProgramSet", state.appliedProgamSet.size()), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoAppliedProgramSet, item->number()));
            }
        }
        ret = true;
    }
    break;
    case SGIItemTypeRenderInfoStateSetStack:
    {
        const RenderInfoData::HashedState & hashedState = data.hashedState();
        if (item->number() == ~0u)
        {
            for (auto it = hashedState.begin(); it != hashedState.end(); ++it)
            {
                const RenderInfoData::HashedStateId & state = it->first;
                treeItem->addChild(helpers::str_plus_hex("State", state), item->clone<SGIItemOsg>((sgi::SGIItemType)SGIItemTypeRenderInfoStateSetStack, state));
            }
        }
        else
        {
            RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
            if (it != hashedState.end())
            {
                const RenderInfoData::State & state = it->second;
                const RenderInfoData::StateSetStack & lastStack = state.stateSetStack;
                for (RenderInfoData::StateSetStack::const_iterator it = lastStack.begin(); it != lastStack.end(); it++)
                {
                    const RenderInfoData::StateSetEntry & entry = *it;
                    SGIHostItemOsg child(entry.stateSet.get());
                    treeItem->addChild(std::string(), &child);
                }

                SGIHostItemOsg combinedStateSet(state.combinedStateSet);
                treeItem->addChild("CombinedStateSet", &combinedStateSet);

            }
        }
        ret = true;
    }
    break;
    case SGIItemTypeRenderInfoRenderBinStack:
    {
        const RenderInfoData::HashedState & hashedState = data.hashedState();
        RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
        if (it != hashedState.end())
        {
            const RenderInfoData::State & state = it->second;
            const RenderInfoData::RenderBinStack & lastStack = state.renderBinStack;
            for (RenderInfoData::RenderBinStack::const_iterator it = lastStack.begin(); it != lastStack.end(); it++)
            {
                SGIHostItemOsg child(*it);
                treeItem->addChild(std::string(), &child);
            }
        }
        ret = true;
    }
    break;
    case SGIItemTypeRenderInfoCameraStack:
    {
        const RenderInfoData::HashedState & hashedState = data.hashedState();
        RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
        if (it != hashedState.end())
        {
            const RenderInfoData::State & state = it->second;
            const RenderInfoData::CameraStack & lastStack = state.cameraStack;
            for (RenderInfoData::CameraStack::const_iterator it = lastStack.begin(); it != lastStack.end(); it++)
            {
                SGIHostItemOsg child(*it);
                treeItem->addChild(std::string(), &child);
            }
        }
        ret = true;
    }
    break;
    case SGIItemTypeRenderInfoAppliedProgramSet:
    {
        const RenderInfoData::HashedState & hashedState = data.hashedState();
        RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
        if (it != hashedState.end())
        {
            const RenderInfoData::State & state = it->second;
            const RenderInfoData::PerContextProgramSet & set = state.appliedProgamSet;
            for (RenderInfoData::PerContextProgramSet::const_iterator it = set.begin(); it != set.end(); it++)
            {
                SGIHostItemOsg child(*it);
                treeItem->addChild(std::string(), &child);
            }
        }
        ret = true;
    }
    break;
    default:
        ret = hostInterface->objectTreeBuildTree(treeItem, itemNext);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<RenderInfoDrawCallback>::build(IObjectTreeItem * treeItem)
{
    RenderInfoDrawCallback * object = getObject<RenderInfoDrawCallback, SGIItemOsg, DynamicCaster>();
    const RenderInfoData & data = object->data();
    bool ret = objectTreeBuildImpl_RenderInfoData(_hostInterface, treeItem, _item, data);
    return ret;
}

bool objectTreeBuildImpl<RenderInfoDrawable>::build(IObjectTreeItem * treeItem)
{
    RenderInfoDrawable * object = getObject<RenderInfoDrawable,SGIItemOsg>();
    const RenderInfoData & data = object->data();
    bool ret = objectTreeBuildImpl_RenderInfoData(_hostInterface, treeItem, _item, data);
    return ret;
}

bool objectTreeBuildImpl<RenderInfoGeometry>::build(IObjectTreeItem * treeItem)
{
    RenderInfoGeometry * object = getObject<RenderInfoGeometry, SGIItemOsg>();
    const RenderInfoData & data = object->data();
    bool ret = objectTreeBuildImpl_RenderInfoData(_hostInterface, treeItem, _item, data);
    return ret;
}

bool objectTreeBuildImpl<ReferencedPickerBase>::build(IObjectTreeItem * treeItem)
{
    ReferencedPickerBase * object = getObject<ReferencedPickerBase,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg root(object->root());
            if(root.hasObject())
                treeItem->addChild("Root", &root);

            SGIHostItemOsg view(object->view());
            if(view.hasObject())
                treeItem->addChild("View", &view);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#ifdef SGI_USE_OSGEARTH
bool objectTreeBuildImpl<ReferencedPicker>::build(IObjectTreeItem * treeItem)
{
    ReferencedPicker * object = getObject<ReferencedPicker,SGIItemOsg>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const ReferencedPicker::Hits & hits = object->hits();
            if(!hits.empty())
                treeItem->addChild(helpers::str_plus_count("Hits", hits.size()), cloneItem<SGIItemOsg>(SGIItemTypePickerHits));
        }
        break;
    case SGIItemTypePickerHits:
        {
            const ReferencedPicker::Hits & hits = object->hits();
            unsigned num = 0;
            for(ReferencedPicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, num++)
            {
                const ReferencedPicker::Hit & hit = *it;
                std::stringstream ss;
                ss << "Hit#" << num << "@" << hit.ratio;
                if(!hit.nodePath.empty())
                {
                    osg::Node * back = hit.nodePath.back();
                    ss << ' ' << getObjectName(back) << " (" << getObjectTypename(back) << ")";
                }
                treeItem->addChild(ss.str(), cloneItem<SGIItemOsg>(SGIItemTypePickerHit, num));
            }
            ret = true;
        }
        break;
    case SGIItemTypePickerHit:
        {
            unsigned number = _item->number();
            const ReferencedPicker::Hits & hits = object->hits();

            unsigned current_num = 0;
            for(ReferencedPicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, current_num++)
            {
                if(current_num == number)
                {
                    const ReferencedPicker::Hit & hit = *it;
                    SGIHostItemOsg drawable(hit.drawable);
                    if(drawable.hasObject())
                        treeItem->addChild("Drawable", &drawable);

                    SGIHostItemOsg matrix(hit.matrix);
                    if(matrix.hasObject())
                        treeItem->addChild("Matrix", &matrix);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif

bool objectTreeBuildImpl<ReferencedLinePicker>::build(IObjectTreeItem * treeItem)
{
    ReferencedLinePicker * object = getObject<ReferencedLinePicker,SGIItemOsg,DynamicCaster>();
    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const ReferencedLinePicker::Hits & hits = object->hits();
            if(!hits.empty())
                treeItem->addChild(helpers::str_plus_count("Hits", hits.size()), cloneItem<SGIItemOsg>(SGIItemTypePickerHits));
        }
        break;
    case SGIItemTypePickerHits:
        {
            const ReferencedLinePicker::Hits & hits = object->hits();
            unsigned num = 0;
            for(ReferencedLinePicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, num++)
            {
                const ReferencedLinePicker::Hit & hit = *it;
                std::stringstream ss;
                ss << "Hit#" << num << "@" << hit.ratio;
                if(!hit.nodePath.empty())
                {
                    osg::Node * back = hit.nodePath.back();
                    ss << ' ' << getObjectName(back) << " (" << getObjectTypename(back) << ")";
                }
                treeItem->addChild(ss.str(), cloneItem<SGIItemOsg>(SGIItemTypePickerHit, num));
            }
            ret = true;
        }
        break;
    case SGIItemTypePickerHit:
        {
            unsigned number = _item->number();
            const ReferencedLinePicker::Hits & hits = object->hits();

            unsigned current_num = 0;
            for(ReferencedLinePicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, current_num++)
            {
                if(current_num == number)
                {
                    const ReferencedLinePicker::Hit & hit = *it;
                    SGIHostItemOsg drawable(hit.drawable);
                    if(drawable.hasObject())
                        treeItem->addChild("Drawable", &drawable);

                    SGIHostItemOsg matrix(hit.matrix);
                    if(matrix.hasObject())
                        treeItem->addChild("Matrix", &matrix);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

struct RegistrySingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemOsg hostItem(osgDB::Registry::instance());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

struct DefaultFontSingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemOsg hostItem(osgText::Font::getDefaultFont());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

struct CullingInfoSingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemOsg hostItem(CullingInfoRegistry::instance());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

    SGIHostItemInternal dbRegistryHostItem(new SGIProxyItemT<RegistrySingleton>(_hostInterface, "osgDB::Registry"));
    treeItem->addChild(std::string(), &dbRegistryHostItem);

    SGIHostItemInternal defaultFontHostItem(new SGIProxyItemT<DefaultFontSingleton>(_hostInterface, "osgText::DefaultFont"));
    treeItem->addChild(std::string(), &defaultFontHostItem);

    SGIHostItemInternal cullingInfoItem(new SGIProxyItemT<CullingInfoSingleton>(_hostInterface, "CullingInfo"));
    treeItem->addChild(std::string(), &cullingInfoItem);

    SGIItemOsg * osgitem = dynamic_cast<SGIItemOsg *>(object->item());
    if (osgitem)
    {
        osg::Node * node = NULL;
        osg::View * view = dynamic_cast<osg::View*>(osgitem->object());
        if(view)
            node = view->getCamera();
        else
            node = dynamic_cast<osg::Node*>(osgitem->object());
        if (node)
        {
            osg_helpers::FindTreeItemNodeVisitor ftinv;
            node->accept(ftinv);
            for(osg_helpers::FindTreeItemNodeVisitor::NodeList::const_iterator it = ftinv.results().begin(); it != ftinv.results().end(); ++it)
            {
                const osg_helpers::FindTreeItemNodeVisitor::NodeItem & item = *it;
                osg::Node * node = item.node.get();
                SGIHostItemOsg hostItem(node);
                if(hostItem.hasObject())
                {
                    // always add the node item to the tree
                    treeItem->addChild((*it).name, &hostItem);

                    if(item.imageGeode)
                    {
                        // ... and if it is a image geode try to add the image to the tree as well
                        SGIHostItemOsg image(item.getImageGeodeTexture());
                        if(image.hasObject())
                            treeItem->addChild(item.name, &image);
                    }
                    else if (item.heightField)
                    {
                        SGIHostItemOsg image(item.getHeightField());
                        if (image.hasObject())
                            treeItem->addChild(item.name, &image);
                    }
                }
            }
        }
    }

    return true;
}

} // namespace osg_plugin
} // namespace sgi
