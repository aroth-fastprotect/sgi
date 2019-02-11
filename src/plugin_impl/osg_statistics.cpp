#include <ostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <stdint.h>

#include <sgi/helpers/osg_statistics>
#include <sgi/helpers/osg>

#include <osg/PagedLOD>
#include <osg/ProxyNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/CoordinateSystemNode>
#include <osg/CameraView>
#include <osgAnimation/AnimationManagerBase>
#include <osgAnimation/Skeleton>
#include <osgAnimation/Bone>
#include <osgText/TextBase>

namespace sgi {
namespace osg_helpers {

MemoryStatisticsVisitor::Numbers::Numbers()
    : totalSize(0)
    , textureMemorySize(0)
    , primitivesMemorySize(0)
{
}

void MemoryStatisticsVisitor::Numbers::clear()
{
    totalSize = 0;
    textureMemorySize = 0;
    primitivesMemorySize = 0;
}

MemoryStatisticsVisitor::MemoryStatisticsVisitor(unsigned int contextID, TraversalMode tm)
    : osg::NodeVisitor(tm)
    , _contextID(contextID)
{
}
MemoryStatisticsVisitor::~MemoryStatisticsVisitor()
{
}

void MemoryStatisticsVisitor::apply(osg::Node& node)
{
#if 0
    if(node.getStateSet())
        apply(*node.getStateSet());
#endif
    osg::NodeVisitor::apply(node);
}

void MemoryStatisticsVisitor::apply(osg::Drawable& node)
{
    _numbers.primitivesMemorySize += node.getGLObjectSizeHint();
    osg::NodeVisitor::apply(node);
}

void MemoryStatisticsVisitor::apply(osg::BufferData& buffer)
{
    if(buffer.asImage())
        _numbers.textureMemorySize += buffer.getTotalDataSize();
    else if(buffer.asPrimitiveSet())
        _numbers.primitivesMemorySize += buffer.getTotalDataSize();
    else
        _numbers.totalSize += buffer.getTotalDataSize();
}

void MemoryStatisticsVisitor::apply(osg::Texture::TextureObject & to)
{
    _numbers.textureMemorySize += to._profile._size;
}

void MemoryStatisticsVisitor::apply(osg::Uniform& uniform)
{
    {
        osg::FloatArray * a = uniform.getFloatArray();
        if(a)
            _numbers.totalSize += a->getTotalDataSize();
    }
    {
        osg::DoubleArray * a = uniform.getDoubleArray();
        if(a)
            _numbers.totalSize += a->getTotalDataSize();
    }
    {
        osg::IntArray * a = uniform.getIntArray();
        if(a)
            _numbers.totalSize += a->getTotalDataSize();
    }
    {
        osg::UIntArray * a = uniform.getUIntArray();
        if(a)
            _numbers.totalSize += a->getTotalDataSize();
    }
}

void MemoryStatisticsVisitor::apply(osg::StateAttribute& attr)
{
    switch(attr.getType())
    {
    case osg::StateAttribute::TEXTURE:
        {
            osg::Texture * texture = attr.asTexture();
            if(texture)
            {
                bool got_txtobj = false;
                if (_contextID != ~0u)
                {
                    osg::Texture::TextureObject * to = texture->getTextureObject(_contextID);
                    if (to)
                    {
                        got_txtobj = true;
                        apply(*to);
                    }
                }
                if (!got_txtobj)
                {
                    unsigned int numImages = texture->getNumImages();
                    for (unsigned n = 0; n < numImages; n++)
                    {
                        osg::Image * image = texture->getImage(n);
                        if (image)
                            apply(*image);
                    }
                }
            }
        }
        break;
    case osg::StateAttribute::PROGRAM:
        {
            osg::Program * program = dynamic_cast<osg::Program*>(&attr);
            if (program)
            {
            }
        }
        break;
    }
}


void MemoryStatisticsVisitor::apply(osg::StateSet& ss)
{
#if 0
    const osg::StateSet::AttributeList& attributes = ss.getAttributeList();
    for(osg::StateSet::AttributeList::const_iterator it = attributes.begin(); it != attributes.end(); it++)
    {
        const osg::StateAttribute::TypeMemberPair & typememberpair = it->first;
        const osg::StateAttribute::Type & type = typememberpair.first;
        const unsigned int & member = typememberpair.second;
        const osg::StateSet::RefAttributePair & attrpair = it->second;
        const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
        const osg::StateAttribute::OverrideValue & value = attrpair.second;
        apply(*attr);
    }

    const osg::StateSet::TextureAttributeList & textureAttributes = ss.getTextureAttributeList();
    for(osg::StateSet::TextureAttributeList::const_iterator it = textureAttributes.begin(); it != textureAttributes.end(); it++)
    {
        const osg::StateSet::AttributeList & attributes = *it;
        for(osg::StateSet::AttributeList::const_iterator it2 = attributes.begin(); it2 != attributes.end(); it2++)
        {
            const osg::StateAttribute::TypeMemberPair & typememberpair = it2->first;
            const osg::StateAttribute::Type & type = typememberpair.first;
            const unsigned int & member = typememberpair.second;
            const osg::StateSet::RefAttributePair & attrpair = it2->second;
            const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
            const osg::StateAttribute::OverrideValue & value = attrpair.second;

            apply(*attr);
        }
    }

    const osg::StateSet::UniformList & uniforms = ss.getUniformList();
    for(osg::StateSet::UniformList::const_iterator it = uniforms.begin(); it != uniforms.end(); it++)
    {
        const std::string & name = it->first;
        const osg::StateAttribute::OverrideValue & overridevalue = it->second.second;
        const osg::ref_ptr<osg::Uniform> & uniform = it->second.first;
        apply(*uniform);
    }
#endif
}


StatisticsVisitor::StatisticsVisitor(unsigned int contextID)
    : osgUtil::StatsVisitor()
    , _contextID(contextID)
    , _numInstancedPagedLOD(0)
    , _numInstancedProxyNode(0)
    , _numInstancedCSN(0)
    , _numInstancedCamera(0)
    , _numInstancedCameraView(0)
    , _numInstancedMatrixTransform(0)
    , _numInstancedPAT(0)
    , _numUpdateCallbacks(0)
    , _numEventCallbacks(0)
    , _numCullCallbacks(0)
    , _numCullInactive(0)
    , _numCullDisabled(0)
    , _numAnimationCallbacks(0)
    , _numInstancedStateAttribute(0)
    , _numInstancedUniform(0)
    , _numInstancedAnimationSkeleton(0)
    , _numInstancedAnimationBone(0)
    , _numInstancedTextBase(0)
    , _numDeprecatedDataGeometries(0)
    , _numInstancedVBODrawables(0)
    , _numInstancedDisplayListDrawables(0)
    , _ignoreKnownPagedLODs(false)
    , _ignoreKnownProxyNodes(false)
    , _uniqueMemory(nullptr)
    , _instancedMemory(nullptr)
{
    for(int n = 0; n < MaxStateAttributeType; n++)
        _numInstancedSA[n] = 0;
}

StatisticsVisitor::~StatisticsVisitor()
{
    if (_uniqueMemory)
        delete _uniqueMemory;
    if (_instancedMemory)
        delete _instancedMemory;
}

void StatisticsVisitor::ignoreKnownPagedLODs(bool enable)
{
    _ignoreKnownPagedLODs = enable;
}

void StatisticsVisitor::ignoreKnownProxyNodes(bool enable)
{
    _ignoreKnownProxyNodes = enable;
}

void StatisticsVisitor::reset()
{
    _numInstancedPagedLOD = 0;
    _numInstancedProxyNode = 0;
    _numInstancedCSN = 0;
    _numInstancedCamera = 0;
    _numInstancedCameraView = 0;
    _numInstancedMatrixTransform = 0;
    _numInstancedPAT = 0;
    _numInstancedSceneGraphNode = 0;
    _numInstancedAnimationSkeleton = 0;
    _numInstancedAnimationBone = 0;
    _numInstancedTextBase = 0;
    _numDeprecatedDataGeometries = 0;
    _numInstancedVBODrawables = 0;
    _numInstancedDisplayListDrawables = 0;

    _numUpdateCallbacks = 0;
    _numEventCallbacks = 0;
    _numCullCallbacks = 0;
    _numCullInactive = 0;
    _numCullDisabled = 0;
    _numAnimationCallbacks = 0;

    _numInstancedStateAttribute = 0;
    for(int n = 0; n < MaxStateAttributeType; n++)
        _numInstancedSA[n] = 0;
    _numInstancedUniform = 0;

    delete _uniqueMemory;
    _uniqueMemory = nullptr;

    delete _instancedMemory;
    _instancedMemory = nullptr;

    _ignoreKnownPagedLODs = false;
    _ignoreKnownProxyNodes = false;

    _pagedlodSet.clear();
    _proxynodeSet.clear();
    _csnSet.clear();
    _cameraSet.clear();
    _cameraViewSet.clear();
    _matrixTransformSet.clear();
    _patSet.clear();
    for(int n = 0; n < MaxStateAttributeType; n++)
        _stateAttributeSets[n].clear();
    _uniformSet.clear();
    _animationSkeletonSet.clear();
    _animationBoneSet.clear();
    _textBaseSet.clear();
    _vboDrawableSet.clear();
    _displayListDrawableSet.clear();

}

void StatisticsVisitor::updateNodeStats(osg::Node & node)
{
#if OSG_VERSION_GREATER_THAN(3,3,1)
    osg::Callback * cb;
#else
    osg::NodeCallback * cb;
#endif
    cb = node.getUpdateCallback();
    while(cb)
    {
        _numUpdateCallbacks++;
        updateCallbackStats(cb);
        cb = cb->getNestedCallback();
    }
    cb = node.getEventCallback();
    while(cb)
    {
        _numEventCallbacks++;
        updateCallbackStats(cb);
        cb = cb->getNestedCallback();
    }
    cb = node.getCullCallback();
    while(cb)
    {
        _numCullCallbacks++;
        updateCallbackStats(cb);
        cb = cb->getNestedCallback();
    }
    if(!node.isCullingActive())
        _numCullInactive++;
    if(!node.getCullingActive())
        _numCullDisabled++;
}

#if OSG_VERSION_GREATER_THAN(3,3,1)
void StatisticsVisitor::updateCallbackStats(osg::Callback * cb)
#else
void StatisticsVisitor::updateCallbackStats(osg::NodeCallback * cb)
#endif
{
    osgAnimation::AnimationManagerBase* anibase = dynamic_cast<osgAnimation::AnimationManagerBase*>(cb);
    if(anibase)
        _numAnimationCallbacks++;
}

void StatisticsVisitor::apply(osg::Node& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::Group& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::Transform& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::LOD& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::Switch& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::Geode& node)
{
    osgUtil::StatsVisitor::apply(node);
    updateNodeStats(node);
}

void StatisticsVisitor::apply(osg::Drawable& node)
{
    osgUtil::StatsVisitor::apply(node);
    if (_contextID != ~0u)
    {
        if (!_instancedMemory)
            _instancedMemory = new MemoryStatisticsVisitor(_contextID);
        _instancedMemory->apply(node);
    }

    if (node.getUseVertexBufferObjects())
    {
        ++_numInstancedVBODrawables;
        _vboDrawableSet.insert(&node);
    }
    else if (node.getUseDisplayList())
    {
        ++_numInstancedDisplayListDrawables;
        _displayListDrawableSet.insert(&node);
    }

    osg::Geometry* geometry = node.asGeometry();
    if (geometry)
    {
        if (geometry->containsDeprecatedData())
            ++_numDeprecatedDataGeometries;

        

        osg::Geometry::ArrayList arrays;
        if(geometry->getArrayList(arrays))
        {
            for(osg::Geometry::ArrayList::const_iterator it = arrays.begin(); it != arrays.end(); it++)
            {
                const osg::ref_ptr<osg::Array> & array = *it;
                apply(*array.get());
            }
        }
        const osg::Geometry::PrimitiveSetList & primSets = geometry->getPrimitiveSetList();
        for(osg::Geometry::PrimitiveSetList::const_iterator it = primSets.begin(); it != primSets.end(); it++)
        {
            const osg::ref_ptr<osg::PrimitiveSet> & primset = *it;
            apply(*primset.get());
        }
    }
    else if (osgText::TextBase * textbase = dynamic_cast<osgText::TextBase *>(&node))
    {
        apply(*textbase);
    }
}

void StatisticsVisitor::apply(osg::PagedLOD& node)
{
    if(_ignoreKnownPagedLODs)
    {
        auto it = _pagedlodSet.find(&node);
        if(it != _pagedlodSet.end())
            return;
    }

    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedPagedLOD;
    _pagedlodSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::ProxyNode& node)
{
    if(_ignoreKnownProxyNodes)
    {
        auto it = _proxynodeSet.find(&node);
        if(it != _proxynodeSet.end())
            return;
    }

    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedProxyNode;
    _proxynodeSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::CoordinateSystemNode& node)
{
    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedCSN;
    _csnSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::Camera& node)
{
    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedCamera;
    _cameraSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::CameraView& node)
{
    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedCameraView;
    _cameraViewSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::BufferData& buffer)
{
    if (_contextID != ~0u)
    {
        if (!_instancedMemory)
            _instancedMemory = new MemoryStatisticsVisitor(_contextID);
        _instancedMemory->apply(buffer);
    }
}

void StatisticsVisitor::apply(osg::MatrixTransform& node)
{
    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    if(dynamic_cast<osgAnimation::Skeleton*>(&node))
    {
        ++_numInstancedAnimationSkeleton;
        _animationSkeletonSet.insert(&node);
    }
    else if(dynamic_cast<osgAnimation::Bone*>(&node))
    {
        ++_numInstancedAnimationBone;
        _animationBoneSet.insert(&node);
    }
    else
    {
        ++_numInstancedMatrixTransform;
        _matrixTransformSet.insert(&node);
    }

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::PositionAttitudeTransform& node)
{
    if (node.getStateSet())
    {
        apply(*node.getStateSet());
    }

    ++_numInstancedPAT;
    _patSet.insert(&node);

    updateNodeStats(node);
    traverse(node);
}

void StatisticsVisitor::apply(osg::StateSet& ss)
{
    osgUtil::StatsVisitor::apply(ss);
    const osg::StateSet::AttributeList& attributes = ss.getAttributeList();
    for(osg::StateSet::AttributeList::const_iterator it = attributes.begin(); it != attributes.end(); it++)
    {
        const osg::StateAttribute::TypeMemberPair & typememberpair = it->first;
        const osg::StateAttribute::Type & type = typememberpair.first;
        const unsigned int & member = typememberpair.second;
        const osg::StateSet::RefAttributePair & attrpair = it->second;
        const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
        const osg::StateAttribute::OverrideValue & value = attrpair.second;
        apply(*attr);
    }

    const osg::StateSet::TextureAttributeList & textureAttributes = ss.getTextureAttributeList();
    for(osg::StateSet::TextureAttributeList::const_iterator it = textureAttributes.begin(); it != textureAttributes.end(); it++)
    {
        const osg::StateSet::AttributeList & attributes = *it;
        for(osg::StateSet::AttributeList::const_iterator it2 = attributes.begin(); it2 != attributes.end(); it2++)
        {
            const osg::StateAttribute::TypeMemberPair & typememberpair = it2->first;
            const osg::StateAttribute::Type & type = typememberpair.first;
            const unsigned int & member = typememberpair.second;
            const osg::StateSet::RefAttributePair & attrpair = it2->second;
            const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
            const osg::StateAttribute::OverrideValue & value = attrpair.second;

            apply(*attr);
        }
    }

    const osg::StateSet::UniformList & uniforms = ss.getUniformList();
    for(osg::StateSet::UniformList::const_iterator it = uniforms.begin(); it != uniforms.end(); it++)
    {
        const std::string & name = it->first;
        const osg::StateAttribute::OverrideValue & overridevalue = it->second.second;
        const osg::ref_ptr<osg::Uniform> & uniform = it->second.first;
        apply(*uniform);
    }

}

void StatisticsVisitor::apply(osg::Texture::TextureObject & to)
{
    if (_contextID != ~0u)
    {
        if (!_instancedMemory)
            _instancedMemory = new MemoryStatisticsVisitor(_contextID);
        _instancedMemory->apply(to);
    }
}

void StatisticsVisitor::apply(osg::StateAttribute & attr)
{
    ++_numInstancedStateAttribute;
    _stateAttributeSets[attr.getType()].insert(&attr);

    switch(attr.getType())
    {
    case osg::StateAttribute::TEXTURE:
        {
            osg::Texture * texture = attr.asTexture();
            if(texture)
            {
                bool got_txtobj = false;
                if (_contextID != ~0u)
                {
                    osg::Texture::TextureObject * to = texture->getTextureObject(_contextID);
                    if (to)
                    {
                        got_txtobj = true;
                        apply(*to);
                    }
                }
                if (!got_txtobj)
                {
                    unsigned int numImages = texture->getNumImages();
                    for (unsigned n = 0; n < numImages; n++)
                    {
                        osg::Image * image = texture->getImage(n);
                        if (image)
                            apply(*image);
                    }
                }
            }
        }
        break;
    }

    ++_numInstancedSA[attr.getType()];
}

void StatisticsVisitor::apply(osg::Uniform & uniform)
{
    ++_numInstancedUniform;
    _uniformSet.insert(&uniform);

    if (_contextID != ~0u)
    {
        if (!_instancedMemory)
            _instancedMemory = new MemoryStatisticsVisitor(_contextID);
        _instancedMemory->apply(uniform);
    }
}

void StatisticsVisitor::apply(osgText::TextBase & text)
{
    ++_numInstancedTextBase;
    _textBaseSet.insert(&text);
}

#define CMU_NODES(__nodelist) \
    { for(auto node : __nodelist ) { \
        node->accept(*_uniqueMemory); \
    } }

void StatisticsVisitor::computeMemoryUsage()
{
    if (!_uniqueMemory)
        _uniqueMemory = new MemoryStatisticsVisitor(_contextID);

    // original sets from this osg::StatisticsVisitor implementation
    CMU_NODES(_groupSet);
    CMU_NODES(_transformSet);
    CMU_NODES(_lodSet);
    CMU_NODES(_switchSet);
    CMU_NODES(_geodeSet);
    CMU_NODES(_drawableSet);
    //CMU_NODES(_geometrySet);
    //CMU_NODES(_fastGeometrySet);
    for (auto ss : _statesetSet) {
        _uniqueMemory->apply(*ss);
    }

    // additional sets from this StatisticsVisitor implementation
    CMU_NODES(_pagedlodSet);
    CMU_NODES(_proxynodeSet);
    CMU_NODES(_csnSet);
    CMU_NODES(_cameraSet);
    CMU_NODES(_cameraViewSet);
    CMU_NODES(_matrixTransformSet);
    CMU_NODES(_patSet);
    CMU_NODES(_animationSkeletonSet);
    CMU_NODES(_animationBoneSet);
    CMU_NODES(_textBaseSet);

    for(int n = 0; n < MaxStateAttributeType; n++)
    {
        for(auto sa : _stateAttributeSets[n] ) {
            _uniqueMemory->apply(*sa);
        }
    }
    for(auto uniform : _uniformSet ) {
        _uniqueMemory->apply(*uniform);
    }
}

void StatisticsVisitor::totalUpStats()
{
    osgUtil::StatsVisitor::totalUpStats();
    // automatically call the update to gets the unique memory usage
    computeMemoryUsage();
}

void StatisticsVisitor::getMemoryInfo(MemoryStatisticsVisitor::Numbers & unique, MemoryStatisticsVisitor::Numbers & instanced)
{
    // automatically call the update to gets the unique vertices and primitives
    totalUpStats();

    if (_uniqueMemory)
        unique = _uniqueMemory->numbers();
    if (_instancedMemory)
        instanced = _instancedMemory->numbers();
}

unsigned StatisticsVisitor::getNumberOfStateSets() const
{
    return _statesetSet.size();
}

osg::ref_ptr<osg::StateSet> StatisticsVisitor::getStateSet(unsigned index) const
{
    if (index >= _statesetSet.size())
        return nullptr;
    unsigned i = 0;
    for (StateSetSet::const_iterator it = _statesetSet.begin(); it != _statesetSet.end(); ++it, ++i)
    {
        osg::StateSet * stateSet = *it;
        if (i == index)
            return stateSet;
    }
    return nullptr;
}

unsigned StatisticsVisitor::getNumberOfStateAttributes(unsigned type) const
{
    if (type >= MaxStateAttributeType)
        return 0;
    return _stateAttributeSets[type].size();
}

osg::ref_ptr<osg::StateAttribute> StatisticsVisitor::getStateAttribute(unsigned type, unsigned index) const
{
    if (type >= MaxStateAttributeType || index >= _stateAttributeSets[type].size())
        return nullptr;
    unsigned i = 0;
    for (StateAttributeSet::const_iterator it = _stateAttributeSets[type].begin(); it != _stateAttributeSets[type].end(); ++it, ++i)
    {
        osg::StateAttribute * sa = *it;
        if (i == index)
            return sa;
    }
    return nullptr;
}


void StatisticsVisitor::printHTML(std::ostream& out)
{
    // automatically call the update to gets the unique vertices and primitives
    totalUpStats();

    unsigned int unique_primitives = 0;
    osgUtil::Statistics::PrimitiveCountMap::iterator pcmitr;
    for(pcmitr = _uniqueStats.GetPrimitivesBegin();
        pcmitr != _uniqueStats.GetPrimitivesEnd();
        ++pcmitr)
    {
        unique_primitives += pcmitr->second;
    }

    unsigned int instanced_primitives = 0;
    for(pcmitr = _instancedStats.GetPrimitivesBegin();
        pcmitr != _instancedStats.GetPrimitivesEnd();
        ++pcmitr)
    {
        instanced_primitives += pcmitr->second;
    }

    out << "<table border=\'1\' width=\'100%\'>" << std::endl;
    out << "<tr><th>Object Type</th><th>Unique</th><th>Instanced</th></tr>" << std::endl;

    out << "<tr><td>est. memory size</td><td>" << (_uniqueMemory?_uniqueMemory->totalSize():0) << "</td><td>"
        << (_instancedMemory?_instancedMemory->totalSize():0) << "</td></tr>" << std::endl;
    out << "<tr><td>est. texture memory</td><td>" << (_uniqueMemory ? _uniqueMemory->textureMemorySize() : 0) << "</td><td>"
        << (_instancedMemory ? _instancedMemory->textureMemorySize() : 0) << "</td></tr>" << std::endl;
    out << "<tr><td>est. primitives memory</td><td>" << (_uniqueMemory ? _uniqueMemory->primitivesMemorySize() : 0) << "</td><td>"
        << (_instancedMemory ? _instancedMemory->primitivesMemorySize() : 0) << "</td></tr>" << std::endl;
    out << "<tr><td>StateSet</td><td>" << _statesetSet.size() << "</td><td>" << _numInstancedStateSet << "</td></tr>" << std::endl;
    for(int n = 0; n < MaxStateAttributeType; n++)
    {
        out << "<tr><td>StateAttribute " << (osg::StateAttribute::Type)n << "</td><td>"  << _stateAttributeSets[n].size() << "</td><td>" << _numInstancedSA[n] << "</td></tr>" << std::endl;
    }
    out << "<tr><td>Uniform</td><td>" << _uniformSet.size() << "</td><td>" << _numInstancedUniform << "</td></tr>" << std::endl;

    out << "<tr><td>Group</td><td>" << _groupSet.size() << "</td><td>" << _numInstancedGroup << "</td></tr>" << std::endl;
    out << "<tr><td>Camera</td><td>" << _cameraSet.size() << "</td><td>" << _numInstancedCamera << "</td></tr>" << std::endl;
    out << "<tr><td>CameraView</td><td>" << _cameraViewSet.size() << "</td><td>" << _numInstancedCameraView << "</td></tr>" << std::endl;
    out << "<tr><td>Transform</td><td>" << _transformSet.size() << "</td><td>" << _numInstancedTransform << "</td></tr>" << std::endl;
    out << "<tr><td>MatrixTransform</td><td>" << _matrixTransformSet.size() << "</td><td>" << _numInstancedMatrixTransform << "</td></tr>" << std::endl;
    out << "<tr><td>PositionAttitudeTransform</td><td>" << _patSet.size() << "</td><td>" << _numInstancedPAT << "</td></tr>" << std::endl;
    out << "<tr><td>LOD</td><td>" << _lodSet.size() << "</td><td>" << _numInstancedLOD << "</td></tr>" << std::endl;
    out << "<tr><td>PagedLOD</td><td>" << _pagedlodSet.size() << "</td><td>" << _numInstancedPagedLOD << "</td></tr>" << std::endl;
    out << "<tr><td>ProxyNode</td><td>" << _proxynodeSet.size() << "</td><td>" << _numInstancedProxyNode << "</td></tr>" << std::endl;
    out << "<tr><td>Switch</td><td>" << _switchSet.size() << "</td><td>" << _numInstancedSwitch << "</td></tr>" << std::endl;
    out << "<tr><td>animation skeleton</td><td>" << _animationSkeletonSet.size() << "</td><td>" << _numInstancedAnimationSkeleton << "</td></tr>" << std::endl;
    out << "<tr><td>animation bone</td><td>" << _animationBoneSet.size() << "</td><td>" << _numInstancedAnimationBone << "</td></tr>" << std::endl;
    out << "<tr><td>Geode</td><td>" << _geodeSet.size() << "</td><td>" << _numInstancedGeode << "</td></tr>" << std::endl;
    out << "<tr><td>Drawable</td><td>" << _drawableSet.size() << "</td><td>" << _numInstancedDrawable << "</td></tr>" << std::endl;
    out << "<tr><td>Geometry</td><td>" << _geometrySet.size() << "</td><td>" << _numInstancedGeometry << "</td></tr>" << std::endl;
    out << "<tr><td>Text base</td><td>" << _textBaseSet.size() << "</td><td>" << _numInstancedTextBase << "</td></tr>" << std::endl;
    out << "<tr><td>Fast geom.</td><td>" << _fastGeometrySet.size() << "</td><td>" << _numInstancedFastGeometry << "</td></tr>" << std::endl;
    out << "<tr><td>Deprecated data geom.</td><td>" << "N/A" << "</td><td>" << _numDeprecatedDataGeometries << "</td></tr>" << std::endl;
    out << "<tr><td>VBO drawables</td><td>" << _vboDrawableSet.size() << "</td><td>" << _numInstancedVBODrawables << "</td></tr>" << std::endl;
    out << "<tr><td>Display list drawables</td><td>" << _displayListDrawableSet.size() << "</td><td>" << _numInstancedDisplayListDrawables << "</td></tr>" << std::endl;

    out << "<tr><td>Vertices</td><td>" << _uniqueStats._vertexCount << "</td><td>" << _instancedStats._vertexCount << "</td></tr>" << std::endl;
    out << "<tr><td>Primitives</td><td>" << unique_primitives << "</td><td>" << instanced_primitives << "</td></tr>" << std::endl;

    out << "<tr><td>event callbacks</td><td>" << _numEventCallbacks << "</td><td>" << _numEventCallbacks << "</td></tr>" << std::endl;
    out << "<tr><td>update callbacks</td><td>" << _numUpdateCallbacks << "</td><td>" << _numUpdateCallbacks << "</td></tr>" << std::endl;
    out << "<tr><td>cull callbacks</td><td>" << _numCullCallbacks << "</td><td>" << _numCullCallbacks << "</td></tr>" << std::endl;
    out << "<tr><td>animation callbacks</td><td>" << _numAnimationCallbacks << "</td><td>" << _numAnimationCallbacks << "</td></tr>" << std::endl;
    out << "<tr><td>cull inactive</td><td>" << _numCullInactive << "</td><td>" << _numCullInactive << "</td></tr>" << std::endl;
    out << "<tr><td>cull disabled</td><td>" << _numCullDisabled << "</td><td>" << _numCullDisabled << "</td></tr>" << std::endl;

    out << "</table>" << std::endl;
}

void StatisticsVisitor::printStateSetsHTML(std::ostream& out)
{
    out << "<ol>";
    for (StateSetSet::const_iterator it = _statesetSet.begin(); it != _statesetSet.end(); ++it)
    {
        osg::StateSet * stateSet = *it;
        out << "<li>" << osg_helpers::getObjectNameAndType(stateSet, true) << "</li>";
    }
    out << "</ol>";
}

} // namespace osg_helpers
} // namespace sgi
