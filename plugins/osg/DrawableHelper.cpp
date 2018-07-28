#include "stdafx.h"
#include "DrawableHelper.h"
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osg/Version>
#include <osg/ComputeBoundsVisitor>
#include <osgUtil/RenderBin>
#include <osgViewer/View>

#include <osgDB/WriteFile>
#include <iostream>

#include <sgi/helpers/osg>

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
            ret = (renderInfoDrawable != nullptr);
        }
    }
    else if (group)
    {
        unsigned numChilds = group->getNumChildren();
        for (unsigned n = 0; !ret && n < numChilds; n++)
        {
            RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(group->getChild(n));
            ret = (renderInfoDrawable != nullptr);
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
                ret = (renderInfoDrawable != nullptr);
            }
        }
        else if (group)
        {
            unsigned numChilds = group->getNumChildren();
            for (unsigned n = 0; !ret && n < numChilds; n++)
            {
                RenderInfoDrawable * renderInfoDrawable = dynamic_cast<RenderInfoDrawable*>(group->getChild(n));
                ret = (renderInfoDrawable != nullptr);
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
    return (rcb != nullptr);
}

bool RenderInfo::installDrawCallback(osg::Drawable* node, bool enable)
{
    osg::Drawable::DrawCallback * cb = node->getDrawCallback();
    RenderInfoDrawCallback* rcb = dynamic_cast<RenderInfoDrawCallback*>(cb);
    if (rcb && !enable)
        node->setDrawCallback(nullptr);
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
        StateSetEntry & entry = dest[dest_index];
        entry.stateSet = static_cast<osg::StateSet*>(src_ss->clone(osg::CopyOp::DEEP_COPY_ALL));
        entry.parentalNodePaths.resize(src_ss->getNumParents());
        for (unsigned i = 0; i < src_ss->getNumParents(); ++i)
        {
            const osg::Node * src_parent = src_ss->getParent(i);
            if (src_parent)
            {
                osg::NodePathList plist = src_parent->getParentalNodePaths();
                entry.parentalNodePaths[i] = plist;
            }
        }
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

void RenderInfoData::captureCurrentState(osg::StateSet * dest, osg::State * src)
{
    src->captureCurrentState(*dest);
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    const osg::State::UniformMap & uniformMap = src->getUniformMap();
    for (osg::State::UniformMap::const_iterator it = uniformMap.begin(); it != uniformMap.end(); it++)
    {
        const osg::State::UniformStack& us = it->second;
        if (!us.uniformVec.empty())
        {
            dest->addUniform(const_cast<osg::Uniform*>(us.uniformVec.back().first));
        }
    }
    const osg::State::TextureModeMapList & textureModeMapList = src->getTextureModeMapList();
    unsigned textureUnit = 0;
    for (osg::State::TextureModeMapList::const_iterator it = textureModeMapList.begin(); it != textureModeMapList.end(); ++it, ++textureUnit)
    {
        const osg::State::ModeMap & modemap = *it;
        for (osg::State::ModeMap::const_iterator it2 = modemap.begin(); it2 != modemap.end(); ++it2)
        {
            const GLenum & glmode = it2->first;
            const osg::State::ModeStack & stack = it2->second;

            dest->setTextureMode(textureUnit, glmode, stack.last_applied_value ? osg::StateAttribute::ON : osg::StateAttribute::OFF);

            for (GLuint value : stack.valueVec)
                dest->setTextureMode(textureUnit, glmode, value);
        }
    }
    const osg::State::TextureAttributeMapList & textureAttributeMapList = src->getTextureAttributeMapList();
    textureUnit = 0;
    for (osg::State::TextureAttributeMapList::const_iterator it = textureAttributeMapList.begin(); it != textureAttributeMapList.end(); ++it, ++textureUnit)
    {
        const osg::State::AttributeMap & attributemap = *it;
        for (osg::State::AttributeMap::const_iterator it2 = attributemap.begin(); it2 != attributemap.end(); ++it2)
        {
            //const osg::StateAttribute::TypeMemberPair & typemem = it2->first;
            const osg::State::AttributeStack & stack = it2->second;

            if(stack.last_applied_attribute)
                dest->setTextureAttribute(textureUnit, const_cast<osg::StateAttribute*>(stack.last_applied_attribute), osg::StateAttribute::ON);

            for (const osg::State::AttributePair & attrpair : stack.attributeVec)
            {
                const osg::StateAttribute * sa = attrpair.first;
                const osg::StateAttribute::OverrideValue & overridevalue = attrpair.second;
                dest->setTextureAttribute(textureUnit, const_cast<osg::StateAttribute*>(sa), overridevalue);
            }
        }
    }
#endif
}

namespace {
    typedef RenderInfoData::HashedStateId register_t;
    typedef RenderInfoData::HashedStateId ptr_int_t;

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
        void copyFrom(const osg::State * src);
    };

#define StateAccess_copyFrom(__field) \
    __field = src->__field

    void StateAccess::copyFrom(const osg::State * src_)
    {
        const StateAccess * src = static_cast<const StateAccess*>(src_);

        StateAccess_copyFrom(_graphicsContext);
        StateAccess_copyFrom(_contextID);

        StateAccess_copyFrom(_globalVertexArrayState);
        StateAccess_copyFrom(_vas);

        StateAccess_copyFrom(_shaderCompositionEnabled);
        StateAccess_copyFrom(_shaderCompositionDirty);
        StateAccess_copyFrom(_shaderComposer);
        StateAccess_copyFrom(_currentShaderCompositionProgram);
        StateAccess_copyFrom(_currentShaderCompositionUniformList);
        StateAccess_copyFrom(_frameStamp);
        StateAccess_copyFrom(_identity);
        StateAccess_copyFrom(_initialViewMatrix);
        StateAccess_copyFrom(_projection);
        StateAccess_copyFrom(_modelView);
        StateAccess_copyFrom(_modelViewCache);
        StateAccess_copyFrom(_useModelViewAndProjectionUniforms);
        StateAccess_copyFrom(_modelViewMatrixUniform);
        StateAccess_copyFrom(_projectionMatrixUniform);
        StateAccess_copyFrom(_modelViewProjectionMatrixUniform);
        StateAccess_copyFrom(_normalMatrixUniform);
        StateAccess_copyFrom(_initialInverseViewMatrix);
        StateAccess_copyFrom(_displaySettings);
        StateAccess_copyFrom(_abortRenderingPtr);
        StateAccess_copyFrom(_checkGLErrors);
        StateAccess_copyFrom(_useVertexAttributeAliasing);
        StateAccess_copyFrom(_vertexAlias);
        StateAccess_copyFrom(_normalAlias);
        StateAccess_copyFrom(_colorAlias);
        StateAccess_copyFrom(_secondaryColorAlias);
        StateAccess_copyFrom(_fogCoordAlias);
        StateAccess_copyFrom(_texCoordAliasList);
        StateAccess_copyFrom(_attributeBindingList);

        //StateAccess_copyFrom(_modeMap);
        //StateAccess_copyFrom(_attributeMap);
        //StateAccess_copyFrom(_uniformMap);
        //StateAccess_copyFrom(_defineMap);
        //StateAccess_copyFrom(_textureModeMapList);
        //StateAccess_copyFrom(_textureAttributeMapList);
        //StateAccess_copyFrom(_lastAppliedProgramObject);
        //StateAccess_copyFrom(_stateStateStack);
        StateAccess_copyFrom(_maxTexturePoolSize);
        StateAccess_copyFrom(_maxBufferObjectPoolSize);
        StateAccess_copyFrom(_currentActiveTextureUnit);
        StateAccess_copyFrom(_currentClientActiveTextureUnit);
        StateAccess_copyFrom(_currentPBO);
        StateAccess_copyFrom(_currentVAO);

        StateAccess_copyFrom(_isSecondaryColorSupported);
        StateAccess_copyFrom(_isFogCoordSupported);
        StateAccess_copyFrom(_isVertexBufferObjectSupported);
        StateAccess_copyFrom(_isVertexArrayObjectSupported);
        StateAccess_copyFrom(_forceVertexBufferObject);
        StateAccess_copyFrom(_forceVertexArrayObject);

        StateAccess_copyFrom(_extensionProcsInitialized);
        StateAccess_copyFrom(_glMaxTextureCoords);
        StateAccess_copyFrom(_glMaxTextureUnits);
        StateAccess_copyFrom(_glClientActiveTexture);
        StateAccess_copyFrom(_glActiveTexture);
        StateAccess_copyFrom(_glMultiTexCoord4f);
        StateAccess_copyFrom(_glVertexAttrib4f);
        StateAccess_copyFrom(_glVertexAttrib4fv);
        StateAccess_copyFrom(_glFogCoordPointer);
        StateAccess_copyFrom(_glSecondaryColorPointer);
        StateAccess_copyFrom(_glVertexAttribPointer);
        StateAccess_copyFrom(_glVertexAttribIPointer);
        StateAccess_copyFrom(_glVertexAttribLPointer);
        StateAccess_copyFrom(_glEnableVertexAttribArray);
        StateAccess_copyFrom(_glDisableVertexAttribArray);
        StateAccess_copyFrom(_glBindBuffer);
        StateAccess_copyFrom(_glDrawArraysInstanced);
        StateAccess_copyFrom(_glDrawElementsInstanced);
        StateAccess_copyFrom(_glExtensions);

        StateAccess_copyFrom(_dynamicObjectCount);
        StateAccess_copyFrom(_completeDynamicObjectRenderingCallback);

        //StateAccess_copyFrom(_arrayDispatchers);

        StateAccess_copyFrom(_graphicsCostEstimator);

        StateAccess_copyFrom(_startTick);
        StateAccess_copyFrom(_gpuTick);
        StateAccess_copyFrom(_gpuTimestamp);
        StateAccess_copyFrom(_timestampBits);

    }

}


void RenderInfoData::copyRenderInfo(osg::RenderInfo& renderInfo)
{
    StateAccess * state = static_cast<StateAccess*>(renderInfo.getState());
    osg::State::StateSetStack& stateSetStack = state->getStateSetStack();
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    osg::RenderInfo::RenderBinStack & renderBinStack = renderInfo.getRenderBinStack();
    osg::RenderInfo::CameraStack & cameraStack = renderInfo.getCameraStack();
#else
    //const osg::State::AppliedProgramObjectSet& appliedProgramObjectSet = state->getAppliedProgramObjectSet();
#endif

    HashedStateId hashStateSetStack = refPathHash(stateSetStack);

    RenderInfoData::State & currentState = _hashedState[hashStateSetStack];

    currentState.stateRef = state;
    currentState.state = new StateAccess;
    static_cast<StateAccess*>(currentState.state.get())->copyFrom(state);
    currentState.userData = renderInfo.getUserData();
    currentState.capturedStateSet = new osg::StateSet;
    RenderInfoData::captureCurrentState(currentState.capturedStateSet.get(), state);

    RenderInfoData::copyStateSetStack(currentState.stateSetStack, stateSetStack);
#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
    RenderInfoData::copyRenderBinStack(currentState.renderBinStack, renderBinStack);
    RenderInfoData::copyCameraStack(currentState.cameraStack, cameraStack);
#else
    //copyPerContextProgramSet(currentState.appliedProgamSet, appliedProgramObjectSet);
#endif
    currentState.combinedStateSet = new osg::StateSet;;
    for (auto it = stateSetStack.begin(); it != stateSetStack.end(); ++it)
    {
        currentState.combinedStateSet->merge(*(*it));
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

CameraCaptureCallback::CameraCaptureCallback(GLenum readBuffer, osg::Image * image, bool depth)
    : _readBuffer(readBuffer)
    , _depth(depth)
    , _image(image)
{
}

void CameraCaptureCallback::operator () (osg::RenderInfo& renderInfo) const
{
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
    glReadBuffer(_readBuffer);
#else
    osg::notify(osg::NOTICE)<<"Error: GLES unable to do glReadBuffer"<<std::endl;
#endif

    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
    if (gc->getTraits())
    {
        GLenum pixelFormat;

        if(_depth)
            pixelFormat = GL_DEPTH_COMPONENT;
        else
        {
            if (gc->getTraits()->alpha)
                pixelFormat = GL_RGBA;
            else
                pixelFormat = GL_RGB;
        }

        osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();
        int width = viewport->width();
        int height = viewport->height();
        _image->readPixels(0, 0, width, height, pixelFormat, _depth?GL_FLOAT:GL_UNSIGNED_BYTE);

        osgDB::writeImageFile(*_image, "/tmp/slave_cam_image_.png");

    }
}

WindowCaptureCallback::ContextData::ContextData(osg::GraphicsContext* gc, Mode mode, GLenum readBuffer)
    : _gc(gc),
    _index(_gc->getState()->getContextID()),
    _mode(mode),
    _readBuffer(readBuffer),
    _pixelFormat(GL_RGBA),
    _type(GL_UNSIGNED_BYTE),
    _width(0),
    _height(0),
    _currentImageIndex(0),
    _currentPboIndex(0),
    _reportTimingFrequency(100),
    _numTimeValuesRecorded(0),
    _timeForReadPixels(0.0),
    _timeForMemCpy(0.0),
    _timeForCaptureOperation(0.0),
    _timeForFullCopy(0.0),
    _timeForFullCopyAndOperation(0.0),
    _previousFrameTick(0)
{
    _previousFrameTick = osg::Timer::instance()->tick();

    osg::NotifySeverity level = osg::INFO;

    if (gc->getTraits())
    {
        if (gc->getTraits()->alpha)
        {
            OSG_NOTIFY(level) << "ScreenCaptureHandler: Selected GL_RGBA read back format" << std::endl;
            _pixelFormat = GL_RGBA;
        }
        else
        {
            OSG_NOTIFY(level) << "ScreenCaptureHandler: Selected GL_RGB read back format" << std::endl;
            _pixelFormat = GL_RGB;
        }
    }

    getSize(gc, _width, _height);

    //OSG_NOTICE<<"Window size "<<_width<<", "<<_height<<std::endl;

    // single buffered image
    _imageBuffer.push_back(new osg::Image);

    // double buffer PBO.
    switch (_mode)
    {
    case(READ_PIXELS):
        OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, without PixelBufferObject." << std::endl;
        break;
    case(SINGLE_PBO):
        OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a single PixelBufferObject." << std::endl;
        _pboBuffer.push_back(0);
        break;
    case(DOUBLE_PBO):
        OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a double buffer PixelBufferObject." << std::endl;
        _pboBuffer.push_back(0);
        _pboBuffer.push_back(0);
        break;
    case(TRIPLE_PBO):
        OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a triple buffer PixelBufferObject." << std::endl;
        _pboBuffer.push_back(0);
        _pboBuffer.push_back(0);
        _pboBuffer.push_back(0);
        break;
    default:
        break;
    }
}

void WindowCaptureCallback::ContextData::getSize(osg::GraphicsContext* gc, int& width, int& height)
{
    if (gc->getTraits())
    {
        width = gc->getTraits()->width;
        height = gc->getTraits()->height;
    }
}

void WindowCaptureCallback::ContextData::updateTimings(osg::Timer_t tick_start,
    osg::Timer_t tick_afterReadPixels,
    osg::Timer_t tick_afterMemCpy,
    osg::Timer_t tick_afterCaptureOperation,
    unsigned int /*dataSize*/)
{
    _timeForReadPixels = osg::Timer::instance()->delta_s(tick_start, tick_afterReadPixels);
    _timeForMemCpy = osg::Timer::instance()->delta_s(tick_afterReadPixels, tick_afterMemCpy);
    _timeForCaptureOperation = osg::Timer::instance()->delta_s(tick_afterMemCpy, tick_afterCaptureOperation);

    _timeForFullCopy = osg::Timer::instance()->delta_s(tick_start, tick_afterMemCpy);
    _timeForFullCopyAndOperation = osg::Timer::instance()->delta_s(tick_start, tick_afterCaptureOperation);
}

void WindowCaptureCallback::ContextData::read()
{
    osg::GLExtensions* ext = osg::GLExtensions::Get(_gc->getState()->getContextID(), true);

    if (ext->isPBOSupported && !_pboBuffer.empty())
    {
        if (_pboBuffer.size() == 1)
        {
            singlePBO(ext);
        }
        else
        {
            multiPBO(ext);
        }
    }
    else
    {
        readPixels();
    }
}


void WindowCaptureCallback::ContextData::readPixels()
{
    unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();
    unsigned int nextPboIndex = _pboBuffer.empty() ? 0 : (_currentPboIndex + 1) % _pboBuffer.size();

    int width = 0, height = 0;
    getSize(_gc, width, height);
    if (width != _width || _height != height)
    {
        //OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
        _width = width;
        _height = height;
    }

    osg::Image* image = _imageBuffer[_currentImageIndex].get();

    osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
    image->readPixels(0, 0, _width, _height,
        _pixelFormat, _type);
#endif

    osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

    if (_captureOperation.valid())
    {
        (*_captureOperation)(*image, _index);
    }

    osg::Timer_t tick_afterCaptureOperation = osg::Timer::instance()->tick();
    updateTimings(tick_start, tick_afterReadPixels, tick_afterReadPixels, tick_afterCaptureOperation, image->getTotalSizeInBytes());

    _currentImageIndex = nextImageIndex;
    _currentPboIndex = nextPboIndex;
}

void WindowCaptureCallback::ContextData::singlePBO(osg::GLExtensions* ext)
{
    unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();

    int width = 0, height = 0;
    getSize(_gc, width, height);
    if (width != _width || _height != height)
    {
        //OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
        _width = width;
        _height = height;
    }

    GLuint& pbo = _pboBuffer[0];

    osg::Image* image = _imageBuffer[_currentImageIndex].get();
    if (image->s() != _width ||
        image->t() != _height)
    {
        //OSG_NOTICE<<"ScreenCaptureHandler: Allocating image "<<std::endl;
        image->allocateImage(_width, _height, 1, _pixelFormat, _type);

        if (pbo != 0)
        {
            //OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<pbo<<std::endl;
            ext->glDeleteBuffers(1, &pbo);
            pbo = 0;
        }
    }


    if (pbo == 0)
    {
        ext->glGenBuffers(1, &pbo);
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
        ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ);

        //OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<pbo<<std::endl;
    }
    else
    {
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
    }

    osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
    glReadPixels(0, 0, _width, _height, _pixelFormat, _type, nullptr);
#endif

    osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

    GLubyte* src = (GLubyte*)ext->glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB,
        GL_READ_ONLY_ARB);
    if (src)
    {
        memcpy(image->data(), src, image->getTotalSizeInBytes());
        ext->glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
    }

    ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);

    osg::Timer_t tick_afterMemCpy = osg::Timer::instance()->tick();

    if (_captureOperation.valid())
    {
        (*_captureOperation)(*image, _index);
    }

    osg::Timer_t tick_afterCaptureOperation = osg::Timer::instance()->tick();
    updateTimings(tick_start, tick_afterReadPixels, tick_afterMemCpy, tick_afterCaptureOperation, image->getTotalSizeInBytes());

    _currentImageIndex = nextImageIndex;
}

