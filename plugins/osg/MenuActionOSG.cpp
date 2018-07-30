#include "stdafx.h"
#include "MenuActionOSG.h"
#include <sgi/helpers/osg>
#include "SGIItemOsg"
#include <stdint.h>

#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectLoggerDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/ImagePreviewDialog>
#include <sgi/plugins/ObjectTree>
#include <sgi/plugins/SGIImage.h>
#include <sgi/SGIItemInternal>
#include <sgi/helpers/string>

#include <osg/PagedLOD>
#include <osg/Geode>
#include <osg/ClipNode>
#include <osg/Depth>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/PolygonStipple>
#include <osg/LightModel>
#include <osg/AlphaFunc>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Point>
#include <osg/PolygonMode>
#include <osg/CullFace>
#include <osg/TexEnv>
#include <osg/TexEnvFilter>
#include <osg/TexMat>
#include <osg/TexGen>
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
#include <osgUtil/Optimizer>
#include <osg/io_utils>
#include <osg/ComputeBoundsVisitor>
#include <osgDB/ReadFile>
#include <osgAnimation/AnimationManagerBase>
#include <osgUtil/IncrementalCompileOperation>
#include <osgUtil/SmoothingVisitor>

#include <osgViewer/ViewerEventHandlers>

#include <QtCore/QMutex>
#include <cassert>

#include "osg_accessor.h"
#include "osgtext_accessor.h"
#include "stateset_helpers.h"
#include "SettingsDialogOSG.h"
#include "DrawableHelper.h"
#include "NodeHelper.h"
#include "ObjectLoggerOSG.h"
#include <sgi/helpers/osg_helper_nodes>

#undef max
#undef min

namespace sgi {
namespace osg_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectSetName)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectSave)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectDataVariance)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeMask)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeMaskAndChilds)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeClearStateSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeNumUpdateTraversal)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeNumEventTraversal)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeRecomputeBound)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeCullingActive)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeLookAt)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeCreateStateSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeStripTextures)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeOptimizerRun)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeSmoothingVisitor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeToggleCenterMarker)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeRenderInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeFixDeprecatedData)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeInspectCulling)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectLogger)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectLoggerVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectLoggerActive)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionGroupAddChild)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionGroupRemoveChild)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetRenderHint)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetRenderBinName)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetRenderBinNumber)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetRenderBinMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetTextureAttributeSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetTextureAttributeRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetTextureModeSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetTextureModeRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetModeSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetModeRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetAttributeAdd)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetAttributeSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetAttributeRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetUniformAdd)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetUniformSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetUniformRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetDefineAdd)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetDefineSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetDefineEdit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetDefineRemove)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionStateSetClear)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProgramAddShader)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraCullSettings)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraClearColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraComputeNearFarMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraProjectionResizePolicy)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraCullMask)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraViewMatrix)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraProjectionMatrix)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraLiveView)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeSetCenterMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeSetCenter)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeSetRadius)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeLoadingExternalReferenceMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeForceLoad)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeReload)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionProxyNodeSetDatabasePath)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLODSetRangeMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPagedLODDisableExternalChildrenPaging)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPagedLODNumChildrenThatCannotBeExpired)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPagedLODFrameNumberOfLastTraversal)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionClipNodeReset)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionClipNodeSetState)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionUniformDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionUniformEdit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionBufferDataEdit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionBufferDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionBufferObjectDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionArrayDataEdit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionArrayBinding)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionArrayNormalize)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionArrayPreserveDataType)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionArrayTrim)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineWidthSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineStipplePattern)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineStippleFactor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightModelColorControl)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightModelLocalViewer)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightModelTwoSided)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightModelAmbientIntensity)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightSourceReferenceFrame)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionBlendColorConstantColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPolygonModeFront)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPolygonModeBack)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPolygonModeFrontAndBack)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCullFaceMode)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialColorMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialAmbient)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialDiffuse)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialSpecular)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialEmission)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaterialShininess)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureBorderColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureBorderWidth)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureSetMinFilter)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureSetMagFilter)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureSetResizeNonPowerOfTwoHint)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureUseHardwareMipMapGeneration)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureUnRefImageDataAfterApply)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureSetImage)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureDirtyTextureObject)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureDirtyTextureParameters)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextureAllocateMipmapLevels)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTexEnvMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTexEnvColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTexEnvFilterLodBias)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableToggleDisabled)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionGeodeAddShapeDrawable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShapeDrawableColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShapeDrawableBuild)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableUseDisplayList)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableSupportsDisplayList)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableDirtyGLObjects)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableUseVAO)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableUseVBO)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDrawableRenderInfoDrawCallback)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionGeometryColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionGeometryFixDeprecatedData)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShapeCenter)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShapeRotation)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShapeBoxHalfLength)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImagePreview)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionBillboardMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTransformReferenceFrame)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMatrixTransformEdit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionPositionAttitudeTransformEdit)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformAutoScaleToScreen)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformAutoRotateMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetAxis)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetNormal)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetScale)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetPivotPoint)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetMinimumScale)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetMaximumScale)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoTransformSetAutoScaleTransitionWidthRatio)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCameraManipulatorAutoComputeHome)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewerBaseMaxFrameRate)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewerBaseRunFrameScheme)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewerBaseThreadingModel)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewerBaseIncrementalCompileOperation)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShaderDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionShaderEditSource)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDepthFunction)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDepthWriteMask)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDepthSetZNear)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDepthSetZFar)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseAutoRotateToScreen)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseCharacterHeight)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseCharacterAspectRatio)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseCharacterSizeMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseLayout)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseSetFontWidth)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseSetFontHeight)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseModifyText)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseDrawMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseAxisAlignment)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseSetMaximumWidth)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseSetMaximumHeight)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropEnableDepthWrites)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropType)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropImplementation)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropHorizontalOffset)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropVerticalOffset)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBackdropColorGradientMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTextBaseComputeGlyphRepresentation)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerPause)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerDatabaseThreads)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerAcceptNewRequests)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerDoPreCompile)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerDeleteSubgraphsInDBThread)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerTargetPageLODNumber)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDatabasePagerIncrementalCompileOperation)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAnimationManagerBaseAutomaticLink)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAnimationManagerBaseDirty)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionIncrementalCompileOperationTargetFrameRate)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionIncrementalCompileOperationMinimumTimeAvailableForGLCompileAndDeletePerFrame)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionIncrementalCompileOperationMaximumNumOfObjectsToCompilePerFrame)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionIncrementalCompileOperationFlushTimeRatio)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionIncrementalCompileOperationConservativeTimeRatio)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindUpdateNodes)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindEventNodes)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindAllStateSets)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindNaNNodes)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindVisibleNodes)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolListNodeMasks)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindPagedLODChilds)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolEffectiveStateSet)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindCamera)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindView)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolDistanceToCamera)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionToolFindClosestNodeToCamera)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewLightingMode)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewCaptureScreenshot)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionViewPortModify)

using namespace sgi::osg_helpers;

namespace {
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
}

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
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

namespace {
    class SetNodeMaskVisitor : public osg::NodeVisitor
    {
        unsigned _nodemask;
    public:
        SetNodeMaskVisitor(unsigned mask, osg::NodeVisitor::TraversalMode tm)
            : osg::NodeVisitor(tm), _nodemask(mask) {
            // ignore node mask when applying the new node mask
            setTraversalMask(~0u);
            setNodeMaskOverride(~0u);
        }
        virtual void apply(osg::Node& node) override
        {
            node.setNodeMask(_nodemask);
            traverse(node);
        }
    };
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
    if (ret)
        object->setNodeMask(nodeMask);
    return true;
}

