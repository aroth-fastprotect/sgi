#pragma once

#include <osg/Version>
#include <osg/Node>
#include <osg/Drawable>

namespace sgi {
namespace osg_plugin {

namespace details {

struct NoParams { };

template<typename OBJECT_TYPE, typename OPERATION_TYPE, typename OPERATION_PARAM_TYPE>
struct invokeObjectUpdateCallbackT {
    void operator()(OBJECT_TYPE * object, osg::NodeVisitor* nv, const OPERATION_PARAM_TYPE & param)
    {
        OPERATION_TYPE op(param);
        op(object, nv);
    }
};

template<typename OBJECT_TYPE, typename OPERATION_TYPE>
struct invokeObjectUpdateCallbackT<OBJECT_TYPE, OPERATION_TYPE, NoParams> {
    void operator()(OBJECT_TYPE * object, osg::NodeVisitor* nv, const NoParams & param)
    {
        OPERATION_TYPE op;
        op(object, nv);
    }
};

template<typename OBJECT_TYPE, typename OPERATION_TYPE, typename OPERATION_PARAM_TYPE>
class ObjectUpdateCallbackT : public osg::Referenced
{
public:
    ObjectUpdateCallbackT(OBJECT_TYPE * object)
    {
        // NOP
    }

protected:
    virtual ~ObjectUpdateCallbackT() {}
};

template<typename OPERATION_TYPE, typename OPERATION_PARAM_TYPE>
class ObjectUpdateCallbackT<osg::Drawable, OPERATION_TYPE, OPERATION_PARAM_TYPE> : public osg::Drawable::UpdateCallback
{
public:
    ObjectUpdateCallbackT(osg::Drawable * drawable, const OPERATION_PARAM_TYPE & param)
        : osg::Drawable::UpdateCallback()
        , _param(param)
    {
        _oldCallback = drawable->getUpdateCallback();
        drawable->setUpdateCallback(this);
    }
    virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable)
    {
        // first let the old update callback know that something happened
        if(_oldCallback.valid())
            _oldCallback->update(nv, drawable);

        invokeObjectUpdateCallbackT<osg::Drawable, OPERATION_TYPE, OPERATION_PARAM_TYPE> op;
        op(drawable, nv, _param);

        // remove ourself and put back the old callback
        drawable->setUpdateCallback(_oldCallback.get());
        _oldCallback = NULL;
    }

private:
    osg::ref_ptr<osg::Drawable::UpdateCallback>   _oldCallback;
    OPERATION_PARAM_TYPE _param;
};

template<typename OPERATION_TYPE, typename OPERATION_PARAM_TYPE>
class ObjectUpdateCallbackT<osg::Node, OPERATION_TYPE, OPERATION_PARAM_TYPE>
#if OSG_VERSION_GREATER_THAN(3,3,1)
    : public osg::Callback
#else
    : public osg::NodeCallback
#endif
{
public:
    ObjectUpdateCallbackT(osg::Node * node, const OPERATION_PARAM_TYPE & param)
#if OSG_VERSION_GREATER_THAN(3,3,1)
        : osg::Callback()
#else
        : osg::NodeCallback()
#endif
        , _param(param)
    {
        _oldCallback = node->getUpdateCallback();
        node->setUpdateCallback(this);
    }
#if OSG_VERSION_GREATER_THAN(3,3,1)
    virtual bool run(osg::Object* object, osg::Object * data)
    {
        // first let the old update callback know that something happened
        if(_oldCallback.valid())
            _oldCallback->run(object, data);

        invokeObjectUpdateCallbackT<osg::Node, OPERATION_TYPE, OPERATION_PARAM_TYPE> op;
        op(static_cast<osg::Node*>(object), dynamic_cast<osg::NodeVisitor*>(data), _param);

        static_cast<osg::Node*>(object)->setUpdateCallback(_oldCallback.get());
        _oldCallback = NULL;
        // note, callback is responsible for scenegraph traversal so
        // they must call traverse(node,nv) to ensure that the
        // scene graph subtree (and associated callbacks) are traversed.
        return osg::Callback::run(object, data);
    }
#else
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        // first let the old update callback know that something happened
        if(_oldCallback.valid())
            (*_oldCallback.get())(node, nv);

        invokeObjectUpdateCallbackT<osg::Node, OPERATION_TYPE, OPERATION_PARAM_TYPE> op;
        op(node, nv, _param);

        node->setUpdateCallback(_oldCallback.get());
        _oldCallback = NULL;
        // note, callback is responsible for scenegraph traversal so
        // they must call traverse(node,nv) to ensure that the
        // scene graph subtree (and associated callbacks) are traversed.
        traverse(node,nv);
    }
#endif

private:
#if OSG_VERSION_GREATER_THAN(3,3,1)
    osg::ref_ptr<osg::Callback>   _oldCallback;
#else
    osg::ref_ptr<osg::NodeCallback>   _oldCallback;
#endif
    OPERATION_PARAM_TYPE _param;
};

} // namespace details

template<typename OPERATION_TYPE, typename OBJECT_TYPE>
inline void manipulateObject(OBJECT_TYPE * object)
{
    new details::ObjectUpdateCallbackT<OBJECT_TYPE, OPERATION_TYPE, details::NoParams>(object, details::NoParams());
}

template<typename OPERATION_TYPE, typename OBJECT_TYPE, typename OPERATION_PARAM_TYPE>
inline void manipulateObject(OBJECT_TYPE * object, const OPERATION_PARAM_TYPE & param)
{
    new details::ObjectUpdateCallbackT<OBJECT_TYPE, OPERATION_TYPE, OPERATION_PARAM_TYPE>(object, param);
}

} // namespace osg_plugin
} // namespace sgi