void WindowCaptureCallback::ContextData::multiPBO(osg::GLExtensions* ext)
{
    unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();
    unsigned int nextPboIndex = (_currentPboIndex + 1) % _pboBuffer.size();

    int width = 0, height = 0;
    getSize(_gc, width, height);
    if (width != _width || _height != height)
    {
        //OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
        _width = width;
        _height = height;
    }

    GLuint& copy_pbo = _pboBuffer[_currentPboIndex];
    GLuint& read_pbo = _pboBuffer[nextPboIndex];

    osg::Image* image = _imageBuffer[_currentImageIndex].get();
    if (image->s() != _width ||
        image->t() != _height)
    {
        //OSG_NOTICE<<"ScreenCaptureHandler: Allocating image "<<std::endl;
        image->allocateImage(_width, _height, 1, _pixelFormat, _type);

        if (read_pbo != 0)
        {
            //OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<read_pbo<<std::endl;
            ext->glDeleteBuffers(1, &read_pbo);
            read_pbo = 0;
        }

        if (copy_pbo != 0)
        {
            //OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<copy_pbo<<std::endl;
            ext->glDeleteBuffers(1, &copy_pbo);
            copy_pbo = 0;
        }
    }


    bool doCopy = copy_pbo != 0;
    if (copy_pbo == 0)
    {
        ext->glGenBuffers(1, &copy_pbo);
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, copy_pbo);
        ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), nullptr, GL_STREAM_READ);

        //OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<read_pbo<<std::endl;
    }

    if (read_pbo == 0)
    {
        ext->glGenBuffers(1, &read_pbo);
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, read_pbo);
        ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), nullptr, GL_STREAM_READ);

        //OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<read_pbo<<std::endl;
    }
    else
    {
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, read_pbo);
    }

    osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
    glReadPixels(0, 0, _width, _height, _pixelFormat, _type, 0);
