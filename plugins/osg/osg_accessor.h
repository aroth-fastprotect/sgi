#pragma once
#include <osg/Version>
#include <osg/ShaderComposer>
#include <osg/OperationThread>
#include <osg/ClipNode>
#include <osg/Texture>

namespace sgi {
namespace osg_plugin {

class NodeAccess : public osg::Node
{
public:
    bool                            isBoundingSphereComputed() const { return _boundingSphereComputed; }

    /** Get the bounding sphere of node.
        Using lazy evaluation computes the bounding sphere if it is 'dirty'.*/
    inline const osg::BoundingSphere& getBoundNoCompute() const
    {
        if(_boundingSphereComputed)
	        return _boundingSphere;
		else
			return _initialBound;
    }

    void    setNumUpdateTraversal(unsigned num) { osg::Node::setNumChildrenRequiringUpdateTraversal(num); }
    void    setNumEventTraversal(unsigned num) { osg::Node::setNumChildrenRequiringEventTraversal(num); }
};

class ClipNodeAccess : public osg::ClipNode
{
public:
    osg::StateAttribute::GLModeValue value() const { return _value; }
};

class ShaderComposerAccess : public osg::ShaderComposer
{
public:
    typedef std::map< osg::ShaderComponents, osg::ref_ptr<osg::Program> > ProgramMap;
    typedef std::map< Shaders, osg::ref_ptr<osg::Shader> > ShaderMainMap;
    
    const ProgramMap & programMap() const { return _programMap; }
    const ShaderMainMap & shaderMainMap() const { return _shaderMainMap; }
};

class StatsAccess : public osg::Stats
{
public:
    void getCollectStats(CollectMap & c)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        c = _collectMap;
    }
};

class OperationQueueAccess : public osg::OperationQueue
{
public:
    typedef std::list< osg::ref_ptr<osg::Operation> > Operations;

    void getOperations(Operations & ops)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_operationsMutex);
        ops = _operations;
    }
};

class TextureAccess : public osg::Texture
{
public:
    typedef std::vector< osg::ref_ptr<TextureObject> > TextureObjectList;
    TextureObjectList getTextureObjectList() const;
};

class TextureObjectManagerAccess : 
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    public osg::TextureObjectManager
#else
    public osg::Texture::TextureObjectManager
#endif
{
public:
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    typedef std::map< osg::Texture::TextureProfile, osg::ref_ptr<osg::TextureObjectSet> > TextureSetMap;
#else
    typedef std::map< osg::Texture::TextureProfile, osg::ref_ptr<osg::Texture::TextureObjectSet> > TextureSetMap;
#endif
    const TextureSetMap & getTextureSetMap() const {
        return _textureSetMap;
    }

    static unsigned hash(const osg::Texture::TextureProfile & profile);
    static std::string shortName(const osg::Texture::TextureProfile & profile);
};

class TextureObjectSetAccess :
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    public osg::TextureObjectSet
#else
    public osg::Texture::TextureObjectSet
#endif
{
public:
    typedef osg::Texture::TextureObject TextureObject;
    typedef std::list<osg::ref_ptr<TextureObject> > TextureObjectList;

    TextureObjectList getTextureObjects();
};

class GraphicsContextAccess : public osg::GraphicsContext
{
public:
    unsigned getContextID() const {
        if (_state.valid())
            return _state->getContextID();
        else
            return ~0u;
    }
};

} // namespace osg_plugin
} // namespace sgi

