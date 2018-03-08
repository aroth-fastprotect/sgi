#include "stdafx.h"
#include "stateset_helpers.h"
#include <osg/Object>
#include <osg/StateSet>
#include <osg/Drawable>

namespace sgi {
namespace osg_plugin {

StateAttributeModeValue getStateAttributeModeFromOverrideValue(osg::StateAttribute::OverrideValue value)
{
    StateAttributeModeValue ret;
    if(value == 0)
        ret = StateAttributeModeValueOff;
    else if(value & osg::StateAttribute::OVERRIDE)
        ret = StateAttributeModeValueOverrideOn;
    else if(value & osg::StateAttribute::INHERIT)
        ret = StateAttributeModeValueInherit;
    else if(value & osg::StateAttribute::PROTECTED)
        ret = StateAttributeModeValueProtected;
    else
        ret = StateAttributeModeValueUnspecified;
    return ret;
}

osg::StateAttribute::OverrideValue stateAttributeModeValueToOverrideValue(StateAttributeModeValue mode)
{
    osg::StateAttribute::OverrideValue ret = 0;
    switch(mode)
    {
    default:
    case StateAttributeModeValueUnspecified: ret = 0; break;
    case StateAttributeModeValueOn: ret = osg::StateAttribute::ON; break;
    case StateAttributeModeValueOff: ret = osg::StateAttribute::OFF; break;
    case StateAttributeModeValueOverrideOff: ret = osg::StateAttribute::INHERIT; break;
    case StateAttributeModeValueOverrideOn: ret = osg::StateAttribute::OVERRIDE; break;
    case StateAttributeModeValueInherit: ret = osg::StateAttribute::INHERIT; break;
    case StateAttributeModeValueProtected: ret = osg::StateAttribute::PROTECTED; break;
    }
    return ret;
}

StateAttributeModeValue getStateAttributeModeFromObject(const osg::Object * object, osg::StateAttribute::GLModeValue mode)
{
    StateAttributeModeValue ret;
    const osg::StateSet * ss = NULL;
    if(const osg::StateSet * stateSet = dynamic_cast<const osg::StateSet*>(object))
        ss = stateSet;
    else if(const osg::Node* node = dynamic_cast<const osg::Node*>(object))
        ss = node->getStateSet();
    else if(const osg::Drawable * drawable = dynamic_cast<const osg::Drawable*>(object))
        ss = drawable->getStateSet();
    if(ss)
    {
        osg::StateAttribute::GLModeValue n = ss->getMode(mode);
        if(n == osg::StateAttribute::INHERIT)
            ret = StateAttributeModeValueUnspecified;
        else
        {
            if(n & osg::StateAttribute::ON)
                ret = (n & osg::StateAttribute::OVERRIDE)?StateAttributeModeValueOverrideOn:StateAttributeModeValueOn;
            else
                ret = (n & osg::StateAttribute::OVERRIDE)?StateAttributeModeValueOverrideOff:StateAttributeModeValueOff;
        }
    }
    else
        ret = StateAttributeModeValueUnspecified;
    return ret;
}

void setStateAttributeModeFromObject(const osg::Object * object, osg::StateAttribute::GLModeValue mode, StateAttributeModeValue value)
{
    if(value == StateAttributeModeValueUnspecified)
    {
        osg::StateSet * ss = NULL;
        if(const osg::StateSet * stateSet = dynamic_cast<const osg::StateSet*>(object))
            ss = const_cast<osg::StateSet*>(stateSet);
        else if(const osg::Node* node = dynamic_cast<const osg::Node*>(object))
            ss = const_cast<osg::Node*>(node)->getStateSet();
        else if(const osg::Drawable * drawable = dynamic_cast<const osg::Drawable*>(object))
            ss = const_cast<osg::Drawable*>(drawable)->getStateSet();
        if(ss)
        {
            osg::StateAttribute::GLModeValue n = ss->getMode(mode);
            if(n != osg::StateAttribute::INHERIT)
                ss->setMode(mode, osg::StateAttribute::INHERIT);
        }
    }
    else
    {
        osg::StateSet * ss = NULL;
        if(const osg::StateSet * stateSet = dynamic_cast<const osg::StateSet*>(object))
            ss = const_cast<osg::StateSet*>(stateSet);
        else if(const osg::Node* node = dynamic_cast<const osg::Node*>(object))
            ss = const_cast<osg::Node*>(node)->getOrCreateStateSet();
        else if(const osg::Drawable * drawable = dynamic_cast<const osg::Drawable*>(object))
            ss = const_cast<osg::Drawable*>(drawable)->getOrCreateStateSet();
        if(ss)
        {
            osg::StateAttribute::GLModeValue glmodevalue;
            switch(value)
            {
            case StateAttributeModeValueOverrideOn: glmodevalue = osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE; break;
            case StateAttributeModeValueOverrideOff: glmodevalue = osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE; break;
            case StateAttributeModeValueOn: glmodevalue = osg::StateAttribute::ON; break;
            case StateAttributeModeValueOff: glmodevalue = osg::StateAttribute::OFF; break;
            default: glmodevalue = osg::StateAttribute::INHERIT; break;
            }
            ss->setMode(mode, glmodevalue);
        }
    }
}

} // namespace osg_plugin
} // namespace sgi