#endif

    osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

    if (doCopy)
    {

        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, copy_pbo);

        GLubyte* src = (GLubyte*)ext->glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB,
            GL_READ_ONLY_ARB);
        if (src)
        {
            memcpy(image->data(), src, image->getTotalSizeInBytes());
            ext->glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
        }

        if (_captureOperation.valid())
        {
            (*_captureOperation)(*image, _index);
        }
    }

    ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);

    osg::Timer_t tick_afterMemCpy = osg::Timer::instance()->tick();

    updateTimings(tick_start, tick_afterReadPixels, tick_afterMemCpy, tick_afterMemCpy, image->getTotalSizeInBytes());

    _currentImageIndex = nextImageIndex;
    _currentPboIndex = nextPboIndex;
}

WindowCaptureCallback::WindowCaptureCallback(int numFrames, Mode mode, FramePosition position, GLenum readBuffer)
    : _mode(mode),
    _position(position),
    _readBuffer(readBuffer),
    _numFrames(numFrames)
{
}

WindowCaptureCallback::ContextData* WindowCaptureCallback::createContextData(osg::GraphicsContext* gc) const
{
    WindowCaptureCallback::ContextData* cd = new WindowCaptureCallback::ContextData(gc, _mode, _readBuffer);
    cd->_captureOperation = _defaultCaptureOperation;
    return cd;
}

