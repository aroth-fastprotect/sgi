// kate: syntax C++;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#include <sgi/helpers/osg_helper_nodes>
#include <osg/Material>
#include <osgDB/Registry>

#include "../../img/microscope64.c"

namespace sgi {

    namespace osg_helpers {

namespace  {
    void applyMaterialToGeometry(osg::Geometry * g, const GeometryParams & params)
    {
        if(params.useMaterial)
        {
            osg::Material * mat = new osg::Material;
            mat->setDiffuse(osg::Material::FRONT, params.color);
            mat->setDiffuse(osg::Material::FRONT_AND_BACK, params.color);
            mat->setDiffuse(osg::Material::BACK, params.color);
            osg::StateSet * stateSet = g->getOrCreateStateSet();
            stateSet->setAttribute(mat, osg::StateAttribute::ON);
        }
    }
    void applyTextureToGeometry(osg::Geometry * g, osg::Image * image)
    {
        if(image)
        {
            osg::Texture2D* texture = new osg::Texture2D;
            texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
            texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
            texture->setResizeNonPowerOfTwoHint(false);
            texture->setImage(image);

            osg::StateSet * stateSet = g->getOrCreateStateSet();
            stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
            stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
            stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
            //stateSet->setMode(GL_TEXTURE_2D, osg::StateAttribute::ON);
            if (image->isImageTranslucent())
            {
                stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
                stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            }
        }
    }
    void applyLogoToGeometry(osg::Geometry * g, const GeometryParams & params)
    {
        if(params.useLogo)
        {
            osg::ref_ptr<osg::Image> logo = getSGILogoImage();
            applyTextureToGeometry(g, logo.get());
        }
    }
    void applyGeometryParams(osg::Geometry * g, const GeometryParams & params)
    {
        applyMaterialToGeometry(g, params);
        applyTextureToGeometry(g, params.image);
        applyLogoToGeometry(g, params);
    }
}

osg::Geometry * createQuadGeometry(float w,float h, const GeometryParams & params)
{
    osg::Geometry * geom = new osg::Geometry;
    osg::Vec3Array* coords = new osg::Vec3Array(6);
    (*coords)[0].set(-w, 0.0f,  h); // 0
    (*coords)[1].set(-w, 0.0f, -h); // 1
    (*coords)[2].set( w, 0.0f, -h); // 2
    (*coords)[3].set(-w, 0.0f,  h); // 0
    (*coords)[4].set( w, 0.0f, -h); // 2
    (*coords)[5].set( w, 0.0f,  h); // 3
    geom->setVertexArray(coords);

    if(params.useTextureCoordinates || params.image || params.useLogo)
    {
        float texcoord_y_b = 0.0f;
        float texcoord_y_t = 1.0f;
        float texcoord_x = 1.0f;

        osg::Vec2Array* tcoords = new osg::Vec2Array(6);
        (*tcoords)[0].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[1].set(0.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[2].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[3].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[4].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[5].set(1.0f*texcoord_x, texcoord_y_t);
        geom->setTexCoordArray(0, tcoords);
    }

    if(!params.useMaterial)
    {
        osg::Vec4Array* colours = new osg::Vec4Array(6);
        if (params.useDefaultColor)
        {
            (*colours)[0].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[1].set(0.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[2].set(0.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[3].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[4].set(0.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[5].set(1.0f, 0.0f, 1.0f, 1.0f);
        }
        else
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        geom->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 6));
    applyGeometryParams(geom, params);

    return geom;
}

osg::Geometry * createTriangleGeometry(float s, const GeometryParams & params)
{
    osg::Geometry * geom = new osg::Geometry;
    osg::Vec3Array* coords = new osg::Vec3Array(3);
    (*coords)[0].set(-(s/0.5f), 0.0f, 0.0f);
    (*coords)[1].set(+(s/0.5f), 0.0f, 0.0f);
    (*coords)[2].set(     0.0f, 0.0f, s);
    geom->setVertexArray(coords);

    if(params.useTextureCoordinates || params.image || params.useLogo)
    {
        osg::Vec2Array* tcoords = new osg::Vec2Array(3);
        (*tcoords)[0].set(0.0f, 1.0f);
        (*tcoords)[1].set(1.0f, 1.0f);
        (*tcoords)[2].set(0.5f, 0.0f);
        geom->setTexCoordArray(0, tcoords);
    }

    if(!params.useMaterial)
    {
        osg::Vec4Array* colours = new osg::Vec4Array(3);
        if (params.useDefaultColor)
        {
            (*colours)[0].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[1].set(0.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[2].set(0.0f, 0.0f, 1.0f, 1.0f);
        }
        else
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        geom->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));
    applyGeometryParams(geom, params);

    return geom;
}

osg::Geometry * createBoxGeometry(const osg::Vec3 & size, const GeometryParams & params)
{
    osg::Geometry * geom = new osg::Geometry;
    osg::Vec3Array* coords = new osg::Vec3Array(8);
    const float & w = size.x();
    const float & d = size.y();
    const float & h = size.z();
    // front
    (*coords)[0].set(-w, -d,  h);
    (*coords)[1].set( w, -d,  h);
    (*coords)[2].set( w,  d,  h);
    (*coords)[3].set(-w,  d,  h);
    // back
    (*coords)[4].set(-w, -d, -h);
    (*coords)[5].set( w, -d, -h);
    (*coords)[6].set( w,  d, -h);
    (*coords)[7].set(-w,  d, -h);
    geom->setVertexArray(coords);

    if(params.useTextureCoordinates || params.image || params.useLogo)
    {
        float texcoord_y_b = 0.0f;
        float texcoord_y_t = 1.0f;
        float texcoord_x = 1.0f;

        osg::Vec2Array* tcoords = new osg::Vec2Array(8);
        (*tcoords)[0].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[1].set(0.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[2].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[3].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[4].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[5].set(1.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[6].set(1.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[7].set(1.0f*texcoord_x, texcoord_y_t);
        geom->setTexCoordArray(0, tcoords);
    }

    if(!params.useMaterial)
    {
        osg::Vec4Array* colours = new osg::Vec4Array(8);
        if (!params.useDefaultColor)
        {
            (*colours)[0].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[1].set(0.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[2].set(0.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[3].set(1.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[4].set(0.0f, 1.0f, 1.0f, 1.0f);
            (*colours)[5].set(1.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[6].set(0.5f, 0.0f, 0.0f, 1.0f);
            (*colours)[7].set(0.0f, 0.5f, 0.0f, 1.0f);
        }
        else
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        geom->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
    }

    GLubyte cube_elements[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };
    osg::DrawElementsUByte * elements = new osg::DrawElementsUByte(osg::PrimitiveSet::TRIANGLES, sizeof(cube_elements)/sizeof(cube_elements[0]), cube_elements);

    geom->addPrimitiveSet(elements);
    applyGeometryParams(geom, params);

    return geom;
}

osg::Image * getSGILogoImage()
{
    osgDB::ReaderWriter * rw = osgDB::Registry::instance()->getReaderWriterForExtension("png");
    std::string microscope64_png_str;
    microscope64_png_str.assign((const char*)microscope64_png, sizeof(microscope64_png));
    std::stringstream ss(microscope64_png_str);
    osgDB::ReaderWriter::ReadResult result;
    if (rw)
        result = rw->readImage(ss);
    return result.takeImage();
}


    } // namespace osg_helpers
} // namespace sgi
