#include <ostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <stdint.h>

#include <sgi/helpers/osg_statistics>

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


    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::Type & t)
    {
        switch (t)
        {
        case osg::StateAttribute::TEXTURE: os << "TEXTURE"; break;
        case osg::StateAttribute::POLYGONMODE: os << "POLYGONMODE"; break;
        case osg::StateAttribute::POLYGONOFFSET: os << "POLYGONOFFSET"; break;
        case osg::StateAttribute::MATERIAL: os << "MATERIAL"; break;
        case osg::StateAttribute::ALPHAFUNC: os << "ALPHAFUNC"; break;
        case osg::StateAttribute::ANTIALIAS: os << "ANTIALIAS"; break;
        case osg::StateAttribute::COLORTABLE: os << "COLORTABLE"; break;
        case osg::StateAttribute::CULLFACE: os << "CULLFACE"; break;
        case osg::StateAttribute::FOG: os << "FOG"; break;
        case osg::StateAttribute::FRONTFACE: os << "FRONTFACE"; break;
        case osg::StateAttribute::LIGHT: os << "LIGHT"; break;
        case osg::StateAttribute::POINT: os << "POINT"; break;
        case osg::StateAttribute::LINEWIDTH: os << "LINEWIDTH"; break;
        case osg::StateAttribute::LINESTIPPLE: os << "LINESTIPPLE"; break;
        case osg::StateAttribute::POLYGONSTIPPLE: os << "POLYGONSTIPPLE"; break;
        case osg::StateAttribute::SHADEMODEL: os << "SHADEMODEL"; break;
        case osg::StateAttribute::TEXENV: os << "TEXENV"; break;
        case osg::StateAttribute::TEXENVFILTER: os << "TEXENVFILTER"; break;
        case osg::StateAttribute::TEXGEN: os << "TEXGEN"; break;
        case osg::StateAttribute::TEXMAT: os << "TEXMAT"; break;
        case osg::StateAttribute::LIGHTMODEL: os << "LIGHTMODEL"; break;
        case osg::StateAttribute::BLENDFUNC: os << "BLENDFUNC"; break;
        case osg::StateAttribute::BLENDEQUATION: os << "BLENDEQUATION"; break;
        case osg::StateAttribute::LOGICOP: os << "LOGICOP"; break;
        case osg::StateAttribute::STENCIL: os << "STENCIL"; break;
        case osg::StateAttribute::COLORMASK: os << "COLORMASK"; break;
        case osg::StateAttribute::DEPTH: os << "DEPTH"; break;
        case osg::StateAttribute::VIEWPORT: os << "VIEWPORT"; break;
        case osg::StateAttribute::SCISSOR: os << "SCISSOR"; break;
        case osg::StateAttribute::BLENDCOLOR: os << "BLENDCOLOR"; break;
        case osg::StateAttribute::MULTISAMPLE: os << "MULTISAMPLE"; break;
        case osg::StateAttribute::CLIPPLANE: os << "CLIPPLANE"; break;
        case osg::StateAttribute::COLORMATRIX: os << "COLORMATRIX"; break;
        case osg::StateAttribute::VERTEXPROGRAM: os << "VERTEXPROGRAM"; break;
        case osg::StateAttribute::FRAGMENTPROGRAM: os << "FRAGMENTPROGRAM"; break;
        case osg::StateAttribute::POINTSPRITE: os << "POINTSPRITE"; break;
        case osg::StateAttribute::PROGRAM: os << "PROGRAM"; break;
        case osg::StateAttribute::CLAMPCOLOR: os << "CLAMPCOLOR"; break;
        case osg::StateAttribute::HINT: os << "HINT"; break;
        case osg::StateAttribute::SAMPLEMASKI: os << "SAMPLEMASKI"; break;
        case osg::StateAttribute::PRIMITIVERESTARTINDEX: os << "PRIMITIVERESTARTINDEX"; break;
        case osg::StateAttribute::CLIPCONTROL: os << "CLIPCONTROL"; break;

            /// osgFX namespace
        case osg::StateAttribute::VALIDATOR: os << "VALIDATOR"; break;
        case osg::StateAttribute::VIEWMATRIXEXTRACTOR: os << "VIEWMATRIXEXTRACTOR"; break;
            /// osgNV namespace
        case osg::StateAttribute::OSGNV_PARAMETER_BLOCK: os << "OSGNV_PARAMETER_BLOCK"; break;
            // osgNVExt namespace
        case osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER: os << "OSGNVEXT_TEXTURE_SHADER"; break;
        case osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM: os << "OSGNVEXT_VERTEX_PROGRAM"; break;
        case osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS: os << "OSGNVEXT_REGISTER_COMBINERS"; break;
            /// osgNVCg namespace
        case osg::StateAttribute::OSGNVCG_PROGRAM: os << "OSGNVCG_PROGRAM"; break;
            // osgNVSlang namespace
        case osg::StateAttribute::OSGNVSLANG_PROGRAM: os << "OSGNVSLANG_PROGRAM"; break;
            // osgNVParse
        case osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER: os << "OSGNVPARSE_PROGRAM_PARSER"; break;

        case osg::StateAttribute::UNIFORMBUFFERBINDING: os << "UNIFORMBUFFERBINDING"; break;
        case osg::StateAttribute::TRANSFORMFEEDBACKBUFFERBINDING: os << "TRANSFORMFEEDBACKBUFFERBINDING"; break;
        case osg::StateAttribute::ATOMICCOUNTERBUFFERBINDING: os << "ATOMICCOUNTERBUFFERBINDING"; break;
        case osg::StateAttribute::PATCH_PARAMETER: os << "PATCH_PARAMETER"; break;
        case osg::StateAttribute::FRAME_BUFFER_OBJECT: os << "FRAME_BUFFER_OBJECT"; break;
        default: os << (int)t; break;
        }
        return os;
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
        , _estimatedMemory(0)
        , _numInstancedAnimationSkeleton(0)
        , _numInstancedAnimationBone(0)
		, _numInstancedTextBase(0)
		, _numDeprecatedDataGeometries(0)
    {
        for(int n = 0; n < MaxStateAttributeType; n++)
            _numInstancedSA[n] = 0;
    }

    StatisticsVisitor::~StatisticsVisitor()
    {
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
        _estimatedMemory = 0;

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
        _estimatedMemory += buffer.getTotalDataSize();
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
        _estimatedMemory += to._profile._size;
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

        {
            osg::FloatArray * a = uniform.getFloatArray();
            if(a)
                _estimatedMemory += a->getTotalDataSize();
        }
        {
            osg::DoubleArray * a = uniform.getDoubleArray();
            if(a)
                _estimatedMemory += a->getTotalDataSize();
        }
        {
            osg::IntArray * a = uniform.getIntArray();
            if(a)
                _estimatedMemory += a->getTotalDataSize();
        }
        {
            osg::UIntArray * a = uniform.getUIntArray();
            if(a)
                _estimatedMemory += a->getTotalDataSize();
        }
    }

	void StatisticsVisitor::apply(osgText::TextBase & text)
	{
		++_numInstancedTextBase;
		_textBaseSet.insert(&text);
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
        out << "<tr><td>est. memory size</td><td>N/A</td><td>" << _estimatedMemory << "</td></tr>" << std::endl;
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

} // namespace osg_helpers
} // namespace sgi
