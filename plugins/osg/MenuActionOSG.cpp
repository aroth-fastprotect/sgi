#include "stdafx.h"
#include "MenuActionOSG.h"
#include "string_helpers.h"
#include "SGIItemOsg.h"

#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>
#include <sgi/ObjectLoggerDialog>
#include <sgi/SGIItemInternal>
#include <sgi/helpers/string>

#include <osg/PagedLOD>
#include <osg/Geode>
#include <osg/ClipNode>
#include <osg/Depth>
#include <osg/Material>
#include <osg/ProxyNode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osgDB/DatabasePager>
#include <osgViewer/ViewerBase>
#include <osgViewer/View>
#include <osgText/Text>
#include <osgUtil/CullVisitor>
#include <osg/io_utils>
#include <osg/ComputeBoundsVisitor>
#include <osgDB/ReadFile>
#include <osgAnimation/AnimationManagerBase>

#include "osg_accessor.h"
#include "stateset_helpers.h"
#include "SettingsDialogOSG.h"
#include "FindTopMostNodeOfType"
#include "DrawableHelper.h"
#include "ManipulateObject.h"

#include "std_ostream_osg.h"

#undef max
#undef min

namespace sgi {
namespace osg_plugin {

ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectSetName)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectSave)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectDataVariance)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeMask)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeNumUpdateTraversal)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeNumEventTraversal)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeRecomputeBound)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeCullingActive)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeLookAt)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeCreateStateSet)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeStripTextures)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectLogger)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectLoggerVisible)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectLoggerActive)

ACTION_HANDLER_IMPL_REGISTER(MenuActionGroupAddChild)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetRenderHint)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetRenderBinName)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetRenderBinNumber)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetRenderBinMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetTextureAttributeList)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetAddUniform)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetAddAttribute)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetAttributeValue)
ACTION_HANDLER_IMPL_REGISTER(MenuActionStateSetUniformValue)

ACTION_HANDLER_IMPL_REGISTER(MenuActionProgramAddShader)

ACTION_HANDLER_IMPL_REGISTER(MenuActionCameraCullSettings)
ACTION_HANDLER_IMPL_REGISTER(MenuActionCameraClearColor)
ACTION_HANDLER_IMPL_REGISTER(MenuActionCameraComputeNearFarMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionProxyNodeLoadingExternalReferenceMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionProxyNodeForceLoad)
ACTION_HANDLER_IMPL_REGISTER(MenuActionClipNodeReset)
ACTION_HANDLER_IMPL_REGISTER(MenuActionClipNodeSetState)

ACTION_HANDLER_IMPL_REGISTER(MenuActionUniformEdit)

ACTION_HANDLER_IMPL_REGISTER(MenuActionTexturePreview)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureBorderColor)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureBorderWidth)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureSetMinFilter)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureSetMagFilter)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureSetResizeNonPowerOfTwoHint)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureUseHardwareMipMapGeneration)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureUnRefImageDataAfterApply)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextureSetImage)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDrawableToggleDisabled)
ACTION_HANDLER_IMPL_REGISTER(MenuActionGeodeAddShapeDrawable)
ACTION_HANDLER_IMPL_REGISTER(MenuActionGeodeRenderInfoDrawable)
ACTION_HANDLER_IMPL_REGISTER(MenuActionShapeDrawableColor)
ACTION_HANDLER_IMPL_REGISTER(MenuActionGeometryColor)
ACTION_HANDLER_IMPL_REGISTER(MenuActionShapeCenter)
ACTION_HANDLER_IMPL_REGISTER(MenuActionShapeRotation)
ACTION_HANDLER_IMPL_REGISTER(MenuActionShapeBoxHalfLength)
ACTION_HANDLER_IMPL_REGISTER(MenuActionImagePreview)
ACTION_HANDLER_IMPL_REGISTER(MenuActionHeightFieldPreview)
ACTION_HANDLER_IMPL_REGISTER(MenuActionBillboardMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTransformReferenceFrame)
ACTION_HANDLER_IMPL_REGISTER(MenuActionMatrixTransformEdit)
ACTION_HANDLER_IMPL_REGISTER(MenuActionPositionAttitudeTransformEdit)

ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformAutoScaleToScreen)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformAutoRotateMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetAxis)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetNormal)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetScale)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetPivotPoint)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetMinimumScale)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetMaximumScale)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoTransformSetAutoScaleTransitionWidthRatio)

ACTION_HANDLER_IMPL_REGISTER(MenuActionCameraManipulatorAutoComputeHome)

ACTION_HANDLER_IMPL_REGISTER(MenuActionShaderEditSource)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDepthFunction)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDepthWriteMask)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDepthSetZNear)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDepthSetZFar)

ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseAutoRotateToScreen)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseCharacterHeight)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseCharacterAspectRatio)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseCharacterSizeMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseModifyText)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseDrawMode)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBaseAxisAlignment)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTextBackdropType)

ACTION_HANDLER_IMPL_REGISTER(MenuActionDatabasePagerPause)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDatabasePagerAcceptNewRequests)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDatabasePagerDoPreCompile)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDatabasePagerDeleteSubgraphsInDBThread)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDatabasePagerTargetPageLODNumber)

ACTION_HANDLER_IMPL_REGISTER(MenuActionAnimationManagerBaseAutomaticLink)

ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindUpdateNodes)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindEventNodes)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindNaNNodes)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindVisibleNodes)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolListNodeMasks)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindPagedLODChilds)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolEffectiveStateSet)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindCamera)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolFindView)
ACTION_HANDLER_IMPL_REGISTER(MenuActionToolDistanceToCamera)

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IContextMenuInfo * info = menuInfo();
    if(info)
        info->showSceneGraphDialog(_item->rootBase());
    else
    {
        ISceneGraphDialog * dlg = _hostInterface->showSceneGraphDialog(menu()->parentWidget(), _item->rootBase());
        if(dlg)
            dlg->show();
    }
    return true;
}

bool actionHandlerImpl<MenuActionObjectSetName>::execute()
{
    osg::Object * object = getObject<osg::Object, SGIItemOsg>();
    std::string name = object->getName();
    bool ret;
    ret = _hostInterface->inputDialogString(menu()->parentWidget(),
                                             name,
                                             "Name:", "Set object name",
                                            SGIPluginHostInterface::InputDialogStringEncodingSystem,
                                             _item
                                            );
    if(ret)
        object->setName(name);
    return true;
}

