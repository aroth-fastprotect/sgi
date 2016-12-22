#pragma once

#include <sgi/plugins/ActionHandlerImpl>
#include <osg/NodeVisitor>
#include <osg/Transform>

namespace sgi {
namespace osg_plugin {

enum MenuActionAddShapeType {
    MenuActionAddShapeInvalid = -1,
    MenuActionAddShapeSphere,
    MenuActionAddShapeBox,
    MenuActionAddShapeCone,
    MenuActionAddShapeCylinder,
    MenuActionAddShapeCapsule
};

enum MenuActionAddChildType {
    MenuActionAddChildInvalid = -1,
    MenuActionAddChildAxis,
    MenuActionAddChildBillboardAxis,
    MenuActionAddChildBoundingBox
};

enum MenuActionOptimizerRunMode {
	MenuActionOptimizerRunModeInvalid = -1,
	MenuActionOptimizerRunModeDefault,
	MenuActionOptimizerRunModeAll,
	MenuActionOptimizerRunModeCheck,
	MenuActionOptimizerRunModeFastGeometry,
};

enum MenuAction {
    MenuActionNone = -1,
    MenuActionObjectInfo,
    MenuActionObjectSave,
    MenuActionObjectSetName,
    MenuActionObjectDataVariance,
    MenuActionNotifyLevel,
    MenuActionNodeMask,
    MenuActionNodeMaskAndChilds,
    MenuActionNodeNumUpdateTraversal,
    MenuActionNodeNumEventTraversal,
    MenuActionNodeRecomputeBound,
    MenuActionNodeCullingActive,
    MenuActionNodeLookAt,
    MenuActionNodeCreateStateSet,
    MenuActionNodeStripTextures,
    MenuActionNodeSmoothingVisitor,
	MenuActionNodeOptimizerRun,
    MenuActionNodeToggleCenterMarker,
    MenuActionNodeRenderInfo,
    MenuActionObjectLogger,
    MenuActionObjectLoggerVisible,
    MenuActionObjectLoggerActive,
    MenuActionGroupAddChild,
    MenuActionGroupRemoveChild,
    MenuActionStateSetRenderHint,
    MenuActionStateSetRenderBinName,
    MenuActionStateSetRenderBinNumber,
    MenuActionStateSetRenderBinMode,
    MenuActionStateSetTextureAttributeSet,
    MenuActionStateSetTextureAttributeRemove,
    MenuActionStateSetTextureModeSet,
    MenuActionStateSetTextureModeRemove,
    MenuActionStateSetModeSet,
    MenuActionStateSetModeRemove,
    MenuActionStateSetAttributeAdd,
    MenuActionStateSetAttributeSet,
    MenuActionStateSetAttributeRemove,
    MenuActionStateSetUniformAdd,
    MenuActionStateSetUniformSet,
    MenuActionStateSetUniformRemove,
    MenuActionStateSetDefineAdd,
    MenuActionStateSetDefineSet,
    MenuActionStateSetDefineEdit,
    MenuActionStateSetDefineRemove,
    MenuActionStateSetClear,
    MenuActionProgramAddShader,
    MenuActionGeodeAddShapeDrawable,
    MenuActionShapeCenter,
    MenuActionShapeRotation,
    MenuActionShapeBoxHalfLength,
    MenuActionDrawableToggleDisabled,
    MenuActionTransformReferenceFrame,
    MenuActionMatrixTransformEdit,
    MenuActionPositionAttitudeTransformEdit,
    MenuActionAutoTransformAutoScaleToScreen,
    MenuActionAutoTransformAutoRotateMode,
    MenuActionAutoTransformSetAxis,
    MenuActionAutoTransformSetNormal,
    MenuActionAutoTransformSetScale,
    MenuActionAutoTransformSetPivotPoint,
    MenuActionAutoTransformSetMinimumScale,
    MenuActionAutoTransformSetMaximumScale,
    MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance,
    MenuActionAutoTransformSetAutoScaleTransitionWidthRatio,
    MenuActionBillboardMode,
    MenuActionCameraClearColor,
    MenuActionCameraCullSettings,
    MenuActionCameraComputeNearFarMode,
    MenuActionCameraViewMatrix, 
    MenuActionCameraProjectionMatrix,
    MenuActionCameraLiveView,
    MenuActionProxyNodeSetCenterMode,
    MenuActionProxyNodeSetCenter,
    MenuActionProxyNodeSetRadius,
    MenuActionProxyNodeLoadingExternalReferenceMode,
    MenuActionProxyNodeForceLoad,
    MenuActionProxyNodeReload,
    MenuActionProxyNodeSetDatabasePath,
    MenuActionLODSetRangeMode,
    MenuActionPagedLODDisableExternalChildrenPaging,
    MenuActionPagedLODNumChildrenThatCannotBeExpired,
    MenuActionPagedLODFrameNumberOfLastTraversal,
	MenuActionDrawableUseDisplayList,
	MenuActionDrawableSupportsDisplayList,
	MenuActionDrawableDirtyDisplayList,
	MenuActionDrawableUseVBO,
    MenuActionDrawableRenderInfoDrawCallback,
	MenuActionShapeDrawableColor,
    MenuActionGeometryColor,
    MenuActionImagePreview,
    MenuActionTextureBorderWidth,
    MenuActionTextureBorderColor,
    MenuActionTextureSetMinFilter,
    MenuActionTextureSetMagFilter,
    MenuActionTextureSetResizeNonPowerOfTwoHint,
    MenuActionTextureUseHardwareMipMapGeneration,
    MenuActionTextureUnRefImageDataAfterApply,
    MenuActionTextureSetImage,
	MenuActionTextureDirtyTextureObject,
	MenuActionTextureDirtyTextureParameters,
	MenuActionTextureAllocateMipmapLevels,
    MenuActionClipNodeReset,
    MenuActionClipNodeSetState,
    MenuActionGraphicsWindowHitTest,
    MenuActionDatabasePagerPause,
	MenuActionDatabasePagerDatabaseThreads,
    MenuActionDatabasePagerAcceptNewRequests,
    MenuActionDatabasePagerDoPreCompile,
    MenuActionDatabasePagerDeleteSubgraphsInDBThread,
    MenuActionDatabasePagerTargetPageLODNumber,
	MenuActionDatabasePagerIncrementalCompileOperation,
    MenuActionEventAdapterHitTest,
    MenuActionEventHandlerIgnoreHandledEventsMask,
    MenuActionCameraManipulatorAutoComputeHome,