WindowCaptureCallback::ContextData* WindowCaptureCallback::getContextData(osg::GraphicsContext* gc) const
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    osg::ref_ptr<ContextData>& data = _contextDataMap[gc];
    if (!data) data = createContextData(gc);

    return data.get();
}

void WindowCaptureCallback::setCaptureOperation(CaptureOperation* operation)
{
    _defaultCaptureOperation = operation;

    // Set the capture operation for each ContextData.
    for (ContextDataMap::iterator it = _contextDataMap.begin(); it != _contextDataMap.end(); ++it)
    {
        it->second->_captureOperation = operation;
    }
}


void WindowCaptureCallback::operator () (osg::RenderInfo& renderInfo) const
{
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
    glReadBuffer(_readBuffer);
#endif

    osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
    osg::ref_ptr<ContextData> cd = getContextData(gc);
    cd->read();

    // If _numFrames is > 0 it means capture that number of frames.
    if (_numFrames > 0)
    {
        --_numFrames;
        if (_numFrames == 0)
        {
            // the callback must remove itself when it's done.
            if (_position == START_FRAME)
                renderInfo.getCurrentCamera()->setInitialDrawCallback(nullptr);
            if (_position == END_FRAME)
                renderInfo.getCurrentCamera()->setFinalDrawCallback(nullptr);
        }
    }

    std::streamsize prec = osg::notify(osg::INFO).precision(5);
    OSG_INFO << "ScreenCaptureHandler: "
        << "copy=" << (cd->_timeForFullCopy*1000.0) << "ms, "
        << "operation=" << (cd->_timeForCaptureOperation*1000.0) << "ms, "
        << "total=" << (cd->_timeForFullCopyAndOperation*1000.0) << std::endl;
    osg::notify(osg::INFO).precision(prec);

    cd->_timeForFullCopy = 0;
}


