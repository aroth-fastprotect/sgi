#include <ostream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGText.h"
#include "SGIItemOsg.h"

// osgText headers
#include <osgText/Font>
#include <osgText/Text>

#include "string_helpers.h"
#include "std_ostream_osg.h"
#include "std_ostream_osgtext.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgText::Font)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgText::TextBase)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgText::Text)

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

            os << "<tr><td>filename</td><td>" << object->getFileName() << "</td></tr>" << std::endl;
            os << "<tr><td>textureWidthHint</td><td>" << object->getTextureWidthHint() << "</td></tr>" << std::endl;
            os << "<tr><td>textureHeightHint</td><td>" << object->getTextureHeightHint() << "</td></tr>" << std::endl;

            os << "<tr><td>hasVertical</td><td>" << (object->hasVertical()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>glyphImageMargin</td><td>" << object->getGlyphImageMargin() << "</td></tr>" << std::endl;
            os << "<tr><td>glyphImageMarginRatio</td><td>" << object->getGlyphImageMarginRatio() << "</td></tr>" << std::endl;

            os << "<tr><td>minFilterHint</td><td>" << sgi::castToEnumValueString<sgi::GLConstant>(object->getMinFilterHint()) << "</td></tr>" << std::endl;
            os << "<tr><td>magFilterHint</td><td>" << sgi::castToEnumValueString<sgi::GLConstant>(object->getMagFilterHint()) << "</td></tr>" << std::endl;
            os << "<tr><td>fontDepth</td><td>" << object->getFontDepth() << "</td></tr>" << std::endl;
            os << "<tr><td>numberCurveSamples</td><td>" << object->getNumberCurveSamples() << "</td></tr>" << std::endl;

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
    osgText::TextBase * object = static_cast<osgText::TextBase*>(item<SGIItemOsg>()->object());
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
    osgText::Text * object = static_cast<osgText::Text*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add drawable properties first
            callNextHandler(os);

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