    MenuActionShaderDirty,
    MenuActionShaderEditSource,
    MenuActionDepthFunction,
    MenuActionDepthWriteMask,
    MenuActionDepthSetZNear,
    MenuActionDepthSetZFar,

    MenuActionUniformDirty,
    MenuActionUniformEdit,
    MenuActionArrayDataEdit,
    MenuActionArrayBinding,
    MenuActionArrayNormalize,
    MenuActionArrayPreserveDataType,
    MenuActionArrayTrim,
    MenuActionBufferDataEdit,
    MenuActionBufferDirty,
    MenuActionBufferObjectDirty,
    MenuActionLineWidthSet,
    MenuActionLineStipplePattern,
    MenuActionLineStippleFactor,
    MenuActionLightModelColorControl,
    MenuActionLightModelLocalViewer,
    MenuActionLightModelTwoSided,
    MenuActionLightModelAmbientIntensity,
    MenuActionBlendColorConstantColor,
    MenuActionPolygonModeFront,
    MenuActionPolygonModeBack,
    MenuActionPolygonModeFrontAndBack,

    MenuActionMaterialColorMode,
    MenuActionMaterialAmbient,
    MenuActionMaterialDiffuse,
    MenuActionMaterialSpecular,
    MenuActionMaterialEmission,
    MenuActionMaterialShininess,

    MenuActionTextBaseAutoRotateToScreen,
    MenuActionTextBaseCharacterHeight,
    MenuActionTextBaseCharacterAspectRatio,
    MenuActionTextBaseCharacterSizeMode,
	MenuActionTextBaseSetFontWidth,
	MenuActionTextBaseSetFontHeight,
    MenuActionTextBaseModifyText,
    MenuActionTextBaseDrawMode,
    MenuActionTextBaseAxisAlignment,
    MenuActionTextBaseSetMaximumWidth,
    MenuActionTextBaseSetMaximumHeight,
    MenuActionTextBackdropEnableDepthWrites,
    MenuActionTextBackdropImplementation,
    MenuActionTextBackdropType,
	MenuActionTextBackdropHorizontalOffset,
    MenuActionTextBackdropVerticalOffset,
    MenuActionTextBackdropColor,
    MenuActionTextBackdropColorGradientMode,

    MenuActionAnimationManagerBaseAutomaticLink,
    MenuActionAnimationManagerBaseDirty,

    MenuActionViewLightingMode,
	MenuActionViewCaptureScreenshot,
    MenuActionViewPortModify,

	MenuActionViewerBaseMaxFrameRate,
	MenuActionViewerBaseRunFrameScheme,
    MenuActionViewerBaseThreadingModel,
	MenuActionViewerBaseIncrementalCompileOperation,

	MenuActionIncrementalCompileOperationTargetFrameRate,
	MenuActionIncrementalCompileOperationMinimumTimeAvailableForGLCompileAndDeletePerFrame,
	MenuActionIncrementalCompileOperationMaximumNumOfObjectsToCompilePerFrame,
	MenuActionIncrementalCompileOperationFlushTimeRatio,
	MenuActionIncrementalCompileOperationConservativeTimeRatio,

    MenuActionToolFindUpdateNodes,
    MenuActionToolFindEventNodes,
    MenuActionToolFindNaNNodes,
    MenuActionToolFindVisibleNodes,
    MenuActionToolListNodeMasks,
    MenuActionToolFindPagedLODChilds,
    MenuActionToolEffectiveStateSet,
    MenuActionToolFindCamera,
    MenuActionToolFindView,
    MenuActionToolDistanceToCamera,
    MenuActionToolFindAllStateSets,
    MenuActionToolFindClosestNodeToCamera,

    MenuActionTexEnvMode,
    MenuActionTexEnvColor,
    MenuActionTexEnvFilterLodBias,

};

struct TextureAttributePair {
    TextureAttributePair(unsigned u, osg::StateAttribute * a)
        : textureUnit(u), attr(a) {}
    unsigned textureUnit;
    osg::ref_ptr<osg::StateAttribute> attr;
};
typedef ReferencedDataT<TextureAttributePair> ReferencedDataTextureAttributePair;

ACTION_HANDLER_IMPL_TEMPLATE()

class ComputeLocalToWorldMatrixVisitor : public osg::NodeVisitor
{
public:
    ComputeLocalToWorldMatrixVisitor():
        osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
    {
    }

public:
    virtual void apply(osg::Node &node)
    {
        if (!done)
        {
            if ( 0 == node.getNumParents() ) // no parents
            {
                _matrix.set( osg::computeLocalToWorld(this->getNodePath()) );
                done = true;
            }
            traverse(node);
        }
    }
    osg::Matrix & matrix()
    {
        return _matrix;
    }
    const osg::Matrix & matrix() const
    {
        return _matrix;
    }

private:
    bool done;
    osg::Matrix _matrix;
};

} // namespace osg_plugin
} // namespace sgi