bool actionHandlerImpl<MenuActionNodeMaskAndChilds>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();

    unsigned nodeMask = object->getNodeMask();
    bool ret;
    ret = _hostInterface->inputDialogBitmask(menu()->parentWidget(),
        nodeMask,
        "Node mask:", "Set node mask (+childs)",
        _item
        );
    if (ret)
    {
        SetNodeMaskVisitor snmv(nodeMask, osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
        object->accept(snmv);
        object->setNodeMask(nodeMask);
    }
    return true;
}

bool actionHandlerImpl<MenuActionNodeClearStateSet>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    object->setStateSet(nullptr);
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

    SGIItemBasePtr view;
    IHostCallback * callback = hostCallback();
    if(callback)
        view = callback->getView();
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
}

bool actionHandlerImpl<MenuActionNodeStripTextures>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    runVisitorInUpdateCallback<StripTexturesVisitor>(object);
    return true;
}

namespace {
	class OptimizerRun
	{
	public:
		OptimizerRun(MenuActionOptimizerRunMode mode)
			: _mode(mode) {}
		void operator()(osg::Node * object, osg::NodeVisitor* nv)
		{
			osgUtil::Optimizer optimizer;
			switch(_mode)
			{
			case MenuActionOptimizerRunModeAll:
				optimizer.optimize(object, osgUtil::Optimizer::ALL_OPTIMIZATIONS);
				break;
			case MenuActionOptimizerRunModeDefault:
				optimizer.optimize(object, osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS);
				break;
			case MenuActionOptimizerRunModeCheck:
				optimizer.optimize(object, osgUtil::Optimizer::CHECK_GEOMETRY);
				break;
			case MenuActionOptimizerRunModeFastGeometry:
				optimizer.optimize(object, osgUtil::Optimizer::MAKE_FAST_GEOMETRY);
				break;
			}
		}
	private:
		MenuActionOptimizerRunMode _mode;
	};

}

bool actionHandlerImpl<MenuActionNodeOptimizerRun>::execute()
{
	osg::Node * object = getObject<osg::Node, SGIItemOsg>();
	MenuActionOptimizerRunMode mode = (MenuActionOptimizerRunMode)menuAction()->mode();
    //runOperationInUpdateCallback(object, std::bind(OptimizerRun, mode));
	return true;
}

bool actionHandlerImpl<MenuActionNodeSmoothingVisitor>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    osgUtil::SmoothingVisitor sv;
    object->accept(sv);
    return true;
}

bool actionHandlerImpl<MenuActionNodeToggleCenterMarker>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
    ToggleCenterMarkerVisitor tcmv(menuAction()->state());
    object->accept(tcmv);
    return true;
}

bool actionHandlerImpl<MenuActionNodeRenderInfo>::execute()
{
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();
    RenderInfo::enable(node, menuAction()->state());
    return true;
}

class FixDeprecatedDataVisitor : public osg::NodeVisitor
{
public:
    FixDeprecatedDataVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }
    void apply(osg::Geometry& geometry) override
    {
        if(geometry.checkForDeprecatedData())
            geometry.fixDeprecatedData();
        osg::NodeVisitor::apply(geometry);
    }
};

bool actionHandlerImpl<MenuActionNodeFixDeprecatedData>::execute()
{
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();
    FixDeprecatedDataVisitor fddv;
    node->accept(fddv);
    return true;
}

bool actionHandlerImpl<MenuActionNodeInspectCulling>::execute()
{
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();
    CullingInfo::enable(node, menuAction()->state(), _hostInterface);
    return true;
}

bool actionHandlerImpl<MenuActionObjectLogger>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showObjectLoggerDialog(menu()->parentWidget(), _item->rootBase());
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
        hostCallback()->showObjectLoggerDialog(menu()->parentWidget(), _item->rootBase());
    }
    return true;
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
    runOperationInUpdateCallback(object, std::bind(static_cast<bool (osg::Group::*)(const osg::ref_ptr<osg::Node> &)>(&osg::Group::addChild), object, child));
    return true;
}