class CaptureImage : public WindowCaptureCallback::CaptureOperation
{
public:
    CaptureImage()
        : _image()
    {
        _mutex.lock();
    }
    ~CaptureImage() override
    {
        // avoid Qt warning
        _mutex.trylock();
        _mutex.unlock();
    }
    osg::Image * takeImage() { return _image.release(); }
public:
    virtual void operator()(const osg::Image& image, const unsigned int context_id) override
    {
        _image = static_cast<osg::Image*>(image.clone(osg::CopyOp::DEEP_COPY_ALL));
        _mutex.unlock();
    }
    bool wait()
    {
        return (_mutex.lock() == 0);
    }
protected:
    osg::ref_ptr<osg::Image> _image;
    OpenThreads::Mutex _mutex;
};

bool captureCameraImage(osg::Camera * camera, osg::ref_ptr<osg::Image> & image, osg::Camera * masterCamera)
{
    bool ret = false;
    image = nullptr;

    Q_ASSERT(camera != nullptr);

    GLenum buffer = GL_FRONT;
    bool modifiedGraphicsContext = false;
    bool useMasterCamera = false;
    if (camera->getGraphicsContext())
        buffer = camera->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
    else if (masterCamera && masterCamera->getGraphicsContext())
    {
        camera->setGraphicsContext(masterCamera->getGraphicsContext());
        modifiedGraphicsContext = true;
        buffer = masterCamera->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
    }

    osg::ref_ptr<osg::Camera::DrawCallback> cb = camera->getFinalDrawCallback();
    WindowCaptureCallback * wndcap = new WindowCaptureCallback();
    wndcap->setReadBuffer(buffer);
    osg::ref_ptr<CaptureImage> handler = new CaptureImage;
    wndcap->setCaptureOperation(handler);
    camera->setFinalDrawCallback(wndcap);

    osgViewer::View* view2 = dynamic_cast<osgViewer::View*>(camera->getView());
    if (!view2 && masterCamera)
    {
        view2 = dynamic_cast<osgViewer::View*>(masterCamera->getView());
        useMasterCamera = true;
    }
    osgViewer::ViewerBase * viewerbase = view2 ? view2->getViewerBase() : nullptr;

    if (viewerbase)
    {
        if (useMasterCamera)
        {
            // Do rendering with capture callback
            view2->addSlave(camera, false);
            viewerbase->renderingTraversals();
            unsigned idx = view2->findSlaveIndexForCamera(camera);
            view2->removeSlave(idx);
        }
        else
        {
            if (view2)
            {
                bool stopThreads = false;
                if (!viewerbase->areThreadsRunning())
                {
                    viewerbase->startThreading();
                    stopThreads = true;
                }
                if (viewerbase->getThreadingModel() != osgViewer::ViewerBase::SingleThreaded)
                    view2->requestRedraw();
                else
                    viewerbase->renderingTraversals();
                handler->wait();
                if (stopThreads)
                    viewerbase->stopThreading();
                image = handler->takeImage();
                ret = image.valid();
            }
        }
    }

    camera->setFinalDrawCallback(cb.get());
    return ret;
}

