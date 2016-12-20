#pragma once

#include <osg/Geometry>
#include <osg/Program>

namespace osgUtil {
    class RenderBin;
}

namespace sgi {
namespace osg_plugin {

class DisabledDrawCallback : public osg::Drawable::DrawCallback
{
public:
    DisabledDrawCallback(osg::Drawable * drawable)
    {
        _oldCallback = drawable->getDrawCallback();
        drawable->setDrawCallback(this);
    }

    void remove(osg::Drawable * drawable)
    {
        drawable->setDrawCallback(_oldCallback.get());
        _oldCallback = NULL;
    }
    static bool isDisabled(osg::Drawable * drawable)
    {
        osg::Drawable::DrawCallback * callback = drawable->getDrawCallback();
        return dynamic_cast<DisabledDrawCallback*>(callback) != NULL;
    }
    static void disable(osg::Drawable * drawable, bool disable)
    {
        osg::Drawable::DrawCallback * callback = drawable->getDrawCallback();
        osg::ref_ptr<DisabledDrawCallback> disabledCallback = dynamic_cast<DisabledDrawCallback*>(callback);
        if(disable && !disabledCallback)
            new DisabledDrawCallback(drawable);
        else if(!disable && disabledCallback)
            disabledCallback->remove(drawable);
    }

    virtual void drawImplementation(osg::RenderInfo& /*renderInfo*/,const osg::Drawable* /*drawable*/) const
    {
        // do nothing to hide/disable the drawable
    }
private:
    osg::ref_ptr<osg::Drawable::DrawCallback>   _oldCallback;
};

class RenderInfo
{
private:
    RenderInfo();
    RenderInfo(const RenderInfo & rhs);

public:
    static bool isPresent(osg::Node * node);
    static bool enable(osg::Node * node, bool enable);

    static bool hasDrawCallback(osg::Drawable* node);
    static bool installDrawCallback(osg::Drawable* node, bool enable);
};

class RenderInfoData
{
public:
    RenderInfoData();

public:
    typedef osg::ref_ptr<osg::StateSet> StateSetPtr;
    typedef std::vector<StateSetPtr> StateSetStack;

    typedef osg::ref_ptr<osgUtil::RenderBin> RenderBinPtr;
    typedef std::vector<RenderBinPtr> RenderBinStack;

    typedef osg::ref_ptr<osg::Camera> CameraPtr;
    typedef std::vector<CameraPtr> CameraStack;

    typedef osg::ref_ptr<osg::Program::PerContextProgram> PerContextProgramPtr;
    typedef std::set<PerContextProgramPtr> PerContextProgramSet;

    struct AttributeStack
    {
        AttributeStack()
        {
            changed = false;
            last_applied_attribute = 0L;
            last_applied_shadercomponent = 0L;
            global_default_attribute = 0L;

        }

        /** apply an attribute if required, passing in attribute and appropriate attribute stack */
        bool                    changed;
        const osg::StateAttribute*   last_applied_attribute;
        const osg::ShaderComponent*  last_applied_shadercomponent;
        osg::ref_ptr<const osg::StateAttribute> global_default_attribute;
        //AttributeVec            attributeVec;
    };

    typedef std::map<osg::StateAttribute::TypeMemberPair, AttributeStack> AttributeMap;

    struct State {
        osg::ref_ptr<osg::State> state;
        StateSetStack stateSetStack;
        RenderBinStack renderBinStack;
        CameraStack cameraStack;
        PerContextProgramSet appliedProgamSet;
        osg::ref_ptr<osg::View> view;
        osg::ref_ptr<osg::StateSet> capturedStateSet;
        osg::ref_ptr<osg::StateSet> combinedStateSet;
    };

    typedef std::map<unsigned, State> HashedState;
    /*
    const StateSetStack & lastStateSetStack() const { return _lastStateSetStack; }
    const RenderBinStack & lastRenderBinStack() const { return _lastRenderBinStack; }
    const CameraStack & lastCameraStack() const { return _lastCameraStack; }
    const osg::View * lastView() const { return _lastView.get(); }
    */
    const HashedState & hashedState() const { return _hashedState; }

public:
    void copyRenderInfo(osg::RenderInfo & info);

protected:
    static void copyStateSetStack(StateSetStack & dest, const std::vector<const osg::StateSet*> & src);
    static void copyRenderBinStack(RenderBinStack & dest, const std::vector<osgUtil::RenderBin*> & src);
    static void copyCameraStack(CameraStack & dest, const std::vector<osg::Camera*> & src);
    static void copyPerContextProgramSet(PerContextProgramSet & dest, const std::set<const osg::Program::PerContextProgram* > & src);
    static void captureCurrentState(osg::StateSet * dest, osg::State * src);

protected:
    /*
    StateSetStack _lastStateSetStack;
    RenderBinStack _lastRenderBinStack;
    CameraStack _lastCameraStack;
    osg::ref_ptr<osg::View> _lastView;
    */
    HashedState _hashedState;
};

class RenderInfoDrawable : public osg::Drawable
{
public:
    RenderInfoDrawable();
    RenderInfoDrawable(const RenderInfoDrawable & rhs, const osg::CopyOp & copyOp=osg::CopyOp::SHALLOW_COPY);

    META_Object(sgi-osg, RenderInfoDrawable);

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

    const RenderInfoData & data() const;

protected:
    RenderInfoData _data;
};

class RenderInfoGeometry : public osg::Geometry
{
public:
    RenderInfoGeometry(osg::Geometry * geometry=NULL);
    RenderInfoGeometry(const RenderInfoGeometry & rhs, const osg::CopyOp & copyOp = osg::CopyOp::SHALLOW_COPY);

    META_Object(sgi-osg, RenderInfoGeometry);

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

    const RenderInfoData & data() const;

protected:
    void setChildGeometry(osg::Geometry * geometry);

protected:
    RenderInfoData _data;
    osg::ref_ptr<osg::Geometry> _childGeometry;
};

class RenderInfoDrawCallback : public osg::Drawable::DrawCallback
{
public:
    RenderInfoDrawCallback();

    void drawImplementation(osg::RenderInfo& /*renderInfo*/, const osg::Drawable* /*drawable*/) const override;

    const RenderInfoData & data() const;

protected:
    RenderInfoData _data;
};

class CameraCaptureCallback : public osg::Camera::DrawCallback
{
public:
    CameraCaptureCallback(GLenum readBuffer, osg::Image *image, bool depth);
    void operator () (osg::RenderInfo& renderInfo) const override;

protected:
    GLenum _readBuffer;
    mutable OpenThreads::Mutex  _mutex;
    bool _depth;
    osg::ref_ptr<osg::Image> _image;
};

osg::Geometry* createGeometryForImage(osg::Image* image,float s,float t);
osg::Geometry * createGeometryForImage(osg::Image* image);
osg::Geometry * createGeometryForTexture(osg::Texture* texture);
bool convertTextureToImage(osg::Camera * masterCamera, osg::Texture * texture, osg::ref_ptr<osg::Image> & image);
bool captureCameraImage(osg::Camera * camera, osg::ref_ptr<osg::Image> & image, osg::Camera * masterCamera=NULL);

} // namespace osg_plugin
} // namespace sgi
