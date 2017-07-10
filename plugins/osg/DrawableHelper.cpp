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
        StateSetEntry & entry = dest[dest_index];
        entry.stateSet = (osg::StateSet*)src_ss->clone(osg::CopyOp::DEEP_COPY_ALL);
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
            const osg::StateAttribute::TypeMemberPair & typemem = it2->first;
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
        StateAccess * src = (StateAccess*)src_;

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
    StateAccess * state = (StateAccess*)renderInfo.getState();
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

bool captureCameraImage(osg::Camera * camera, osg::ref_ptr<osg::Image> & image, osg::Camera * masterCamera)
{
    image = new osg::Image;

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
    camera->setFinalDrawCallback(new CameraCaptureCallback(buffer, image, false));

    osgViewer::View* view = dynamic_cast<osgViewer::View*>(camera->getView());
    if (!view && masterCamera)
    {
        view = dynamic_cast<osgViewer::View*>(masterCamera->getView());
        useMasterCamera = true;
    }
    osgViewer::ViewerBase * viewer = view ? view->getViewerBase() : nullptr;

    if (viewer)
    {
        if (useMasterCamera)
        {
            // Do rendering with capture callback
            view->addSlave(camera, false);
            viewer->renderingTraversals();
            unsigned idx = view->findSlaveIndexForCamera(camera);
            view->removeSlave(idx);
        }
        else
        {
            // Do rendering with capture callback
            viewer->renderingTraversals();
        }
    }
//     if (modifiedGraphicsContext)
//         camera->setGraphicsContext(nullptr);
    camera->setFinalDrawCallback(cb.get());
    return true;
}

osg::Geometry* createImageGeometry(float s,float t, osg::Image::Origin origin, osg::Texture * texture)
{
    osg::Geometry* geom = NULL;
    float y = 1.0;
    float x = y*(s/t);

    float texcoord_y_b = (origin == osg::Image::BOTTOM_LEFT) ? 0.0f : 1.0f;
    float texcoord_y_t = (origin == osg::Image::BOTTOM_LEFT) ? 1.0f : 0.0f;
    float texcoord_x = 1.0f;

    // set up the drawstate.
    osg::StateSet* dstate = new osg::StateSet;
    dstate->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
    dstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    dstate->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

    geom = new osg::Geometry;
    geom->setStateSet(dstate);

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0].set(-x,0.0f,y);
    (*coords)[1].set(-x,0.0f,-y);
    (*coords)[2].set(x,0.0f,-y);
    (*coords)[3].set(x,0.0f,y);
    geom->setVertexArray(coords);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f*texcoord_x,texcoord_y_t);
    (*tcoords)[1].set(0.0f*texcoord_x,texcoord_y_b);
    (*tcoords)[2].set(1.0f*texcoord_x,texcoord_y_b);
    (*tcoords)[3].set(1.0f*texcoord_x,texcoord_y_t);
    geom->setTexCoordArray(0,tcoords);

    osg::Vec4Array* colours = new osg::Vec4Array(1);
    (*colours)[0].set(1.0f,1.0f,1.0,1.0f);
    geom->setColorArray(colours, osg::Array::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    return geom;
}

osg::Geometry* createGeometryForImage(osg::Image* image,float s,float t)
{
    osg::Geometry* geom = NULL;
    if (image && s>0 && t>0)
    {
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
        texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
        texture->setResizeNonPowerOfTwoHint(false);
        geom = createImageGeometry(s, t, image->getOrigin(), texture);
    }
    return geom;
}
osg::Geometry * createGeometryForImage(osg::Image* image)
{
    return createGeometryForImage(image,image->s(),image->t());
}

osg::Geometry * createGeometryForTexture(osg::Texture* texture)
{
    return createImageGeometry(texture->getTextureWidth(), texture->getTextureHeight(), osg::Image::BOTTOM_LEFT, texture);
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
    osg::ref_ptr<osg::Geometry> geom = createGeometryForTexture(texture);

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

    if(viewer)
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
    slaveCamera->setFinalDrawCallback(0);
    return true;
}

} // namespace osg_plugin
} // namespace sgi
