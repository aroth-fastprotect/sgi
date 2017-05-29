#pragma once
#include <osgText/Text>

namespace sgi {
	namespace osg_plugin {

class osgTextBaseAccess : public osgText::TextBase
{
public:
    const osg::Vec3 & getOffset() const { return _offset; }
    const osg::Vec3 & getNormal() const { return _normal; }
    const osg::BoundingBox & getTextBB() const { return _textBB; }


    const osg::Matrix & getMatrix() const { return _matrix; }
    const Primitives & getPrimitives() const { return _decorationPrimitives; }

    const osg::VertexBufferObject * getVBO() const { return _vbo.get(); }
    const osg::ElementBufferObject * getEBO() const { return _ebo.get(); }

    const Coords & getCoords() const { return _coords; }
    const Coords & getNormals() const { return _normals; }
    const ColorCoords & getColorCoords() const { return _colorCoords; }
    const TexCoords & getTexCoords() const { return _texcoords; }
    
    const osg::Vec3 & getScaleFont() const { return _scaleFont; };

    void forceComputeGlyphRepresentation() {
        computeGlyphRepresentation();
    }
};

class osgTextAccess : public osgText::Text
{
public:
	float modelSpaceHeight(unsigned int contextID) const
	{
#if 0
		if (contextID > _autoTransformCache.size())
			return FLT_MIN;
		const osg::Matrixd & matrix = _autoTransformCache[contextID]._matrix;
		osg::Vec3d topLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMin(), _textBB.zMin())*matrix;
		osg::Vec3d bottomLeft = osg::Vec3d(_textBB.xMin(), _textBB.yMax(), _textBB.zMin())*matrix;

		return (bottomLeft - topLeft).length();
#else
        const osg::Matrix & m = _matrix;
        osg::Vec3f topLeft = osg::componentMultiply(osg::Vec3f(_textBB.xMin(), _textBB.yMin(), _textBB.zMin()) * m, _scaleFont);
        osg::Vec3f bottomLeft = osg::componentMultiply(osg::Vec3f(_textBB.xMin(), _textBB.yMax(), _textBB.zMin()) * m, _scaleFont);
        return (bottomLeft - topLeft).length();
#endif 
	}

	void writeAutoTransformCache(std::basic_ostream<char>& os, unsigned int contextID);
};

	} // namespace osg_plugin
} // namespace sgi
