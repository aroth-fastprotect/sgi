#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSG.h"
#include "SGIItemOsg"

// osgText headers
#include <osgText/Font>
#include <osgText/Text>
#include <osg/io_utils>

#include <sgi/helpers/osg>
#include "std_ostream_osgtext.h"
#include "osgtext_accessor.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgText::Font)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgText::TextBase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgText::Text)

using namespace sgi::osg_helpers;

void osgTextAccess::writeAutoTransformCache(std::basic_ostream<char>& os, unsigned int contextID)
{
#if 0
	AutoTransformCache * atc = (contextID < _autoTransformCache.size()) ? &_autoTransformCache[contextID] : NULL;
	os << "<tr><td>AutoTransformCache</td><td>" << (void*)atc << "</td></tr>" << std::endl;

	if (atc)
	{
		os << "<tr><td>modelSpaceHeight</td><td>" << modelSpaceHeight << "</td></tr>" << std::endl;

		os << "<tr><td>transformedPosition</td><td>";
		writePrettyPositionHTML(os, atc->_transformedPosition, this);
		os << "</td></tr>" << std::endl;

		os << "<tr><td>matrix</td><td>";
		writePrettyHTML(os, matrix, MatrixUsageTypeGeneric, this);
		os << "</td></tr>" << std::endl;

		os << "<tr><td>projection</td><td>";
		writePrettyHTML(os, atc->_projection, MatrixUsageTypePerspective, this);
		os << "</td></tr>" << std::endl;

		os << "<tr><td>modelview</td><td>";
		writePrettyHTML(os, atc->_modelview, MatrixUsageTypeModelView, this);
		os << "</td></tr>" << std::endl;
	}
#endif
}


