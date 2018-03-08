#pragma once

#include <osg/StateAttribute>

namespace sgi {
namespace osg_plugin {

enum StateAttributeModeValue
{
    StateAttributeModeValueUnspecified = 0,
    StateAttributeModeValueOn,
    StateAttributeModeValueOff,
    StateAttributeModeValueOverrideOff,
    StateAttributeModeValueOverrideOn,
    StateAttributeModeValueInherit,
    StateAttributeModeValueProtected,
};

osg::StateAttribute::OverrideValue stateAttributeModeValueToOverrideValue(StateAttributeModeValue mode);
StateAttributeModeValue getStateAttributeModeFromOverrideValue(osg::StateAttribute::OverrideValue value);
StateAttributeModeValue getStateAttributeModeFromObject(const osg::Object * object, osg::StateAttribute::GLModeValue mode);
void setStateAttributeModeFromObject(const osg::Object * object, osg::StateAttribute::GLModeValue mode, StateAttributeModeValue value);

} // namespace osg_plugin
} // namespace sgi
