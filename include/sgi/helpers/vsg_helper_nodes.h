// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#pragma once

#include <sgi/Export>
#include <functional>
#include <vsg/Nodes/Node.h>

namespace sgi {
namespace vsg_helpers {

inline void tagNodeForObjectTree(vsg::Node * node, const std::string & name=std::string())
{
    node->setUserValue("sgi_tree_item", true);
    if(!name.empty())
        node->setUserValue("sgi_tree_itemname", name);
}

inline vsg::Node* createNodeForImage(vsg::Image* image,float s,float t)
{
    vsg::Node* ret = nullptr;
    if (image && s>0 && t>0)
    {
        vsg::Geometry* geom = nullptr;
        float y = 1.0;
        float x = y*(s/t);

        float texcoord_y_b = (image->getOrigin() == osg::Image::BOTTOM_LEFT) ? 0.0f : 1.0f;
        float texcoord_y_t = (image->getOrigin() == osg::Image::BOTTOM_LEFT) ? 1.0f : 0.0f;

        // set up the texture.

        osg::Texture2D* texture = new osg::Texture2D;
        texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
        texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
        texture->setResizeNonPowerOfTwoHint(false);
        float texcoord_x = 1.0f;

        texture->setImage(image);

        // set up the drawstate.
        osg::StateSet* dstate = new osg::StateSet;
        dstate->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
        dstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        dstate->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        if (image->isImageTranslucent())
        {
            dstate->setMode(GL_BLEND, osg::StateAttribute::ON);
            dstate->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        }

        geom = new osg::Geometry;
        geom->setStateSet(dstate);

        osg::Vec3Array* coords = new osg::Vec3Array(6);
        (*coords)[0].set(-x,0.0f,y);
        (*coords)[1].set(-x,0.0f,-y);
        (*coords)[2].set(x,0.0f,-y);
        (*coords)[3].set(x,0.0f,-y);
        (*coords)[4].set(x,0.0f,y);
        (*coords)[5].set(-x,0.0f,y);
        geom->setVertexArray(coords);

        osg::Vec2Array* tcoords = new osg::Vec2Array(6);
        (*tcoords)[0].set(0.0f*texcoord_x,texcoord_y_t);
        (*tcoords)[1].set(0.0f*texcoord_x,texcoord_y_b);
        (*tcoords)[2].set(1.0f*texcoord_x,texcoord_y_b);
        (*tcoords)[3].set(1.0f*texcoord_x,texcoord_y_b);
        (*tcoords)[4].set(1.0f*texcoord_x,texcoord_y_t);
        (*tcoords)[5].set(0.0f*texcoord_x,texcoord_y_t);
        geom->setTexCoordArray(0,tcoords);

        osg::Vec4Array* colours = new osg::Vec4Array(1);
        (*colours)[0].set(1.0f,1.0f,1.0,1.0f);
        geom->setColorArray(colours, osg::Array::BIND_OVERALL);

        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,6));
        geom->setUseVertexArrayObject(true);
        geom->setUseVertexBufferObjects(true);
        geom->setUseDisplayList(false);
#if OSG_VERSION_GREATER_OR_EQUAL(3,4,0)
        ret = geom;
#else
        ret = new osg::Geode;
        ret->addDrawable(geom);
#endif
        ret->setUserValue("sgi_tree_imagegeode", true);
        ret->setName("ImageGeode");
    }
    return ret;
}
inline vsg::Node * createNodeForImage(vsg::Image* image)
{
    return createNodeForImage(image,image->s(),image->t());
}


} // namespace vsg_helpers
} // namespace sgi