bool writePrettyHTMLImpl<osgText::Font>::process(std::basic_ostream<char>& os)
{
    osgText::Font * object = static_cast<osgText::Font*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

			os << "<tr><td>texEnv</td><td>" << getObjectNameAndType(object->getTexEnv()) << "</td></tr>" << std::endl;
			os << "<tr><td>stateSet</td><td>" << getObjectNameAndType(object->getStateSet()) << "</td></tr>" << std::endl;
            os << "<tr><td>filename</td><td>" << object->getFileName() << "</td></tr>" << std::endl;
            os << "<tr><td>textureWidthHint</td><td>" << object->getTextureWidthHint() << "</td></tr>" << std::endl;
            os << "<tr><td>textureHeightHint</td><td>" << object->getTextureHeightHint() << "</td></tr>" << std::endl;

            os << "<tr><td>hasVertical</td><td>" << (object->hasVertical()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>glyphImageMargin</td><td>" << object->getGlyphImageMargin() << "</td></tr>" << std::endl;
            os << "<tr><td>glyphImageMarginRatio</td><td>" << object->getGlyphImageMarginRatio() << "</td></tr>" << std::endl;

            os << "<tr><td>minFilterHint</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getMinFilterHint()) << "</td></tr>" << std::endl;
            os << "<tr><td>magFilterHint</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getMagFilterHint()) << "</td></tr>" << std::endl;
            os << "<tr><td>fontDepth</td><td>" << object->getFontDepth() << "</td></tr>" << std::endl;
            os << "<tr><td>numberCurveSamples</td><td>" << object->getNumberCurveSamples() << "</td></tr>" << std::endl;
			os << "<tr><td>implementation</td><td>" << getObjectNameAndType(object->getImplementation()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
	case SGIItemTypeFontTextureList:
		{
			const osgText::Font::GlyphTextureList & textureList = object->getGlyphTextureList();
			os << "<ul>";
			for(const auto & texture : textureList)
			{
				os << "<li>" << getObjectNameAndType(texture.get()) << "</li>" << std::endl;
			}
			os << "</ul>";
			ret = true;
		}
		break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::DrawModeMask& t)
{
    if(t == 0)
        os << "none";
    else
    {
        if((unsigned)t & osgText::TextBase::TEXT)
            os << "text";
        else
            os << "notext";
        if((unsigned)t & osgText::TextBase::BOUNDINGBOX)
            os << ", bbox";
        if((unsigned)t & osgText::TextBase::FILLEDBOUNDINGBOX)
            os << ", filledbbox";
        if((unsigned)t & osgText::TextBase::ALIGNMENT)
            os << ", align";
    }
    return os;
}


bool writePrettyHTMLImpl<osgText::TextBase>::process(std::basic_ostream<char>& os)
{
    osgTextBaseAccess * object = static_cast<osgTextBaseAccess*>(getObject<osgText::TextBase, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add drawable properties first
            callNextHandler(os);

            os << "<tr><td>font</td><td>" << getObjectNameAndType(object->getFont()) << "</td></tr>" << std::endl;
            os << "<tr><td>style</td><td>" << getObjectNameAndType(object->getStyle()) << "</td></tr>" << std::endl;
            os << "<tr><td>color</td><td>" << vec4fToHtmlColor(object->getColor()) << "</td></tr>" << std::endl;
            os << "<tr><td>fontWidth</td><td>" << object->getFontWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>fontHeight</td><td>" << object->getFontHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>text</td><td>" << object->getText().createUTF8EncodedString() << "</td></tr>" << std::endl;
            os << "<tr><td>characterHeight</td><td>" << object->getCharacterHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>characterAspectRatio</td><td>" << object->getCharacterAspectRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>characterSizeMode</td><td>" << object->getCharacterSizeMode() << "</td></tr>" << std::endl;
            os << "<tr><td>maximumWidth</td><td>" << object->getMaximumWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>maximumHeight</td><td>" << object->getMaximumHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>lineSpacing</td><td>" << object->getLineSpacing() << "</td></tr>" << std::endl;
            os << "<tr><td>position</td><td>";
            writePrettyPositionHTML(os, object->getPosition(), object);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>alignment</td><td>" << object->getAlignment() << "</td></tr>" << std::endl;
            os << "<tr><td>axis alignment</td><td>" << object->getAxisAlignment() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>autoRotateToScreen</td><td>" << (object->getAutoRotateToScreen()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>layout</td><td>" << object->getLayout() << "</td></tr>" << std::endl;
            os << "<tr><td>drawMode</td><td>" << ((osgText::TextBase::DrawModeMask)object->getDrawMode()) << "</td></tr>" << std::endl;
            os << "<tr><td>boundingBoxMargin</td><td>" << object->getBoundingBoxMargin() << "</td></tr>" << std::endl;
            os << "<tr><td>boundingBoxColor</td><td>" << vec4fToHtmlColor(object->getBoundingBoxColor()) << "</td></tr>" << std::endl;
            os << "<tr><td>kerningType</td><td>" << object->getKerningType() << "</td></tr>" << std::endl;
            os << "<tr><td>lineCount</td><td>" << object->getLineCount() << "</td></tr>" << std::endl;

            os << "<tr><td>offset</td><td>" << object->getOffset() << "</td></tr>" << std::endl;
            os << "<tr><td>normal</td><td>" << object->getNormal() << "</td></tr>" << std::endl;
            os << "<tr><td>textBB</td><td>";
            writePrettyHTML(os, object->getTextBB());
            os << "</td></tr>" << std::endl;

            os << "<tr><td>matrix</td><td>" << object->getMatrix() << "</td></tr>" << std::endl;
            os << "<tr><td>primitives</td><td><ul>";
            for (auto prim : object->getPrimitives())
                os << "<li>" << prim->getType() << '/' << prim->getMode() << ':' << getObjectNameAndType(prim) << "</li>";
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>vbo</td><td>" << getObjectNameAndType(object->getVBO()) << "</td></tr>" << std::endl;
            os << "<tr><td>ebo</td><td>" << getObjectNameAndType(object->getEBO()) << "</td></tr>" << std::endl;

            os << "<tr><td>coords</td><td>" << getObjectNameAndType(object->getCoords()) << "</td></tr>" << std::endl;
            os << "<tr><td>normals</td><td>" << getObjectNameAndType(object->getNormals()) << "</td></tr>" << std::endl;
            os << "<tr><td>colorCoords</td><td>" << getObjectNameAndType(object->getColorCoords()) << "</td></tr>" << std::endl;
            os << "<tr><td>texCoords</td><td>" << getObjectNameAndType(object->getTexCoords()) << "</td></tr>" << std::endl;
            os << "<tr><td>scaleFont</td><td>" << object->getScaleFont() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgText::Text>::process(std::basic_ostream<char>& os)
{
    osgTextAccess * object = static_cast<osgTextAccess *>(getObject<osgText::Text,SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add drawable properties first
            callNextHandler(os);

			osg::Camera * camera = osg_helpers::findFirstParentOfType<osg::Camera>(object);

            os << "<tr><td>enableDepthWrites</td><td>" << (object->getEnableDepthWrites()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>backdropType</td><td>" << object->getBackdropType() << "</td></tr>" << std::endl;
            os << "<tr><td>backdropImpl</td><td>" << object->getBackdropImplementation() << "</td></tr>" << std::endl;
            os << "<tr><td>backdropHorzOffset</td><td>" << object->getBackdropHorizontalOffset() << "</td></tr>" << std::endl;
            os << "<tr><td>backdropVertOffset</td><td>" << object->getBackdropVerticalOffset() << "</td></tr>" << std::endl;
            os << "<tr><td>backdropColor</td><td>" << vec4fToHtmlColor(object->getBackdropColor()) << "</td></tr>" << std::endl;

            os << "<tr><td>colorGradientMode</td><td>" << object->getColorGradientMode() << "</td></tr>" << std::endl;

            os << "<tr><td>colorGradientTopLeft</td><td>"
                << vec4fToHtmlColor(object->getColorGradientTopLeft())
                << "</td></tr>" << std::endl;
            os << "<tr><td>colorGradientBottomLeft</td><td>"
                << vec4fToHtmlColor(object->getColorGradientBottomLeft())
                << "</td></tr>" << std::endl;
            os << "<tr><td>colorGradientBottomRight</td><td>"
                << vec4fToHtmlColor(object->getColorGradientBottomRight())
                << "</td></tr>" << std::endl;
            os << "<tr><td>colorGradientTopRight</td><td>"
                << vec4fToHtmlColor(object->getColorGradientTopRight())
                << "</td></tr>" << std::endl;

			unsigned contextId = ~0u;
			os << "<tr><td>camera</td><td>" << getObjectNameAndType(camera) << "</td></tr>" << std::endl;
			if (camera)
			{
				osg::GraphicsContext * ctx = camera->getGraphicsContext();
				os << "<tr><td>graphicsContext</td><td>" << getObjectNameAndType(ctx) << "</td></tr>" << std::endl;
				if (ctx)
					contextId = ctx->getState()->getContextID();
			}
			os << "<tr><td>contextId</td><td>" << contextId << "</td></tr>" << std::endl;
            os << "<tr><td>modelSpaceHeight</td><td>" << object->modelSpaceHeight(contextId) << "</td></tr>" << std::endl;

			object->writeAutoTransformCache(os, contextId);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi

