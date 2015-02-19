#include <ostream>
#include <sstream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGFX.h"

#include <osgFX/Outline>

#include "SGIItemOsg"

#include "string_helpers.h"
#include "std_ostream_osg.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgFX::Effect)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgFX::Outline)

bool writePrettyHTMLImpl<osgFX::Effect>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgFX::Effect * object = getObject<osgFX::Effect,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>effectName</td><td>" << object->effectName() << "</td></tr>" << std::endl;
            os << "<tr><td>effectDescription</td><td>" << object->effectDescription() << "</td></tr>" << std::endl;
            os << "<tr><td>effectAuthor</td><td>" << object->effectAuthor() << "</td></tr>" << std::endl;
            os << "<tr><td>enabled</td><td>" << (object->getEnabled()?"true":"false") << "</td></tr>" << std::endl;
            
            unsigned num = object->getNumTechniques();
            os << "<tr><td>techniques</td><td><ol>";
            for(unsigned n = 0; n < num; n++)
            {
                const osgFX::Technique * tech = object->getTechnique(n);
                os << "<li>" << getObjectName(tech) << " (" << getObjectTypename(tech) << ")</li>";
            }
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>selectedTechnique</td><td>" << object->getSelectedTechnique() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgFX::Outline>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgFX::Outline * object = getObject<osgFX::Outline,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);
            os << "<tr><td>width</td><td>" << object->getWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>color</td><td>" << object->getColor() << "</td></tr>" << std::endl;

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

