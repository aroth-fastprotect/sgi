#include "stdafx.h"
#include "DrawableHelper.h"
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Version>
#include <osgUtil/RenderBin>

namespace sgi {
namespace osg_plugin {

RenderInfoDrawable::RenderInfoDrawable()
{
}

RenderInfoDrawable::RenderInfoDrawable(const RenderInfoDrawable & rhs, const osg::CopyOp & copyOp)
    : osg::Drawable(rhs, copyOp)
    , _hashedState(rhs._hashedState)
{
}

bool RenderInfoDrawable::isPresent(osg::Geode * geode)
{
    bool ret = false;
    unsigned numDrawables = geode->getNumDrawables();
    for(unsigned n = 0; !ret && n < numDrawables; n++)
    {
        RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
        ret = (renderInfoDrawable != NULL);
    }
    return ret;
}

bool RenderInfoDrawable::enable(osg::Geode * geode, bool enable)
{
    bool ret = false;
    if(!enable)
    {
        unsigned numDrawables = geode->getNumDrawables();
        for(unsigned n = 0; !ret && n < numDrawables; n++)
        {
            RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
            if(renderInfoDrawable)
            {
                geode->removeDrawables(n, 1);
                ret = true;
            }
        }
    }
    else
    {
        unsigned numDrawables = geode->getNumDrawables();
        for(unsigned n = 0; !ret && n < numDrawables; n++)
        {
            RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
            ret = (renderInfoDrawable != NULL);
        }
        if(!ret)
        {
            geode->addDrawable(new RenderInfoDrawable());
            ret = true;
        }
    }
    return ret;
}

void RenderInfoDrawable::copyStateSetStack(StateSetStack & dest, const std::vector<const osg::StateSet*> & src)
{
    dest.clear();
    dest.resize(src.size());
    unsigned dest_index = 0;
    for(osg::State::StateSetStack::const_iterator it = src.begin(); it != src.end(); it++, dest_index++)
    {
        const osg::StateSet* src_ss = *it;
        dest[dest_index] = (osg::StateSet*)src_ss->clone(osg::CopyOp::DEEP_COPY_ALL);
    }
}

void RenderInfoDrawable::copyRenderBinStack(RenderBinStack & dest, const std::vector<osgUtil::RenderBin*> & src)
{
    dest.clear();
    dest.resize(src.size());
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    unsigned dest_index = 0;
    for(osg::RenderInfo::RenderBinStack::const_iterator it = src.begin(); it != src.end(); it++, dest_index++)
    {
        osgUtil::RenderBin* src_bin = *it;
        dest[dest_index] = (osgUtil::RenderBin*)src_bin->clone(osg::CopyOp::DEEP_COPY_ALL);
    }
#endif
}

void RenderInfoDrawable::copyCameraStack(CameraStack & dest, const std::vector<osg::Camera*> & src)
{
    dest.clear();
    dest.resize(src.size());
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    unsigned dest_index = 0;
    for(osg::RenderInfo::CameraStack::const_iterator it = src.begin(); it != src.end(); it++, dest_index++)
    {
        osg::Camera* src_camera = *it;
        dest[dest_index] = src_camera;
    }
#endif
}

void RenderInfoDrawable::copyPerContextProgramSet(PerContextProgramSet & dest, const std::set<const osg::Program::PerContextProgram* > & src)
{
    dest.clear();
#if OSG_VERSION_GREATER_THAN(3,3,1)
#else
    /*
    unsigned dest_index = 0;
    for(osg::State::AppliedProgramObjectSet::const_iterator it = src.begin(); it != src.end(); it++)
    {
        const osg::Program::PerContextProgram* src_program = *it;
        dest.insert(PerContextProgramPtr(const_cast<osg::Program::PerContextProgram*>(src_program)));
    }
    */
#endif
}

namespace {
    typedef unsigned long long register_t;
    typedef unsigned long long ptr_int_t;

    inline unsigned register_t_to_unsigned(register_t key)
    {
        if (sizeof(register_t) > sizeof(unsigned)) {
            return unsigned(((key >> (8 * sizeof(uint) - 1)) ^ key) & (~0U));
        } else {
            return unsigned(key & (~0U));
        }
    }

    template<typename T>
    unsigned refPathHash(const std::vector<T*> & path)
    {
        unsigned ret = 0;
        for(typename std::vector<T*>::const_iterator it = path.begin(); it != path.end(); it++)
        {
            register_t r = (register_t)(void*)(*it);
            ret ^= register_t_to_unsigned(r);
        }
        return ret;
    }

    template<typename T>
    unsigned refPathHash(const std::vector< osg::ref_ptr<T> > & path)
    {
        unsigned ret = 0;
        for(typename std::vector< osg::ref_ptr<T> >::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::ref_ptr<T> & cur = *it;
            register_t r = (register_t)(void*)(cur.get());
            ret ^= register_t_to_unsigned(r);
        }
        return ret;
    }
    
    class StateAccess : public osg::State
    {
    public:
#if OSG_VERSION_GREATER_THAN(3,3,1)
#else
        const osg::State::AppliedProgramObjectSet& getAppliedProgramObjectSet() const
        {
            return _appliedProgramObjectSet;
        }
#endif
    };
}



void RenderInfoDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
    StateAccess * state = (StateAccess*)renderInfo.getState();
    osg::State::StateSetStack& stateSetStack = state->getStateSetStack();
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    osg::RenderInfo::RenderBinStack & renderBinStack = renderInfo.getRenderBinStack();
    osg::RenderInfo::CameraStack & cameraStack = renderInfo.getCameraStack();
#else
    //const osg::State::AppliedProgramObjectSet& appliedProgramObjectSet = state->getAppliedProgramObjectSet();
#endif

    unsigned hashStateSetStack = refPathHash(stateSetStack);

    State & currentState = const_cast<RenderInfoDrawable*>(this)->_hashedState[hashStateSetStack];
    
    currentState.capturedStateSet = new osg::StateSet;
    state->captureCurrentState(*currentState.capturedStateSet.get());
    
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    const osg::State::UniformMap & uniformMap = state->getUniformMap();
    for(osg::State::UniformMap::const_iterator it = uniformMap.begin(); it != uniformMap.end(); it++)
    {
        const osg::State::UniformStack& us = it->second;
        if (!us.uniformVec.empty())
        {
            currentState.capturedStateSet->addUniform(const_cast<osg::Uniform*>(us.uniformVec.back().first));
        }
    }
#endif

    copyStateSetStack(currentState.stateSetStack, stateSetStack);
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    copyRenderBinStack(currentState.renderBinStack, renderBinStack);
    copyCameraStack(currentState.cameraStack, cameraStack);
#else
    //copyPerContextProgramSet(currentState.appliedProgamSet, appliedProgramObjectSet);
#endif
    currentState.combinedStateSet = new osg::StateSet;;
    for(auto it = currentState.stateSetStack.begin(); it != currentState.stateSetStack.end(); ++it)
    {
        currentState.combinedStateSet->merge(*(it->get()));
    }
    currentState.view = renderInfo.getView();

/*
    copyStateSetStack(const_cast<RenderInfoDrawable*>(this)->_lastStateSetStack, stateSetStack);
    copyRenderBinStack(const_cast<RenderInfoDrawable*>(this)->_lastRenderBinStack, renderBinStack);
    copyCameraStack(const_cast<RenderInfoDrawable*>(this)->_lastCameraStack, cameraStack);
    const_cast<RenderInfoDrawable*>(this)->_lastView = renderInfo.getView();
    */
}

} // namespace osg_plugin
} // namespace sgi