bool actionHandlerImpl<MenuActionObjectSave>::execute()
{
    SGIItemBasePtr root = _item->rootBase();
    std::string filename;
    std::string filename_ext;
    std::vector<std::string> filters;

    _hostInterface->getObjectSuggestedFilename(filename, root);
    _hostInterface->getObjectSuggestedFilenameExtension(filename_ext, root);
    if(!_hostInterface->getObjectFilenameFilters(filters, root))
        filters.push_back("All files (*.*)");

    if(_hostInterface->inputDialogFilename(menu()->parentWidget(), SGIPluginHostInterface::InputDialogFilenameSave, filename, filters, "Save object", root))
    {
        bool writeResult;
        const SGIItemBase * options = NULL;
        bool result = _hostInterface->writeObjectFile(writeResult, root, filename, options);
        if(!result || (result && !writeResult))
        {
            /// @todo show error message box
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionObjectDataVariance>::execute()
{
    osg::Object * object = getObject<osg::Object, SGIItemOsg>();
    object->setDataVariance((osg::Object::DataVariance)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionNotifyLevel>::execute()
{
    osg::NotifySeverity notifyLevel = (osg::NotifySeverity)menuAction()->mode();
    osg::setNotifyLevel(notifyLevel);
    return true;
}

bool actionHandlerImpl<MenuActionNodeMask>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();

    unsigned nodeMask = object->getNodeMask();
    bool ret;
    ret = _hostInterface->inputDialogBitmask(menu()->parentWidget(),
                                             nodeMask,
                                             "Node mask:", "Set node mask",
                                             _item
                                            );
    if(ret)
        object->setNodeMask((osg::Node::NodeMask)nodeMask);
    return true;
}

bool actionHandlerImpl<MenuActionNodeNumUpdateTraversal>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();

    int number = (int)object->getNumChildrenRequiringUpdateTraversal();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                             number,
                                             "Number:", "Set number of update traversals",
                                             0, 1000, 1,
                                             _item
                                            );
    if(ret)
        ((NodeAccess*)object)->setNumUpdateTraversal((unsigned)number);
    return true;
}

bool actionHandlerImpl<MenuActionNodeNumEventTraversal>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();

    int number = (int)object->getNumChildrenRequiringEventTraversal();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                             number,
                                             "Number:", "Set number of event traversals",
                                             0, 1000, 1,
                                             _item
                                            );
    if(ret)
        ((NodeAccess*)object)->setNumEventTraversal((unsigned)number);
    return true;
}

bool actionHandlerImpl<MenuActionNodeRecomputeBound>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    // just invalidate the old bounds (and new bounds will be calculated in next cull traversals)
    object->dirtyBound();
    return true;
}

bool actionHandlerImpl<MenuActionNodeCullingActive>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    object->setCullingActive(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionNodeLookAt>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    NodeLookAt mode = (NodeLookAt)userData->data();

    SGIHostItemOsg viewpointItem(new ReferencedSetViewNodeLookAt(SetViewNodeLookAt(object, mode)));

    SGIHostItemBasePtr view;
    IContextMenuInfo * info = menuAction()->menu()->getInfo();
    if(info)
        view = info->getView();
    _hostInterface->setView(view, &viewpointItem);
    return true;
}

bool actionHandlerImpl<MenuActionNodeCreateStateSet>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    osg::ref_ptr<osg::StateSet> stateSet = object->getOrCreateStateSet();
    return true;
}

namespace {

    class StripTexturesVisitor : public osg::NodeVisitor
    {
    public:
        StripTexturesVisitor()
            : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
        }

        enum { MAX_NUM_TEXTURE_UNITS = 4 };
        virtual void apply(osg::Node& node)
        {
            osg::StateSet* ss = node.getStateSet();
            if(ss != NULL)
            {
                for(unsigned int i = 0; i < MAX_NUM_TEXTURE_UNITS; ++i)
                {
                    ss->removeTextureAttribute(i, osg::StateAttribute::TEXTURE);
                }
            }
            traverse(node);
        }

        virtual void apply(osg::Geode& node)
        {
            for(unsigned int i = 0; i < node.getNumDrawables(); ++i)
            {
                process(node.getDrawable(i));
            }

            osg::NodeVisitor::apply(node);
        }

    private:
        void process(osg::Drawable* drawable)
        {
            osg::Geometry* geometry = drawable->asGeometry();

            osg::StateSet* ss = drawable->getStateSet();
            if(ss)
            {
                for(unsigned int i = 0; i < MAX_NUM_TEXTURE_UNITS; ++i)
                {
                    ss->removeTextureAttribute(i, osg::StateAttribute::TEXTURE);
                    if(geometry != NULL)
                        geometry->setTexCoordArray(i, NULL);
                }
                ss->releaseGLObjects();
            }
        }
    };

    struct StripTextures
    {
    public:
        void operator()(osg::Node * object, osg::NodeVisitor* nv)
        {
            StripTexturesVisitor stv;
            object->accept(stv);
        }
    };
}

bool actionHandlerImpl<MenuActionNodeStripTextures>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    manipulateObject<StripTextures>(object);
    return true;
}

bool actionHandlerImpl<MenuActionObjectLogger>::execute()
{
    IContextMenuInfo * info = menuInfo();
    if(info)
        info->showObjectLoggerDialog(_item->rootBase());
    else
    {
        IObjectLoggerDialogPtr dialog = _hostInterface->showObjectLoggerDialog(menu()->parentWidget(), _item->rootBase());
        if(dialog)
            dialog->show();
    }
    return true;
}

