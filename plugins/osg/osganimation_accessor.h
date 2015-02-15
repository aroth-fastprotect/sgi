#pragma once
#include <osgAnimation/AnimationManagerBase>

namespace sgi {
namespace osg_plugin {

class AnimationManagerBaseAccess : public osgAnimation::AnimationManagerBase
{
public:
    osgAnimation::LinkVisitor *   getLinkVisitor() { return _linker.get(); }
};

} // namespace osg_plugin
} // namespace sgi

