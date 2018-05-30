#include "stdafx.h"
#include "ContextMenuOSG.h"
#include <sgi/helpers/osg>
#include "SGIItemOsg"

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectLoggerDialog>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/helpers/string>

#include <osg/Version>
#include <osg/PagedLOD>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/CameraView>
#include <osg/ProxyNode>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TexEnv>
#include <osg/TexEnvFilter>
#include <osg/TexMat>
#include <osg/ClipNode>
#include <osg/Depth>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/LightModel>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/PolygonMode>
#include <osg/Point>
#include <osg/CullFace>
#include <osg/io_utils>

#include <osgDB/Registry>

#include <osgViewer/View>
#include <osgViewer/ViewerBase>

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#endif
#include <osgText/Text>
#include <osgAnimation/AnimationManagerBase>
#include <osgUtil/IncrementalCompileOperation>

#include "MenuActionOSG.h"
#include "stateset_helpers.h"
#include "osg_accessor.h"
#include "osgtext_accessor.h"
#include "DrawableHelper.h"
#include "osganimation_accessor.h"
#include "ObjectLoggerOSG.h"

namespace sgi {

class SGIItemOsg;

namespace osg_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Referenced)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Object)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Node)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Group)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::StateSet)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Geode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Transform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::MatrixTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::PositionAttitudeTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::AutoTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Billboard)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::CameraView)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Viewport)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Camera)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::View)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::ProxyNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::LOD)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::PagedLOD)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Drawable)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::ShapeDrawable)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Geometry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Sphere)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Box)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Cone)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Cylinder)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Capsule)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::InfinitePlane)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::TriangleMesh)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::HeightField)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::CompositeShape)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Image)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::StateAttribute)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Texture)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Texture2D)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Texture3D)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::TexEnv)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::TexEnvFilter)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::TexMat)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::ClipNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::GraphicsContext)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Shader)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Program)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Depth)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Uniform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::BufferObject)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::BufferData)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Array)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Material)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::LineWidth)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::LineStipple)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::LightModel)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::BlendFunc)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::BlendColor)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::PolygonMode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::CullFace)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgDB::Registry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgDB::ImagePager)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgGA::GUIEventHandler)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgGA::GUIEventAdapter)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgGA::CameraManipulator)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgViewer::View)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgViewer::Scene)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgViewer::ViewerBase)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgViewer::GraphicsWindow)

#ifdef SGI_USE_OSGQT
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgQt::GraphicsWindowQt)
#endif

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgText::TextBase)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgText::Text)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgUtil::IncrementalCompileOperation)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgAnimation::AnimationManagerBase)

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(sgi::ISceneGraphDialogToolsMenu)

using namespace sgi::osg_helpers;

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::Type & t);

namespace {
    static void buildStateAttributeOverrideMenu(IContextMenuItem * childMenu)
    {
        childMenu->addModeAction("Unspecified", StateAttributeModeValueUnspecified);
        childMenu->addModeAction("Off", StateAttributeModeValueOff);
        childMenu->addModeAction("On", StateAttributeModeValueOn);
        childMenu->addModeAction("Override On", StateAttributeModeValueOverrideOn);
        childMenu->addModeAction("Override Off", StateAttributeModeValueOverrideOff);
        childMenu->addModeAction("Protected", StateAttributeModeValueProtected);
    }
    static IContextMenuItem * createTextureAttributeMenu(unsigned textureUnit, unsigned member, osg::StateAttribute::Type type, IContextMenuItem * menuItem,
                                             SGIItemBase * item, osg::StateAttribute * attr, const osg::StateAttribute::OverrideValue value)
    {
        std::stringstream ss;
        ss << helpers::str_plus_number("Texture", textureUnit) << '/' << member << ':' << type;

        StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(value);
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetTextureAttributeSet, ss.str(), item, currentMode, new ReferencedDataTextureAttributePair(TextureAttributePair(textureUnit, attr)));
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetTextureAttributeRemove, "Remove", item, new ReferencedDataTextureAttributePair(TextureAttributePair(textureUnit, attr)));
        return childMenu;
    }
    static IContextMenuItem * createTextureModeMenu(unsigned textureUnit, osg::StateAttribute::GLMode mode, IContextMenuItem * menuItem,
                                             SGIItemBase * item, const osg::StateAttribute::GLModeValue value)
    {
        std::stringstream ss;
        ss << helpers::str_plus_number("Texture", textureUnit) << '/' << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode);

        StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(value);
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetTextureModeSet, ss.str(), item, currentMode, new ReferencedDataIntPair(IntPair(textureUnit, mode)));
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetTextureModeRemove, "Remove", item, new ReferencedDataIntPair(IntPair(textureUnit, mode)));
        return childMenu;
    }

    static IContextMenuItem * createStateAttributeMenu(unsigned member, osg::StateAttribute::Type type, osg::StateAttribute * attr, IContextMenuItem * menuItem,
                                             SGIItemBase * item, const osg::StateAttribute::OverrideValue value)
    {
        std::stringstream ss;
        ss << '#' << member << ':' << type << ' ' << attr->getName();

        StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(value);
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetAttributeSet, ss.str(), item, currentMode, attr);
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetAttributeRemove, "Remove", item, new ReferencedDataIntPair(IntPair((int)type, (int)member)));
        return childMenu;
    }
    static IContextMenuItem * createUniformMenu(osg::Uniform * uniform, IContextMenuItem * menuItem, SGIItemBase * item,
                                                const osg::StateAttribute::OverrideValue value)
    {
        StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(value);
        SGIHostItemOsg uniformItem(uniform);
        std::stringstream ss;
        ss << osg::Uniform::getTypename(uniform->getType()) << ' ' << uniform->getName();
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetUniformSet, ss.str(), item, currentMode, uniform);
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetUniformRemove, "Remove", item, new ReferencedDataString(uniform->getName()));
        return childMenu;
    }

    static IContextMenuItem * createDefineMenu(const std::string & name, const std::string & value, IContextMenuItem * menuItem,
                                               SGIItemBase * item, const osg::StateAttribute::OverrideValue overridevalue)
    {
        StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(overridevalue);
        std::stringstream ss;
        ss << name << '=' << value;
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetDefineSet, ss.str(), item, currentMode, new ReferencedDataString(name));
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetDefineEdit, "Edit...", item, new ReferencedDataString(name));
        childMenu->addSimpleAction(MenuActionStateSetDefineRemove, "Remove", item, new ReferencedDataString(name));
        return childMenu;
    }


    static IContextMenuItem * createStateAttributeModeMenu(unsigned actionId, osg::StateAttribute::GLMode mode, IContextMenuItem * menuItem,
                                             const std::string & name, SGIItemBase * item, const StateAttributeModeValue value)
    {
        IContextMenuItem * childMenu = menuItem->addModeMenu(actionId, name, item, value, new ReferencedDataInt(mode));
        buildStateAttributeOverrideMenu(childMenu);
        return childMenu;
    }

    static IContextMenuItem * addStateSetModeMenu(IContextMenuItem * menuItem, osg::StateAttribute::GLMode mode, SGIItemBase * item)
    {
        std::string name = sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode);
        StateAttributeModeValue currentMode = getStateAttributeModeFromObject(static_cast<osg::Object*>(static_cast<SGIItemOsg*>(item)->object()), mode);
        IContextMenuItem * childMenu = menuItem->addModeMenu(MenuActionStateSetModeSet, name, item, currentMode, new ReferencedDataInt(mode));
        buildStateAttributeOverrideMenu(childMenu);
        childMenu->addSeparator();
        childMenu->addSimpleAction(MenuActionStateSetModeRemove, "Remove", item, new ReferencedDataInt(mode));
        return childMenu;
    }
}

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::Type & t);

bool contextMenuPopulateImpl<osg::Referenced>::populate(IContextMenuItem * menuItem)
{
    osg::Referenced * object = static_cast<osg::Referenced*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);
        ret = true;
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Object>::populate(IContextMenuItem * menuItem)
{
    osg::Object * object = static_cast<osg::Object*>(item<SGIItemOsg>()->object());

    bool ret;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionObjectSetName, "Set name...", _item);
            menuItem->addSimpleAction(MenuActionObjectSave, "Save as...", _item);
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                IContextMenuItem * dataVarianceMenu = manipulateMenu->addModeMenu(MenuActionObjectDataVariance, "Data variance", _item, (int)object->getDataVariance());
                if(dataVarianceMenu)
                {
                    dataVarianceMenu->addModeAction("Unspecified", osg::Object::UNSPECIFIED);
                    dataVarianceMenu->addModeAction("Static", osg::Object::STATIC);
                    dataVarianceMenu->addModeAction("Dynamic", osg::Object::DYNAMIC);
                }
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}


class HasDeprecatedDataVisitor : public osg::NodeVisitor
{
public:
    bool result;
    HasDeprecatedDataVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        , result(false)
    {
    }
    void apply(osg::Geometry& geometry) override
    {
        if (geometry.checkForDeprecatedData())
            result = true;
        else
            osg::NodeVisitor::apply(geometry);
    }
};

