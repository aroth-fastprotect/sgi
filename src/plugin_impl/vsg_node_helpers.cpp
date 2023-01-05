// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#include <sgi/helpers/vsg_helper_nodes>
#include <osg/Material>
#include <osgDB/Registry>
#include <osg/TexEnv>

#include "../../img/microscope64.c"

namespace sgi {

    namespace vsg_helpers {

GeometryParams::GeometryParams()
    : color()
    , image(nullptr)
    , useDefaultColor(true)
    , useMaterial(false)
    , useTextureCoordinates(false)
    , useNormals(true)
    , useLogo(false)
    , _useCustomColor(false)
{}
bool GeometryParams::needTextureCoordinates() const
{
    return useTextureCoordinates || image != nullptr || useLogo;
}

bool GeometryParams::useCustomColor() const
{
    return _useCustomColor || (image == nullptr && !useLogo && !useDefaultColor);
}

void GeometryParams::setCustomColor(const vsg::vec4 & c)
{
    color = c;
    _useCustomColor = true;
}

bool GeometryParams::useWhiteColorForTexture() const
{
    return (image != nullptr || useLogo) && useDefaultColor;
}

unsigned GeometryParams::textureUnit() const
{
    return useMaterial ? 1 : 0;
}

bool GeometryParams::useTextureModulate() const
{
    return useMaterial;
}

namespace  {
    void applyMaterialToGeometry(vsg::Geometry * g, const GeometryParams & params)
    {
        if(params.useMaterial)
        {
#if 0
            vsg::Material * mat = new vsg::Material;
            vsg::vec4 diffuse;
            if (params.useCustomColor())
            {
                diffuse = params.color;
            }
            else if(params.useWhiteColorForTexture())
            {
                diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                diffuse.set(0.8f, 0.8f, 0.8f, 1.0f);
            }

            mat->setDiffuse(vsg::Material::FRONT, diffuse);
            mat->setDiffuse(vsg::Material::FRONT_AND_BACK, diffuse);
            mat->setDiffuse(vsg::Material::BACK, diffuse);
            vsg::StateSet * stateSet = g->getOrCreateStateSet();
            stateSet->setAttribute(mat, vsg::StateAttribute::ON);
#endif
        }
    }
    void applyTextureToGeometry(vsg::Geometry * g, unsigned unit, vsg::Image * image, bool modulate)
    {
        if(image)
        {
#if 0
            vsg::Texture2D* texture = new vsg::Texture2D;
            texture->setFilter(vsg::Texture::MIN_FILTER, vsg::Texture::LINEAR);
            texture->setFilter(vsg::Texture::MAG_FILTER, vsg::Texture::LINEAR);
            texture->setResizeNonPowerOfTwoHint(false);
            texture->setImage(image);

            vsg::StateSet * stateSet = g->getOrCreateStateSet();
            stateSet->setMode(GL_CULL_FACE, vsg::StateAttribute::OFF);
            stateSet->setMode(GL_LIGHTING, vsg::StateAttribute::OFF);
            stateSet->setTextureAttributeAndModes(unit, texture, vsg::StateAttribute::ON);
            //stateSet->setMode(GL_TEXTURE_2D, vsg::StateAttribute::ON);
            if (image->isImageTranslucent())
            {
                stateSet->setMode(GL_BLEND, vsg::StateAttribute::ON);
                stateSet->setRenderingHint(vsg::StateSet::TRANSPARENT_BIN);
            }
            if(modulate)
            {
                vsg::TexEnv * env = new vsg::TexEnv(vsg::TexEnv::MODULATE);
                stateSet->setTextureAttributeAndModes(unit, env, vsg::StateAttribute::ON);
            }
#endif
        }
    }
    void applyLogoToGeometry(vsg::Geometry * g, const GeometryParams & params)
    {
        if(params.useLogo)
        {
#if 0
            vsg::ref_ptr<vsg::Image> logo = getSGILogoImage();
            bool modulate = params.useMaterial;
            applyTextureToGeometry(g, params.textureUnit(), logo.get(), params.useTextureModulate());
#endif
        }
    }
    void applyGeometryParams(vsg::Geometry * g, const GeometryParams & params)
    {
        applyMaterialToGeometry(g, params);
        applyTextureToGeometry(g, params.textureUnit(), params.image, params.useTextureModulate());
        applyLogoToGeometry(g, params);
    }
}

vsg::Geometry * createQuadGeometry(float w,float h, const GeometryParams & params)
{
    vsg::Geometry * geom = new vsg::Geometry;
#if 0
    vsg::Vec3Array* coords = new vsg::Vec3Array(6);
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

        vsg::Vec2Array* tcoords = new vsg::Vec2Array(6);
        (*tcoords)[0].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[1].set(0.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[2].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[3].set(0.0f*texcoord_x, texcoord_y_t);
        (*tcoords)[4].set(1.0f*texcoord_x, texcoord_y_b);
        (*tcoords)[5].set(1.0f*texcoord_x, texcoord_y_t);
        geom->setTexCoordArray(params.textureUnit(), tcoords);
    }
    if (params.useNormals)
    {
        vsg::Vec3Array* normals = new vsg::Vec3Array(6);
        (*normals)[0].set(0.0f, 1.0f, 0.0f); // 0
        (*normals)[1].set(0.0f, 1.0f, 0.0f); // 1
        (*normals)[2].set(0.0f, 1.0f, 0.0f); // 2
        (*normals)[3].set(0.0f, 1.0f, 0.0f); // 0
        (*normals)[4].set(0.0f, 1.0f, 0.0f); // 2
        (*normals)[5].set(0.0f, 1.0f, 0.0f); // 3
        geom->setNormalArray(normals);
    }
    if(!params.useMaterial)
    {
        vsg::Vec4Array* colours = new vsg::Vec4Array(6);
        if (params.useCustomColor())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        else if(params.useWhiteColorForTexture())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i].set(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            (*colours)[0].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[1].set(0.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[2].set(0.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[3].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[4].set(0.0f, 0.0f, 1.0f, 1.0f);
            (*colours)[5].set(1.0f, 0.0f, 1.0f, 1.0f);
        }
        geom->setColorArray(colours, vsg::Array::BIND_PER_VERTEX);
    }

    geom->addPrimitiveSet(new vsg::DrawArrays(vsg::PrimitiveSet::TRIANGLES, 0, 6));
#endif
    applyGeometryParams(geom, params);

    return geom;
}

vsg::Geometry * createTriangleGeometry(float s, const GeometryParams & params)
{
    vsg::Geometry * geom = new vsg::Geometry;
#if 0
    vsg::Vec3Array* coords = new vsg::Vec3Array(3);
    (*coords)[0].set(-(s*0.5f), 0.0f, 0.0f);
    (*coords)[1].set(+(s*0.5f), 0.0f, 0.0f);
    (*coords)[2].set(     0.0f, 0.0f, s * sqrt(0.75));
    geom->setVertexArray(coords);

    if(params.useTextureCoordinates || params.image || params.useLogo)
    {
        vsg::Vec2Array* tcoords = new vsg::Vec2Array(3);
        (*tcoords)[0].set(0.0f, 0.0f);
        (*tcoords)[1].set(1.0f, 0.0f);
        (*tcoords)[2].set(0.5f, 1.0f);
        geom->setTexCoordArray(params.textureUnit(), tcoords);
    }
    if (params.useNormals)
    {
        vsg::Vec3Array* normals = new vsg::Vec3Array(3);
        (*normals)[0].set(0.0f, 1.0f, 0.0f); // 0
        (*normals)[1].set(0.0f, 1.0f, 0.0f); // 1
        (*normals)[2].set(0.0f, 1.0f, 0.0f); // 2
        geom->setNormalArray(normals);
    }

    if(!params.useMaterial)
    {
        vsg::Vec4Array* colours = new vsg::Vec4Array(3);
        if (params.useCustomColor())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        else if(params.useWhiteColorForTexture())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i].set(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            (*colours)[0].set(1.0f, 0.0f, 0.0f, 1.0f);
            (*colours)[1].set(0.0f, 1.0f, 0.0f, 1.0f);
            (*colours)[2].set(0.0f, 0.0f, 1.0f, 1.0f);
        }
        geom->setColorArray(colours, vsg::Array::BIND_PER_VERTEX);
    }

    geom->addPrimitiveSet(new vsg::DrawArrays(vsg::PrimitiveSet::TRIANGLES, 0, 3));
    applyGeometryParams(geom, params);
#endif
    return geom;
}

vsg::Geometry * createBoxGeometry(const vsg::vec3 & size, const GeometryParams & params)
{
    vsg::Geometry * geom = new vsg::Geometry;
#if 0
    vsg::Vec3Array* coords = new vsg::Vec3Array(8);
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
        vsg::Vec2Array* tcoords = new vsg::Vec2Array(8);
        (*tcoords)[0].set(0.0f, 0.0f);
        (*tcoords)[1].set(1.0f, 0.0f);
        (*tcoords)[2].set(1.0f, 1.0f);
        (*tcoords)[3].set(0.0f, 1.0f);
        (*tcoords)[4].set(0.0f, 0.0f);
        (*tcoords)[5].set(1.0f, 0.0f);
        (*tcoords)[6].set(1.0f, 1.0f);
        (*tcoords)[7].set(0.0f, 1.0f);
        geom->setTexCoordArray(params.textureUnit(), tcoords);
    }
    if (params.useNormals)
    {
        vsg::Vec3Array* normals = new vsg::Vec3Array(8);
        (*normals)[0].set(0.0f, 0.0f,  1.0f);
        (*normals)[1].set(0.0f, 0.0f,  1.0f);
        (*normals)[2].set(0.0f, 0.0f,  1.0f);
        (*normals)[3].set(0.0f, 0.0f,  1.0f);
        (*normals)[4].set(0.0f, 0.0f, -1.0f);
        (*normals)[5].set(0.0f, 0.0f, -1.0f);
        (*normals)[6].set(0.0f, 0.0f, -1.0f);
        (*normals)[7].set(0.0f, 0.0f, -1.0f);
        geom->setNormalArray(normals);
    }