bool captureViewImage(osg::View * view, osg::ref_ptr<osg::Image> & image)
{
    bool ret = false;
    image = nullptr;

    osg::Camera * camera = view->getCamera();
    if (camera)
    {
        GLenum buffer = GL_FRONT;
        if (camera->getGraphicsContext())
            buffer = camera->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;

        osg::ref_ptr<osg::Camera::DrawCallback> cb = camera->getFinalDrawCallback();
        osg::ref_ptr<WindowCaptureCallback> wndcap = new WindowCaptureCallback();
        wndcap->setMode(WindowCaptureCallback::SINGLE_PBO);
        wndcap->setReadBuffer(buffer);
        osg::ref_ptr<CaptureImage> handler = new CaptureImage;
        wndcap->setCaptureOperation(handler);
        camera->setFinalDrawCallback(wndcap);

        osgViewer::View* view2 = dynamic_cast<osgViewer::View*>(view);
        osgViewer::ViewerBase * viewerbase = view2 ? view2->getViewerBase() : nullptr;

        if (view2)
        {
            bool stopThreads = false;
            if (!viewerbase->areThreadsRunning())
            {
                viewerbase->startThreading();
                stopThreads = true;
            }
            if (viewerbase->getThreadingModel() != osgViewer::ViewerBase::SingleThreaded)
            {
                view2->requestRedraw();
                handler->wait();
            }
            else
                viewerbase->renderingTraversals();
            if (stopThreads)
                viewerbase->stopThreading();
            image = handler->takeImage();
            ret = image.valid();
        }

        camera->setFinalDrawCallback(cb.get());
    }
    return ret;
}

