#pragma once
#include <osgText/Text>

namespace sgi {
	namespace osg_plugin {

class osgTextAccess : public osgText::Text
{
public:
	const osg::BoundingBox & textBB() const { return _textBB; }

	float modelSpaceHeight(unsigned int contextID) const
	{
		const osg::Matrixd & matrix = _autoTransformCache[contextID]._matrix;
		osg::Vec3d topLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMin(), _textBB.zMin())*matrix;
		osg::Vec3d bottomLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMax(), _textBB.zMin())*matrix;

		return (bottomLeft - topLeft).length();
	}
};

	} // namespace osg_plugin
} // namespace sgi