    if(!params.useMaterial)
    {
        vsg::Vec4Array* colours = new vsg::Vec4Array(8);
        if (params.useCustomColor())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i] = params.color;
        }
        else if(params.useWhiteColorForTexture())
        {
            for (unsigned i = 0; i < colours->size(); ++i)
                (*colours)[i].set(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
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
        geom->setColorArray(colours, vsg::Array::BIND_PER_VERTEX);
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
    vsg::DrawElementsUByte * elements = new vsg::DrawElementsUByte(vsg::PrimitiveSet::TRIANGLES, sizeof(cube_elements)/sizeof(cube_elements[0]), cube_elements);

    geom->addPrimitiveSet(elements);
#endif
    applyGeometryParams(geom, params);

    return geom;
}

vsg::Image * getSGILogoImage()
{
#if 0
    osgDB::ReaderWriter * rw = osgDB::Registry::instance()->getReaderWriterForExtension("png");
    std::string microscope64_png_str;
    microscope64_png_str.assign((const char*)microscope64_png, sizeof(microscope64_png));
    std::stringstream ss(microscope64_png_str);
    osgDB::ReaderWriter::ReadResult result;
    if (rw)
        result = rw->readImage(ss);
    return result.takeImage();
#else
    return nullptr;
#endif
}


    } // namespace vsg_helpers
} // namespace sgi
