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
	MenuActionNodeOptimizerRun,
    MenuActionObjectLogger,
    MenuActionObjectLoggerVisible,
    MenuActionObjectLoggerActive,
    MenuActionGroupAddChild,
    MenuActionGroupRemoveChild,
    MenuActionStateSetRenderHint,
    MenuActionStateSetRenderBinName,
    MenuActionStateSetRenderBinNumber,
    MenuActionStateSetRenderBinMode,
    MenuActionStateSetTextureAttributeList,
    MenuActionStateSetMode,
    MenuActionStateSetAddUniform,
    MenuActionStateSetAddAttribute,
    MenuActionStateSetAttributeValue,
    MenuActionStateSetAttributeDelete,
    MenuActionStateSetUniformValue,
    MenuActionStateSetUniformDelete,
    MenuActionStateSetClear,
    MenuActionProgramAddShader,
    MenuActionGeodeAddShapeDrawable,
    MenuActionGeodeRenderInfoDrawable,
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
    MenuActionProxyNodeSetCenterMode,
    MenuActionProxyNodeSetCenter,
    MenuActionProxyNodeSetRadius,
    MenuActionProxyNodeLoadingExternalReferenceMode,
    MenuActionProxyNodeForceLoad,
    MenuActionProxyNodeSetDatabasePath,
    MenuActionLODSetRangeMode,
    MenuActionPagedLODDisableExternalChildrenPaging,
    MenuActionPagedLODNumChildrenThatCannotBeExpired,
    MenuActionPagedLODFrameNumberOfLastTraversal,
	MenuActionDrawableUseDisplayList,
	MenuActionDrawableSupportsDisplayList,
	MenuActionDrawableDirtyDisplayList,
	MenuActionDrawableUseVBO,
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

    MenuActionShaderEditSource,
    MenuActionDepthFunction,
    MenuActionDepthWriteMask,
    MenuActionDepthSetZNear,
    MenuActionDepthSetZFar,

    MenuActionUniformEdit,
    MenuActionArrayDataEdit,
    MenuActionBufferDataEdit,
    MenuActionBufferDirty,
    MenuActionLineWidthSet,
    MenuActionLineStipplePattern,
    MenuActionLineStippleFactor,
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
    MenuActionTextBackdropEnableDepthWrites,
    MenuActionTextBackdropImplementation,
    MenuActionTextBackdropType,
	MenuActionTextBackdropHorizontalOffset,
    MenuActionTextBackdropVerticalOffset,
    MenuActionTextBackdropColor,
    MenuActionTextBackdropColorGradientMode,

    MenuActionAnimationManagerBaseAutomaticLink,
    MenuActionAnimationManagerBaseDirty,

	MenuActionViewCaptureScreenshot,

	MenuActionViewerBaseMaxFrameRate,
	MenuActionViewerBaseMaxRunFrameScheme,
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
};

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectSetName)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectSave)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectDataVariance)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeMask)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeMaskAndChilds)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeNumUpdateTraversal)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeNumEventTraversal)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeRecomputeBound)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeCullingActive)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeLookAt)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeCreateStateSet)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeStripTextures)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeOptimizerRun)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectLogger)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectLoggerVisible)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectLoggerActive)

ACTION_HANDLER_IMPL_DECLARE(MenuActionGroupAddChild)
ACTION_HANDLER_IMPL_DECLARE(MenuActionGroupRemoveChild)

ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetRenderHint)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetRenderBinName)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetRenderBinNumber)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetRenderBinMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetTextureAttributeList)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetAddUniform)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetAddAttribute)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetAttributeValue)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetAttributeDelete)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetUniformValue)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetUniformDelete)
ACTION_HANDLER_IMPL_DECLARE(MenuActionStateSetClear)

ACTION_HANDLER_IMPL_DECLARE(MenuActionProgramAddShader)

ACTION_HANDLER_IMPL_DECLARE(MenuActionCameraCullSettings)
ACTION_HANDLER_IMPL_DECLARE(MenuActionCameraClearColor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionCameraComputeNearFarMode)

ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeSetCenterMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeSetCenter)
ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeSetRadius)
ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeLoadingExternalReferenceMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeForceLoad)
ACTION_HANDLER_IMPL_DECLARE(MenuActionProxyNodeSetDatabasePath)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLODSetRangeMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionPagedLODDisableExternalChildrenPaging)
ACTION_HANDLER_IMPL_DECLARE(MenuActionPagedLODNumChildrenThatCannotBeExpired)
ACTION_HANDLER_IMPL_DECLARE(MenuActionPagedLODFrameNumberOfLastTraversal)

ACTION_HANDLER_IMPL_DECLARE(MenuActionClipNodeReset)
ACTION_HANDLER_IMPL_DECLARE(MenuActionClipNodeSetState)

ACTION_HANDLER_IMPL_DECLARE(MenuActionUniformEdit)
ACTION_HANDLER_IMPL_DECLARE(MenuActionBufferDataEdit)
ACTION_HANDLER_IMPL_DECLARE(MenuActionBufferDirty)
ACTION_HANDLER_IMPL_DECLARE(MenuActionArrayDataEdit)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLineWidthSet)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLineStipplePattern)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLineStippleFactor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialColorMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialAmbient)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialDiffuse)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialSpecular)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialEmission)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaterialShininess)

ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureBorderColor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureBorderWidth)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureSetMinFilter)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureSetMagFilter)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureSetResizeNonPowerOfTwoHint)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureUseHardwareMipMapGeneration)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureUnRefImageDataAfterApply)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureSetImage)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureDirtyTextureObject)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureDirtyTextureParameters)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextureAllocateMipmapLevels)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDrawableToggleDisabled)
ACTION_HANDLER_IMPL_DECLARE(MenuActionGeodeAddShapeDrawable)
ACTION_HANDLER_IMPL_DECLARE(MenuActionGeodeRenderInfoDrawable)
ACTION_HANDLER_IMPL_DECLARE(MenuActionShapeDrawableColor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionShapeCenter)
ACTION_HANDLER_IMPL_DECLARE(MenuActionShapeRotation)
ACTION_HANDLER_IMPL_DECLARE(MenuActionShapeBoxHalfLength)
ACTION_HANDLER_IMPL_DECLARE(MenuActionGeometryColor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDrawableUseDisplayList)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDrawableSupportsDisplayList)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDrawableDirtyDisplayList)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDrawableUseVBO)

ACTION_HANDLER_IMPL_DECLARE(MenuActionImagePreview)
ACTION_HANDLER_IMPL_DECLARE(MenuActionBillboardMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTransformReferenceFrame)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMatrixTransformEdit)
ACTION_HANDLER_IMPL_DECLARE(MenuActionPositionAttitudeTransformEdit)

ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformAutoScaleToScreen)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformAutoRotateMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetAxis)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetNormal)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetScale)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetPivotPoint)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetMinimumScale)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetMaximumScale)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetAutoUpdateEyeMovementTolerance)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoTransformSetAutoScaleTransitionWidthRatio)

ACTION_HANDLER_IMPL_DECLARE(MenuActionCameraManipulatorAutoComputeHome)

ACTION_HANDLER_IMPL_DECLARE(MenuActionViewerBaseMaxFrameRate)
ACTION_HANDLER_IMPL_DECLARE(MenuActionViewerBaseMaxRunFrameScheme)
ACTION_HANDLER_IMPL_DECLARE(MenuActionViewerBaseIncrementalCompileOperation)

ACTION_HANDLER_IMPL_DECLARE(MenuActionShaderEditSource)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDepthFunction)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDepthWriteMask)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDepthSetZNear)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDepthSetZFar)

ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseAutoRotateToScreen)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseCharacterHeight)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseCharacterAspectRatio)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseCharacterSizeMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseSetFontWidth)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseSetFontHeight)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseModifyText)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseDrawMode)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBaseAxisAlignment)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropEnableDepthWrites)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropType)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropImplementation)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropHorizontalOffset)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropVerticalOffset)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropColor)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTextBackdropColorGradientMode)

ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerPause)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerDatabaseThreads)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerAcceptNewRequests)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerDoPreCompile)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerDeleteSubgraphsInDBThread)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerTargetPageLODNumber)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDatabasePagerIncrementalCompileOperation)

ACTION_HANDLER_IMPL_DECLARE(MenuActionAnimationManagerBaseAutomaticLink)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAnimationManagerBaseDirty)

ACTION_HANDLER_IMPL_DECLARE(MenuActionIncrementalCompileOperationTargetFrameRate)
ACTION_HANDLER_IMPL_DECLARE(MenuActionIncrementalCompileOperationMinimumTimeAvailableForGLCompileAndDeletePerFrame)
ACTION_HANDLER_IMPL_DECLARE(MenuActionIncrementalCompileOperationMaximumNumOfObjectsToCompilePerFrame)
ACTION_HANDLER_IMPL_DECLARE(MenuActionIncrementalCompileOperationFlushTimeRatio)
ACTION_HANDLER_IMPL_DECLARE(MenuActionIncrementalCompileOperationConservativeTimeRatio)

ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindUpdateNodes)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindEventNodes)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindNaNNodes)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindVisibleNodes)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolListNodeMasks)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindPagedLODChilds)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolEffectiveStateSet)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindCamera)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolFindView)
ACTION_HANDLER_IMPL_DECLARE(MenuActionToolDistanceToCamera)

ACTION_HANDLER_IMPL_DECLARE(MenuActionViewCaptureScreenshot)

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
