#include <ostream>
#include <sstream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGAnimation.h"

#include <osgAnimation/AnimationManagerBase>

#include "SGIItemOsg.h"

#include "string_helpers.h"
#include "std_ostream_osg.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgAnimation::AnimationManagerBase)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgAnimation::Animation)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgAnimation::Channel)

bool writePrettyHTMLImpl<osgAnimation::AnimationManagerBase>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgAnimation::AnimationManagerBase * object = getObject<osgAnimation::AnimationManagerBase,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>animations</td><td><ol>";
            const osgAnimation::AnimationList& animations = object->getAnimationList();
            for(osgAnimation::AnimationList::const_iterator it = animations.begin(); it != animations.end(); it++)
            {
                const osg::ref_ptr<osgAnimation::Animation> & animation = *it;
                os << "<li>" << getObjectNameAndType(animation.get()) << "</li>";

            }
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>automatic link</td><td>" << (object->isAutomaticLink()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>need to link</td><td>" << (object->needToLink()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeAnimations:
        {
            os << "<ol>";
            const osgAnimation::AnimationList& animations = object->getAnimationList();
            for(osgAnimation::AnimationList::const_iterator it = animations.begin(); it != animations.end(); it++)
            {
                const osg::ref_ptr<osgAnimation::Animation> & animation = *it;
                os << "<li>" << getObjectNameAndType(animation.get()) << "</li>";

            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgAnimation::Animation>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgAnimation::Animation * object = getObject<osgAnimation::Animation,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>duration</td><td>" << object->getDuration() << "</td></tr>" << std::endl;
            os << "<tr><td>weight</td><td>" << object->getWeight() << "</td></tr>" << std::endl;
            os << "<tr><td>play mode</td><td>" << object->getPlayMode() << "</td></tr>" << std::endl;
            os << "<tr><td>start time</td><td>" << object->getStartTime() << "</td></tr>" << std::endl;

            os << "<tr><td>channels</td><td><ol>";
            const osgAnimation::ChannelList & channels = object->getChannels();
            for(osgAnimation::ChannelList::const_iterator it = channels.begin(); it != channels.end(); it++)
            {
                const osg::ref_ptr<osgAnimation::Channel> & channel = *it;
                os << "<li>" << getObjectNameAndType(channel.get()) << "</li>";
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChannels:
        {
            os << "<ol>";
            const osgAnimation::ChannelList & channels = object->getChannels();
            for(osgAnimation::ChannelList::const_iterator it = channels.begin(); it != channels.end(); it++)
            {
                const osg::ref_ptr<osgAnimation::Channel> & channel = *it;
                os << "<li>" << getObjectNameAndType(channel.get()) << "</li>";
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<osgAnimation::Channel>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgAnimation::Channel * object = getObject<osgAnimation::Channel,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>target name</td><td>" << object->getTargetName() << "</td></tr>" << std::endl;
            os << "<tr><td>target</td><td>" << getObjectNameAndType(object->getTarget()) << "</td></tr>" << std::endl;
            os << "<tr><td>start time</td><td>" << object->getStartTime() << "</td></tr>" << std::endl;
            os << "<tr><td>end time</td><td>" << object->getEndTime() << "</td></tr>" << std::endl;
            os << "<tr><td>sampler</td><td>" << getObjectNameAndType(object->getSampler()) << "</td></tr>" << std::endl;

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

