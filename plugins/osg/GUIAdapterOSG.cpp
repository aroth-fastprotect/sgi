#include "GUIAdapterOSG.h"
#include "MenuActionOSG.h"
#include "SGIItemOsg.h"
#include <osgViewer/View>

namespace sgi {
namespace osg_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_REGISTER(osg::Camera)
GUI_ADAPTER_SET_VIEW_IMPL_REGISTER(osg::View)

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

bool guiAdapterSetViewImpl<osg::Camera>::execute()
{
    bool ret = false;
    osg::Camera * object = getObject<osg::Camera, SGIItemOsg>();

    const osg::Matrixd& currentViewMatrix = object->getViewMatrix();
    osg::Vec3d currentEye, currentCenter, currentUp;
    currentViewMatrix.getLookAt(currentEye, currentCenter, currentUp);

    osg::Vec3d lookVector = (currentCenter - currentEye);

    osg::Vec3d newEye;
    osg::Matrixd newViewMatrix;
    const ReferencedSetViewNodeLookAt * refLookAt = getTargetObject<ReferencedSetViewNodeLookAt, SGIItemOsg>();
    if(refLookAt)
    {
        const SetViewNodeLookAt & lookAt = refLookAt->data();
        osg::BoundingSphere bs = lookAt.node->getBound();
        ComputeLocalToWorldMatrixVisitor cl2wv;
        const osg::Matrix & l2w = cl2wv.matrix();
        osg::Vec3d newCenter = bs.center() * l2w;
        float lookDistance = bs.radius();

        switch(lookAt.mode)
        {
        default:
        case NodeLookAtDirect:
            newEye = newCenter;
            newCenter = newCenter + lookVector;
            newViewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
            break;
        case NodeLookAtTopView:
            newEye = newCenter + (currentUp * lookDistance);
            newViewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
            break;
        case NodeLookAtFromBehind:
            newEye = newCenter - (lookVector * lookDistance);
            newViewMatrix = osg::Matrixd::lookAt(newEye, newCenter, currentUp);
            break;
        }
        ret = true;
    }

    if(ret)
        object->setViewMatrix(newViewMatrix);

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

} // namespace osg_plugin
} // namespace sgi