bool contextMenuPopulateImpl<osg::Node>::populate(IContextMenuItem * menuItem)
{
    osg::Node * object = static_cast<osg::Node*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                unsigned numUpdateTraversal = object->getNumChildrenRequiringUpdateTraversal();
                unsigned numEventTraversal = object->getNumChildrenRequiringEventTraversal();

                manipulateMenu->addSimpleAction(MenuActionNodeMask, helpers::str_plus_hex("Node mask", object->getNodeMask()), _item);
                manipulateMenu->addSimpleAction(MenuActionNodeMaskAndChilds, helpers::str_plus_hex("Node mask (+childs)", object->getNodeMask()), _item);

                manipulateMenu->addSimpleAction(MenuActionNodeNumUpdateTraversal, helpers::str_plus_count("Num update traversal", numUpdateTraversal), _item);
                manipulateMenu->addSimpleAction(MenuActionNodeNumEventTraversal, helpers::str_plus_count("Num event traversal", numEventTraversal), _item);

                manipulateMenu->addSimpleAction(MenuActionNodeRecomputeBound, "Recompute bound", _item);
                manipulateMenu->addBoolAction(MenuActionNodeCullingActive, "Culling Active", _item, object->getCullingActive());
                manipulateMenu->addSimpleAction(MenuActionNodeStripTextures, "Strip textures", _item);
                manipulateMenu->addSimpleAction(MenuActionNodeSmoothingVisitor, "Smoothing visitor", _item);

                manipulateMenu->addBoolAction(MenuActionNodeRenderInfo, "Render info", _item, RenderInfo::isPresent(object));
                HasDeprecatedDataVisitor hddv;
                object->accept(hddv);
                if(hddv.result)
                    manipulateMenu->addSimpleAction(MenuActionNodeFixDeprecatedData, "Fix deprecated data", _item);
                manipulateMenu->addBoolAction(MenuActionNodeInspectCulling, "Inspect culling", _item, CullingInfo::isPresent(object));

                manipulateMenu->addBoolAction(MenuActionNodeToggleCenterMarker, "Show center marker", _item, false);
            }

            menuItem->addMenu("Node visitor...", cloneItem<SGIItemOsg>(SGIItemTypeObjectLogger));

            IContextMenuItem * lookAtMenu = menuItem->getOrCreateMenu("Look at");
            if(lookAtMenu)
            {
                lookAtMenu->addSimpleAction(MenuActionNodeLookAt, "Direct", _item, new ReferencedDataInt(NodeLookAtDirect));
                lookAtMenu->addSimpleAction(MenuActionNodeLookAt, "From behind", _item, new ReferencedDataInt(NodeLookAtFromBehind));
                lookAtMenu->addSimpleAction(MenuActionNodeLookAt, "Top view", _item, new ReferencedDataInt(NodeLookAtTopView));
            }
			IContextMenuItem * optimizerMenu = manipulateMenu->addModeMenu(MenuActionNodeOptimizerRun, "Optimizer", _item, MenuActionOptimizerRunModeInvalid);
			if (optimizerMenu)
			{
				optimizerMenu->addModeAction("Check", MenuActionOptimizerRunModeCheck);
				optimizerMenu->addModeAction("Default", MenuActionOptimizerRunModeDefault);
				optimizerMenu->addModeAction("Fast geometry", MenuActionOptimizerRunModeFastGeometry);
				optimizerMenu->addModeAction("All", MenuActionOptimizerRunModeAll);
			}

            SGIHostItemOsg eventCallback(object->getEventCallback());
            if(eventCallback.hasObject())
                menuItem->addMenu("EventCallback", &eventCallback);

            SGIHostItemOsg udpateCallback(object->getUpdateCallback());
            if(udpateCallback.hasObject())
                menuItem->addMenu("UpdateCallback", &udpateCallback);

            SGIHostItemOsg cullCallback(object->getCullCallback());
            if(cullCallback.hasObject())
                menuItem->addMenu("CullCallback", &cullCallback);

            SGIHostItemOsg stateSet(object->getStateSet());
            if(stateSet.hasObject())
                menuItem->addMenu("StateSet", &stateSet);
            else
                manipulateMenu->addSimpleAction(MenuActionNodeCreateStateSet, "Create StateSet", _item);
        }
        break;
    case SGIItemTypeObjectLogger:
        {
            IObjectLoggerPtr logger;
            SGIItemBasePtr root = _item->rootBase();
            _hostInterface->getObjectLogger(logger, root);
            menuItem->addBoolAction(MenuActionObjectLoggerActive, "Active", _item, (logger.valid())?logger->isItemPresent(root):false);
            if(logger.valid())
            {
                IObjectLoggerDialogPtr dialog = logger->getDialog();
                menuItem->addBoolAction(MenuActionObjectLoggerVisible, "Visible", _item, (dialog.valid())?dialog->isVisible():false);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Group>::populate(IContextMenuItem * menuItem)
{
    osg::Group * object = static_cast<osg::Group*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            unsigned numChildren = object->getNumChildren();
            if(numChildren)
                menuItem->addMenu(helpers::str_plus_count("Childs", numChildren), cloneItem<SGIItemOsg>(SGIItemTypeChilds));

            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                IContextMenuItem * addChildMenu = manipulateMenu->addModeMenu(MenuActionGroupAddChild, "Add child", _item, MenuActionAddChildInvalid);
                if(addChildMenu)
                {
                    addChildMenu->addModeAction("Axis", MenuActionAddChildAxis);
                    addChildMenu->addModeAction("Billboard axis", MenuActionAddChildBillboardAxis);
                    addChildMenu->addModeAction("Bounding box", MenuActionAddChildBoundingBox);
                }
            }
        }
        break;
    case SGIItemTypeChilds:
        {
            unsigned numChildren = object->getNumChildren();
            for(unsigned i = 0; i < numChildren; i++)
            {
                osg::Node * child = object->getChild(i);
                SGIHostItemOsg childItem(child);
                IContextMenuItem * childMenu = menuItem->addMenu(std::string(), &childItem);
                childMenu->addSimpleAction(MenuActionGroupRemoveChild, "Remove", _item, child);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::StateSet>::populate(IContextMenuItem * menuItem)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                osg::StateSet::RenderingHint renderingHint = (osg::StateSet::RenderingHint)object->getRenderingHint();
                IContextMenuItem * renderingHintMenu = manipulateMenu->addModeMenu(MenuActionStateSetRenderHint, "RenderingHint", _item, renderingHint);
                renderingHintMenu->addModeAction("DEFAULT_BIN", osg::StateSet::DEFAULT_BIN);
                renderingHintMenu->addModeAction("OPAQUE_BIN", osg::StateSet::OPAQUE_BIN);
                renderingHintMenu->addModeAction("TRANSPARENT_BIN", osg::StateSet::TRANSPARENT_BIN);
            }

            manipulateMenu->addSimpleAction(MenuActionStateSetRenderBinNumber, helpers::str_plus_info("RenderBinNum", object->getBinNumber()), _item);
            manipulateMenu->addSimpleAction(MenuActionStateSetRenderBinName, helpers::str_plus_info("RenderBinName", object->getBinName()), _item);
            manipulateMenu->addSimpleAction(MenuActionStateSetClear, "Clear", _item);

            osg::StateSet::RenderBinMode renderBinMode = (osg::StateSet::RenderBinMode)object->getRenderBinMode();
            IContextMenuItem * renderBinModeMenu = menuItem->addModeMenu(MenuActionStateSetRenderBinMode, "RenderBinMode", _item, renderBinMode);
            if(renderBinModeMenu)
            {
                renderBinModeMenu->addModeAction("Inherit", osg::StateSet::INHERIT_RENDERBIN_DETAILS);
                renderBinModeMenu->addModeAction("Use", osg::StateSet::USE_RENDERBIN_DETAILS);
                renderBinModeMenu->addModeAction("Override", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
            }

            const osg::StateSet::ModeList & modelist = object->getModeList();
            menuItem->addMenu(helpers::str_plus_count("Mode list", modelist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetModeList, ~0u));

            const osg::StateSet::AttributeList & attributelist = object->getAttributeList();
            menuItem->addMenu(helpers::str_plus_count("Attribute list", attributelist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetAttributeList, ~0u));

            menuItem->addMenu(helpers::str_plus_count("Texture mode lists", object->getNumTextureModeLists()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetTextureModeList, ~0u));

            menuItem->addMenu(helpers::str_plus_count("Texture attribute lists", object->getNumTextureAttributeLists()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetTextureAttributeLists, ~0u));

            const osg::StateSet::UniformList & uniformlist = object->getUniformList();
            menuItem->addMenu(helpers::str_plus_count("Uniform list", uniformlist.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSetUniformList, ~0u));
        }
        break;
    case SGIItemTypeStateSetTextureAttributeLists:
        {
            unsigned itemNumber = _item->number();
            unsigned textureunit = 0;
            const osg::StateSet::TextureAttributeList & textureAttributes = object->getTextureAttributeList();
            for(osg::StateSet::TextureAttributeList::const_iterator it = textureAttributes.begin(); it != textureAttributes.end(); it++, textureunit++)
            {
                if(itemNumber == ~0u || itemNumber == textureunit)
                {
                    const osg::StateSet::AttributeList & attributes = *it;
                    unsigned attrcount = 0;
                    for(osg::StateSet::AttributeList::const_iterator it2 = attributes.begin(); it2 != attributes.end(); it2++, attrcount++)
                    {
                        const osg::StateAttribute::TypeMemberPair & typememberpair = it2->first;
                        const osg::StateAttribute::Type & type = typememberpair.first;
                        const unsigned int & member = typememberpair.second;
                        const osg::StateSet::RefAttributePair & attrpair = it2->second;
                        const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                        const osg::StateAttribute::OverrideValue & value = attrpair.second;

                        createTextureAttributeMenu(textureunit, member, type, menuItem, _item, attr, value);
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetTextureModeList:
        {
            unsigned itemNumber = _item->number();
            unsigned textureunit = 0;
            const osg::StateSet::TextureModeList & textureModeList = object->getTextureModeList();

            for (osg::StateSet::TextureModeList::const_iterator it = textureModeList.begin(); it != textureModeList.end(); it++, textureunit++)
            {
                if(itemNumber == ~0u || itemNumber == textureunit)
                {
                    const osg::StateSet::ModeList & modeList = *it;
                    for (osg::StateSet::ModeList::const_iterator it2 = modeList.begin(); it2 != modeList.end(); it2++)
                    {
                        const osg::StateAttribute::GLMode & mode = it2->first;
                        const osg::StateAttribute::GLModeValue & value = it2->second;

                        createTextureModeMenu(textureunit, mode, menuItem, _item, value);
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSetModeList:
        {
            addStateSetModeMenu(menuItem, GL_LIGHTING, _item);
            addStateSetModeMenu(menuItem, GL_BLEND, _item);
            addStateSetModeMenu(menuItem, GL_CULL_FACE, _item);
            addStateSetModeMenu(menuItem, GL_DEPTH_TEST, _item);
            addStateSetModeMenu(menuItem, GL_NORMALIZE, _item);
            addStateSetModeMenu(menuItem, GL_RESCALE_NORMAL, _item);
            addStateSetModeMenu(menuItem, GL_POINT_SMOOTH, _item);
            addStateSetModeMenu(menuItem, GL_LINE_SMOOTH, _item);
            addStateSetModeMenu(menuItem, GL_ALPHA_TEST, _item);
            addStateSetModeMenu(menuItem, GL_CLIP_PLANE0, _item);
            addStateSetModeMenu(menuItem, GL_CLIP_PLANE1, _item);
            addStateSetModeMenu(menuItem, GL_CLIP_PLANE2, _item);
            ret = true;
        }
        break;
    case SGIItemTypeStateSetAttributeList:
        {
            IContextMenuItem * addAttrMenu = menuItem->addModeMenu(MenuActionStateSetAttributeAdd, "Add", _item, (osg::StateAttribute::Type)-1);
            if(addAttrMenu)
            {
                addAttrMenu->addModeAction("Program", osg::StateAttribute::PROGRAM);
                addAttrMenu->addModeAction("View port", osg::StateAttribute::VIEWPORT);
                addAttrMenu->addModeAction("ClipPlane", osg::StateAttribute::CLIPPLANE);
                addAttrMenu->addModeAction("Depth", osg::StateAttribute::DEPTH);
                addAttrMenu->addModeAction("Material", osg::StateAttribute::MATERIAL);
                addAttrMenu->addModeAction("Polygon mode", osg::StateAttribute::POLYGONMODE);
                addAttrMenu->addModeAction("Alpha func", osg::StateAttribute::ALPHAFUNC);
                addAttrMenu->addModeAction("Blend func", osg::StateAttribute::BLENDFUNC);
                addAttrMenu->addModeAction("Blend color", osg::StateAttribute::BLENDCOLOR);
                addAttrMenu->addModeAction("Light model", osg::StateAttribute::LIGHTMODEL);
                addAttrMenu->addModeAction("Light", osg::StateAttribute::LIGHT);
                addAttrMenu->addModeAction("Line stipple", osg::StateAttribute::LINESTIPPLE);
                addAttrMenu->addModeAction("Line width", osg::StateAttribute::LINEWIDTH);
                addAttrMenu->addModeAction("Point", osg::StateAttribute::POINT);
                addAttrMenu->addModeAction("Polygon stipple", osg::StateAttribute::POLYGONSTIPPLE);
                addAttrMenu->addModeAction("TexEnv", osg::StateAttribute::TEXENV);
                addAttrMenu->addModeAction("TexEnvFilter", osg::StateAttribute::TEXENVFILTER);
                addAttrMenu->addModeAction("TexMat", osg::StateAttribute::TEXMAT);
                addAttrMenu->addModeAction("TexGen", osg::StateAttribute::TEXGEN);
            }
            unsigned childNo = 0;
            const osg::StateSet::AttributeList & list = object->getAttributeList();
            if(!list.empty())
                menuItem->addSeparator();

            for(osg::StateSet::AttributeList::const_iterator it = list.begin(); it != list.end(); it++, childNo++)
            {
                const osg::StateAttribute::TypeMemberPair & typepair = it->first;
                const osg::StateAttribute::Type & type = typepair.first;
                const unsigned & member = typepair.second;
                const osg::StateSet::RefAttributePair & attrpair = it->second;
                const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                const osg::StateAttribute::OverrideValue & value = attrpair.second;
                IContextMenuItem * attrmenu = createStateAttributeMenu(member, type, attr.get(), menuItem, _item, value);
            }

            ret = true;
        }
        break;
    case SGIItemTypeStateSetUniformList:
        {
            IContextMenuItem * addUniformMenu = menuItem->addModeMenu(MenuActionStateSetUniformAdd, "Add", _item, (osg::Uniform::Type)-1);
            if(addUniformMenu)
            {
                addUniformMenu->addModeAction("Bool", osg::Uniform::BOOL);
                addUniformMenu->addModeAction("Integer", osg::Uniform::INT);
                addUniformMenu->addModeAction("Unsigned", osg::Uniform::UNSIGNED_INT);
                addUniformMenu->addModeAction("Float", osg::Uniform::FLOAT);
                addUniformMenu->addModeAction("Double", osg::Uniform::DOUBLE);
                addUniformMenu->addModeAction("Vec2f", osg::Uniform::FLOAT_VEC2);
                addUniformMenu->addModeAction("Vec3f", osg::Uniform::FLOAT_VEC3);
                addUniformMenu->addModeAction("Vec4f", osg::Uniform::FLOAT_VEC4);
                addUniformMenu->addModeAction("Vec2d", osg::Uniform::DOUBLE_VEC2);
                addUniformMenu->addModeAction("Vec3d", osg::Uniform::DOUBLE_VEC3);
                addUniformMenu->addModeAction("Vec4d", osg::Uniform::DOUBLE_VEC4);
                addUniformMenu->addModeAction("Integer 64-bit", osg::Uniform::INT64);
                addUniformMenu->addModeAction("Unsigned 64-bit", osg::Uniform::UNSIGNED_INT64);
            }

            unsigned childNo = 0;
            const osg::StateSet::UniformList & list = object->getUniformList();
            if(!list.empty())
                menuItem->addSeparator();

            for(osg::StateSet::UniformList::const_iterator it = list.begin(); it != list.end(); it++, childNo++)
            {
                const std::string & name = it->first;
                const osg::StateSet::RefUniformPair & unipair = it->second;
                const osg::ref_ptr<osg::Uniform> & uniform = unipair.first;
                const osg::StateAttribute::OverrideValue & value = unipair.second;
                createUniformMenu(uniform.get(), menuItem, _item, value);
            }

            ret = true;
        }
        break;
    case SGIItemTypeStateSetDefineList:
        {
            menuItem->addSimpleAction(MenuActionStateSetDefineAdd, "Add", _item);
            unsigned childNo = 0;
            const osg::StateSet::DefineList & list = object->getDefineList();
            if(!list.empty())
                menuItem->addSeparator();

            for(osg::StateSet::DefineList::const_iterator it = list.begin(); it != list.end(); it++, childNo++)
            {
                const std::string & name = it->first;
                const osg::StateSet::DefinePair & defpair = it->second;
                const std::string & value = defpair.first;
                const osg::StateAttribute::OverrideValue & overridevalue = defpair.second;
                createDefineMenu(name, value, menuItem, _item, overridevalue);
            }

            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Program>::populate(IContextMenuItem * menuItem)
{
    osg::Program * object = getObject<osg::Program,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * addShaderMenu = menuItem->addModeMenu(MenuActionProgramAddShader, "Add shader", _item, osg::Shader::UNDEFINED);
            if(addShaderMenu)
            {
                addShaderMenu->addModeAction("Vertex", osg::Shader::VERTEX);
                addShaderMenu->addModeAction("Fragment", osg::Shader::FRAGMENT);
                addShaderMenu->addModeAction("Geometry", osg::Shader::GEOMETRY);
                addShaderMenu->addModeAction("Tesselation control", osg::Shader::TESSCONTROL);
                addShaderMenu->addModeAction("Tesselation evaluation", osg::Shader::TESSEVALUATION);
                addShaderMenu->addModeAction("Compute", osg::Shader::COMPUTE);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Depth>::populate(IContextMenuItem * menuItem)
{
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * functionMenu = menuItem->addModeMenu(MenuActionDepthFunction, "Function", _item, object->getFunction());
            if(functionMenu)
            {
                functionMenu->addModeAction("GL_NEVER", osg::Depth::NEVER);
                functionMenu->addModeAction("GL_LESS", osg::Depth::LESS);
                functionMenu->addModeAction("GL_EQUAL", osg::Depth::EQUAL);
                functionMenu->addModeAction("GL_LEQUAL", osg::Depth::LEQUAL);
                functionMenu->addModeAction("GL_GREATER", osg::Depth::GREATER);
                functionMenu->addModeAction("GL_NOTEQUAL", osg::Depth::NOTEQUAL);
                functionMenu->addModeAction("GL_GEQUAL", osg::Depth::GEQUAL);
                functionMenu->addModeAction("GL_ALWAYS", osg::Depth::ALWAYS);
            }
            menuItem->addBoolAction(MenuActionDepthWriteMask, "Write mask", _item, object->getWriteMask());
            menuItem->addSimpleAction(MenuActionDepthSetZNear, helpers::str_plus_info("ZNear", object->getZNear()), _item);
            menuItem->addSimpleAction(MenuActionDepthSetZFar, helpers::str_plus_info("ZFar", object->getZFar()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Uniform>::populate(IContextMenuItem * menuItem)
{
    osg::Uniform * object = getObject<osg::Uniform,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionUniformDirty, "Dirty", _item);
            }

            std::stringstream ss;
            ss << "Edit " << osg::Uniform::getTypename(object->getType());
            if(object->getType() == osg::Uniform::BOOL)
            {
                bool value;
                object->get(value);
                menuItem->addBoolAction(MenuActionUniformEdit, ss.str(), _item, value);
            }
            else
            {
                menuItem->addSimpleAction(MenuActionUniformEdit, ss.str(), _item);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::BufferObject>::populate(IContextMenuItem * menuItem)
{
    osg::BufferObject * object = getObject<osg::BufferObject, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionBufferObjectDirty, "Dirty", _item);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::BufferData>::populate(IContextMenuItem * menuItem)
{
    osg::BufferData * object = getObject<osg::BufferData,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionBufferDirty, "Dirty", _item);
                manipulateMenu->addSimpleAction(MenuActionBufferDataEdit, "Edit...", _item);
            }
        }
        break;
    case SGIItemTypeArrayData:
        menuItem->addSimpleAction(MenuActionBufferDataEdit, "Edit...", _item);
        ret = true;
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Array>::populate(IContextMenuItem * menuItem)
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionArrayDataEdit, "Modify data...", _item);
                manipulateMenu->addSimpleAction(MenuActionArrayTrim, "Trim", _item);
            }

            IContextMenuItem * bindingMenu = menuItem->addModeMenu(MenuActionArrayBinding, "Binding", _item, object->getBinding());
            if(bindingMenu)
            {
                bindingMenu->addModeAction("Undefined", osg::Array::BIND_UNDEFINED);
                bindingMenu->addModeAction("Off", osg::Array::BIND_OFF);
                bindingMenu->addModeAction("Overall", osg::Array::BIND_OVERALL);
                bindingMenu->addModeAction("Per primitive set", osg::Array::BIND_PER_PRIMITIVE_SET);
                bindingMenu->addModeAction("Per vertex", osg::Array::BIND_PER_VERTEX);
            }
            menuItem->addBoolAction(MenuActionArrayNormalize, "Normalize", _item, object->getNormalize());
            menuItem->addBoolAction(MenuActionArrayPreserveDataType, "Preserve data type", _item, object->getPreserveDataType());
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

#define contextMenuPopulateImpl_material_per_face_property(__id) \
    menuItem->addSimpleAction(MenuActionMaterial##__id, helpers::str_plus_info(#__id " Front", object->get##__id(osg::Material::FRONT)), _item, new ReferencedDataInt(osg::Material::FRONT)); \
    menuItem->addSimpleAction(MenuActionMaterial##__id, helpers::str_plus_info(#__id " Back", object->get##__id(osg::Material::BACK)), _item, new ReferencedDataInt(osg::Material::BACK)); \
    menuItem->addSimpleAction(MenuActionMaterial##__id, helpers::str_plus_info(#__id " Front+Back", object->get##__id(osg::Material::FRONT_AND_BACK)), _item, new ReferencedDataInt(osg::Material::FRONT_AND_BACK)); \
    (void(0))

bool contextMenuPopulateImpl<osg::Material>::populate(IContextMenuItem * menuItem)
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * colorModeMenu = menuItem->addModeMenu(MenuActionMaterialColorMode, "Color mode", _item, object->getColorMode());
            if(colorModeMenu)
            {
                colorModeMenu->addModeAction("AMBIENT", osg::Material::AMBIENT);
                colorModeMenu->addModeAction("DIFFUSE", osg::Material::DIFFUSE);
                colorModeMenu->addModeAction("SPECULAR", osg::Material::SPECULAR);
                colorModeMenu->addModeAction("EMISSION", osg::Material::EMISSION);
                colorModeMenu->addModeAction("AMBIENT_AND_DIFFUSE", osg::Material::AMBIENT_AND_DIFFUSE);
                colorModeMenu->addModeAction("OFF", osg::Material::OFF);
            }
            contextMenuPopulateImpl_material_per_face_property(Ambient);
            contextMenuPopulateImpl_material_per_face_property(Diffuse);
            contextMenuPopulateImpl_material_per_face_property(Specular);
            contextMenuPopulateImpl_material_per_face_property(Emission);
            contextMenuPopulateImpl_material_per_face_property(Shininess);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::LineWidth>::populate(IContextMenuItem * menuItem)
{
    osg::LineWidth * object = getObject<osg::LineWidth,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionLineWidthSet, helpers::str_plus_info("Width", object->getWidth()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::LineStipple>::populate(IContextMenuItem * menuItem)
{
    osg::LineStipple * object = getObject<osg::LineStipple,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionLineStipplePattern, helpers::str_plus_info("Pattern", object->getPattern()), _item);
            menuItem->addSimpleAction(MenuActionLineStippleFactor, helpers::str_plus_info("Factor", object->getFactor()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::LightModel>::populate(IContextMenuItem * menuItem)
{
    osg::LightModel * object = getObject<osg::LightModel, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * colorControlMenu = menuItem->addModeMenu(MenuActionLightModelColorControl, "Color control", _item, object->getColorControl());
            if (colorControlMenu)
            {
                colorControlMenu->addModeAction("Separate specular color", osg::LightModel::SEPARATE_SPECULAR_COLOR);
                colorControlMenu->addModeAction("Single color", osg::LightModel::SINGLE_COLOR);
            }

            menuItem->addBoolAction(MenuActionLightModelLocalViewer, "Local viewer", _item, object->getLocalViewer());
            menuItem->addBoolAction(MenuActionLightModelTwoSided, "Two-sided", _item, object->getTwoSided());
            menuItem->addSimpleAction(MenuActionLightModelAmbientIntensity, "Ambient intensity...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::BlendFunc>::populate(IContextMenuItem * menuItem)
{
    osg::BlendFunc * object = getObject<osg::BlendFunc, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::BlendColor>::populate(IContextMenuItem * menuItem)
{
    osg::BlendColor * object = getObject<osg::BlendColor, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addSimpleAction(MenuActionBlendColorConstantColor, "Constant color...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::PolygonMode>::populate(IContextMenuItem * menuItem)
{
    osg::PolygonMode * object = getObject<osg::PolygonMode, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * menuFront = menuItem->addModeMenu(MenuActionPolygonModeFront, "Front", _item, object->getMode(osg::PolygonMode::FRONT));
            if (menuFront)
            {
                menuFront->addModeAction("Point", osg::PolygonMode::POINT);
                menuFront->addModeAction("Wire frame", osg::PolygonMode::LINE);
                menuFront->addModeAction("Solid", osg::PolygonMode::FILL);
            }
            IContextMenuItem * menuBack = menuItem->addModeMenu(MenuActionPolygonModeBack, "Back", _item, object->getMode(osg::PolygonMode::BACK));
            if (menuBack)
            {
                menuBack->addModeAction("Point", osg::PolygonMode::POINT);
                menuBack->addModeAction("Wire frame", osg::PolygonMode::LINE);
                menuBack->addModeAction("Solid", osg::PolygonMode::FILL);
            }
            IContextMenuItem * menuFrontAndBack = menuItem->addModeMenu(MenuActionPolygonModeFrontAndBack, "Front+Back", _item, object->getMode(osg::PolygonMode::FRONT_AND_BACK));
            if (menuFrontAndBack)
            {
                menuFrontAndBack->addModeAction("Point", osg::PolygonMode::POINT);
                menuFrontAndBack->addModeAction("Wire frame", osg::PolygonMode::LINE);
                menuFrontAndBack->addModeAction("Solid", osg::PolygonMode::FILL);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::CullFace>::populate(IContextMenuItem * menuItem)
{
    osg::CullFace * object = getObject<osg::CullFace, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * menuMode = menuItem->addModeMenu(MenuActionCullFaceMode, "Mode", _item, object->getMode());
            if (menuMode)
            {
                menuMode->addModeAction("FRONT", osg::CullFace::FRONT);
                menuMode->addModeAction("BACK", osg::CullFace::BACK);
                menuMode->addModeAction("FRONT_AND_BACK", osg::CullFace::FRONT_AND_BACK);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Geode>::populate(IContextMenuItem * menuItem)
{
    osg::Geode * object = static_cast<osg::Geode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                IContextMenuItem * addShapeMenu = manipulateMenu->addModeMenu(MenuActionGeodeAddShapeDrawable, "Add shape", _item, MenuActionAddShapeInvalid);
                if(addShapeMenu)
                {
                    addShapeMenu->addModeAction("Sphere", MenuActionAddShapeSphere);
                    addShapeMenu->addModeAction("Box", MenuActionAddShapeBox);
                    addShapeMenu->addModeAction("Cone", MenuActionAddShapeCone);
                    addShapeMenu->addModeAction("Cylinder", MenuActionAddShapeCylinder);
                    addShapeMenu->addModeAction("Capsule", MenuActionAddShapeCapsule);
                }
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Transform>::populate(IContextMenuItem * menuItem)
{
    osg::Transform * object = static_cast<osg::Transform*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * refFrameMenu = menuItem->addModeMenu(MenuActionTransformReferenceFrame, "Reference Frame", _item, object->getReferenceFrame());
            if(refFrameMenu)
            {
                refFrameMenu->addModeAction("Relative", osg::Transform::RELATIVE_RF);
                refFrameMenu->addModeAction("Absolute inherit viewpoint", osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
                refFrameMenu->addModeAction("Absolute", osg::Transform::ABSOLUTE_RF);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::MatrixTransform>::populate(IContextMenuItem * menuItem)
{
    osg::MatrixTransform * object = static_cast<osg::MatrixTransform*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionMatrixTransformEdit, "Set matrix...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::PositionAttitudeTransform>::populate(IContextMenuItem * menuItem)
{
    osg::PositionAttitudeTransform * object = static_cast<osg::PositionAttitudeTransform*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionPositionAttitudeTransformEdit, "Edit PAT...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::AutoTransform>::populate(IContextMenuItem * menuItem)
{
    osg::AutoTransform * object = static_cast<osg::AutoTransform*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionAutoTransformSetAxis, helpers::str_plus_info("Axis", object->getAxis()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetNormal, helpers::str_plus_info("Normal", object->getNormal()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetScale, helpers::str_plus_info("Scale", object->getScale()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetPivotPoint, helpers::str_plus_info("Pivot point", object->getPivotPoint()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetMinimumScale, helpers::str_plus_info("Minimum scale", object->getMinimumScale()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetMaximumScale, helpers::str_plus_info("Maximum scale", object->getMaximumScale()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance, helpers::str_plus_info("Auto update eye mvmt tolerance", object->getAutoUpdateEyeMovementTolerance()), _item);
            menuItem->addSimpleAction(MenuActionAutoTransformSetAutoScaleTransitionWidthRatio, helpers::str_plus_info("Auto scal trans width ratio", object->getAutoScaleTransitionWidthRatio()), _item);

            IContextMenuItem * autoRotateModeMenu = menuItem->addModeMenu(MenuActionAutoTransformAutoRotateMode, "Auto rotate mode", _item, object->getAutoRotateMode());
            if(autoRotateModeMenu)
            {
                autoRotateModeMenu->addModeAction("No rotation", osg::AutoTransform::NO_ROTATION);
                autoRotateModeMenu->addModeAction("To screen", osg::AutoTransform::ROTATE_TO_SCREEN);
                autoRotateModeMenu->addModeAction("To camera", osg::AutoTransform::ROTATE_TO_CAMERA);
                autoRotateModeMenu->addModeAction("To axis", osg::AutoTransform::ROTATE_TO_AXIS);
            }
            menuItem->addBoolAction(MenuActionAutoTransformAutoScaleToScreen, "Auto scale to screen", _item, object->getAutoScaleToScreen());
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::CameraView>::populate(IContextMenuItem * menuItem)
{
    osg::CameraView * object = static_cast<osg::CameraView*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Billboard>::populate(IContextMenuItem * menuItem)
{
    osg::Billboard * object = static_cast<osg::Billboard*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            osg::Billboard::Mode currentMode = object->getMode();
            IContextMenuItem * modeMenu = menuItem->addModeMenu(MenuActionBillboardMode, "Mode", _item, currentMode);
            if(modeMenu)
            {
                modeMenu->addModeAction("Rotate to Eye", osg::Billboard::POINT_ROT_EYE);
                modeMenu->addModeAction("Rotate to World", osg::Billboard::POINT_ROT_WORLD);
                modeMenu->addModeAction("Axial rotation", osg::Billboard::AXIAL_ROT);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Viewport>::populate(IContextMenuItem * menuItem)
{
    osg::Viewport * object = static_cast<osg::Viewport*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
                manipulateMenu->addSimpleAction(MenuActionViewPortModify, "Modify...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Camera>::populate(IContextMenuItem * menuItem)
{
    osg::Camera * object = static_cast<osg::Camera*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
			menuItem->addSimpleAction(MenuActionViewCaptureScreenshot, "Capture screenshot", _item);
            menuItem->addSimpleAction(MenuActionCameraLiveView, "Live view", _item);

            SGIHostItemOsg graphicscontext(object->getGraphicsContext());
            if(graphicscontext.hasObject())
                menuItem->addMenu("Graphics context", &graphicscontext);

            IContextMenuItem * computeNearFarModeMenu = menuItem->addModeMenu(MenuActionCameraComputeNearFarMode, "Compute near/far mode", _item, object->getComputeNearFarMode());
            if(computeNearFarModeMenu)
            {
                computeNearFarModeMenu->addModeAction("Do not compute", osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
                computeNearFarModeMenu->addModeAction("Using bounding volumes", osg::Camera::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
                computeNearFarModeMenu->addModeAction("Near/far using primitives", osg::Camera::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
                computeNearFarModeMenu->addModeAction("Near using primitives", osg::Camera::COMPUTE_NEAR_USING_PRIMITIVES);
            }
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionCameraCullMask, helpers::str_plus_hex("Cull mask", object->getCullMask()), _item);
                manipulateMenu->addSimpleAction(MenuActionCameraViewMatrix, "View matrix", _item);
                manipulateMenu->addSimpleAction(MenuActionCameraProjectionMatrix, "Projection matrix", _item);

                IContextMenuItem * projectionResizePolicyModeMenu = menuItem->addModeMenu(MenuActionCameraProjectionResizePolicy, "Projection resize policy", _item, object->getProjectionResizePolicy());
                if (projectionResizePolicyModeMenu)
                {
                    projectionResizePolicyModeMenu->addModeAction("Fixed", osg::Camera::FIXED);
                    projectionResizePolicyModeMenu->addModeAction("Horizontal", osg::Camera::HORIZONTAL);
                    projectionResizePolicyModeMenu->addModeAction("Vertical", osg::Camera::VERTICAL);
                }
            }

            menuItem->addSimpleAction(MenuActionCameraClearColor, "Clear color...", _item);
            menuItem->addSimpleAction(MenuActionCameraCullSettings, "Cull settings...", _item);
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
                    ss << "Preview " << buffercomponent << "...";
                    menuItem->addSimpleAction(MenuActionImagePreview, ss.str(), _item);
                }
            }
            else
            {
                const osg::Camera::BufferComponent buffercomponent = (osg::Camera::BufferComponent)_item->number();
                auto it = bufferAttachmentMap.find(buffercomponent);
                if(it != bufferAttachmentMap.end())
                {
                    menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
                }
            }
            ret = true;
        }
        break;

    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::View>::populate(IContextMenuItem * menuItem)
{
    osg::View * object = getObject<osg::View, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            SGIHostItemOsg camera(object->getCamera());
            if (camera.hasObject())
                menuItem->addMenu("Camera", &camera);

            SGIHostItemOsg light(object->getLight());
            if (light.hasObject())
                menuItem->addMenu("Light", &light);

            IContextMenuItem * lightingModeMenu = menuItem->addModeMenu(MenuActionViewLightingMode, "Lighting mode", _item, object->getLightingMode());
            if (lightingModeMenu)
            {
                lightingModeMenu->addModeAction("No light", osg::View::NO_LIGHT);
                lightingModeMenu->addModeAction("Sky light", osg::View::SKY_LIGHT);
                lightingModeMenu->addModeAction("Headlight", osg::View::HEADLIGHT);
            }

            SGIHostItemOsg stats(object->getStats());
            if (stats.hasObject())
                menuItem->addMenu("Stats", &stats);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgViewer::View>::populate(IContextMenuItem * menuItem)
{
    osgViewer::View * object = getObject<osgViewer::View, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
			menuItem->addSimpleAction(MenuActionViewCaptureScreenshot, "Capture screen shot", _item);

            SGIHostItemOsg cameraManipulator(object->getCameraManipulator());
            if(cameraManipulator.hasObject())
                menuItem->addMenu("Camera manipulator", &cameraManipulator);

            SGIHostItemOsg viewerbase(object->getViewerBase());
            if(viewerbase.hasObject())
                menuItem->addMenu("ViewerBase", &viewerbase);

            SGIHostItemOsg scene(object->getScene());
            if(scene.hasObject())
                menuItem->addMenu("Scene", &scene);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgViewer::Scene>::populate(IContextMenuItem * menuItem)
{
    osgViewer::Scene * object = static_cast<osgViewer::Scene*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            SGIHostItemOsg sceneData(object->getSceneData());
            if(sceneData.hasObject())
                menuItem->addMenu("SceneData", &sceneData);

            SGIHostItemOsg imagepager(object->getImagePager());
            if(imagepager.hasObject())
                menuItem->addMenu("ImagePager", &imagepager);

            SGIHostItemOsg databasepager(object->getDatabasePager());
            if(databasepager.hasObject())
                menuItem->addMenu("DatabasePager", &databasepager);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::ProxyNode>::populate(IContextMenuItem * menuItem)
{
    osg::ProxyNode * object = static_cast<osg::ProxyNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionProxyNodeSetDatabasePath, helpers::str_plus_info("Database path", object->getDatabasePath()), _item);
            osg::ProxyNode::CenterMode centerMode = object->getCenterMode();
            IContextMenuItem * centerModeMenu = menuItem->addModeMenu(MenuActionProxyNodeSetCenterMode, "Center mode", _item, centerMode);
            if(centerModeMenu)
            {
                centerModeMenu->addModeAction("Bounding sphere", osg::ProxyNode::USE_BOUNDING_SPHERE_CENTER);
                centerModeMenu->addModeAction("User defined", osg::ProxyNode::USER_DEFINED_CENTER);
                centerModeMenu->addModeAction("Union bounding sphere and user defined", osg::ProxyNode::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED);
            }
            menuItem->addSimpleAction(MenuActionProxyNodeSetCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionProxyNodeSetRadius, helpers::str_plus_info("Radius", object->getRadius()), _item);

            osg::ProxyNode::LoadingExternalReferenceMode loadingMode = object->getLoadingExternalReferenceMode();
            IContextMenuItem * loadingModeMenu = menuItem->addModeMenu(MenuActionProxyNodeLoadingExternalReferenceMode, "Loading mode", _item, loadingMode);
            if(loadingModeMenu)
            {
                loadingModeMenu->addModeAction("Immediately", osg::ProxyNode::LOAD_IMMEDIATELY);
                loadingModeMenu->addModeAction("Database pager", osg::ProxyNode::DEFER_LOADING_TO_DATABASE_PAGER);
                loadingModeMenu->addModeAction("No loading", osg::ProxyNode::NO_AUTOMATIC_LOADING);
            }
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionProxyNodeForceLoad, "Force load", _item);
                manipulateMenu->addSimpleAction(MenuActionProxyNodeReload, "Reload", _item);
            }

        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::LOD>::populate(IContextMenuItem * menuItem)
{
    osg::LOD * object = getObject<osg::LOD,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            osg::LOD::CenterMode centerMode = object->getCenterMode();
            IContextMenuItem * centerModeMenu = menuItem->addModeMenu(MenuActionProxyNodeSetCenterMode, "Center mode", _item, centerMode);
            if(centerModeMenu)
            {
                centerModeMenu->addModeAction("Bounding sphere", osg::LOD::USE_BOUNDING_SPHERE_CENTER);
                centerModeMenu->addModeAction("User defined", osg::LOD::USER_DEFINED_CENTER);
                centerModeMenu->addModeAction("Union bounding sphere and user defined", osg::LOD::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED);
            }
            menuItem->addSimpleAction(MenuActionProxyNodeSetCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionProxyNodeSetRadius, helpers::str_plus_info("Radius", object->getRadius()), _item);

            IContextMenuItem * rangeModeMenu = menuItem->addModeMenu(MenuActionLODSetRangeMode, "Range mode", _item, centerMode);
            if(rangeModeMenu)
            {
                rangeModeMenu->addModeAction("Distance from eye point", osg::LOD::DISTANCE_FROM_EYE_POINT);
                rangeModeMenu->addModeAction("Pixel size on screen", osg::LOD::PIXEL_SIZE_ON_SCREEN);
            }

        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::PagedLOD>::populate(IContextMenuItem * menuItem)
{
    osg::PagedLOD * object = getObject<osg::PagedLOD,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionPagedLODDisableExternalChildrenPaging, "Disable external children paging", _item, object->getDisableExternalChildrenPaging());
            menuItem->addSimpleAction(MenuActionPagedLODNumChildrenThatCannotBeExpired, helpers::str_plus_info("Num children that cannot be expired", object->getNumChildrenThatCannotBeExpired()), _item);
            menuItem->addSimpleAction(MenuActionPagedLODFrameNumberOfLastTraversal, helpers::str_plus_info("Frame number of last traversal", object->getFrameNumberOfLastTraversal()), _item);
            menuItem->addSimpleAction(MenuActionProxyNodeSetDatabasePath, helpers::str_plus_info("Database path", object->getDatabasePath()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Drawable>::populate(IContextMenuItem * menuItem)
{
    osg::Drawable * object = getObject<osg::Drawable,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionDrawableToggleDisabled, "Disabled", _item, DisabledDrawCallback::isDisabled(object));
#if OSG_VERSION_LESS_THAN(3,4,0)
            SGIHostItemOsg stateSet(object->getStateSet());
            if(stateSet.hasObject())
                menuItem->addMenu("StateSet", &stateSet);
#endif // OSG_VERSION_LESS_THAN(3,4,0)

			IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
			if (manipulateMenu)
			{
                manipulateMenu->addSimpleAction(MenuActionDrawableDirtyGLObjects, "Dirty GL objects", _item);
                manipulateMenu->addBoolAction(MenuActionDrawableUseDisplayList, "Use display list", _item, object->getUseDisplayList());
				manipulateMenu->addBoolAction(MenuActionDrawableSupportsDisplayList, "Supports display list", _item, object->getSupportsDisplayList());
                manipulateMenu->addBoolAction(MenuActionDrawableUseVAO, "Use VAO", _item, object->getUseVertexArrayObject());
                manipulateMenu->addBoolAction(MenuActionDrawableUseVBO, "Use VBO", _item, object->getUseVertexBufferObjects());
                manipulateMenu->addBoolAction(MenuActionDrawableRenderInfoDrawCallback, "Render info draw callback", _item, RenderInfo::hasDrawCallback(object));
			}

            SGIHostItemOsg shape(object->getShape());
            if(shape.hasObject())
                menuItem->addMenu("Shape", &shape);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::ShapeDrawable>::populate(IContextMenuItem * menuItem)
{
    osg::ShapeDrawable * object = getObject<osg::ShapeDrawable,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionShapeDrawableColor, "Color...", _item);
                manipulateMenu->addSimpleAction(MenuActionShapeDrawableBuild, "Build", _item);
            }
            SGIHostItemOsg tessellationHints(object->getTessellationHints());
            if (tessellationHints.hasObject())
                menuItem->addMenu("TessellationHints", &tessellationHints);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Geometry>::populate(IContextMenuItem * menuItem)
{
    osg::Geometry * object = getObject<osg::Geometry,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionGeometryColor, "Color...", _item);
                if(object->checkForDeprecatedData())
                    manipulateMenu->addSimpleAction(MenuActionGeometryFixDeprecatedData, "Fix deprecated data", _item);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Image>::populate(IContextMenuItem * menuItem)
{
    osg::Image * object = static_cast<osg::Image*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}



bool contextMenuPopulateImpl<osg::Sphere>::populate(IContextMenuItem * menuItem)
{
    osg::Sphere * object = getObject<osg::Sphere,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionShapeCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Box>::populate(IContextMenuItem * menuItem)
{
    osg::Box * object = getObject<osg::Box,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionShapeCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionShapeRotation, helpers::str_plus_info("Rotation", object->getRotation()), _item);
            menuItem->addSimpleAction(MenuActionShapeBoxHalfLength, helpers::str_plus_info("Half length", object->getHalfLengths()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Cone>::populate(IContextMenuItem * menuItem)
{
    osg::Cone * object = getObject<osg::Cone,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionShapeCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionShapeRotation, helpers::str_plus_info("Rotation", object->getRotation()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Cylinder>::populate(IContextMenuItem * menuItem)
{
    osg::Cylinder * object = getObject<osg::Cylinder,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionShapeCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionShapeRotation, helpers::str_plus_info("Rotation", object->getRotation()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Capsule>::populate(IContextMenuItem * menuItem)
{
    osg::Capsule * object = getObject<osg::Capsule,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionShapeCenter, helpers::str_plus_info("Center", object->getCenter()), _item);
            menuItem->addSimpleAction(MenuActionShapeRotation, helpers::str_plus_info("Rotation", object->getRotation()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::InfinitePlane>::populate(IContextMenuItem * menuItem)
{
    osg::InfinitePlane * object = getObject<osg::InfinitePlane,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::TriangleMesh>::populate(IContextMenuItem * menuItem)
{
    osg::TriangleMesh * object = getObject<osg::TriangleMesh,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::HeightField>::populate(IContextMenuItem * menuItem)
{
    osg::HeightField * object = getObject<osg::HeightField,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::CompositeShape>::populate(IContextMenuItem * menuItem)
{
    osg::CompositeShape * object = getObject<osg::CompositeShape,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::StateAttribute>::populate(IContextMenuItem * menuItem)
{
    osg::StateAttribute * object = getObject<osg::StateAttribute,SGIItemOsg>();

    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Texture>::populate(IContextMenuItem * menuItem)
{
    osg::Texture * object = static_cast<osg::Texture*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionImagePreview, "Preview...", _item);
            menuItem->addSimpleAction(MenuActionTextureSetImage, "Set image...", _item);
            menuItem->addSimpleAction(MenuActionTextureBorderWidth, "Border width...", _item);
            menuItem->addSimpleAction(MenuActionTextureBorderColor, "Border color...", _item);

			IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
			if (manipulateMenu)
			{
				manipulateMenu->addSimpleAction(MenuActionTextureDirtyTextureObject, "Dirty texture object", _item);
				manipulateMenu->addSimpleAction(MenuActionTextureDirtyTextureParameters, "Dirty texture parameters", _item);
				manipulateMenu->addSimpleAction(MenuActionTextureAllocateMipmapLevels, "Allocate mipmap levels", _item);
			}

            IContextMenuItem * minFilterMenu = menuItem->addModeMenu(MenuActionTextureSetMinFilter, "Min filter", _item, object->getFilter(osg::Texture::MIN_FILTER));
            if(minFilterMenu)
            { 
                minFilterMenu->addModeAction("Linear", osg::Texture::LINEAR);
                minFilterMenu->addModeAction("Linear, MipMap linear", osg::Texture::LINEAR_MIPMAP_LINEAR);
                minFilterMenu->addModeAction("Linear, MipMap nearest", osg::Texture::LINEAR_MIPMAP_NEAREST);
                minFilterMenu->addModeAction("Nearest", osg::Texture::NEAREST);
                minFilterMenu->addModeAction("Nearest, MipMap linear", osg::Texture::NEAREST_MIPMAP_LINEAR);
                minFilterMenu->addModeAction("Nearest, MipMap nearest", osg::Texture::NEAREST_MIPMAP_NEAREST);
            }
            IContextMenuItem * magFilterMenu = menuItem->addModeMenu(MenuActionTextureSetMagFilter, "Mag filter", _item, object->getFilter(osg::Texture::MAG_FILTER));
            if(magFilterMenu)
            {
                magFilterMenu->addModeAction("Linear", osg::Texture::LINEAR);
                magFilterMenu->addModeAction("Linear, MipMap linear", osg::Texture::LINEAR_MIPMAP_LINEAR);
                magFilterMenu->addModeAction("Linear, MipMap nearest", osg::Texture::LINEAR_MIPMAP_NEAREST);
                magFilterMenu->addModeAction("Nearest", osg::Texture::NEAREST);
                magFilterMenu->addModeAction("Nearest, MipMap linear", osg::Texture::NEAREST_MIPMAP_LINEAR);
                magFilterMenu->addModeAction("Nearest, MipMap nearest", osg::Texture::NEAREST_MIPMAP_NEAREST);
            }

            menuItem->addBoolAction(MenuActionTextureSetResizeNonPowerOfTwoHint, "Resize NPOT", _item, object->getResizeNonPowerOfTwoHint());
            menuItem->addBoolAction(MenuActionTextureUseHardwareMipMapGeneration, "Use hardware MipMap generation", _item, object->getUseHardwareMipMapGeneration());
            menuItem->addBoolAction(MenuActionTextureUnRefImageDataAfterApply, "Unref image data after apply", _item, object->getUnRefImageDataAfterApply());

        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Texture2D>::populate(IContextMenuItem * menuItem)
{
    osg::Texture2D * object = static_cast<osg::Texture2D*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Texture3D>::populate(IContextMenuItem * menuItem)
{
    osg::Texture3D * object = static_cast<osg::Texture3D*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::TexEnv>::populate(IContextMenuItem * menuItem)
{
    osg::TexEnv * object = static_cast<osg::TexEnv*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            IContextMenuItem * modeMenu = menuItem->addModeMenu(MenuActionTexEnvMode, "Mode", _item, object->getMode());
            if (modeMenu)
            {
                modeMenu->addModeAction("DECAL", osg::TexEnv::DECAL);
                modeMenu->addModeAction("MODULATE", osg::TexEnv::MODULATE);
                modeMenu->addModeAction("BLEND", osg::TexEnv::BLEND);
                modeMenu->addModeAction("REPLACE", osg::TexEnv::REPLACE);
                modeMenu->addModeAction("ADD", osg::TexEnv::ADD);
            }
            menuItem->addSimpleAction(MenuActionTexEnvColor, "Color...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::TexEnvFilter>::populate(IContextMenuItem * menuItem)
{
    osg::TexEnvFilter * object = static_cast<osg::TexEnvFilter*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addSimpleAction(MenuActionTexEnvFilterLodBias, helpers::str_plus_number("LOD Bias", object->getLodBias()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::TexMat>::populate(IContextMenuItem * menuItem)
{
    osg::TexMat * object = static_cast<osg::TexMat*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::ClipNode>::populate(IContextMenuItem * menuItem)
{
    osg::ClipNode * object = static_cast<osg::ClipNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionClipNodeReset, "Reset", _item);

            StateAttributeModeValue currentMode = getStateAttributeModeFromOverrideValue(((const ClipNodeAccess*)object)->value());
            createStateAttributeModeMenu(MenuActionClipNodeSetState, GL_CLIP_PLANE0, menuItem, "State", _item, currentMode);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::GraphicsContext>::populate(IContextMenuItem * menuItem)
{
    osg::GraphicsContext * object = static_cast<osg::GraphicsContext*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osg::Shader>::populate(IContextMenuItem * menuItem)
{
    osg::Shader * object = static_cast<osg::Shader*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionShaderDirty, "Dirty", _item);
            }
            menuItem->addSimpleAction(MenuActionShaderEditSource, "Edit...", _item);
        }
        break;
    case SGIItemTypeShaderSource:
        menuItem->addSimpleAction(MenuActionShaderEditSource, "Edit...", _item);
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgViewer::ViewerBase>::populate(IContextMenuItem * menuItem)
{
	osgViewer::ViewerBase * object = dynamic_cast<osgViewer::ViewerBase*>(item<SGIItemOsg>()->object());
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(menuItem);
		if (ret)
		{
			menuItem->addSimpleAction(MenuActionViewCaptureScreenshot, "Capture screenshot", _item);

			menuItem->addSimpleAction(MenuActionViewerBaseMaxFrameRate, helpers::str_plus_count("Max frame rate", object->getRunMaxFrameRate()), _item);

			IContextMenuItem * runFrameSchemeMenu = menuItem->addModeMenu(MenuActionViewerBaseRunFrameScheme, "Run frame scheme", _item, object->getRunFrameScheme());
			if (runFrameSchemeMenu)
			{
				runFrameSchemeMenu->addModeAction("On demand", osgViewer::ViewerBase::ON_DEMAND);
				runFrameSchemeMenu->addModeAction("Continuous", osgViewer::ViewerBase::CONTINUOUS);
			}

            IContextMenuItem * threadingModelMenu = menuItem->addModeMenu(MenuActionViewerBaseThreadingModel, "Threading model", _item, object->getThreadingModel());
            if (threadingModelMenu)
            {
                threadingModelMenu->addModeAction("SingleThreaded", osgViewer::ViewerBase::SingleThreaded);
                threadingModelMenu->addModeAction("CullDrawThreadPerContext", osgViewer::ViewerBase::CullDrawThreadPerContext);
                threadingModelMenu->addModeAction("DrawThreadPerContext", osgViewer::ViewerBase::DrawThreadPerContext);
                threadingModelMenu->addModeAction("CullThreadPerCameraDrawThreadPerContext", osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext);
                threadingModelMenu->addModeAction("AutomaticSelection", osgViewer::ViewerBase::AutomaticSelection);
            }
			
			menuItem->addSimpleAction(MenuActionViewerBaseMaxFrameRate, helpers::str_plus_count("Max frame rate", object->getRunMaxFrameRate()), _item);

			SGIHostItemOsg ico(object->getIncrementalCompileOperation());
			if (ico.hasObject())
				menuItem->addMenu("Incremental compile operation", &ico);
			else
			{
				IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
				if (manipulateMenu)
				{
					manipulateMenu->addSimpleAction(MenuActionViewerBaseIncrementalCompileOperation, "Create Incremental compile operation", _item);
				}
			}

		}
		break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgViewer::GraphicsWindow>::populate(IContextMenuItem * menuItem)
{
    osgViewer::GraphicsWindow * object = static_cast<osgViewer::GraphicsWindow*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionGraphicsWindowHitTest, "Hit test", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

#ifdef SGI_USE_OSGQT
bool contextMenuPopulateImpl<osgQt::GraphicsWindowQt>::populate(IContextMenuItem * menuItem)
{
    osgQt::GraphicsWindowQt * object = static_cast<osgQt::GraphicsWindowQt*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            SGIHostItemQt widget(object->getGLWidget());
            if(widget.hasObject())
                menuItem->addMenu("GLWidget", &widget);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}
#endif // SGI_USE_OSGQT

bool contextMenuPopulateImpl<osgDB::Registry>::populate(IContextMenuItem * menuItem)
{
    osgDB::Registry * object = getObject<osgDB::Registry,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            osg::NotifySeverity notifyLevel = osg::getNotifyLevel();
            IContextMenuItem * notifyLevelMenu = menuItem->addModeMenu(MenuActionNotifyLevel, "Notify level", _item, notifyLevel);
            if(notifyLevelMenu)
            {
                notifyLevelMenu->addModeAction("Always", osg::ALWAYS);
                notifyLevelMenu->addModeAction("Fatal", osg::FATAL);
                notifyLevelMenu->addModeAction("Warn", osg::WARN);
                notifyLevelMenu->addModeAction("Notice", osg::NOTICE);
                notifyLevelMenu->addModeAction("Info", osg::INFO);
                notifyLevelMenu->addModeAction("Debug info", osg::DEBUG_INFO);
                notifyLevelMenu->addModeAction("Debug FP", osg::DEBUG_FP);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgDB::ImagePager>::populate(IContextMenuItem * menuItem)
{
    osgDB::ImagePager * object = static_cast<osgDB::ImagePager*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgDB::DatabasePager>::populate(IContextMenuItem * menuItem)
{
    osgDB::DatabasePager * object = static_cast<osgDB::DatabasePager*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionDatabasePagerPause, "Pause", _item, object->getDatabasePagerThreadPause());
			menuItem->addSimpleAction(MenuActionDatabasePagerDatabaseThreads, helpers::str_plus_count("Database threads", object->getNumDatabaseThreads()), _item);

			
            menuItem->addBoolAction(MenuActionDatabasePagerAcceptNewRequests, "Accept new requests", _item, object->getAcceptNewDatabaseRequests());
            menuItem->addBoolAction(MenuActionDatabasePagerDoPreCompile, "Do pre-compile", _item, object->getDoPreCompile());
            menuItem->addBoolAction(MenuActionDatabasePagerDeleteSubgraphsInDBThread, "Delete Subgraphs in DB thread", _item, object->getDeleteRemovedSubgraphsInDatabaseThread());
            menuItem->addSimpleAction(MenuActionDatabasePagerTargetPageLODNumber, helpers::str_plus_count("Target PagedLOD number",object->getTargetMaximumNumberOfPageLOD()), _item);

			SGIHostItemOsg ico(object->getIncrementalCompileOperation());
			if (ico.hasObject())
				menuItem->addMenu("Incremental compile operation", &ico);
			else
			{
				IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
				if (manipulateMenu)
					manipulateMenu->addSimpleAction(MenuActionDatabasePagerIncrementalCompileOperation, "Create Incremental compile operation", _item);
			}

        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgGA::GUIEventHandler>::populate(IContextMenuItem * menuItem)
{
    osgGA::GUIEventHandler * object = dynamic_cast<osgGA::GUIEventHandler*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
#if OSG_VERSION_LESS_THAN(3,3,1)
            menuItem->addSimpleAction(MenuActionEventHandlerIgnoreHandledEventsMask, helpers::str_plus_hex("Ignore handled events mask",object->getIgnoreHandledEventsMask()), _item);
#endif
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgGA::GUIEventAdapter>::populate(IContextMenuItem * menuItem)
{
    osgGA::GUIEventAdapter * object = static_cast<osgGA::GUIEventAdapter*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            SGIHostItemOsg context(object->getGraphicsContext());
            if (context.hasObject())
                menuItem->addMenu("GraphicsContext", &context);

            menuItem->addSimpleAction(MenuActionEventAdapterHitTest, "Hit test", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgGA::CameraManipulator>::populate(IContextMenuItem * menuItem)
{
    osgGA::CameraManipulator * object = dynamic_cast<osgGA::CameraManipulator*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionCameraManipulatorAutoComputeHome, "Auto compute home", _item, object->getAutoComputeHomePosition());

            SGIHostItemOsg node(object->getNode());
            if(node.hasObject())
                menuItem->addMenu("Node", &node);

        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgText::TextBase>::populate(IContextMenuItem * menuItem)
{
    osgTextBaseAccess * object = static_cast<osgTextBaseAccess*>(getObject<osgText::TextBase, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
			SGIHostItemOsg font(object->getFont());
			if (font.hasObject())
				menuItem->addMenu("Font", &font);
			SGIHostItemOsg style(object->getStyle());
			if (style.hasObject())
				menuItem->addMenu("Style", &style);

            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                menuItem->addSimpleAction(MenuActionTextBaseComputeGlyphRepresentation, "ComputeGlyphRepresentation", _item);
            }

            menuItem->addBoolAction(MenuActionTextBaseAutoRotateToScreen, "Auto rotate to screen", _item, object->getAutoRotateToScreen());
            menuItem->addSimpleAction(MenuActionTextBaseCharacterHeight, helpers::str_plus_info("Character height", object->getCharacterHeight()), _item);
            menuItem->addSimpleAction(MenuActionTextBaseCharacterAspectRatio, helpers::str_plus_info("Character aspect ratio", object->getCharacterAspectRatio()), _item);
			menuItem->addSimpleAction(MenuActionTextBaseSetFontWidth, helpers::str_plus_info("Set font width", object->getFontWidth()), _item);
			menuItem->addSimpleAction(MenuActionTextBaseSetFontHeight, helpers::str_plus_info("Set font height", object->getFontHeight()), _item);

            IContextMenuItem * characterSizeMenu = menuItem->addModeMenu(MenuActionTextBaseCharacterSizeMode, "Character size mode", _item, object->getCharacterSizeMode());
            if(characterSizeMenu)
            {
                characterSizeMenu->addModeAction("Object", osgText::TextBase::OBJECT_COORDS);
                characterSizeMenu->addModeAction("Screen", osgText::TextBase::SCREEN_COORDS);
                characterSizeMenu->addModeAction("Screen+Max font height", osgText::TextBase::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
            }
            IContextMenuItem * layoutMenu = menuItem->addModeMenu(MenuActionTextBaseLayout, "Layout", _item, object->getLayout());
            if (layoutMenu)
            {
                layoutMenu->addModeAction("Left to right", osgText::TextBase::LEFT_TO_RIGHT);
                layoutMenu->addModeAction("Right to left", osgText::TextBase::RIGHT_TO_LEFT);
                layoutMenu->addModeAction("Vertical", osgText::TextBase::VERTICAL);
            }
            menuItem->addSimpleAction(MenuActionTextBaseSetMaximumWidth, helpers::str_plus_info("Set maximum width", object->getMaximumWidth()), _item);
            menuItem->addSimpleAction(MenuActionTextBaseSetMaximumHeight, helpers::str_plus_info("Set maximum height", object->getMaximumHeight()), _item);

            IContextMenuItem * drawModeMenu = menuItem->addModeMenu(MenuActionTextBaseDrawMode, "Draw mode", _item, object->getDrawMode());
            if(drawModeMenu)
            {
				drawModeMenu->addModeAction("Text", osgText::Text::TEXT);
				drawModeMenu->addModeAction("BoundingBox", osgText::Text::BOUNDINGBOX);
				drawModeMenu->addModeAction("FilledBoundingBox", osgText::Text::FILLEDBOUNDINGBOX);
				drawModeMenu->addModeAction("Alignment", osgText::Text::ALIGNMENT);
				drawModeMenu->addModeAction("Text+BBox", osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
				drawModeMenu->addModeAction("Text+BBox+Align", osgText::Text::TEXT | osgText::Text::BOUNDINGBOX | osgText::Text::ALIGNMENT);
				drawModeMenu->addModeAction("Text+FilledBBox", osgText::Text::TEXT | osgText::Text::FILLEDBOUNDINGBOX);
				drawModeMenu->addModeAction("Text+FilledBBox+Align", osgText::Text::TEXT | osgText::Text::FILLEDBOUNDINGBOX | osgText::Text::ALIGNMENT);
			}
            IContextMenuItem * axisAlignmentMenu = menuItem->addModeMenu(MenuActionTextBaseAxisAlignment, "Axis alignment", _item, object->getAxisAlignment());
            if(axisAlignmentMenu)
            {
                axisAlignmentMenu->addModeAction("XY plane", osgText::Text::XY_PLANE);
                axisAlignmentMenu->addModeAction("Rev-XY plane", osgText::Text::REVERSED_XY_PLANE);
                axisAlignmentMenu->addModeAction("XZ plane", osgText::Text::XZ_PLANE);
                axisAlignmentMenu->addModeAction("Rev-XZ plane", osgText::Text::REVERSED_XZ_PLANE);
                axisAlignmentMenu->addModeAction("YZ plane", osgText::Text::YZ_PLANE);
                axisAlignmentMenu->addModeAction("Rev-YZ plane", osgText::Text::REVERSED_YZ_PLANE);
                axisAlignmentMenu->addModeAction("Screen", osgText::Text::SCREEN);
                axisAlignmentMenu->addModeAction("User defined", osgText::Text::USER_DEFINED_ROTATION);
            }

            menuItem->addSimpleAction(MenuActionTextBaseModifyText, "Modify text...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgText::Text>::populate(IContextMenuItem * menuItem)
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionTextBackdropEnableDepthWrites, "Depth Writes Enabled", _item, object->getEnableDepthWrites());
            IContextMenuItem * backdropTypeMenu = menuItem->addModeMenu(MenuActionTextBackdropType, "Backdrop type", _item, object->getBackdropType());
            if(backdropTypeMenu)
            {
                backdropTypeMenu->addModeAction("BOTTOM_RIGHT", osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
                backdropTypeMenu->addModeAction("CENTER_RIGHT", osgText::Text::DROP_SHADOW_CENTER_RIGHT);
                backdropTypeMenu->addModeAction("TOP_RIGHT", osgText::Text::DROP_SHADOW_TOP_RIGHT);
                backdropTypeMenu->addModeAction("BOTTOM_CENTER", osgText::Text::DROP_SHADOW_BOTTOM_CENTER);
                backdropTypeMenu->addModeAction("TOP_CENTER", osgText::Text::DROP_SHADOW_TOP_CENTER);
                backdropTypeMenu->addModeAction("BOTTOM_LEFT", osgText::Text::DROP_SHADOW_BOTTOM_LEFT);
                backdropTypeMenu->addModeAction("CENTER_LEFT", osgText::Text::DROP_SHADOW_CENTER_LEFT);
                backdropTypeMenu->addModeAction("TOP_LEFT", osgText::Text::DROP_SHADOW_TOP_LEFT);
                backdropTypeMenu->addModeAction("Outline", osgText::Text::OUTLINE);
                backdropTypeMenu->addModeAction("None", osgText::Text::NONE);
            }
#if OSG_VERSION_LESS_THAN(3,5,8)
			IContextMenuItem * backdropImplMenu = menuItem->addModeMenu(MenuActionTextBackdropImplementation, "Backdrop Implementation", _item, object->getBackdropImplementation());
			if (backdropImplMenu)
			{
				backdropImplMenu->addModeAction("POLYGON_OFFSET", osgText::Text::POLYGON_OFFSET);
				backdropImplMenu->addModeAction("NO_DEPTH_BUFFER", osgText::Text::NO_DEPTH_BUFFER);
				backdropImplMenu->addModeAction("DEPTH_RANGE", osgText::Text::DEPTH_RANGE);
				backdropImplMenu->addModeAction("STENCIL_BUFFER", osgText::Text::STENCIL_BUFFER);
				backdropImplMenu->addModeAction("DELAYED_DEPTH_WRITES", osgText::Text::DELAYED_DEPTH_WRITES);
			}
#endif
            menuItem->addSimpleAction(MenuActionTextBackdropHorizontalOffset, helpers::str_plus_info("Backdrop Horz. Offset", object->getBackdropHorizontalOffset()), _item);
            menuItem->addSimpleAction(MenuActionTextBackdropVerticalOffset, helpers::str_plus_info("Backdrop Vert. Offset", object->getBackdropVerticalOffset()), _item);
            menuItem->addSimpleAction(MenuActionTextBackdropColor, "Backdrop color...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgUtil::IncrementalCompileOperation>::populate(IContextMenuItem * menuItem)
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(menuItem);
		if (ret)
		{
			menuItem->addSimpleAction(MenuActionIncrementalCompileOperationTargetFrameRate, helpers::str_plus_info("Target frame rate", object->getTargetFrameRate()), _item);
			menuItem->addSimpleAction(MenuActionIncrementalCompileOperationMinimumTimeAvailableForGLCompileAndDeletePerFrame, helpers::str_plus_info("Min time for GL compile and delete", object->getMinimumTimeAvailableForGLCompileAndDeletePerFrame()), _item);
			menuItem->addSimpleAction(MenuActionIncrementalCompileOperationMaximumNumOfObjectsToCompilePerFrame, helpers::str_plus_info("Max # of objects to compile", object->getMaximumNumOfObjectsToCompilePerFrame()), _item);
			menuItem->addSimpleAction(MenuActionIncrementalCompileOperationFlushTimeRatio, helpers::str_plus_info("Flush time ratio", object->getFlushTimeRatio()), _item);
			menuItem->addSimpleAction(MenuActionIncrementalCompileOperationConservativeTimeRatio, helpers::str_plus_info("Conservative time ratio", object->getConservativeTimeRatio()), _item);
		}
		break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgAnimation::AnimationManagerBase>::populate(IContextMenuItem * menuItem)
{
    osgAnimation::AnimationManagerBase * object = getObject<osgAnimation::AnimationManagerBase, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionAnimationManagerBaseAutomaticLink, "Automatic link", _item, object->getAutomaticLink());
            menuItem->addBoolAction(MenuActionAnimationManagerBaseDirty, "Dirty", _item, object->needToLink());

            AnimationManagerBaseAccess * access = (AnimationManagerBaseAccess*)object;
            SGIHostItemOsg linkVisitor(access->getLinkVisitor());
            if(linkVisitor.hasObject())
                menuItem->addMenu("LinkVisitor", &linkVisitor);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<sgi::ISceneGraphDialogToolsMenu>::populate(IContextMenuItem * menuItem)
{
    ISceneGraphDialogToolsMenu * object = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        menuItem->addSimpleAction(MenuActionToolFindUpdateNodes, "Find update nodes", _item);
        menuItem->addSimpleAction(MenuActionToolFindEventNodes, "Find event nodes", _item);
        menuItem->addSimpleAction(MenuActionToolFindAllStateSets, "Find all state sets", _item);
        menuItem->addSimpleAction(MenuActionToolFindNaNNodes, "Find NaN nodes", _item);
        menuItem->addSimpleAction(MenuActionToolFindVisibleNodes, "Find visible", _item);
        menuItem->addSimpleAction(MenuActionToolListNodeMasks, "List node masks", _item);
        menuItem->addSimpleAction(MenuActionToolFindPagedLODChilds, "Find PagedLOD childs", _item);
        menuItem->addSimpleAction(MenuActionToolFindClosestNodeToCamera, "Find closest node", _item);
        menuItem->addSimpleAction(MenuActionToolEffectiveStateSet, "Effective StateSet", _item);
        menuItem->addSimpleAction(MenuActionToolFindCamera, "Find camera", _item);
        menuItem->addSimpleAction(MenuActionToolFindView, "Find view", _item);
        menuItem->addSimpleAction(MenuActionToolDistanceToCamera, "Distance to camera", _item);
        ret = true;
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi
