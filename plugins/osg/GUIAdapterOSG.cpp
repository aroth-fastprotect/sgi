#include "stdafx.h"
#include "GUIAdapterOSG.h"
#include "MenuActionOSG.h"
#include "SGIItemOsg"
#include <sgi/plugins/SGIHostItemOsg.h>
#include <osgViewer/View>
#include <osgGA/CameraManipulator>

namespace sgi {
namespace osg_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(osg::Camera)
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(osg::View)
GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(osgGA::CameraManipulator)

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

	static bool computeViewMatrix(osg::Matrixd & viewMatrix, const SetViewNodeLookAt & lookAt)
	{
		osg::BoundingSphere bs = lookAt.node->getBound();
		ComputeLocalToWorldMatrixVisitor cl2wv;
		lookAt.node->accept(cl2wv);
		const osg::Matrix & l2w = cl2wv.matrix();
		osg::Vec3d newCenter = bs.center() * l2w;
		float lookDistance = bs.radius();

		osg::Vec3d currentEye, currentCenter, currentUp;
		viewMatrix.getLookAt(currentEye, currentCenter, currentUp);

		osg::Vec3d lookVector = (currentEye - currentCenter);
		osg::Vec3d newEye = currentEye;

		switch (lookAt.mode)
		{
		default:
		case NodeLookAtDirect:
			newEye = newCenter;
			newCenter = newCenter + lookVector;
			viewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
			break;
		case NodeLookAtTopView:
			newEye = newCenter + (currentUp * lookDistance);
			viewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
			break;
		case NodeLookAtFromBehind:
			newEye = newCenter - (lookVector * lookDistance);
			viewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
			break;
		}
		return true;

	}
}

bool guiAdapterSetViewImpl<osg::Camera>::execute()
{
    bool ret = false;
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();

    const ReferencedSetViewNodeLookAt * refLookAt = getTargetObject<ReferencedSetViewNodeLookAt, SGIItemOsg>();
    if(refLookAt)
    {
        const SetViewNodeLookAt & lookAt = refLookAt->data();
		osg::Matrixd viewMatrix = object->getViewMatrix();
		ret = computeViewMatrix(viewMatrix, lookAt);
		if (ret)
			object->setViewMatrix(viewMatrix);
	}


    return ret;
}

bool guiAdapterSetViewImpl<osg::View>::execute()
{
    bool ret = false;
    osg::View * object = getObject<osg::View, SGIItemOsg, DynamicCaster>();

    osgViewer::View* viewer_object = dynamic_cast<osgViewer::View*>(object);
    if(viewer_object)
    {
        SGIHostItemOsg manipulator(viewer_object->getCameraManipulator());
        ret = _hostInterface->setView(&manipulator, _targetItem, _animationTime);
    }

    if(!ret)
    {
        SGIHostItemOsg camera(object->getCamera());
        ret = _hostInterface->setView(&camera, _targetItem, _animationTime);
    }
    if(ret)
    {
        if(viewer_object)
            viewer_object->requestRedraw();
    }
    return ret;
}


bool guiAdapterSetViewImpl<osgGA::CameraManipulator>::execute()
{
	bool ret = false;
	osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator, SGIItemOsg, DynamicCaster>();

	const ReferencedSetViewNodeLookAt * refLookAt = getTargetObject<ReferencedSetViewNodeLookAt, SGIItemOsg>();
	if (refLookAt)
	{
		const SetViewNodeLookAt & lookAt = refLookAt->data();
		osg::Matrixd viewMatrix = object->getMatrix();
		ret = computeViewMatrix(viewMatrix, lookAt);
		if (ret)
			object->setByMatrix(viewMatrix);
	}

	return ret;
}

} // namespace osg_plugin
} // namespace sgi
