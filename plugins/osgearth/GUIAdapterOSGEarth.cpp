#include "stdafx.h"
#include "GUIAdapterOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>

#include "ContextMenuOSGEarth.h"

#include <osgEarth/Registry>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/EarthManipulator>
#else
#include <osgEarthUtil/EarthManipulator>
#endif
#include <sgi/helpers/osg>

#include "../osg/MenuActionOSG.h"

namespace sgi {
namespace osgearth_plugin {

GUI_ADAPTER_SET_VIEW_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::EarthManipulator)

namespace {
const osg::EllipsoidModel & getDefaultEllipsoid()
{
    static osg::EllipsoidModel s_default;
    return s_default;
}

osgEarth::GeoPoint fromXYZToGpsCoordinate(const osg::EllipsoidModel & ellipsoid, const osg::Vec3d & v)
{
    const osgEarth::SpatialReference * srs = osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS();

    osgEarth::GeoPoint ret(srs, v, osgEarth::ALTMODE_ABSOLUTE);
    ellipsoid.convertXYZToLatLongHeight(v.x(), v.y(), v.z(), ret.y(), ret.x(), ret.z());
    ret.x() = osg::RadiansToDegrees(ret.x());
    ret.y() = osg::RadiansToDegrees(ret.y());
    return ret;
}

osgEarth::GeoPoint fromXYZToGpsCoordinate(const osg::Vec3d & v)
{
    return fromXYZToGpsCoordinate(getDefaultEllipsoid(), v);
}

osgEarth::Viewpoint computeViewpointForNode(const osg::Camera * viewCamera, osg::Node * targetNode, osg_plugin::NodeLookAt mode)
{
    osgEarth::Viewpoint ret;
    const osg::Matrixd& currentViewMatrix = viewCamera->getViewMatrix();
    osg::Vec3d currentEye, currentCenter, currentUp;
    currentViewMatrix.getLookAt(currentEye, currentCenter, currentUp);

    osg::Vec3d lookVector = (currentCenter - currentEye);

    osg::Vec3d newEye;

    osg::BoundingSphere bs = targetNode->getBound();
    osg_plugin::ComputeLocalToWorldMatrixVisitor cl2wv;
    const osg::Matrix & l2w = cl2wv.matrix();
    osg::Vec3d newCenter = bs.center() * l2w;
    float lookDistance = bs.radius();

    //const osgEarth::SpatialReference * srs_ecef = osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS()->getGeocentricSRS();
    const osgEarth::SpatialReference * srs_ecef = osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS();

    switch (mode)
    {
    default:
    case osg_plugin::NodeLookAtDirect:
        newEye = newCenter;
        newCenter = newCenter + lookVector;
        ret.range() = osgEarth::Distance(lookDistance, osgEarth::Units::METERS);
        ret.pitch() = osgEarth::Angle(-90.0, osgEarth::Units::DEGREES);
        ret.focalPoint() = fromXYZToGpsCoordinate(newCenter);
        break;
    case osg_plugin::NodeLookAtTopView:
        ret.range() = osgEarth::Distance(lookDistance, osgEarth::Units::METERS);
        ret.pitch() = osgEarth::Angle(-90.0, osgEarth::Units::DEGREES);
        ret.focalPoint() = fromXYZToGpsCoordinate(newCenter);
        break;
    case osg_plugin::NodeLookAtFromBehind:
        ret.range() = osgEarth::Distance(lookDistance, osgEarth::Units::METERS);
        ret.pitch() = osgEarth::Angle(-60.0, osgEarth::Units::DEGREES);
        ret.focalPoint() = fromXYZToGpsCoordinate(newCenter);
        break;
    }
    return ret;
}
} // namespace


bool guiAdapterSetViewImpl<osgEarth::Util::EarthManipulator>::execute()
{
    bool ret = false;
    osgEarth::Util::EarthManipulator * object = getObject<osgEarth::Util::EarthManipulator, SGIItemOsg, DynamicCaster>();
    const sgi::osg_plugin::ReferencedSetViewNodeLookAt * refNodeLookAt = getTargetObject<sgi::osg_plugin::ReferencedSetViewNodeLookAt, SGIItemOsg>();

    const NamedViewpoint * vp = getTargetObject<NamedViewpoint, SGIItemOsg>();
    if(vp)
    {
        object->setViewpoint(*vp, _animationTime);
        ret = true;
    }
    else if (refNodeLookAt)
    {
        double useAnimationTime = (_animationTime >= 0.0) ? _animationTime : DBL_MAX;
        const sgi::osg_plugin::SetViewNodeLookAt & data = refNodeLookAt->data();
        osg::Camera * camera = osg_helpers::findCamera(object->getNode());
        osgEarth::Viewpoint vp = computeViewpointForNode(camera, data.node, data.mode);
        if (vp.isValid())
        {
            object->setViewpoint(vp, 0.0);
            ret = true;
        }
    }

    return ret;
}

} // namespace osgearth_plugin
} // namespace sgi
