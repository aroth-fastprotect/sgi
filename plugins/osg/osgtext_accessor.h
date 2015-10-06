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
		if (contextID > _autoTransformCache.size())
			return FLT_MIN;
		const osg::Matrixd & matrix = _autoTransformCache[contextID]._matrix;
		osg::Vec3d topLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMin(), _textBB.zMin())*matrix;
		osg::Vec3d bottomLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMax(), _textBB.zMin())*matrix;

		return (bottomLeft - topLeft).length();
	}

	void writeAutoTransformCache(std::basic_ostream<char>& os, unsigned int contextID);
};

	} // namespace osg_plugin
} // namespace sgi