bool convertTextureToImage(osg::Camera * masterCamera, osg::Texture * texture, osg::ref_ptr<osg::Image> & image)
{
    osg::ref_ptr<osg::Camera> slaveCamera = new osg::Camera;
    slaveCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    slaveCamera->setGraphicsContext(masterCamera->getGraphicsContext());
    slaveCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    slaveCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    slaveCamera->setAllowEventFocus(false);
    slaveCamera->setDrawBuffer(GL_FRONT);
    slaveCamera->setReadBuffer(GL_FRONT);
    slaveCamera->setProjectionResizePolicy(osg::Camera::FIXED);
    //slaveCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    slaveCamera->setCullingMode(osg::CullSettings::NO_CULLING);
    slaveCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    slaveCamera->setRenderOrder(osg::Camera::NESTED_RENDER);
    slaveCamera->setViewport(0, 0, texture->getTextureWidth(), texture->getTextureHeight());
    //slaveCamera->setProjectionMatrixAsOrtho2D(-texture->getTextureWidth() / 2, texture->getTextureWidth() / 2, -texture->getTextureHeight() / 2, texture->getTextureHeight() / 2);
    //slaveCamera->setProjectionMatrixAsOrtho2D(-2.0, 2.0, -2.0, 2.0);
    //slaveCamera->setProjectionMatrixAsOrtho2D(-1.0, 1.0, -1.0, 1.0);
    osg::ref_ptr<osg::Geometry> geom = osg_helpers::createGeometryForTexture(texture);

    osg::ComputeBoundsVisitor v;
    geom->accept(v);
    osg::BoundingBox bb = v.getBoundingBox();

    slaveCamera->setProjectionMatrixAsOrtho2D(bb.xMin(), bb.xMax(), bb.zMin(), bb.zMax());


    osg::Vec3d center(0, 0, 0.0);
    osg::Vec3d eye(0.0, -10.0, 0.0);
    osg::Vec3d up(0, 0, 1);

    osg::Matrixd matrix;
    matrix.makeLookAt(eye, center, up);
    slaveCamera->setViewMatrix(matrix);

    slaveCamera->addChild(geom);

    image = new osg::Image;
    //image->allocateImage(texture->getTextureWidth(), texture->getTextureHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE);
    //slaveCamera->attach(osg::Camera::COLOR_BUFFER, image.get(), 0, 0);

    osgDB::writeNodeFile(*slaveCamera, "/tmp/slave_cam.osgb");
    osgDB::writeNodeFile(*geom, "/tmp/slave_cam_geom.osgb");

    std::cout << "convertTextureToImage " << masterCamera << " txt=" << texture << " img=" << image.get() << std::endl;

    GLenum buffer = GL_FRONT;
    //GLenum buffer = masterCamera->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
    slaveCamera->setFinalDrawCallback(new CameraCaptureCallback(buffer, image, false));

    osgViewer::View* view = dynamic_cast<osgViewer::View*>(masterCamera->getView());
    osgViewer::ViewerBase * viewer = view ? view->getViewerBase() : nullptr;

    if (viewer)
    {
        std::cout << "convertTextureToImage viewer=" << viewer << std::endl;

        // Do rendering with capture callback
        masterCamera->addChild(slaveCamera);
        view->addSlave(slaveCamera, false);
        viewer->renderingTraversals();
        unsigned idx = view->findSlaveIndexForCamera(slaveCamera);
        view->removeSlave(idx);
        masterCamera->removeChild(slaveCamera);

        osgDB::writeImageFile(*image, "/tmp/slave_cam_image.png");
    }
    slaveCamera->setFinalDrawCallback(nullptr);
    return true;
}

} // namespace osg_plugin
} // namespace sgi
