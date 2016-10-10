#include "stdafx.h"
#include "DrawableHelper.h"
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Version>
#include <osgUtil/RenderBin>

namespace sgi {
namespace osg_plugin {

bool RenderInfo::isPresent(osg::Node * node)
{
    bool ret = false;
    osg::Group* group = node->asGroup();
    osg::Geode* geode = node->asGeode();
    if (geode)
    {
        unsigned numDrawables = geode->getNumDrawables();
        for (unsigned n = 0; !ret && n < numDrawables; n++)
        {
            RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
            ret = (renderInfoDrawable != NULL);
        }
    }
    else if (group)
    {
        unsigned numChilds = group->getNumChildren();
        for (unsigned n = 0; !ret && n < numChilds; n++)
        {
            RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(group->getChild(n));
            ret = (renderInfoDrawable != NULL);
        }
    }
    return ret;
}

bool RenderInfo::enable(osg::Node * node, bool enable)
{
    bool ret = false;
    osg::Group* group = node->asGroup();
    osg::Geode* geode = node->asGeode();
    if (!enable)
    {
        if (geode)
        {
            unsigned numDrawables = geode->getNumDrawables();
            for (unsigned n = 0; !ret && n < numDrawables; n++)
            {
                RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
                if (renderInfoDrawable)
                {
                    geode->removeDrawables(n, 1);
                    ret = true;
                }
            }
        }
        else if(group)
        {
            unsigned numChilds = group->getNumChildren();
            for (unsigned n = 0; !ret && n < numChilds; n++)
            {
                RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(group->getChild(n));
                if (renderInfoDrawable)
                {
                    group->removeChild(n, 1);
                    ret = true;
                }
            }
        }
    }
    else
    {
        if (geode)
        {
            unsigned numDrawables = geode->getNumDrawables();
            for (unsigned n = 0; !ret && n < numDrawables; n++)
            {
                RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(geode->getDrawable(n));
                ret = (renderInfoDrawable != NULL);
            }
        }
        else if (group)
        {
            unsigned numChilds = group->getNumChildren();
            for (unsigned n = 0; !ret && n < numChilds; n++)
            {
                RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(group->getChild(n));
                ret = (renderInfoDrawable != NULL);
            }
        }
        if (!ret)
        {
            if(geode)
                geode->addDrawable(new RenderInfoDrawable());
            else if(group)
                group->addChild(new RenderInfoDrawable());
            ret = true;
        }
    }
    return ret;
}

bool RenderInfo::hasDrawCallback(osg::Drawable* node)
{
    osg::Drawable::DrawCallback * cb = node->getDrawCallback();
    RenderInfoDrawCallback* rcb = dynamic_cast<RenderInfoDrawCallback*>(cb);
    return (rcb != NULL);
}

bool RenderInfo::installDrawCallback(osg::Drawable* node, bool enable)
{
    osg::Drawable::DrawCallback * cb = node->getDrawCallback();
    RenderInfoDrawCallback* rcb = dynamic_cast<RenderInfoDrawCallback*>(cb);
    if (rcb && !enable)
        node->setDrawCallback(NULL);
    else if (!rcb && enable)
        node->setDrawCallback(new RenderInfoDrawCallback);
    return true;
}

RenderInfoData::RenderInfoData()
{
}

void RenderInfoData::copyStateSetStack(StateSetStack & dest, const std::vector<const osg::StateSet*> & src)
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

void RenderInfoData::copyRenderBinStack(RenderBinStack & dest, const std::vector<osgUtil::RenderBin*> & src)
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

void RenderInfoData::copyCameraStack(CameraStack & dest, const std::vector<osg::Camera*> & src)
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

void RenderInfoData::copyPerContextProgramSet(PerContextProgramSet & dest, const std::set<const osg::Program::PerContextProgram* > & src)
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
        }
        else {
            return unsigned(key & (~0U));
        }
    }

    template<typename T>
    unsigned refPathHash(const std::vector<T*> & path)
    {
        unsigned ret = 0;
        for (typename std::vector<T*>::const_iterator it = path.begin(); it != path.end(); it++)
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
        for (typename std::vector< osg::ref_ptr<T> >::const_iterator it = path.begin(); it != path.end(); it++)
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


void RenderInfoData::copyRenderInfo(osg::RenderInfo& renderInfo)
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

    RenderInfoData::State & currentState = _hashedState[hashStateSetStack];

    currentState.capturedStateSet = new osg::StateSet;
    state->captureCurrentState(*currentState.capturedStateSet.get());

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    const osg::State::UniformMap & uniformMap = state->getUniformMap();
    for (osg::State::UniformMap::const_iterator it = uniformMap.begin(); it != uniformMap.end(); it++)
    {
        const osg::State::UniformStack& us = it->second;
        if (!us.uniformVec.empty())
        {
            currentState.capturedStateSet->addUniform(const_cast<osg::Uniform*>(us.uniformVec.back().first));
        }
    }
#endif

    RenderInfoData::copyStateSetStack(currentState.stateSetStack, stateSetStack);
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    RenderInfoData::copyRenderBinStack(currentState.renderBinStack, renderBinStack);
    RenderInfoData::copyCameraStack(currentState.cameraStack, cameraStack);
#else
    //copyPerContextProgramSet(currentState.appliedProgamSet, appliedProgramObjectSet);
#endif
    currentState.combinedStateSet = new osg::StateSet;;
    for (auto it = currentState.stateSetStack.begin(); it != currentState.stateSetStack.end(); ++it)
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


RenderInfoDrawable::RenderInfoDrawable()
{
}

RenderInfoDrawable::RenderInfoDrawable(const RenderInfoDrawable & rhs, const osg::CopyOp & copyOp)
    : osg::Drawable(rhs, copyOp)
    , _data(rhs._data)
{
}

const RenderInfoData & RenderInfoDrawable::data() const
{
    return _data;
}

void RenderInfoDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
    const_cast<RenderInfoData&>(_data).copyRenderInfo(renderInfo);
}

RenderInfoGeometry::RenderInfoGeometry(osg::Geometry * geometry)
    : osg::Geometry()
    , _childGeometry(geometry)
{
    if (geometry)
        setChildGeometry(geometry);
}

RenderInfoGeometry::RenderInfoGeometry(const RenderInfoGeometry & rhs, const osg::CopyOp & copyOp)
    : osg::Geometry(rhs, copyOp)
    , _data(rhs._data)
{
}

const RenderInfoData & RenderInfoGeometry::data() const
{
    return _data;
}

void RenderInfoGeometry::setChildGeometry(osg::Geometry * geometry)
{
    setStateSet(geometry->getStateSet());
    setUserData(geometry->getUserData());
    _primitives = geometry->getPrimitiveSetList();
    _vertexArray = geometry->getVertexArray();
    _normalArray = geometry->getNormalArray();
    _colorArray = geometry->getColorArray();
    _secondaryColorArray = geometry->getSecondaryColorArray();
    _fogCoordArray = geometry->getFogCoordArray();
    _texCoordList = geometry->getTexCoordArrayList();
    _vertexAttribList = geometry->getVertexAttribArrayList();
    _useVertexBufferObjects = geometry->getUseVertexBufferObjects();
    _childGeometry = geometry;
}

void RenderInfoGeometry::drawImplementation(osg::RenderInfo& renderInfo) const
{
    if (_childGeometry.valid())
        _childGeometry->drawImplementation(renderInfo);
    const_cast<RenderInfoData&>(_data).copyRenderInfo(renderInfo);
}

RenderInfoDrawCallback::RenderInfoDrawCallback()
{
}

const RenderInfoData & RenderInfoDrawCallback::data() const
{
    return _data;
}

void RenderInfoDrawCallback::drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const
{
    const_cast<RenderInfoData&>(_data).copyRenderInfo(renderInfo);
    drawable->drawImplementation(renderInfo);
}

} // namespace osg_plugin
} // namespace sgi