bool actionHandlerImpl<MenuActionGroupRemoveChild>::execute()
{
    osg::Group * object = getObject<osg::Group, SGIItemOsg>();
    osg::ref_ptr<osg::Node> child = userData<osg::Node>();
    runOperationInUpdateCallback(object, std::bind(static_cast<bool (osg::Group::*)(const osg::ref_ptr<osg::Node> &)>(&osg::Group::removeChild), object, child));
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

bool actionHandlerImpl<MenuActionStateSetTextureAttributeSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    TextureAttributePair pair = modeUserData<ReferencedDataTextureAttributePair>()->data();
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->setTextureAttribute(pair.textureUnit, pair.attr.get(), value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetTextureAttributeRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    TextureAttributePair pair = userData<ReferencedDataTextureAttributePair>()->data();
    object->removeTextureAttribute(pair.textureUnit, pair.attr.get());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetTextureModeSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    IntPair pair = modeUserData<ReferencedDataIntPair>()->data();
    unsigned textureUnit = (unsigned)pair.first;
    osg::StateAttribute::GLMode mode = (osg::StateAttribute::GLMode)pair.second;
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->setTextureMode(textureUnit, mode, value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetTextureModeRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    IntPair pair = userData<ReferencedDataIntPair>()->data();
    unsigned textureUnit = (unsigned)pair.first;
    osg::StateAttribute::GLMode mode = (osg::StateAttribute::GLMode)pair.second;
    object->removeTextureMode(textureUnit, mode);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetModeSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute::GLModeValue mode = (osg::StateAttribute::GLModeValue)modeUserData<ReferencedDataInt>()->data();

    setStateAttributeModeFromObject(object, mode, (StateAttributeModeValue)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetModeRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute::GLModeValue mode = (osg::StateAttribute::GLModeValue)userData<ReferencedDataInt>()->data();
    std::cout << "remove " << std::hex << mode << std::endl;
    object->removeMode(mode);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAttributeAdd>::execute()
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
    case osg::StateAttribute::ALPHAFUNC:
        newAttr = new osg::AlphaFunc;
        break;
    case osg::StateAttribute::BLENDCOLOR:
        newAttr = new osg::BlendColor;
        break;
    case osg::StateAttribute::BLENDFUNC:
        newAttr = new osg::BlendFunc;
        break;
    case osg::StateAttribute::LIGHTMODEL:
        newAttr = new osg::LightModel;
        break;
    case osg::StateAttribute::LIGHT:
        newAttr = new osg::Light;
        break;
    case osg::StateAttribute::POLYGONMODE:
        newAttr = new osg::PolygonMode;
        break;
    case osg::StateAttribute::LINESTIPPLE:
        newAttr = new osg::LineStipple;
        break;
    case osg::StateAttribute::LINEWIDTH:
        newAttr = new osg::LineWidth;
        break;
    case osg::StateAttribute::POINT:
        newAttr = new osg::Point;
        break;
    case osg::StateAttribute::POLYGONSTIPPLE:
        newAttr = new osg::PolygonStipple;
        break;
    case osg::StateAttribute::TEXENV:
        newAttr = new osg::TexEnv;
        break;
    case osg::StateAttribute::TEXENVFILTER:
        newAttr = new osg::TexEnvFilter;
        break;
    case osg::StateAttribute::TEXMAT:
        newAttr = new osg::TexMat;
        break;
    case osg::StateAttribute::TEXGEN:
        newAttr = new osg::TexGen;
        break;
    }
    if(newAttr.valid())
        object->setAttribute(newAttr.get(), osg::StateAttribute::OFF);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAttributeSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::StateAttribute * attr = modeUserData<osg::StateAttribute>();
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->setAttribute(attr, value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetAttributeRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    const ReferencedDataIntPair * attr = userData<ReferencedDataIntPair>();
    const osg::StateAttribute::Type & type = (const osg::StateAttribute::Type &)attr->data().first;
    const unsigned & member = (const unsigned &)attr->data().second;
    object->removeAttribute(type, member);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetUniformAdd>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::Uniform::Type type = (osg::Uniform::Type)menuAction()->mode();
    std::string name;
    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), name, "Name:", "Enter name of new uniform", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
        object->addUniform(new osg::Uniform(type, name, 1), osg::StateAttribute::OFF);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetUniformSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    osg::ref_ptr<osg::Uniform> uniform = modeUserData<osg::Uniform>();
    object->removeUniform(uniform);
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->addUniform(uniform, value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetUniformRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    const ReferencedDataString * uniformName = userData<ReferencedDataString>();
    object->removeUniform(uniformName->data());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetDefineAdd>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    std::string name;
    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), name, "Name:", "Enter name of new define", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
        object->setDefine(name);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetDefineSet>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    const ReferencedDataString * defineName = userData<ReferencedDataString>();
    const osg::StateSet::DefinePair* defpair = object->getDefinePair(defineName->data());
    osg::StateAttribute::OverrideValue value = stateAttributeModeValueToOverrideValue((StateAttributeModeValue)menuAction()->mode());
    object->setDefine(defineName->data(), defpair->first, value);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetDefineEdit>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    const ReferencedDataString * defineName = userData<ReferencedDataString>();
    const osg::StateSet::DefinePair* defpair = object->getDefinePair(defineName->data());

    std::string value = defpair->first;
    bool ret;
    ret = _hostInterface->inputDialogText(menu()->parentWidget(),
                                             value,
                                             "Value:", "Edit value for define " + defineName->data(),
                                             SGIPluginHostInterface::InputDialogStringEncodingSystem,
                                            _item
                                            );
    if(ret)
        object->setDefine(defineName->data(), value, defpair->second);
    return true;
}

bool actionHandlerImpl<MenuActionStateSetDefineRemove>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    const ReferencedDataString * defineName = userData<ReferencedDataString>();
    object->removeDefine(defineName->data());
    return true;
}

bool actionHandlerImpl<MenuActionStateSetClear>::execute()
{
    osg::StateSet * object = getObject<osg::StateSet, SGIItemOsg>();
    object->clear();
    return true;
}

bool actionHandlerImpl<MenuActionProgramAddShader>::execute()
{
    osg::Program * object = getObject<osg::Program, SGIItemOsg>();
    osg::Shader::Type type = (osg::Shader::Type)menuAction()->mode();
    object->addShader(new osg::Shader(type));
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeSetCenterMode>::execute()
{
    osg::ProxyNode * proxynode = getObject<osg::ProxyNode, SGIItemOsg,DynamicCaster>();
    osg::LOD * lod = getObject<osg::LOD, SGIItemOsg,DynamicCaster>();
    if(proxynode)
        proxynode->setCenterMode((osg::ProxyNode::CenterMode)menuAction()->mode());
    else if(lod)
        lod->setCenterMode((osg::LOD::CenterMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeSetCenter>::execute()
{
    osg::ProxyNode * proxynode = getObject<osg::ProxyNode, SGIItemOsg,DynamicCaster>();
    osg::LOD * lod = getObject<osg::LOD, SGIItemOsg,DynamicCaster>();

    osg::Vec3d value;
    if(proxynode)
        value = proxynode->getCenter();
    else if(lod)
        value = lod->getCenter();
    bool gotInput = _hostInterface->inputDialogValueAsString(menuAction()->menu()->parentWidget(), value, "Center", "Set center position", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item.get());
    if(gotInput)
    {
        if(proxynode)
            proxynode->setCenter(value);
        else if(lod)
            lod->setCenter(value);
    }
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeSetRadius>::execute()
{
    osg::ProxyNode * proxynode = getObject<osg::ProxyNode, SGIItemOsg,DynamicCaster>();
    osg::LOD * lod = getObject<osg::LOD, SGIItemOsg,DynamicCaster>();

    double value;
    if(proxynode)
        value = proxynode->getRadius();
    else if(lod)
        value = lod->getRadius();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Radius", "Set radius", 0.0, 10000000.0, 1, _item.get());
    if(gotInput)
    {
        if(proxynode)
            proxynode->setRadius(value);
        else if(lod)
            lod->setRadius(value);
    }
    return true;

    if(proxynode)
        proxynode->setCenterMode((osg::ProxyNode::CenterMode)menuAction()->mode());
    else if(lod)
        lod->setCenterMode((osg::LOD::CenterMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionLODSetRangeMode>::execute()
{
    osg::LOD * object = getObject<osg::LOD, SGIItemOsg>();
    object->setRangeMode((osg::LOD::RangeMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeLoadingExternalReferenceMode>::execute()
{
    osg::ProxyNode * object = getObject<osg::ProxyNode, SGIItemOsg>();
    object->setLoadingExternalReferenceMode((osg::ProxyNode::LoadingExternalReferenceMode)menuAction()->mode());
    return true;
}

bool forceLoadingProxyNode(osg::ProxyNode * object)
{
    bool ret = false;
    unsigned numFilenames = object->getNumFileNames();
    unsigned numChildren = object->getNumChildren();
    if (numFilenames > numChildren)
    {
        osg::Camera* camera = findFirstParentOfType<osg::Camera>(object);
        osgViewer::View * viewptr = camera ? dynamic_cast<osgViewer::View*>(camera->getView()) : NULL;
        osg::NodePathList nodepaths = object->getParentalNodePaths();
        if (!nodepaths.empty() && viewptr)
        {
            osg::NodePath nodepath = nodepaths.front();
            const osgDB::DatabasePager * pager = viewptr->getDatabasePager();
            const osg::FrameStamp * fs = viewptr->getFrameStamp();
            osg::ref_ptr<osg::Referenced> opts = object->getDatabaseOptions();
            const std::string & databasePath = object->getDatabasePath();

            for (unsigned int i = numChildren; i < numFilenames; ++i)
            {
                const std::string & filename = object->getFileName(i);
                const float priority = 1.0f;
                const_cast<osgDB::DatabasePager*>(pager)->requestNodeFile(databasePath + filename, nodepath, priority, fs, object->getDatabaseRequest(i), opts);
                ret = true;
            }
        }
    }
    return ret;
}

bool actionHandlerImpl<MenuActionProxyNodeForceLoad>::execute()
{
    osg::ProxyNode * object = getObject<osg::ProxyNode, SGIItemOsg>();
    forceLoadingProxyNode(object);
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeReload>::execute()
{
    osg::ProxyNode * object = getObject<osg::ProxyNode, SGIItemOsg>();
    unsigned num = object->getNumFileNames();
    std::vector<std::string> filenames(num);
    for (unsigned i = 0; i < num; ++i)
        filenames[i] = object->getFileName(i);
    object->removeChildren(0, object->getNumChildren());
    for (unsigned i = 0; i < num; ++i)
        object->setFileName(i, filenames[i]);
    forceLoadingProxyNode(object);
    return true;
}

bool actionHandlerImpl<MenuActionProxyNodeSetDatabasePath>::execute()
{
    osg::PagedLOD* pagedlod = getObject<osg::PagedLOD,SGIItemOsg,DynamicCaster>();
    osg::ProxyNode * proxynode = getObject<osg::ProxyNode,SGIItemOsg,DynamicCaster>();

    bool ret;
    std::string value;
    if(pagedlod)
        value = pagedlod->getDatabasePath();
    else if(proxynode)
        value = proxynode->getDatabasePath();
    ret = _hostInterface->inputDialogString(menu()->parentWidget(),
                                            value,
                                            "Database path:", "Set database path",
                                            SGIPluginHostInterface::InputDialogStringEncodingSystem,
                                            _item
                                            );
    if(ret)
    {
        if(pagedlod)
            pagedlod->setDatabasePath(value);
        else if(proxynode)
            proxynode->setDatabasePath(value);
    }
    return true;
}

bool actionHandlerImpl<MenuActionPagedLODDisableExternalChildrenPaging>::execute()
{
    osg::PagedLOD* object = getObject<osg::PagedLOD,SGIItemOsg>();
    object->setDisableExternalChildrenPaging(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionPagedLODNumChildrenThatCannotBeExpired>::execute()
{
    osg::PagedLOD* object = getObject<osg::PagedLOD,SGIItemOsg>();
    int value = (int)object->getNumChildrenThatCannotBeExpired();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                            value,
                                            "Number:", "Set number of children that cannot be expired",
                                            std::numeric_limits<unsigned>::min(), std::numeric_limits<unsigned>::max(), 1,
                                            _item
                                            );
    if(ret)
        object->setNumChildrenThatCannotBeExpired((unsigned)value);
    return true;
}

bool actionHandlerImpl<MenuActionPagedLODFrameNumberOfLastTraversal>::execute()
{
    osg::PagedLOD* object = getObject<osg::PagedLOD,SGIItemOsg>();
    int value = (int)object->getFrameNumberOfLastTraversal();
    bool ret;
    ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                            value,
                                            "Frame number:", "Set frame number of last traversal",
                                            std::numeric_limits<unsigned>::min(), std::numeric_limits<unsigned>::max(), 1,
                                            _item
                                            );
    if(ret)
        object->setFrameNumberOfLastTraversal((unsigned)value);
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
    return openSettingsDialog(SettingsDialogCamera);
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

bool actionHandlerImpl<MenuActionCameraProjectionResizePolicy>::execute()
{
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();
    object->setProjectionResizePolicy((osg::Camera::ProjectionResizePolicy)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionCameraCullMask>::execute()
{
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();
    unsigned cullMask = object->getCullMask();
    bool ret;
    ret = _hostInterface->inputDialogBitmask(menu()->parentWidget(),
        cullMask,
        "Cull mask:", "Set cull mask",
        _item
    );
    if (ret)
        object->setCullMask(cullMask);
    return true;
}

bool actionHandlerImpl<MenuActionCameraViewMatrix>::execute()
{
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();
    Matrix matrix = osgMatrix(object->getViewMatrix());
    if(_hostInterface->inputDialogMatrix(menu()->parentWidget(), matrix, MatrixUsageView, "Matrix:", "Modify projection matrix", _item.get()))
    {
        object->setViewMatrix(osgMatrix(matrix));
    }
    return true;
}

bool actionHandlerImpl<MenuActionCameraProjectionMatrix>::execute()
{
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();
    Matrix matrix = osgMatrix(object->getProjectionMatrix());
    if(_hostInterface->inputDialogMatrix(menu()->parentWidget(), matrix, MatrixUsageProjection, "Matrix:", "Modify projection matrix"))
    {
        object->setProjectionMatrix(osgMatrix(matrix));
    }
    return true;
}

bool actionHandlerImpl<MenuActionCameraLiveView>::execute()
{
    return openSettingsDialog(SettingsDialogExtraView);
}

bool actionHandlerImpl<MenuActionUniformDirty>::execute()
{
    osg::Uniform * object = getObject<osg::Uniform, SGIItemOsg>();
    object->dirty();
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

#define writeDrawElementsDataImpl(__elem_type) \
    { \
        const __elem_type * d = (const __elem_type*)object->getDataPointer(); \
        for(unsigned n = 0; n < object->getNumElements(); n++) \
            os << d[n] << std::endl; \
    }

#define readDrawElementsDataImpl(__elem_type) \
    { \
        unsigned d = (__elem_type*)object->getElement(); \
        for(unsigned n = 0; n < tokens.size(); n++) \
        { \
            std::istringstream ss(tokens[n]); \
            ss >> d[n]; \
        } \
    }


bool actionHandlerImpl<MenuActionBufferDataEdit>::execute()
{
    osg::BufferData * object = getObject<osg::BufferData,SGIItemOsg>();
    osg::Array* array = object->asArray();
    osg::PrimitiveSet* primitiveSet = object->asPrimitiveSet();
    osg::Image* image = object->asImage();
    if (array)
    {
    }
    else if (primitiveSet)
    {
        std::stringstream os;

        osg::DrawElements* drawElements = primitiveSet->getDrawElements();
        if(drawElements)
        {
            unsigned maxNum = drawElements->getNumIndices();
            for(unsigned n = 0; n < maxNum; n++)
                os << drawElements->getElement(n) << std::endl;

            std::string value = os.str();
            bool ret;
            ret = _hostInterface->inputDialogText(menu()->parentWidget(),
                value,
                "Indices:", "Indices",
                SGIPluginHostInterface::InputDialogStringEncodingSystem,
                _item
                );
            if(ret)
            {
                std::istringstream iss(value);
                std::vector<std::string> tokens;
                split(value, '\n', tokens);
                if(tokens.size() != maxNum)
                    drawElements->resizeElements(tokens.size());

                for(unsigned n = 0; n < tokens.size(); n++)
                {
                    std::istringstream ss(tokens[n]);
                    unsigned idx = 0;
                    ss >> idx;
                    drawElements->setElement(n, idx);
                }
            }
        }
    }
    else if (image)
    {
        SGIHostItemOsg item(image);
        IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), &item);
        if (dialog.valid())
            dialog->show();
    }

    return true;
}

bool actionHandlerImpl<MenuActionBufferDirty>::execute()
{
    osg::BufferData * object = getObject<osg::BufferData,SGIItemOsg>();
    object->dirty();
    return true;
}

bool actionHandlerImpl<MenuActionBufferObjectDirty>::execute()
{
    osg::BufferObject * object = getObject<osg::BufferObject, SGIItemOsg>();
    object->dirty();
    return true;
}

#define writeArrayDataImpl(__elem_type) \
    { \
        const __elem_type * d = (const __elem_type*)object->getDataPointer(); \
        for(unsigned n = 0; n < object->getNumElements(); n++) \
            os << d[n] << std::endl; \
    }

#define readArrayDataImpl(__elem_type) \
    { \
        __elem_type * d = (__elem_type*)object->getDataPointer(); \
        for(unsigned n = 0; n < tokens.size(); n++) \
        { \
            std::istringstream ss(tokens[n]); \
            ss >> d[n]; \
        } \
    }

bool actionHandlerImpl<MenuActionArrayDataEdit>::execute()
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();

    std::stringstream os;
    switch(object->getType())
    {
    default:
    case osg::Array::ArrayType:
    case osg::Array::MatrixArrayType:
        os << "array type " << object->getType() << " not implemented";
        break;
    case osg::Array::ByteArrayType: writeArrayDataImpl(int8_t); break;
    case osg::Array::ShortArrayType: writeArrayDataImpl(int16_t); break;
    case osg::Array::IntArrayType: writeArrayDataImpl(int32_t); break;
    case osg::Array::UByteArrayType:  writeArrayDataImpl(uint8_t); break;
    case osg::Array::UShortArrayType:  writeArrayDataImpl(uint16_t); break;
    case osg::Array::UIntArrayType:  writeArrayDataImpl(uint32_t); break;
    case osg::Array::Vec4ubArrayType:  writeArrayDataImpl(osg::Vec4ub); break;
    case osg::Array::FloatArrayType: writeArrayDataImpl(float); break;
    case osg::Array::Vec2ArrayType: writeArrayDataImpl(osg::Vec2); break;
    case osg::Array::Vec3ArrayType: writeArrayDataImpl(osg::Vec3); break;
    case osg::Array::Vec4ArrayType: writeArrayDataImpl(osg::Vec4); break;
    case osg::Array::Vec2sArrayType: writeArrayDataImpl(osg::Vec2s); break;
    case osg::Array::Vec3sArrayType: writeArrayDataImpl(osg::Vec3s); break;
    case osg::Array::Vec4sArrayType: writeArrayDataImpl(osg::Vec4s); break;
    case osg::Array::Vec2bArrayType: writeArrayDataImpl(osg::Vec2b); break;
    case osg::Array::Vec3bArrayType: writeArrayDataImpl(osg::Vec3b); break;
    case osg::Array::Vec4bArrayType: writeArrayDataImpl(osg::Vec4b); break;
    case osg::Array::DoubleArrayType: writeArrayDataImpl(double); break;
    case osg::Array::Vec2dArrayType: writeArrayDataImpl(osg::Vec2d); break;
    case osg::Array::Vec3dArrayType: writeArrayDataImpl(osg::Vec3d); break;
    case osg::Array::Vec4dArrayType: writeArrayDataImpl(osg::Vec4d); break;
    }

    std::string value = os.str();
    bool ret;
    ret = _hostInterface->inputDialogText(menu()->parentWidget(),
        value,
        "Data:", "Data",
        SGIPluginHostInterface::InputDialogStringEncodingSystem,
        _item
        );
    if(ret)
    {
        std::istringstream iss(value);
        std::vector<std::string> tokens;
        split(value, '\n', tokens);
        if(tokens.size() != object->getNumElements())
            object->resizeArray(tokens.size());

        switch(object->getType())
        {
        default:
        case osg::Array::ArrayType:
        case osg::Array::MatrixArrayType:
            break;
        case osg::Array::ByteArrayType: readArrayDataImpl(int8_t); break;
        case osg::Array::ShortArrayType: readArrayDataImpl(int16_t); break;
        case osg::Array::IntArrayType: readArrayDataImpl(int32_t); break;
        case osg::Array::UByteArrayType:  readArrayDataImpl(uint8_t); break;
        case osg::Array::UShortArrayType:  readArrayDataImpl(uint16_t); break;
        case osg::Array::UIntArrayType:  readArrayDataImpl(uint32_t); break;
        case osg::Array::Vec4ubArrayType:  readArrayDataImpl(osg::Vec4ub); break;
        case osg::Array::FloatArrayType: readArrayDataImpl(float); break;
        case osg::Array::Vec2ArrayType: readArrayDataImpl(osg::Vec2); break;
        case osg::Array::Vec3ArrayType: readArrayDataImpl(osg::Vec3); break;
        case osg::Array::Vec4ArrayType: readArrayDataImpl(osg::Vec4); break;
        case osg::Array::Vec2sArrayType: readArrayDataImpl(osg::Vec2s); break;
        case osg::Array::Vec3sArrayType: readArrayDataImpl(osg::Vec3s); break;
        case osg::Array::Vec4sArrayType: readArrayDataImpl(osg::Vec4s); break;
        case osg::Array::Vec2bArrayType: readArrayDataImpl(osg::Vec2b); break;
        case osg::Array::Vec3bArrayType: readArrayDataImpl(osg::Vec3b); break;
        case osg::Array::Vec4bArrayType: readArrayDataImpl(osg::Vec4b); break;
        case osg::Array::DoubleArrayType: readArrayDataImpl(double); break;
        case osg::Array::Vec2dArrayType: readArrayDataImpl(osg::Vec2d); break;
        case osg::Array::Vec3dArrayType: readArrayDataImpl(osg::Vec3d); break;
        case osg::Array::Vec4dArrayType: readArrayDataImpl(osg::Vec4d); break;
        }
        object->dirty();
    }

    return true;
}

bool actionHandlerImpl<MenuActionArrayBinding>::execute()
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    object->setBinding((osg::Array::Binding)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionArrayNormalize>::execute()
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    object->setNormalize(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionArrayPreserveDataType>::execute()
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    object->setPreserveDataType(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionArrayTrim>::execute()
{
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    object->trim();
    return true;
}

bool actionHandlerImpl<MenuActionLineWidthSet>::execute()
{
    osg::LineWidth * object = getObject<osg::LineWidth,SGIItemOsg>();
    double value = object->getWidth();
    bool ok;
    ok = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                            value,
                                            "Width:", "Line width",
                                            0.0, 64.0, 1,
                                            _item
                                            );
    if(ok)
        object->setWidth(value);
    return true;
}

bool actionHandlerImpl<MenuActionLineStipplePattern>::execute()
{
    osg::LineStipple * object = getObject<osg::LineStipple,SGIItemOsg>();
    unsigned value = object->getPattern();
    bool ok;
    ok = _hostInterface->inputDialogBitmask(menu()->parentWidget(),
                                            value,
                                            "Pattern:", "Stipple pattern",
                                            _item
                                            );
    if(ok)
        object->setPattern(value);
    return true;
}

bool actionHandlerImpl<MenuActionLineStippleFactor>::execute()
{
    osg::LineStipple * object = getObject<osg::LineStipple,SGIItemOsg>();
    int value = object->getFactor();
    bool ok;
    ok = _hostInterface->inputDialogInteger(menu()->parentWidget(),
                                            value,
                                            "Factor:", "Stipple factor",
                                            1, 256, 1,
                                            _item
                                            );
    if(ok)
        object->setFactor(value);
    return true;
}

bool actionHandlerImpl<MenuActionLightModelColorControl>::execute()
{
    osg::LightModel * object = getObject<osg::LightModel, SGIItemOsg>();
    object->setColorControl((osg::LightModel::ColorControl)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionLightModelLocalViewer>::execute()
{
    osg::LightModel * object = getObject<osg::LightModel, SGIItemOsg>();
    object->setLocalViewer(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionLightModelTwoSided>::execute()
{
    osg::LightModel * object = getObject<osg::LightModel, SGIItemOsg>();
    object->setTwoSided(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionLightSourceReferenceFrame>::execute()
{
    osg::LightSource * object = getObject<osg::LightSource, SGIItemOsg>();
    object->setReferenceFrame((osg::LightSource::ReferenceFrame)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionLightModelAmbientIntensity>::execute()
{
    osg::LightModel * object = getObject<osg::LightModel, SGIItemOsg>();

    sgi::Color color = osgColor(object->getAmbientIntensity());
    if (_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Ambient intensity", "Set ambient intensity", _item))
    {
        object->setAmbientIntensity(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionBlendColorConstantColor>::execute()
{
    osg::BlendColor * object = getObject<osg::BlendColor, SGIItemOsg>();

    sgi::Color color = osgColor(object->getConstantColor());
    if (_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Constant color", "Set constant color", _item))
    {
        object->setConstantColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionPolygonModeFront>::execute()
{
    osg::PolygonMode * object = getObject<osg::PolygonMode, SGIItemOsg>();
    object->setMode(osg::PolygonMode::FRONT, (osg::PolygonMode::Mode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionPolygonModeBack>::execute()
{
    osg::PolygonMode * object = getObject<osg::PolygonMode, SGIItemOsg>();
    object->setMode(osg::PolygonMode::BACK, (osg::PolygonMode::Mode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionPolygonModeFrontAndBack>::execute()
{
    osg::PolygonMode * object = getObject<osg::PolygonMode, SGIItemOsg>();
    object->setMode(osg::PolygonMode::FRONT_AND_BACK, (osg::PolygonMode::Mode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionCullFaceMode>::execute()
{
    osg::CullFace * object = getObject<osg::CullFace, SGIItemOsg>();
    object->setMode((osg::CullFace::Mode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionMaterialColorMode>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    object->setColorMode((osg::Material::ColorMode)menuAction()->mode());
    return true;
}

namespace {
    static const char * getMaterialFaceName(osg::Material::Face face)
    {
        switch(face)
        {
        case osg::Material::FRONT: return "Front"; break;
        case osg::Material::BACK: return "Back"; break;
        case osg::Material::FRONT_AND_BACK: return "Front+Back"; break;
        default: return "Unknown"; break;
        }
        return NULL;
    }
}

bool actionHandlerImpl<MenuActionMaterialAmbient>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    osg::Material::Face face = (osg::Material::Face)userData->data();

    sgi::Color color = osgColor(object->getAmbient(face));
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color,
                                        helpers::str_plus_info("Ambient color", getMaterialFaceName(face)), "Set ambient color", _item))
    {
        object->setAmbient(face, osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionMaterialDiffuse>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    osg::Material::Face face = (osg::Material::Face)userData->data();

    sgi::Color color = osgColor(object->getDiffuse(face));
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color,
                                        helpers::str_plus_info("Diffuse color", getMaterialFaceName(face)), "Set diffuse color", _item))
    {
        object->setDiffuse(face, osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionMaterialSpecular>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    osg::Material::Face face = (osg::Material::Face)userData->data();

    sgi::Color color = osgColor(object->getSpecular(face));
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color,
                                        helpers::str_plus_info("Specular color", getMaterialFaceName(face)), "Set specular color", _item))
    {
        object->setSpecular(face, osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionMaterialEmission>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    osg::Material::Face face = (osg::Material::Face)userData->data();

    sgi::Color color = osgColor(object->getEmission(face));
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color,
                                        helpers::str_plus_info("Emission color", getMaterialFaceName(face)), "Set emission color", _item))
    {
        object->setEmission(face, osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionMaterialShininess>::execute()
{
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    const ReferencedDataInt * userData = static_cast<const ReferencedDataInt *>(menuAction()->userData());
    osg::Material::Face face = (osg::Material::Face)userData->data();

    double value = object->getShininess(face);
    bool ok;
    ok = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                            value,
                                            helpers::str_plus_info("Shininess", getMaterialFaceName(face)), "Set shininess",
                                            0.0, 128.0, 1,
                                            _item
                                            );
    if(ok)
    {
        object->setShininess(face, value);
        triggerRepaint();
    }
    return true;
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
    filters.push_back("Image files (*.png *.jpg *.bmp *.tiff *.dds)");
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
            object->dirtyTextureObject();
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionTextureDirtyTextureObject>::execute()
{
	osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
	object->dirtyTextureObject();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionTextureDirtyTextureParameters>::execute()
{
	osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
	object->dirtyTextureParameters();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionTextureAllocateMipmapLevels>::execute()
{
	osg::Texture * object = getObject<osg::Texture,SGIItemOsg>();
	object->allocateMipmapLevels();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionTexEnvMode>::execute()
{
    osg::TexEnv * object = getObject<osg::TexEnv, SGIItemOsg>();
    object->setMode((osg::TexEnv::Mode)menuAction()->mode());
    triggerRepaint();
    return true;
}

bool actionHandlerImpl<MenuActionTexEnvColor>::execute()
{
    osg::TexEnv * object = getObject<osg::TexEnv, SGIItemOsg>();
    sgi::Color color = osgColor(object->getColor());
    if (_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Color", "Select color", _item))
    {
        object->setColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionTexEnvFilterLodBias>::execute()
{
    osg::TexEnvFilter * object = getObject<osg::TexEnvFilter, SGIItemOsg>();
    double value = object->getLodBias();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set Lod bias", 0.0, 1000.0, 1, _item);
    if (gotInput)
    {
        object->setLodBias(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionDrawableUseDisplayList>::execute()
{
	osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
	object->setUseDisplayList(menuAction()->state());
	object->dirtyGLObjects();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionDrawableSupportsDisplayList>::execute()
{
	osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
	object->setSupportsDisplayList(menuAction()->state());
	object->dirtyGLObjects();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionDrawableDirtyGLObjects>::execute()
{
	osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
	object->dirtyGLObjects();
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionDrawableUseVAO>::execute()
{
    osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
    object->setUseVertexArrayObject(menuAction()->state());
    triggerRepaint();
    return true;
}

bool actionHandlerImpl<MenuActionDrawableUseVBO>::execute()
{
	osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
	object->setUseVertexBufferObjects(menuAction()->state());
	triggerRepaint();
	return true;
}

bool actionHandlerImpl<MenuActionDrawableRenderInfoDrawCallback>::execute()
{
    osg::Drawable * object = getObject<osg::Drawable, SGIItemOsg>();
    RenderInfo::installDrawCallback(object, menuAction()->state());
    triggerRepaint();
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
            object->dirtyGLObjects();
            triggerRepaint();
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionGeometryFixDeprecatedData>::execute()
{
    osg::Geometry * object = getObject<osg::Geometry, SGIItemOsg>();
    object->fixDeprecatedData();
    return true;
}

bool actionHandlerImpl<MenuActionImagePreview>::execute()
{
    bool ret = false;
    osg::Texture * texture = getObject<osg::Texture, SGIItemOsg, DynamicCaster>();
    if (texture)
    {
        ret = openSettingsDialog(SettingsDialogExtraView);
    }
    else
    {
        osg::Image * image = getObject<osg::Image, SGIItemOsg, DynamicCaster>();
        osg::HeightField * hf = getObject<osg::HeightField, SGIItemOsg, DynamicCaster>();
        IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());

        if (dialog.valid())
        {
            if (image)
                dialog->setObject(_item.get(), osg_helpers::convertImage(image), std::string(), hostCallback());
            else if (hf)
                dialog->setObject(_item.get(), osg_helpers::convertImage(hf), std::string(), hostCallback());
            else if (texture)
            {

                bool imageOk = false;
                osg::ref_ptr<const sgi::Image> textureImage;
                /*
                for (unsigned n = 0; n < texture->getNumImages() && !textureImage.valid(); ++n)
                {
                    osg::Image * txtimg = texture->getImage(n);
                    if(txtimg)
                    {
                        textureImage = osg_helpers::convertImage(txtimg);
                        if(textureImage.valid())
                            imageOk = true;
                    }
                }
                */
                if (!imageOk)
                {
                    osg::Camera * camera = findCamera(texture);
                    if (camera)
                    {
                        osg::ref_ptr<osg::Image> image;
                        if (convertTextureToImage(camera, texture, image))
                            textureImage = osg_helpers::convertImage(image);
                    }
                }
                dialog->setObject(_item.get(), textureImage, std::string(), hostCallback());
            }
            else
                dialog->setObject(_item.get(), nullptr, std::string(), hostCallback());
            dialog->show();
            ret = true;
        }
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
        newShape = new osg::Capsule;
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

bool actionHandlerImpl<MenuActionShapeDrawableBuild>::execute()
{
    osg::ShapeDrawable * object = getObject<osg::ShapeDrawable, SGIItemOsg>();
    object->build();
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

bool actionHandlerImpl<MenuActionDatabasePagerDatabaseThreads>::execute()
{
	osgDB::DatabasePager * object = getObject<osgDB::DatabasePager, SGIItemOsg>();
	int number = (int)object->getNumDatabaseThreads();

	int defaultNumThreads = OpenThreads::GetNumberOfProcessors();
	bool ret;
	ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
		number,
		"Number:", helpers::str_plus_count("Set number of database threads", defaultNumThreads),
		0, 64, 1,
		_item
		);
	if (ret)
		object->setUpThreads((unsigned)number, 0);
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

bool actionHandlerImpl<MenuActionDatabasePagerIncrementalCompileOperation>::execute()
{
	osgDB::DatabasePager * object = getObject<osgDB::DatabasePager, SGIItemOsg>();
	object->setIncrementalCompileOperation(new osgUtil::IncrementalCompileOperation);
	return true;
}

bool actionHandlerImpl<MenuActionCameraManipulatorAutoComputeHome>::execute()
{
    osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator,SGIItemOsg, DynamicCaster>();
    object->setAutoComputeHomePosition(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionShaderDirty>::execute()
{
    osg::Shader * object = getObject<osg::Shader, SGIItemOsg>();
    object->dirtyShader();
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

bool actionHandlerImpl<MenuActionViewerBaseMaxFrameRate>::execute()
{
	osgViewer::ViewerBase * object = getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>();
	double number = object->getRunMaxFrameRate();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Number:", "Set target maximum number of PageLODs",
		0, 120.0, 1,
		_item
		);
	if (ret)
		object->setRunMaxFrameRate(number);
	return true;
}

bool actionHandlerImpl<MenuActionViewerBaseRunFrameScheme>::execute()
{
	osgViewer::ViewerBase * object = getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>();
	object->setRunFrameScheme((osgViewer::ViewerBase::FrameScheme)menuAction()->mode());
	return true;
}

bool actionHandlerImpl<MenuActionViewerBaseThreadingModel>::execute()
{
    osgViewer::ViewerBase * object = getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>();
    object->setThreadingModel((osgViewer::ViewerBase::ThreadingModel)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionViewerBaseIncrementalCompileOperation>::execute()
{
	osgViewer::ViewerBase * object = getObject<osgViewer::ViewerBase, SGIItemOsg, DynamicCaster>();
	object->setIncrementalCompileOperation(new osgUtil::IncrementalCompileOperation);
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
                                             0.0, 100000.0, 1,
                                             _item
                                            );
    if(ret)
        object->setCharacterSize((float)number);
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

bool actionHandlerImpl<MenuActionTextBaseLayout>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase, SGIItemOsg>();
    object->setLayout((osgText::TextBase::Layout)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseSetFontWidth>::execute()
{
	osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
	double number = object->getFontWidth();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Font width:", "Set font width (resolution)",
		0.0, 100.0, 1,
		_item
		);
	if (ret)
		object->setFontResolution(number, object->getFontHeight());
	return true;
}

bool actionHandlerImpl<MenuActionTextBaseSetFontHeight>::execute()
{
	osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
	double number = object->getFontHeight();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Font height:", "Set font height (resolution)",
		0.0, 100.0, 1,
		_item
		);
	if (ret)
		object->setFontResolution(object->getFontWidth(), number);
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

bool actionHandlerImpl<MenuActionTextBaseSetMaximumWidth>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase, SGIItemOsg>();
    double number = object->getMaximumWidth();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
        number,
        "Width:", "Set maximum width",
        0.0, 1000.0, 1,
        _item
        );
    if (ret)
        object->setMaximumWidth(number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseSetMaximumHeight>::execute()
{
    osgText::TextBase * object = getObject<osgText::TextBase, SGIItemOsg>();
    double number = object->getMaximumHeight();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
        number,
        "Height:", "Set maximum height",
        0.0, 1000.0, 1,
        _item
        );
    if (ret)
        object->setMaximumHeight(number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBaseComputeGlyphRepresentation>::execute()
{
    osgTextBaseAccess * object = static_cast<osgTextBaseAccess*>(getObject<osgText::TextBase, SGIItemOsg>());
    object->forceComputeGlyphRepresentation();
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropEnableDepthWrites>::execute()
{
    osgText::Text * object = getObject<osgText::Text,SGIItemOsg>();
    object->setEnableDepthWrites(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropType>::execute()
{
    osgText::Text * object = getObject<osgText::Text,SGIItemOsg>();
    object->setBackdropType((osgText::Text::BackdropType)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropImplementation>::execute()
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
    object->setBackdropImplementation((osgText::Text::BackdropImplementation)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropHorizontalOffset>::execute()
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
    double number = object->getBackdropHorizontalOffset();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "Horizontal offset:", "Set backdrop horizontal offset",
                                             0.0, 100.0, 1,
                                             _item
                                            );
    if(ret)
        object->setBackdropOffset(number, object->getBackdropVerticalOffset());
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropVerticalOffset>::execute()
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
    double number = object->getBackdropVerticalOffset();
    bool ret;
    ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
                                             number,
                                             "Vertical offset:", "Set backdrop vertical offset",
                                             0.0, 100.0, 1,
                                             _item
                                            );
    if(ret)
        object->setBackdropOffset(object->getBackdropHorizontalOffset(), number);
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropColor>::execute()
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();

    sgi::Color color = osgColor(object->getBackdropColor());
    if(_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Backdrop color", "Select backdrop color", _item))
    {
        object->setBackdropColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionTextBackdropColorGradientMode>::execute()
{
    osgText::Text * object = getObject<osgText::Text, SGIItemOsg>();
    object->setColorGradientMode((osgText::Text::ColorGradientMode)menuAction()->mode());
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

bool actionHandlerImpl<MenuActionIncrementalCompileOperationTargetFrameRate>::execute()
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	double number = object->getTargetFrameRate();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Frame rate:", "Set target frame rate",
		0.0, 120.0, 1,
		_item
		);
	if (ret)
		object->setTargetFrameRate(number);
	return true;
}

bool actionHandlerImpl<MenuActionIncrementalCompileOperationMinimumTimeAvailableForGLCompileAndDeletePerFrame>::execute()
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	double number = object->getMinimumTimeAvailableForGLCompileAndDeletePerFrame();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Time:", "Set minimum time available for GL compile and delete per frame",
		0.0, 600.0, 1,
		_item
		);
	if (ret)
		object->setMinimumTimeAvailableForGLCompileAndDeletePerFrame(number);
	return true;
}

bool actionHandlerImpl<MenuActionIncrementalCompileOperationMaximumNumOfObjectsToCompilePerFrame>::execute()
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	int number = object->getMaximumNumOfObjectsToCompilePerFrame();
	bool ret;
	ret = _hostInterface->inputDialogInteger(menu()->parentWidget(),
		number,
		"Number:", "Set maximum number of objects to compile per frame",
		0, 1000, 1,
		_item
		);
	if (ret)
		object->setMaximumNumOfObjectsToCompilePerFrame(number);
	return true;
}

bool actionHandlerImpl<MenuActionIncrementalCompileOperationFlushTimeRatio>::execute()
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	double number = object->getFlushTimeRatio();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Time ratio:", "Set flush time ratio",
		0.0, 100.0, 1,
		_item
		);
	if (ret)
		object->setFlushTimeRatio(number);
	return true;
}

bool actionHandlerImpl<MenuActionIncrementalCompileOperationConservativeTimeRatio>::execute()
{
	osgUtil::IncrementalCompileOperation * object = getObject<osgUtil::IncrementalCompileOperation, SGIItemOsg, DynamicCaster>();
	double number = object->getConservativeTimeRatio();
	bool ret;
	ret = _hostInterface->inputDialogDouble(menu()->parentWidget(),
		number,
		"Time ratio:", "Set convervative time ratio",
		0.0, 100.0, 1,
		_item
		);
	if (ret)
		object->setConservativeTimeRatio(number);
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


class FindStateSetTraversalNodesVisitor : public osg::NodeVisitor
{
public:
    FindStateSetTraversalNodesVisitor(TraversalMode tm = TRAVERSE_ALL_CHILDREN)
        : osg::NodeVisitor(tm), _results(), _unique(true) {}

    FindStateSetTraversalNodesVisitor(VisitorType type, TraversalMode tm = TRAVERSE_ALL_CHILDREN)
        : osg::NodeVisitor(type, tm), _results(), _unique(true) {}

    void clear()
    {
        _results.clear();
    }

    typedef std::vector< osg::ref_ptr<osg::StateSet> > StateSetList;

    const StateSetList &   results() const
    {
        return _results;
    }

    virtual void apply(osg::Node& node)
    {
        if (node.getStateSet() != 0)
            foundStateSet(node.getStateSet());
        traverse(node);
    }

protected:
    void                    foundStateSet(osg::StateSet * ss)
    {
        if(!_unique)
            _results.push_back(ss);
        else
        {
            auto it = std::find(_results.begin(), _results.end(), ss);
            if (it == _results.end())
                _results.push_back(ss);
        }
    }
protected:
    StateSetList       _results;
    bool _unique;
};

bool actionHandlerImpl<MenuActionToolFindAllStateSets>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu) ? toolsMenu->getDialog() : NULL;
    IObjectTreeItem * selectedItem = (dialog) ? dialog->selectedItem() : NULL;
    SGIItemOsg * item = selectedItem ? dynamic_cast<SGIItemOsg *>(selectedItem->item()) : NULL;
    osg::Node * object = item ? dynamic_cast<osg::Node*>(item->object()) : NULL;

    if (object)
    {
        FindStateSetTraversalNodesVisitor visitor;
        object->accept(visitor);

        const auto & results = visitor.results();
        if (results.empty())
        {
            std::stringstream os;
            os << "<i>no state sets found</i><br/>" << std::endl;
            dialog->setInfoText(os.str());
        }
        else
        {
            selectedItem->expand();
            IObjectTreeItem * stateSetsItem = selectedItem->addChild("Found StateSets", (SGIItemBase*)NULL);

            unsigned num = 0;
            for (auto it = results.begin(); it != results.end(); num++, it++)
            {
                const osg::ref_ptr<osg::StateSet> & ss = *it;
                SGIHostItemOsg item(ss.get());
                stateSetsItem->addChild(std::string(), &item);
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

class FindClosestNodeVisitor : public osg::NodeVisitor
{
public:
    typedef std::map<osg::Node::NodeMask, osg::NodePathList> NodeMaskUsers;

public:
    FindClosestNodeVisitor(const osg::Camera * camera, bool onlyLODs, const osg::NodeVisitor::TraversalMode mode= osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
        : osg::NodeVisitor(mode) 
        , _onlyLODs(onlyLODs)
    {
        camera->getViewMatrixAsLookAt(_eye, _center, _up);
    }

    /** Get the eye point in local coordinates.
    * Note, not all NodeVisitor implement this method, it is mainly cull visitors which will implement.*/
    osg::Vec3 getEyePoint() const override { return _eye; }

    /** Get the distance from a point to the eye point, distance value in local coordinate system.
    * Note, not all NodeVisitor implement this method, it is mainly cull visitors which will implement.
    * If the getDistanceFromEyePoint(pos) is not implemented then a default value of 0.0 is returned.*/
    virtual float getDistanceToEyePoint(const osg::Vec3& pos, bool /*useLODScale*/) const override
    {
        return (pos - _eye).length();
    }

    /** Get the distance of a point from the eye point, distance value in the eye coordinate system.
    * Note, not all NodeVisitor implement this method, it is mainly cull visitors which will implement.
    * If the getDistanceFromEyePoint(pos) is not implemented than a default value of 0.0 is returned.*/
    virtual float getDistanceFromEyePoint(const osg::Vec3& pos, bool /*useLODScale*/) const override
    {
        return (_eye - pos).length();
    }

    /** Get the distance from a point to the view point, distance value in local coordinate system.
    * Note, not all NodeVisitor implement this method, it is mainly cull visitors which will implement.
    * If the getDistanceToViewPoint(pos) is not implemented then a default value of 0.0 is returned.*/
    virtual float getDistanceToViewPoint(const osg::Vec3& pos, bool /*useLODScale*/) const override
    {
        return (_eye - pos).length();
    }


    void clear()
    {
        _list.clear();
    }

    typedef std::pair<float, osg::NodePath> DistancedNodePath;
    typedef std::list<DistancedNodePath> DistancedNodePathList;

    const DistancedNodePathList & results() const
    {
        return _list;
    }

    static bool sort_by_distance(const DistancedNodePath& first, const DistancedNodePath& second)
    {
        return first.first < second.first;
    }

    const DistancedNodePathList & sorted_results()
    {
        _list.sort(sort_by_distance);
        return _list;
    }

    void apply(osg::Node& node) override
    {
        if (!_onlyLODs)
        {
            float distance = getDistanceToViewPoint(node.getBound().center(), true);
            DistancedNodePath item(distance, getNodePath());
            _list.push_back(item);
            traverse(node);
        }
    }
    void apply(osg::PagedLOD& node) override
    {
        if (_onlyLODs)
        {
            float distance = getDistanceToViewPoint(node.getCenter(), true);
            DistancedNodePath item(distance, getNodePath());
            _list.push_back(item);
            traverse(node);
        }
        else
            osg::NodeVisitor::apply(node);
    }
    void apply(osg::LOD& node) override
    {
        if (_onlyLODs)
        {
            float distance = getDistanceToViewPoint(node.getCenter(), true);
            DistancedNodePath item(distance, getNodePath());
            _list.push_back(item);
            traverse(node);
        }
        else
            osg::NodeVisitor::apply(node);
    }

private:
    osg::Vec3d _eye, _center, _up;
    DistancedNodePathList _list;
    bool _onlyLODs;
};

bool actionHandlerImpl<MenuActionToolFindClosestNodeToCamera>::execute()
{
    ISceneGraphDialogToolsMenu * toolsMenu = static_cast<ISceneGraphDialogToolsMenu*>(item<SGIItemInternal>()->object());
    ISceneGraphDialog * dialog = (toolsMenu) ? toolsMenu->getDialog() : NULL;
    IObjectTreeItem * selectedItem = (dialog) ? dialog->selectedItem() : NULL;
    SGIItemOsg * item = selectedItem ? dynamic_cast<SGIItemOsg *>(selectedItem->item()) : NULL;
    osg::Node * object = item ? dynamic_cast<osg::Node*>(item->object()) : NULL;
    if (object)
    {
        osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
        if (!camera)
            camera = findFirstParentOfType<osg::Camera>(object);
        if (camera)
        {
            FindClosestNodeVisitor fcnv(camera, true);
            object->accept(fcnv);

            selectedItem->expand();
            for (const FindClosestNodeVisitor::DistancedNodePath & nodepath : fcnv.sorted_results())
            {
                SGIHostItemOsg item(nodepath.second.back());
                std::string name;
                _hostInterface->getObjectDisplayName(name, &item);
                selectedItem->addChild(helpers::str_plus_number(name, nodepath.first), &item);
            }
        }
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


bool actionHandlerImpl<MenuActionViewLightingMode>::execute()
{
    osg::View * object = getObject<osg::View, SGIItemOsg, DynamicCaster>();
    object->setLightingMode((osg::View::LightingMode)menuAction()->mode());
    return true;
}

bool actionHandlerImpl<MenuActionViewCaptureScreenshot>::execute()
{
	SGIItemOsg * osgitem = static_cast<SGIItemOsg*>(_item.get());

    osgViewer::View * view = nullptr;
	osgViewer::ViewerBase * viewerbase = dynamic_cast<osgViewer::ViewerBase *>(osgitem->object());
    osg::Camera * camera = nullptr;
    osg::Camera * masterCamera = nullptr;
	if(!viewerbase)
	{
		view = dynamic_cast<osgViewer::View*>(osgitem->object());
		if(view)
			viewerbase = view->getViewerBase();
		else
        {
            camera = dynamic_cast<osg::Camera*>(osgitem->object());
            if (camera)
            {
                view = dynamic_cast<osgViewer::View*>(camera->getView());
                if(view)
                    viewerbase = view->getViewerBase();

                for (auto * parent : camera->getParents())
                {
                    osg::Camera * nextCamera = findFirstParentOfType<osg::Camera>(parent);
                    if (nextCamera && nextCamera->getNodeMask() != 0x0)
                    {
                        view = dynamic_cast<osgViewer::View*>(nextCamera->getView());
                        if (view)
                        {
                            masterCamera = nextCamera;
                            viewerbase = view->getViewerBase();
                            break;
                        }
                    }
                }
            }
        }
	}
	
    osg::ref_ptr<osg::Image> image;
    if (camera && (!view || !viewerbase))
    {
        captureCameraImage(camera, image, masterCamera);
    }
    else if(viewerbase)
	{
        if (!view)
        {
            osgViewer::ViewerBase::Views views;
            viewerbase->getViews(views);
            if (!views.empty())
                view = views.front();
        }
        captureViewImage(view, image);
	}
    if (image.valid())
    {
        IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());
        if (dialog.valid())
        {
            dialog->setObject(_item.get(), convertImage(image), std::string(), hostCallback());
            dialog->show();
        }
    }

	return true;
}

bool actionHandlerImpl<MenuActionViewPortModify>::execute()
{
    osg::Viewport * object = getObject<osg::Viewport, SGIItemOsg>();
    std::stringstream ss;
    ss << object->x() << ' ' << object->y() << ' ' << object->width() << ' ' << object->height();
    std::string str = ss.str();
    bool ret;
    ret = _hostInterface->inputDialogString(menu()->parentWidget(),
        str,
        "Viewport:", "Modify viewport",
        SGIPluginHostInterface::InputDialogStringEncodingSystem,
        _item
    );
    if (ret)
    {
        double x, y, w, h;
        std::stringstream ss(str);
        ss >> x >> y >> w >> h;
        object->setViewport(x, y, w, h);
    }
    return true;
}


} // namespace osg_plugin
} // namespace sgi