bool actionHandlerImpl<MenuActionObjectLoggerVisible>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();

    IObjectLoggerPtr logger;
    if(_hostInterface->getOrCreateObjectLogger(logger, _item->rootBase()))
    {
        if(logger.valid())
        {
            IObjectLoggerDialogPtr dialog = logger->getOrCreateDialog(menu()->parentWidget(), NULL);
            if(dialog.valid())
            {
                if(menuAction()->state())
                    dialog->show();
                else
                    dialog->hide();
            }
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionObjectLoggerActive>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    IObjectLoggerPtr logger;
    if(_hostInterface->getObjectLogger(logger, _item->rootBase()))
    {
        if(logger.valid())
        {
            if(menuAction()->state())
                logger->addItem(_item);
            else
                logger->removeItem(_item);
        }
    }
    else
    {
        menuInfo()->showObjectLoggerDialog(_item->rootBase());
    }
    return true;
}

namespace {
    struct AddChildUpdateOperation
    {
    public:
        AddChildUpdateOperation(osg::Node * child)
            : _child(child) { }
        void operator()(osg::Node * object, osg::NodeVisitor* nv)
        {
            static_cast<osg::Group*>(object)->addChild(_child.release());
        }
        osg::ref_ptr<osg::Node> _child;
    };
}

bool actionHandlerImpl<MenuActionGroupAddChild>::execute()
{
    osg::Group * object = getObject<osg::Group, SGIItemOsg>();
    osg::ref_ptr<osg::Node> child;
    switch(menuAction()->mode())
    {
    default:
        break;
    case MenuActionAddChildAxis:
        //child = new AxisGeode(100.0f);
        break;
    case MenuActionAddChildBillboardAxis:
        break;
    case MenuActionAddChildBoundingBox:
        {
            osg::ComputeBoundsVisitor cbv;
            object->accept(cbv);
            const osg::BoundingBox& bbox = cbv.getBoundingBox();

            osg::BoundingBox::value_type length_x = bbox.xMax() - bbox.xMin();
            osg::BoundingBox::value_type length_y = bbox.yMax() - bbox.yMin();
            osg::BoundingBox::value_type length_z = bbox.zMax() - bbox.zMin();

            osg::Geode * geode = new osg::Geode;
            osg::Box * box = new osg::Box(bbox.center(), length_x, length_y, length_z);
            osg::ShapeDrawable * shape = new osg::ShapeDrawable(box);
            geode->addDrawable(shape);
            child = geode;
        }
        break;
    }
    manipulateObject<AddChildUpdateOperation>(static_cast<osg::Node*>(object), child);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetRenderHint>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet,SGIItemOsg>();
    object->setRenderingHint((osg::StateSet::RenderingHint)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetRenderBinName>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet,SGIItemOsg>();
    std::string name = object->getBinName();
    bool ret;
    ret = _hostInterface->inputDialogString(menu()->parentWidget(),
                                             name,
                                             "Name:", "Set render bin name",
                                            SGIPluginHostInterface::InputDialogStringEncodingSystem,
                                             _item
                                            );
    if(ret)
        object->setBinName(name);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetRenderBinNumber>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet,SGIItemOsg>();
    int number = (int)object->getBinNumber();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                             number,
                                             "Number:", "Set render bin number",
                                             0, 1000, 1,
                                             _item
                                            );
    if(ret)
        object->setBinNumber((unsigned)number);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetRenderBinMode>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    object->setRenderBinMode((osg::StateSet::RenderBinMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetTextureAttributeList>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    return true;
}

bool actionHandlerImpl<MenuActionStateSetMode>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute::GLModeValue mode = (osg::StateAttribute::GLModeValue)modeUserData<ReferencedDataInt>()->data();

    setStateAttributeModeFromObject(object, mode, (StateAttributeModeValue)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAddUniform>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::Uniform::Type type = (osg::Uniform::Type)menuAction()->mode();
    std::string name;
    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), name, "Name:", "Enter name of new uniform", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
        object->addUniform(new osg::Uniform(type, name, 1), osg::StateAttribute::OFF);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAddAttribute>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute::Type type = (osg::StateAttribute::Type)menuAction()->mode();
    osg::ref_ptr<osg::StateAttribute> newAttr;
    switch(type)
    {
    case osg::StateAttribute::PROGRAM:
        newAttr = new osg::Program;
        break;
    case osg::StateAttribute::VIEWPORT:
        newAttr = new osg::Viewport;
        break;
    case osg::StateAttribute::CLIPPLANE:
        newAttr = new osg::ClipPlane;
        break;
    case osg::StateAttribute::DEPTH:
        newAttr = new osg::Depth;
        break;
    case osg::StateAttribute::MATERIAL:
        newAttr = new osg::Material;
        break;
    }
    if(newAttr.valid())
        object->setAttribute(newAttr.get(), osg::StateAttribute::OFF);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAttributeValue>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute * attr = modeUserData<osg::StateAttribute>();
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->setAttribute(attr, value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetUniformValue>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::ref_ptr<osg::Uniform> uniform = modeUserData<osg::Uniform>();
    object->removeUniform(uniform);
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->addUniform(uniform, value);
    return true;
}

bool actionHandlerImpl<MenuActionProgramAddShader>::execute()
{
    osg::Program * object = getObject<osg::Program, SGIItemOsg>();
    osg::Shader::Type type = (osg::Shader::Type)menuAction()->mode();
    object->addShader(new osg::Shader(type));
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeLoadingExternalReferenceMode>::execute()
{
    osg::ProxyNode * object = getObject<osg::ProxyNode, SGIItemOsg>();
    object->setLoadingExternalReferenceMode((osg::ProxyNode::LoadingExternalReferenceMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeForceLoad>::execute()
{
    osg::ProxyNode * object = getObject<osg::ProxyNode, SGIItemOsg>();
    unsigned numFilenames = object->getNumFileNames();
    unsigned numChildren = object->getNumChildren();
    if(numFilenames > numChildren)
    {
        osg::Camera* camera = findFirstParentOfType<osg::Camera>(object);
        osgViewer::View * viewptr = camera?dynamic_cast<osgViewer::View*>(camera->getView()):NULL;
        osg::NodePathList nodepaths = object->getParentalNodePaths();
        if(!nodepaths.empty() && viewptr)
        {
            osg::NodePath nodepath = nodepaths.front();
            const osgDB::DatabasePager * pager = viewptr->getDatabasePager();
            const osg::FrameStamp * fs = viewptr->getFrameStamp();
            osg::ref_ptr<osg::Referenced> opts = object->getDatabaseOptions();
            const std::string & databasePath = object->getDatabasePath();

            for(unsigned int i=numChildren; i<numFilenames; ++i)
            {
                const std::string & filename = object->getFileName(i);
                const float priority = 1.0f;
                const_cast<osgDB::DatabasePager*>(pager)->requestNodeFile(databasePath + filename, nodepath, priority, fs, object->getDatabaseRequest(i), opts);
            }
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionClipNodeReset>::execute()
{
    osg::ClipNode * object = getObject<osg::ClipNode,SGIItemOsg>();
    unsigned numPlanes = object->getNumClipPlanes();
    for(unsigned n = 0; n < numPlanes; n++)
        object->removeClipPlane(n);
    return true;
}

bool actionHandlerImpl<MenuActionClipNodeSetState>::execute()
{
    osg::ClipNode * object = getObject<osg::ClipNode,SGIItemOsg>();
    object->setLocalStateSetModes((osg::StateAttribute::GLModeValue)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionCameraCullSettings>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogCamera, menu()->parentWidget());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
}

bool actionHandlerImpl<MenuActionCameraClearColor>::execute()
{
    osg::Camera * object = getObject<osg::Camera,SGIItemOsg>();

    sgi::Color color = osgColor(object->getClearColor());
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Clear color", "Select camera clear color", _item))
    {
        object->setClearColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionCameraComputeNearFarMode>::execute()
{
    osg::Camera * object = getObject<osg::Camera,SGIItemOsg>();
    object->setComputeNearFarMode((osg::Camera::ComputeNearFarMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionUniformEdit>::execute()
{
    osg::Uniform * object = getObject<osg::Uniform,SGIItemOsg>();
    switch(object->getType())
    {
    case osg::Uniform::BOOL:
        {
            bool value = menuAction()->state();
            object->set(value);
        }
        break;
    case osg::Uniform::INT:
    case osg::Uniform::UNSIGNED_INT:
        {
            int value = 0;
            object->get(value);
            bool ret;
            ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                                    value,
                                                    "Value:", "Value",
                                                    std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1,
                                                    _item
                                                    );
            if(ret)
                object->set(value);
        }
        break;
    case osg::Uniform::FLOAT:
        {
            double value = 0;
            float tmp;
            if(object->get(tmp))
                value = tmp;
            bool ret;
            ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                                    value,
                                                    "Value:", "Value",
                                                    std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), 1,
                                                    _item
                                                    );
            if(ret)
                object->set((float)value);
        }
        break;
    case osg::Uniform::DOUBLE:
        {
            double value = 0;
            object->get(value);
            bool ret;
            ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                                    value,
                                                    "Value:", "Value",
                                                    std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), 1,
                                                    _item
                                                    );
            if(ret)
                object->set(value);
        }
        break;
    default:
        // Type not yet implemented
        break;
    }
    return true;
}

bool actionHandlerImpl<MenuActionTexturePreview>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogImagePreview, menu()->parentWidget());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
}

bool actionHandlerImpl<MenuActionTextureBorderColor>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();

    sgi::Color color = osgColor(object->getBorderColor());
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Border color", "Select texture border color", _item))
    {
        object->setBorderColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionTextureBorderWidth>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    int number = (int)object->getBorderWidth();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                             number,
                                             "Width:", "Set border width",
                                             0, 1000, 1,
                                             _item
                                            );
    if(ret)
        object->setBorderWidth((GLint)number);
    return true;
}

bool actionHandlerImpl<MenuActionTextureSetMinFilter>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    object->setFilter(osg::Texture::MIN_FILTER, (osg::Texture::FilterMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextureSetMagFilter>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    object->setFilter(osg::Texture::MAG_FILTER, (osg::Texture::FilterMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextureSetResizeNonPowerOfTwoHint>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    object->setResizeNonPowerOfTwoHint(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTextureUseHardwareMipMapGeneration>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    object->setUseHardwareMipMapGeneration(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTextureUnRefImageDataAfterApply>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    object->setUnRefImageDataAfterApply(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTextureSetImage>::execute()
{
    osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
    std::string filename;
    std::vector<std::string> filters;
    filters.push_back("Image files (*.png *.jpg *.bmp *.tiff)");
    filters.push_back("All files (*.*)");

    bool ret;
    ret = _hostInterface->inputDialogFilename(menu()->parentWidget(), SGIPluginHostInterface::InputDialogFilenameOpen,
                                             filename, filters, "Load image for texture", _item );
    if(ret)
    {
        osg::ref_ptr<osg::Image> image;
        image = osgDB::readImageFile(filename);
        if(image.valid())
        {
            unsigned numImages = std::max(object->getNumImages(), 1u);
            for(unsigned n = 0; n < numImages; n++)
                object->setImage(n, image);
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionGeometryColor>::execute()
{
    osg::Geometry * object = getObject<osg::Geometry,SGIItemOsg>();
    osg::Vec4Array * colorArray = (osg::Vec4Array *)object->getColorArray();
    if(colorArray && colorArray->size() > 0)
    {
        sgi::Color color = osgColor((*colorArray)[0]);
        if(_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Color", "Select geometry color", _item))
        {
            for(unsigned i = 0; i < colorArray->size(); i++)
                (*colorArray)[i] = osgColor(color);
            triggerRepaint();
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionImagePreview>::execute()
{
    osg::Image * object = getObject<osg::Image,SGIItemOsg>();
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogImagePreview, menu()->parentWidget());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
}

bool actionHandlerImpl<MenuActionHeightFieldPreview>::execute()
{
    osg::HeightField * object = getObject<osg::HeightField,SGIItemOsg>();
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogImagePreview, menu()->parentWidget());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
}

bool actionHandlerImpl<MenuActionDrawableToggleDisabled>::execute()
{
    osg::Drawable * object = getObject<osg::Drawable,SGIItemOsg>();
    DisabledDrawCallback::disable(object, menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionGeodeAddShapeDrawable>::execute()
{
    osg::Geode * object = getObject<osg::Geode,SGIItemOsg>();
    MenuActionAddShapeType shapeType = (MenuActionAddShapeType)menuAction()->mode();
    osg::ref_ptr<osg::Shape> newShape;
    switch(shapeType)
    {
    case MenuActionAddShapeInvalid:
    default:
        break;
    case MenuActionAddShapeSphere:
        newShape = new osg::Sphere;
        break;
    case MenuActionAddShapeBox:
        newShape = new osg::Box;
        break;
    case MenuActionAddShapeCone:
        newShape = new osg::Cone;
        break;
    case MenuActionAddShapeCylinder:
        newShape = new osg::Cylinder;
        break;
    case MenuActionAddShapeCapsule:
        newShape = new osg::Cylinder;
        break;
    }
    if(newShape.valid())
    {
        osg::ref_ptr<osg::ShapeDrawable> newDrawable = new osg::ShapeDrawable(newShape.get());
        object->addDrawable(newDrawable.get());
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionGeodeRenderInfoDrawable>::execute()
{
    osg::Geode * object = getObject<osg::Geode,SGIItemOsg>();
    RenderInfoDrawable::enable(object, menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionShapeDrawableColor>::execute()
{
    osg::ShapeDrawable * object = getObject<osg::ShapeDrawable,SGIItemOsg>();
    sgi::Color color = osgColor(object->getColor());
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Color", "Select geometry color", _item))
    {
        object->setColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionShapeCenter>::execute()
{
    osg::Shape * shape = getObject<osg::Shape,SGIItemOsg>();
    osg::Sphere * sphere = dynamic_cast<osg::Sphere*>(shape);
    osg::Box * box = dynamic_cast<osg::Box*>(shape);
    osg::Cone * cone = dynamic_cast<osg::Cone*>(shape);
    osg::Cylinder * cylinder = dynamic_cast<osg::Cylinder*>(shape);
    osg::Capsule * capsule = dynamic_cast<osg::Capsule*>(shape);

    osg::Vec3d value;
    if(sphere)
        value = sphere->getCenter();
    if(box)
        value = box->getCenter();
    if(cone)
        value = cone->getCenter();
    if(cylinder)
        value = cylinder->getCenter();
    if(capsule)
        value = capsule->getCenter();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Center", "Set center position", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        if(sphere)
            sphere->setCenter(value);
        if(box)
            box->setCenter(value);
        if(cone)
            cone->setCenter(value);
        if(cylinder)
            cylinder->setCenter(value);
        if(capsule)
            capsule->setCenter(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionShapeRotation>::execute()
{
    osg::Shape * shape = getObject<osg::Shape,SGIItemOsg>();
    osg::Box * box = dynamic_cast<osg::Box*>(shape);
    osg::Cone * cone = dynamic_cast<osg::Cone*>(shape);
    osg::Cylinder * cylinder = dynamic_cast<osg::Cylinder*>(shape);
    osg::Capsule * capsule = dynamic_cast<osg::Capsule*>(shape);

    osg::Quat value;
    if(box)
        value = box->getRotation();
    if(cone)
        value = cone->getRotation();
    if(cylinder)
        value = cylinder->getRotation();
    if(capsule)
        value = capsule->getRotation();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Rotation", "Set rotation", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        if(box)
            box->setRotation(value);
        if(cone)
            cone->setRotation(value);
        if(cylinder)
            cylinder->setRotation(value);
        if(capsule)
            capsule->setRotation(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionShapeBoxHalfLength>::execute()
{
    osg::Box * object = getObject<osg::Box,SGIItemOsg>();
    osg::Vec3d value = object->getHalfLengths();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Half length", "Set half length", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        object->setHalfLengths(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionBillboardMode>::execute()
{
    osg::Billboard * object = getObject<osg::Billboard,SGIItemOsg>();
    object->setMode((osg::Billboard::Mode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTransformReferenceFrame>::execute()
{
    osg::Transform * object = getObject<osg::Transform,SGIItemOsg>();
    object->setReferenceFrame((osg::Transform::ReferenceFrame)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionMatrixTransformEdit>::execute()
{
    osg::MatrixTransform * object = getObject<osg::MatrixTransform,SGIItemOsg>();
    /// @todo implement MatrixTransform edit dialog
    return true;
}

bool actionHandlerImpl<MenuActionPositionAttitudeTransformEdit>::execute()
{
    osg::PositionAttitudeTransform * object = getObject<osg::PositionAttitudeTransform,SGIItemOsg>();
    /// @todo implement PAT edit dialog
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformAutoScaleToScreen>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    object->setAutoScaleToScreen(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformAutoRotateMode>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    object->setAutoRotateMode((osg::AutoTransform::AutoRotateMode)menuAction()->mode());
    return true;
}
bool actionHandlerImpl<MenuActionAutoTransformSetAxis>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    osg::Vec3d value = object->getAxis();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Axis", "Set axis", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        object->setAxis(value);
        triggerRepaint();
    }
    return true;
}


bool actionHandlerImpl<MenuActionAutoTransformSetNormal>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    osg::Vec3d value = object->getNormal();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Normal", "Set normal", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        object->setNormal(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetScale>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    osg::Vec3d value = object->getScale();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Scale", "Set scale", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        object->setScale(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetPivotPoint>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    osg::Vec3d value = object->getPivotPoint();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Pivot point", "Set pivot point", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        object->setPivotPoint(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetMinimumScale>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    double value = object->getMinimumScale();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set minimum scale", 0.0, 1000.0, 1, _item);
    if(gotInput)
    {
        object->setMinimumScale(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetMaximumScale>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    double value = object->getMaximumScale();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set maximum scale", 0.0, 1000.0, 1, _item);
    if(gotInput)
    {
        object->setMaximumScale(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    double value = object->getAutoUpdateEyeMovementTolerance();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set auto update eye movement tolerance", 0.0, 1000.0, 1, _item);
    if(gotInput)
    {
        object->setAutoUpdateEyeMovementTolerance(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoTransformSetAutoScaleTransitionWidthRatio>::execute()
{
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    double value = object->getAutoScaleTransitionWidthRatio();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set auto scale transition width ratio", 0.0, 1000.0, 1, _item);
    if(gotInput)
    {
        object->setAutoScaleTransitionWidthRatio(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionDatabasePagerPause>::execute()
{
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    object->setDatabasePagerThreadPause(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionDatabasePagerAcceptNewRequests>::execute()
{
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    object->setAcceptNewDatabaseRequests(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionDatabasePagerDoPreCompile>::execute()
{
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    object->setDoPreCompile(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionDatabasePagerDeleteSubgraphsInDBThread>::execute()
{
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    object->setDeleteRemovedSubgraphsInDatabaseThread(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionDatabasePagerTargetPageLODNumber>::execute()
{
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    int number = (int)object->getTargetMaximumNumberOfPageLOD();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                             number,
                                             "Number:", "Set target maximum number of PageLODs",
                                             0, 10000, 1,
                                             _item
                                            );
    if(ret)
        object->setTargetMaximumNumberOfPageLOD((unsigned)number);
    return true;
}

bool actionHandlerImpl<MenuActionCameraManipulatorAutoComputeHome>::execute()
{
    osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator,SGIItemOsg, DynamicCaster>();
    object->setAutoComputeHomePosition(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionShaderEditSource>::execute()
{
    osg::Shader * object = getObject<osg::Shader,SGIItemOsg>();
    std::string source = object->getShaderSource();
    bool ret;
    ret = _hostInterface->inputDialogText(menu()->parentWidget(),
                                             source,
                                             "Source:", "Edit shader source code",
                                             SGIPluginHostInterface::InputDialogStringEncodingSystem,
                                            _item
                                            );
    if(ret)
        object->setShaderSource(source);
    return true;
}

bool actionHandlerImpl<MenuActionDepthFunction>::execute()
{
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    object->setFunction((osg::Depth::Function)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionDepthWriteMask>::execute()
{
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    object->setWriteMask(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionDepthSetZNear>::execute()
{
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    double number = object->getZNear();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "ZNear:", "Set near value for depth buffer",
                                             0.0, 10.0, 2,
                                             _item
                                            );
    if(ret)
        object->setZNear((float)number);
    return true;
}

bool actionHandlerImpl<MenuActionDepthSetZFar>::execute()
{
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    double number = object->getZFar();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "ZFar:", "Set far value for depth buffer",
                                             0.0, 10.0, 2,
                                             _item
                                            );
    if(ret)
        object->setZFar((float)number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseAutoRotateToScreen>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    object->setAutoRotateToScreen(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseCharacterHeight>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    double number = object->getCharacterHeight();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "Character height:", "Set height for characters",
                                             0.0, 1000.0, 1,
                                             _item
                                            );
    if(ret)
        object->setCharacterSize(number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseCharacterAspectRatio>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    double number = object->getCharacterAspectRatio();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "Character aspect ratio:", "Set aspect ratio for characters",
                                             0.0, 1000.0, 1,
                                             _item
                                            );
    if(ret)
        object->setCharacterSize(object->getCharacterHeight(), number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseCharacterSizeMode>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    object->setCharacterSizeMode((osgText::TextBase::CharacterSizeMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseDrawMode>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    object->setDrawMode((osgText::TextBase::DrawModeMask)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseAxisAlignment>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    object->setAxisAlignment((osgText::TextBase::AxisAlignment)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropType>::execute()
{
    osgText::Text * object = getObject<osgText::Text,SGIItemOsg>();
    object->setBackdropType((osgText::Text::BackdropType)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseModifyText>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase,SGIItemOsg>();
    std::string text = object->getText().createUTF8EncodedString();
    bool ret;
    ret = _hostInterface->inputDialogText(menu()->parentWidget(),
                                             text,
                                             "Text:", "Edit text",
                                             SGIPluginHostInterface::InputDialogStringEncodingUTF8,
                                            _item
                                            );
    if(ret)
        object->setText(text, osgText::String::ENCODING_UTF8);
    return true;
}

bool actionHandlerImpl<MenuActionAnimationManagerBaseAutomaticLink>::execute()
{
    osgAnimation::AnimationManagerBase * object = getObject<osgAnimation::AnimationManagerBase, SGIItemOsg, DynamicCaster>();
    object->setAutomaticLink(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionAnimationManagerBaseDirty>::execute()
{
    osgAnimation::AnimationManagerBase * object = getObject<osgAnimation::AnimationManagerBase, SGIItemOsg, DynamicCaster>();
    object->dirty();
    return true;
}

namespace {

class DiagnosticNodeVisitor : public osg::NodeVisitor
{
public:
    DiagnosticNodeVisitor(TraversalMode tm=TRAVERSE_NONE)
        : osg::NodeVisitor(tm) {}

    DiagnosticNodeVisitor(VisitorType type,TraversalMode tm=TRAVERSE_NONE)
        : osg::NodeVisitor(type, tm) {}

    void clear()
        { _nodes.clear(); }

    const osg::NodePathList &   results() const
        { return _nodes; }

protected:
    void                    foundNode(osg::Node & node)
    {
        const osg::NodePath & path = getNodePath();
        _nodes.push_back(path);
    }
    void                    foundChild(osg::Node * child)
    {
        osg::NodePath path = getNodePath();
        path.push_back(child);
        _nodes.push_back(path);
    }
protected:
    osg::NodePathList       _nodes;
};

class FindVisibleNodesVisitor : public DiagnosticNodeVisitor
{
public:
    FindVisibleNodesVisitor(osg::Node::NodeMask mask=~0u)
        : DiagnosticNodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            setTraversalMask(mask);
        }

    virtual void apply(osg::Node& node)
    {
        foundNode(node);
        traverse(node);
    }
};

class FindUpdateTraversalNodesVisitor : public DiagnosticNodeVisitor
{
public:
    FindUpdateTraversalNodesVisitor()
        : DiagnosticNodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
        }

    virtual void apply(osg::Node& node)
    {
        if(_nodePath.size() == 1)
        {
            // only check this node if this is the only node
            if(node.getNumChildrenRequiringUpdateTraversal() != 0)
                foundNode(node);
        }
        traverse(node);
    }
    virtual void apply(osg::Group& node)
    {
        unsigned numUpdates = node.getNumChildrenRequiringUpdateTraversal();
        unsigned numChildUpdates = 0;
        unsigned numChildren = node.getNumChildren();
        for(unsigned n = 0; n < numChildren; n++)
        {
            osg::Node * child = node.getChild(n);
            if(child)
            {
                if(child->getNumChildrenRequiringUpdateTraversal() != 0)
                {
                    numChildUpdates++;
                }
            }
        }
        if(numUpdates > numChildUpdates)
            foundNode(node);

        traverse(node);
    }
};

class FindEventTraversalNodesVisitor : public DiagnosticNodeVisitor
{
public:
    FindEventTraversalNodesVisitor()
        : DiagnosticNodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
        }

    virtual void apply(osg::Node& node)
    {
        if(node.getNumChildrenRequiringEventTraversal() != 0)
            foundNode(node);
        traverse(node);
    }
};

class FindPagedLODChildsNodesVisitor : public DiagnosticNodeVisitor
{
public:
    FindPagedLODChildsNodesVisitor()
        : DiagnosticNodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
        }

    virtual void apply(osg::PagedLOD& node)
    {
        unsigned numChildren = node.getNumChildren();
        for(unsigned n = 0; n < numChildren; n++)
        {
            osg::Node * child = node.getChild(n);
            if(child)
                foundChild(child);
        }

        traverse(node);
    }
};
}

bool actionHandlerImpl<MenuActionToolFindUpdateNodes>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;

    if(object)
    {
        FindUpdateTraversalNodesVisitor visitor;
        object->accept(visitor);

        const osg::NodePathList & results = visitor.results();
        if(results.empty())
        {
            std::stringstream os;
            os << "<i>no nodes found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * updateNodes = selectedItem->addChild("UpdateNodes", (SGIItemBase*)NULL);

            unsigned num = 0;
            for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
            {
                const osg::NodePath & path = *it;
                IObjectTreeItem * pathNode = updateNodes->addChild(helpers::str_plus_number("Path", num), (SGIItemBase*)NULL);

                for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
                {
                    const osg::ref_ptr<osg::Node> & node = *it;
                    SGIHostItemOsg item(node.get());
                    pathNode->addChild(std::string(), &item);
                }
            }
        }

    }
    return true;
}

bool actionHandlerImpl<MenuActionToolFindEventNodes>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;

    if(object)
    {
        FindEventTraversalNodesVisitor visitor;
        object->accept(visitor);

        const osg::NodePathList & results = visitor.results();
        if(results.empty())
        {
            std::stringstream os;
            os << "<i>no nodes found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * updateNodes = selectedItem->addChild("EventNodes", (SGIItemBase*)NULL);

            unsigned num = 0;
            for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
            {
                const osg::NodePath & path = *it;
                IObjectTreeItem * pathNode = updateNodes->addChild(helpers::str_plus_number("Path", num), (SGIItemBase*)NULL);

                for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
                {
                    const osg::ref_ptr<osg::Node> & node = *it;
                    SGIHostItemOsg item(node.get());
                    pathNode->addChild(std::string(), &item);
                }
            }
        }

    }
    return true;
}

bool actionHandlerImpl<MenuActionToolFindNaNNodes>::execute()
{
    return true;
}

bool actionHandlerImpl<MenuActionToolFindVisibleNodes>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;

    if(object)
    {
        osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
        if(!camera)
            camera = findFirstParentOfType<osg::Camera>(object);

        FindVisibleNodesVisitor visitor(camera?camera->getCullMask():~0u);
        object->accept(visitor);

        const osg::NodePathList & results = visitor.results();
        if(results.empty())
        {
            std::stringstream os;
            os << "<i>no nodes found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * updateNodes = selectedItem->addChild("Visible nodes", (SGIItemBase*)NULL);

            unsigned num = 0;
            for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
            {
                const osg::NodePath & path = *it;
                IObjectTreeItem * pathNode = updateNodes->addChild(helpers::str_plus_number("Path", num), (SGIItemBase*)NULL);

                for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
                {
                    const osg::ref_ptr<osg::Node> & node = *it;
                    SGIHostItemOsg item(node.get());
                    pathNode->addChild(std::string(), &item);
                }
            }
        }

    }
    return true;
}

class ListNodeMasksVisitor : public osg::NodeVisitor
{
public:
    typedef std::map<osg::Node::NodeMask, osg::NodePathList> NodeMaskUsers;

public:
    ListNodeMasksVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    void clear()
        { _list.clear(); }

    const NodeMaskUsers &   results() const
        { return _list; }

    virtual void apply(osg::Node& node)
    {
        osg::Node::NodeMask mask = node.getNodeMask();
        const osg::NodePath & path = getNodePath();
        _list[mask].push_back(path);
        traverse(node);
    }

private:
    NodeMaskUsers _list;
};

bool actionHandlerImpl<MenuActionToolListNodeMasks>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;

    if(object)
    {
        ListNodeMasksVisitor visitor;
        object->accept(visitor);

        const ListNodeMasksVisitor::NodeMaskUsers & results = visitor.results();
        if(results.empty())
        {
            std::stringstream os;
            os << "<i>no nodes found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * nodeMaskItems = selectedItem->addChild("Node masks", (SGIItemBase*)NULL);

            for(ListNodeMasksVisitor::NodeMaskUsers::const_iterator itMask = results.begin(); itMask != results.end(); itMask++)
            {
                const osg::Node::NodeMask & nodemask = itMask->first;
                const osg::NodePathList & nodes = itMask->second;

                IObjectTreeItem * nodeMaskItem = nodeMaskItems->addChild(helpers::str_plus_hex("Mask", nodemask), (SGIItemBase*)NULL);
                unsigned num = 0;
                for(osg::NodePathList::const_iterator it = nodes.begin(); it != nodes.end(); num++, it++)
                {
                    const osg::NodePath & path = *it;
                    IObjectTreeItem * pathNode = nodeMaskItem->addChild(helpers::str_plus_number("Path", num), (SGIItemBase*)NULL);

                    for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
                    {
                        const osg::ref_ptr<osg::Node> & node = *it;
                        SGIHostItemOsg item(node.get());
                        pathNode->addChild(std::string(), &item);
                    }
                }
            }
        }

    }
    return true;
}

bool actionHandlerImpl<MenuActionToolFindPagedLODChilds>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;

    if(object)
    {
        FindPagedLODChildsNodesVisitor visitor;
        object->accept(visitor);

        const osg::NodePathList & results = visitor.results();
        if(results.empty())
        {
            std::stringstream os;
            os << "<i>no nodes found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * updateNodes = selectedItem->addChild("PagedLODChilds", (SGIItemBase*)NULL);

            unsigned num = 0;
            for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
            {
                const osg::NodePath & path = *it;
                IObjectTreeItem * pathNode = updateNodes->addChild(helpers::str_plus_number("Path", num), (SGIItemBase*)NULL);

                for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
                {
                    const osg::ref_ptr<osg::Node> & node = *it;
                    SGIHostItemOsg item(node.get());
                    pathNode->addChild(std::string(), &item);
                }
            }
        }

    }
    return true;
}

class EffectiveStateSetVisitor : public osg::NodeVisitor
{
public:
    EffectiveStateSetVisitor()
        : osg::NodeVisitor(TRAVERSE_PARENTS)
        , _stateSet()
    {
    }

    ~EffectiveStateSetVisitor()
    {
    }

    virtual void apply(osg::Node& node)
    {
        const osg::StateSet * stateSet = node.getStateSet();
        merge(stateSet);
        osg::NodeVisitor::apply(node);
    }

    virtual void apply(osg::Geode& node)
    {
        osg::NodeVisitor::apply(node);
    }

    osg::StateSet * stateSet() { return _stateSet; }

private:
    void merge(const osg::StateSet * ss)
    {
        if(ss)
        {
            if(!_stateSet.valid())
                _stateSet = new osg::StateSet;
            _stateSet->merge(*ss);
        }
    }

private:
    osg::ref_ptr<osg::StateSet> _stateSet;
};

namespace {

    typedef osg::ref_ptr<osg::StateSet> StateSetPtr;
    typedef std::vector<StateSetPtr> StateSetVector;

    typedef osg::ref_ptr<osgUtil::StateGraph> StateGraphPtr;
    typedef std::vector<StateGraphPtr> StateGraphVector;

    void computeStateGraph(const osg::NodePath & path, StateGraphPtr & ret_stategraph)
    {
        osg::ref_ptr<osgUtil::CullVisitor> cv = osgUtil::CullVisitor::create();

        osg::ref_ptr<osg::Camera> camera;
        const osg::ref_ptr<osg::Node> & front = path.front();
        if(osg::Camera* firstIsCamera = dynamic_cast<osg::Camera*>(front.get()))
            camera = firstIsCamera;
        else
        {
            camera = new osg::Camera;
            camera->getProjectionMatrix().makePerspective(50.0f,1.4f,1.0f,10000.0f);
            camera->getViewMatrix().makeIdentity();
        }
        cv->reset();
        
        osg::ref_ptr<osg::FrameStamp> frameStamp = new osg::FrameStamp;
        cv->setFrameStamp(frameStamp.get());
        // use the frame number for the traversal number.
        if (frameStamp.valid())
            cv->setTraversalNumber(frameStamp->getFrameNumber());

        //cv->inheritCullSettings(*this);
        cv->setStateGraph(new osgUtil::StateGraph);
        osg::ref_ptr<osgUtil::RenderStage> renderStage = new osgUtil::RenderStage;
        renderStage->setCamera(camera);
        cv->setRenderStage(renderStage);

        for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::ref_ptr<osg::Node> & node = *it;
            osg::Camera * camera = node->asCamera();
            osg::Geode * geode = node->asGeode();
            osg::Switch * switchnode = node->asSwitch();
            osg::Group * group = node->asGroup();
            osg::Transform * transform = node->asTransform();
            
            if(osg::StateSet * ss = node->getStateSet())
                cv->pushStateSet(ss);
/*
            if(camera)
                cv->apply(*camera);
            else if(transform)
                cv->apply(*transform);
            else if(switchnode)
                cv->apply(*switchnode);
            else if(group)
                cv->apply(*group);
            else if(geode)
                cv->apply(*geode);
            else
                cv->apply(*node.get());
*/
        }
        ret_stategraph = cv->getCurrentStateGraph();
    }

    void computeEffectiveStateGraph(StateGraphVector & stateGraphs, osg::Node * node)
    {
        osg::NodePathList parentalPaths = node->getParentalNodePaths();
        stateGraphs.resize(parentalPaths.size());
        unsigned num = 0;
        for(osg::NodePathList::const_iterator it = parentalPaths.begin(); it != parentalPaths.end(); it++, num++)
        {
            const osg::NodePath & path = *it;
            computeStateGraph(path, stateGraphs[num]);
        }
    }

    void computeStateSets(const osg::NodePath & path, StateSetVector & ret_statesets)
    {
        ret_statesets.clear();
        for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::ref_ptr<osg::Node> & node = *it;
            osg::Camera * camera = node->asCamera();
            osg::Geode * geode = node->asGeode();
            osg::Switch * switchnode = node->asSwitch();
            osg::Group * group = node->asGroup();
            osg::Transform * transform = node->asTransform();

            if(osg::StateSet * ss = node->getStateSet())
            {
                if(ret_statesets.empty() || ret_statesets.back() != ss)
                    ret_statesets.push_back(ss);
            }
        }
    }

    
}

bool actionHandlerImpl<MenuActionToolEffectiveStateSet>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;
    if(object)
    {
        selectedItem->expand();
        IObjectTreeItem * resultGroup = selectedItem->addChild("EffectiveStateSets", (SGIItemBase*)NULL);

        osg::NodePathList parentalPaths = object->getParentalNodePaths();
        unsigned num = 0;
        for(osg::NodePathList::const_iterator it = parentalPaths.begin(); it != parentalPaths.end(); it++, num++)
        {
            const osg::NodePath & path = *it;
            StateSetVector statesets;
            computeStateSets(path, statesets);

            IObjectTreeItem * stateSetItem = resultGroup->addChild(helpers::str_plus_number("EffectiveStateSet", num), (SGIItemBase*)NULL);

            for(StateSetVector::const_iterator it = statesets.begin(); it != statesets.end(); it++)
            {
                SGIHostItemOsg item(*it);
                stateSetItem->addChild(std::string(), &item);
            }
        }

#if 0
        StateGraphVector results;
        computeEffectiveStateGraph(results, object);

        IObjectTreeItem * resultGroup = selectedItem->addChild("EffectiveStateGraph", (SGIItemBase*)NULL);

        unsigned num = 0;
        for(auto it = results.begin(); it != results.end(); num++, it++)
        {
            auto result = *it;
            osgUtil::StateGraph * stateGraph = result.get();
            osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet(*stateGraph->getStateSet(), osg::CopyOp::DEEP_COPY_ALL);
            SGIHostItemOsg item(stateSet);
            resultGroup->addChild(helpers::str_plus_number("EffectiveStateGraph", num), &item);
        }
#endif
#if 0
        EffectiveStateSetVisitor essv;
        object->accept(essv);

        SGIHostItemOsg item(essv.stateSet());
        selectedItem->addChild("EffectiveStateSet", &item);
#endif

    }
    return true;
}

namespace {
    /// @brief Given a valid node placed in a scene under a transform, return the
    ///        bounding sphere of the node in world coordinates.
    /// @param node pointer to a node
    /// @return bounding sphere of the given node in world coordinates
    static osg::BoundingSphere computeWorldBoundingSphere(osg::Node* node)
    {
        osg::BoundingSphere ret = node->getBound();
        ComputeLocalToWorldMatrixVisitor ncv;
        node->accept(ncv);
        const osg::Matrix & l2w = ncv.matrix();
        ret.center() = ret.center() * l2w;
        return ret;
    }
}

bool actionHandlerImpl<MenuActionToolDistanceToCamera>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;
    if(object)
    {
        std::stringstream os;
        
        osg::Vec3d eye, center, up;

        osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
        if(!camera)
            camera = findFirstParentOfType<osg::Camera>(object);
        if(camera)
            camera->getViewMatrixAsLookAt(eye, center, up);
        
        os << std::setprecision(12);
        os << "<b>Results for " << getObjectNameAndType(object) << "</b><li>" << std::endl;
        os << "<p>Camera " << getObjectNameAndType(camera) << " at ";
        writePrettyPositionHTML(os, eye, (const osg::Node*)NULL);
        os << "</p>";
        os << "<ul>" << std::endl;
        
        osg::BoundingSphere bs = object->getBound();
        ComputeLocalToWorldMatrixVisitor ncv;
        object->accept(ncv);

        osg::BoundingSphere bsWorld = bs;
        const osg::Matrix & l2w = ncv.matrix();
        bsWorld.center() = bsWorld.center() * l2w;

        os << "<li>Bounding sphere radius: " << bs.radius() << "</li>" << std::endl;

        os << "<li>Bounding sphere center<br/>";
        writePrettyPositionHTML(os, bs.center(), (const osg::Node*)NULL);
        os << "</li>" << std::endl;

        os << "<li>World Bounding sphere center<br/>";
        writePrettyPositionHTML(os, bsWorld.center(), (const osg::Node*)NULL);
        osg::Vec3d bsWorldDiff = eye - bsWorld.center();
        os << "<br/>dist=" << bsWorldDiff.length() << "</li>" << std::endl;
        
        osg::LOD * lod = dynamic_cast<osg::LOD *>(object);
        if(lod)
        {
            double lodDistance = 0;
            switch(lod->getCenterMode())
            {
            case osg::LOD::USE_BOUNDING_SPHERE_CENTER:
            default:
                {
                    osg::Vec3d bsDiff = eye - lod->getCenter();
                    os << "<li>osg::LOD::USE_BOUNDING_SPHERE_CENTER</li>" << std::endl;
                    lodDistance = bsWorldDiff.length();
                }
                break;
            case osg::LOD::USER_DEFINED_CENTER:
                {
                    osg::Vec3d userDefinedCenterWorld = lod->getCenter() * l2w;
                    osg::Vec3d bsUserDefinedDiff = eye - userDefinedCenterWorld;
                    os << "<li>osg::LOD::USER_DEFINED_CENTER<br/>" << std::endl;
                    writePrettyPositionHTML(os, userDefinedCenterWorld, (const osg::Node*)NULL);
                    os << "<br/>dist=" << bsUserDefinedDiff.length() << "</li>" << std::endl;
                    lodDistance = bsUserDefinedDiff.length();
                }
                break;
            case osg::LOD::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED:
                {
                    osg::Vec3d userDefinedCenterWorld = lod->getCenter() * l2w;
                    osg::Vec3d bsUserDefinedDiff = eye - userDefinedCenterWorld;
                    os << "<li>osg::LOD::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED<br/>" << std::endl;
                    writePrettyPositionHTML(os, userDefinedCenterWorld, (const osg::Node*)NULL);
                    os << "<br/>dist=" << bsUserDefinedDiff.length() << "</li>" << std::endl;
                    lodDistance = bsUserDefinedDiff.length();
                }
                break;
            }
            for(unsigned n = 0; n < lod->getNumRanges(); n++)
            {
                float minRange = lod->getMinRange(n);
                float maxRange = lod->getMaxRange(n);
                os << "<li>Child #" << n << " " << getObjectNameAndType(object) << "<br/>";
                if(lodDistance > minRange && lodDistance < maxRange)
                {
                    os << "visible";
                }
                else if(lodDistance < minRange)
                {
                    os << "too close (" << (minRange - lodDistance) << " m)";
                }
                else if(lodDistance > maxRange)
                {
                    os << "too far away (" << (lodDistance - maxRange) << " m)";
                }
                os << "</li>" << std::endl;
            }
        }
        osg::ProxyNode * proxy = dynamic_cast<osg::ProxyNode*>(object);
        if(proxy)
        {
            switch(lod->getCenterMode())
            {
            case osg::ProxyNode::USE_BOUNDING_SPHERE_CENTER:
            default:
                {
                    os << "<li>osg::ProxyNode::USE_BOUNDING_SPHERE_CENTER</li>" << std::endl;
                }
                break;
            case osg::ProxyNode::USER_DEFINED_CENTER:
                {
                    osg::Vec3d userDefinedCenterWorld = lod->getCenter() * l2w;
                    osg::Vec3d bsUserDefinedDiff = eye - userDefinedCenterWorld;
                    os << "<li>osg::ProxyNode::USER_DEFINED_CENTER<br/>" << std::endl;
                    writePrettyPositionHTML(os, userDefinedCenterWorld, (const osg::Node*)NULL);
                    os << "<br/>dist=" << bsUserDefinedDiff.length() << "</li>" << std::endl;
                }
                break;
            case osg::ProxyNode::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED:
                {
                    osg::Vec3d userDefinedCenterWorld = lod->getCenter() * l2w;
                    osg::Vec3d bsUserDefinedDiff = eye - userDefinedCenterWorld;
                    os << "<li>osg::ProxyNode::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED<br/>" << std::endl;
                    writePrettyPositionHTML(os, userDefinedCenterWorld, (const osg::Node*)NULL);
                    os << "<br/>dist=" << bsUserDefinedDiff.length() << "</li>" << std::endl;
                }
                break;
            }
        }

        dialog->setInfoText(os.str());
    }
    return true;
}

bool actionHandlerImpl<MenuActionToolFindCamera>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;
    if(object)
    {
        osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
        if(!camera)
            camera = findFirstParentOfType<osg::Camera>(object);

        if(camera)
        {
            SGIHostItemOsg item(camera);
            selectedItem->expand();
            selectedItem->addChild("Camera", &item);
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionToolFindView>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu)?toolsMenu->getDialog():NULL;
    IObjectTreeItem * selectedItem = (dialog)?dialog->selectedItem():NULL;
    SGIItemOsg * item = selectedItem?dynamic_cast<SGIItemOsg *>(selectedItem->item()):NULL;
    osg::Node * object = item?dynamic_cast<osg::Node*>(item->object()):NULL;
    if(object)
    {
        osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
        if(!camera)
            camera = findFirstParentOfType<osg::Camera>(object);

        if(camera)
        {
            SGIHostItemOsg item(camera->getView());
            if(item.hasObject())
            {
                selectedItem->expand();
                selectedItem->addChild("View", &item);
            }
        }
    }
    return true;
}

} // namespace osg_plugin
} // namespace sgi
