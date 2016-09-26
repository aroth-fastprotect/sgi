#include "stdafx.h"
#include "NodeHelper.h"
#include <osg/ComputeBoundsVisitor>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PagedLOD>
#include <osg/ProxyNode>
#include <osg/Depth>
#include <osg/LineWidth>
#include <osgText/Text>
#include <osgUtil/CullVisitor>
#include <osg/ComputeBoundsVisitor>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>

#define TOGGLECENTERMARKER_PL_EXTENSION "sgi_togglecentermarker"

namespace sgi {
namespace osg_plugin {

AxisGeometry::AxisGeometry(const osg::Vec3 & pos, float length, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_length(length, length, length)
    , m_colorScheme(scheme)
{
    setVertexArray (new osg::Vec3Array(6));
    setColorArray (new osg::Vec4Array(6), osg::Array::BIND_PER_VERTEX);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    applyLength();
    applyColorScheme();
}

AxisGeometry::AxisGeometry(const osg::Vec3 & pos, const osg::Vec3 & length, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_length(length)
    , m_colorScheme(scheme)
{
    setVertexArray (new osg::Vec3Array(6));
    setColorArray (new osg::Vec4Array(6), osg::Array::BIND_PER_VERTEX);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    applyLength();
    applyColorScheme();
}

AxisGeometry::AxisGeometry(const AxisGeometry& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geometry(obj, copyop)
    , m_pos(obj.m_pos)
    , m_length(obj.m_length)
    , m_colorScheme(obj.m_colorScheme)
{
}

AxisGeometry::~AxisGeometry()
{
}

void AxisGeometry::applyLength()
{
    osg::Vec3Array * vertices = (osg::Vec3Array *)getVertexArray();
    (*vertices)[0] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[1] = m_pos + osg::Vec3 ( m_length.x(), 0.0, 0.0);
    (*vertices)[2] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[3] = m_pos + osg::Vec3 ( 0.0, m_length.y(), 0.0);
    (*vertices)[4] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[5] = m_pos + osg::Vec3 ( 0.0, 0.0, m_length.z());
    setVertexArray (vertices);

}

void AxisGeometry::applyColorScheme()
{
    osg::Vec4Array * colors = (osg::Vec4Array *)getColorArray();
    switch(m_colorScheme)
    {
    case ColorSchemePrimary:
    default:
        (*colors)[0] = osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f);
        (*colors)[1] = osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f);
        (*colors)[2] = osg::Vec4 (0.0f, 1.0f, 0.0f, 1.0f);
        (*colors)[3] = osg::Vec4 (0.0f, 1.0f, 0.0f, 1.0f);
        (*colors)[4] = osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f);
        (*colors)[5] = osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeSecondary:
        (*colors)[0] = osg::Vec4 (1.0f, 0.0f, 1.0f, 1.0f);
        (*colors)[1] = osg::Vec4 (1.0f, 0.0f, 1.0f, 1.0f);
        (*colors)[2] = osg::Vec4 (1.0f, 1.0f, 0.0f, 1.0f);
        (*colors)[3] = osg::Vec4 (1.0f, 1.0f, 0.0f, 1.0f);
        (*colors)[4] = osg::Vec4 (0.0f, 1.0f, 1.0f, 1.0f);
        (*colors)[5] = osg::Vec4 (0.0f, 1.0f, 1.0f, 1.0f);
        break;
    }
    setColorArray (colors, osg::Array::BIND_PER_VERTEX);
}

void AxisGeometry::setLength(float length)
{
    m_length.set(length, length, length);
    applyLength();
}

void AxisGeometry::setLength(const osg::Vec3 & length)
{
    m_length = length;
    applyLength();
}

const osg::Vec3 & AxisGeometry::position() const
{
    return m_pos;
}

void AxisGeometry::setPosition(const osg::Vec3 & position)
{
    m_pos = position;
    applyLength();
}

void AxisGeometry::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

void AxisGeometry::set(const osg::Vec3d & front, const osg::Vec3d & right, const osg::Vec3d & up)
{
    osg::Vec3Array * vertices;
    vertices = (osg::Vec3Array *)getVertexArray();

    osg::Vec3 v;
    
    v = front;
    v.normalize();

    (*vertices)[0] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[1] = m_pos + v * m_length.x();

    v = right;
    v.normalize();
    (*vertices)[2] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[3] = m_pos + v * m_length.y();

    v = up;
    v.normalize();
    (*vertices)[4] = m_pos + osg::Vec3 ( 0.0, 0.0, 0.0);
    (*vertices)[5] = m_pos + v * m_length.z();
    setVertexArray (vertices);
}

CircleGeometry::CircleGeometry(const osg::Vec3 & pos, float radius, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_rotation()
    , m_radius(radius)
    , m_colorScheme(scheme)
{
    osg::Vec3d upVector = pos;
    upVector.normalize();
    m_rotation.makeRotate(osg::Vec3d(0, 0, 1), upVector);

    setVertexArray(new osg::Vec3Array(36));
    setColorArray(new osg::Vec4Array(1), osg::Array::BIND_OVERALL);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 36));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    applyRadius();
    applyColorScheme();
}

CircleGeometry::CircleGeometry(const osg::Vec3 & pos, const osg::Vec3 & upVector, float radius, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_rotation()
    , m_radius(radius)
    , m_colorScheme(scheme)
{
    m_rotation.makeRotate(osg::Vec3d(0, 0, 1), osg::Vec3d(upVector));

    setVertexArray(new osg::Vec3Array(36));
    setColorArray(new osg::Vec4Array(1), osg::Array::BIND_OVERALL);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 36));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    applyRadius();
    applyColorScheme();
}

CircleGeometry::CircleGeometry(const CircleGeometry& obj, const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geometry(obj, copyop)
    , m_pos(obj.m_pos)
    , m_rotation(obj.m_rotation)
    , m_radius(obj.m_radius)
    , m_colorScheme(obj.m_colorScheme)
{
}

CircleGeometry::~CircleGeometry()
{
}

void CircleGeometry::applyRadius()
{
    osg::Vec3Array * vertices = (osg::Vec3Array *)getVertexArray();

    unsigned numCirclePoints = vertices->getNumElements();
    for (unsigned n = 0; n < numCirclePoints; ++n)
    {
        float angle = ((float)((double)n  * (osg::PI * 2.0)) / (float)numCirclePoints);
        float x1 = m_radius * cos(angle);
        float y1 = m_radius * sin(angle);
        osg::Vec3d v(x1, y1, 0.f);
        (*vertices)[n] = m_pos + (m_rotation * v);
    }
    setVertexArray(vertices);
}

void CircleGeometry::applyColorScheme()
{
    osg::Vec4Array * colors = (osg::Vec4Array *)getColorArray();
    switch (m_colorScheme)
    {
    case ColorSchemePrimary:
    default:
        (*colors)[0] = osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    case ColorSchemeSecondary:
        (*colors)[0] = osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f);
        break;
    }
    setColorArray(colors, osg::Array::BIND_OVERALL);
}

void CircleGeometry::setRadius(float radius)
{
    m_radius = radius;
    applyRadius();
}

const osg::Vec3 & CircleGeometry::position() const
{
    return m_pos;
}

void CircleGeometry::setPosition(const osg::Vec3 & position)
{
    m_pos = position;
    applyRadius();
}

const osg::Quat & CircleGeometry::rotation() const
{
    return m_rotation;
}

void CircleGeometry::setRotation(const osg::Quat & rotation)
{
    m_rotation = rotation;
    applyRadius();
}

void CircleGeometry::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}


AxisGeode::AxisGeode(const osg::Vec3 & pos, float length, AxisGeometry::ColorScheme scheme)
    : osg::Geode()
    , m_geom(new AxisGeometry(pos, length, scheme))
{
    addDrawable( m_geom.get() );
}

AxisGeode::AxisGeode(const AxisGeode& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geode(obj, copyop)
    , m_geom(new AxisGeometry(*obj.m_geom, copyop))
{
}

AxisGeode::~AxisGeode()
{
}

void AxisGeode::set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side)
{
    m_geom->set(up, front, side);
}

const osg::Vec3 & AxisGeode::length() const 
{ 
    return m_geom->length(); 
}

void AxisGeode::setLength(float length)
{
    return m_geom->setLength(length);
}

void AxisGeode::setLength(const osg::Vec3 & length)
{
    return m_geom->setLength(length);
}

const osg::Vec3 & AxisGeode::position() const
{
    return m_geom->position();
}

void AxisGeode::setPosition(const osg::Vec3 & position)
{
    m_geom->setPosition(position);
}

AxisGeometry::ColorScheme AxisGeode::colorScheme() const
{ 
    return m_geom->colorScheme();
}

void AxisGeode::setColorScheme(AxisGeometry::ColorScheme scheme)
{
    m_geom->setColorScheme(scheme);
}

AxisBillboard::AxisBillboard(const osg::Vec3 & pos, float length, AxisGeometry::ColorScheme scheme)
    : osg::Billboard()
    , m_geom(new AxisGeometry(pos, length, scheme))
{
    addDrawable( m_geom.get() );
}

AxisBillboard::AxisBillboard(const AxisBillboard& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Billboard(obj, copyop)
    , m_geom(new AxisGeometry(*obj.m_geom, copyop))
{
}

AxisBillboard::~AxisBillboard()
{
}

void AxisBillboard::set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side)
{
    m_geom->set(up, front, side);
}

const osg::Vec3 & AxisBillboard::length() const 
{ 
    return m_geom->length(); 
}

void AxisBillboard::setLength(float length)
{
    return m_geom->setLength(length);
}

void AxisBillboard::setLength(const osg::Vec3 & length)
{
    return m_geom->setLength(length);
}

const osg::Vec3 & AxisBillboard::position() const
{
    return m_geom->position();
}

void AxisBillboard::setPosition(const osg::Vec3 & position)
{
    m_geom->setPosition(position);
}

AxisGeometry::ColorScheme AxisBillboard::colorScheme() const
{ 
    return m_geom->colorScheme();
}

void AxisBillboard::setColorScheme(AxisGeometry::ColorScheme scheme)
{
    m_geom->setColorScheme(scheme);
}

AxisGroup::AxisGroup(const osg::Vec3 & pos, float length, AxisGeometry::ColorScheme scheme, bool autoSize)
    : osg::Group()
    , m_geode(new AxisGeode(pos, length, scheme))
    , m_autoSize(autoSize)
{
}

/** Copy constructor using CopyOp to manage deep vs shallow copy.*/
AxisGroup::AxisGroup(const AxisGroup& obj,const osg::CopyOp& copyop)
    : osg::Group()
    , m_geode(new AxisGeode(*obj.m_geode, copyop))
    , m_autoSize(obj.m_autoSize)
{
}

AxisGroup::~AxisGroup()
{
}

void AxisGroup::traverse(osg::NodeVisitor& nv)
{
    osg::Group::traverse(nv);
    if(m_autoSize && nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
        osgUtil::CullVisitor * cv = (osgUtil::CullVisitor *)&nv;
        osg::ComputeBoundsVisitor cbv;
        for(osg::NodeList::const_iterator itr=_children.begin();
            itr!=_children.end();
            ++itr)
        {
            (*itr)->accept(cbv);
        }
        const osg::BoundingSphere & bs = getBound();
        const osg::BoundingBox& bb = cbv.getBoundingBox();
        osg::Vec3 length = (bb._max - bb._min) * 1.25f;
        m_geode->setLength(length);
    }
    m_geode->accept(nv);
}

void AxisGroup::set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side)
{
    m_geode->set(up, front, side);
}

const osg::Vec3 & AxisGroup::length() const
{
    return m_geode->length();
}

void AxisGroup::setLength(float length)
{
    m_geode->setLength(length);
}

void AxisGroup::setLength(const osg::Vec3 & length)
{
    return m_geode->setLength(length);
}

const osg::Vec3 & AxisGroup::position() const
{
    return m_geode->position();
}

void AxisGroup::setPosition(const osg::Vec3 & position)
{
    m_geode->setPosition(position);
}

AxisGeometry::ColorScheme AxisGroup::colorScheme() const
{
    return m_geode->colorScheme();
}

void AxisGroup::setColorScheme(AxisGeometry::ColorScheme scheme)
{
    m_geode->setColorScheme(scheme);
}

osg::Node * osgCreateLine(const osg::Vec3d & a, const osg::Vec3d & b, const osg::Vec4 & color)
{
    osg::MatrixTransform * mat = new osg::MatrixTransform();
    mat->setName("line");
    osg::ref_ptr<osg::Geode> geode (new osg::Geode());  
    osg::ref_ptr<osg::Geometry> geometry (new osg::Geometry());

    osg::ref_ptr<osg::Vec3Array> vertices (new osg::Vec3Array(2));
    (*vertices)[0].set(0.0, 0.0, 0.0);
    (*vertices)[1] = b - a;
    geometry->setVertexArray (vertices.get());

    osg::ref_ptr<osg::Vec4Array> colors (new osg::Vec4Array(2));
    (*colors)[0] = color;
    (*colors)[1] = color;
    geometry->setColorArray (colors.get(), osg::Array::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

    geode->addDrawable( geometry.get() );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
    mat->setMatrix(osg::Matrix::translate(a));
    mat->addChild(geode);
    return mat;
}



LineGeode::LineGeode(const osg::Vec3d & a, const osg::Vec3d & b, ColorScheme scheme)
    : osg::Geode()
    , m_geom(new osg::Geometry())
    , m_colorScheme(scheme)
{
    m_geom->setVertexArray (new osg::Vec3Array(2));
    m_geom->setColorArray (new osg::Vec4Array(2), osg::Array::BIND_PER_VERTEX);
    m_geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,2));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    osg::Vec3Array * vertices = (osg::Vec3Array *)m_geom->getVertexArray();
    vertices->resize(2);
    (*vertices)[0] = a;
    (*vertices)[1] = b;
    m_geom->setVertexArray (vertices);

    addDrawable( m_geom.get() );

    applyColorScheme();
}

LineGeode::LineGeode(const LineGeode& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geode(obj, copyop)
    , m_geom(new osg::Geometry(*obj.m_geom, copyop))
{
}

LineGeode::~LineGeode()
{
}

void LineGeode::set(const osg::Vec3d & a, const osg::Vec3d & b)
{
    osg::Vec3Array * vertices = (osg::Vec3Array *)m_geom->getVertexArray();
    vertices->resize(2);
    (*vertices)[0] = a;
    (*vertices)[1] = b;
    m_geom->setVertexArray (vertices);
}

LineGeode::ColorScheme LineGeode::colorScheme() const
{ 
    return m_colorScheme;
}

void LineGeode::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

void LineGeode::applyColorScheme()
{
    osg::Vec3Array * vertices = (osg::Vec3Array *)m_geom->getVertexArray();
    osg::Vec4Array * colors = (osg::Vec4Array *)m_geom->getColorArray();
    unsigned num_vertices = vertices->size();
    colors->resize(num_vertices);
    switch(m_colorScheme)
    {
    case ColorSchemeWhite:
    default:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (1.0f, 1.0f, 1.0f, 1.0f);
        break;
        
    case ColorSchemeRed:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f);
        break;
    case ColorSchemeGreen:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (0.0f, 1.0f, 0.0f, 1.0f);
        break;
    case ColorSchemeBlue:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeCyan:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (0.0f, 1.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeMagenta:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (1.0f, 0.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeYellow:
        for(unsigned i = 0; i < num_vertices; i++)
            (*colors)[i] = osg::Vec4 (1.0f, 1.0f, 0.0f, 1.0f);
        break;
        
    case ColorSchemePinkCyan:
        for(unsigned i = 0; i < num_vertices; i++)
            if(i % 2)
                (*colors)[i] = osg::Vec4 (0.0f, 1.0f, 1.0f, 1.0f);
            else
                (*colors)[i] = osg::Vec4 (1.0f, 0.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeRedBlue:
        for(unsigned i = 0; i < num_vertices; i++)
            if(i % 2)
                (*colors)[i] = osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f);
            else
                (*colors)[i] = osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f);
        break;
        
    }
    m_geom->setColorArray (colors, osg::Array::BIND_PER_VERTEX);
}


BoundingBoxGeode::BoundingBoxGeode(const osg::BoundingBox & bbox, ColorScheme scheme)
    : osg::Geode()
    , m_mainBox()
    , m_cornerBox()
    , m_colorScheme(scheme)
{
    osg::Vec3d s = bbox._max - bbox._min;
    m_mainBox = new osg::ShapeDrawable(new osg::Box(bbox.center(), s.x(), s.y(), s.z()));
    addDrawable( m_mainBox.get() );

    float s_max = std::max(s.x(), std::max(s.y(), s.z()));
    osg::Vec3d s_10(s_max / 10, s_max / 10, s_max / 10);
    for(unsigned n = 0; n < NUM_CORNERS; n++)
    {
        m_cornerBox[n] = new osg::ShapeDrawable(new osg::Box(bbox.corner(n), s_10.x(), s_10.y(), s_10.z()));
        //addDrawable( m_cornerBox[n].get() );
        
        m_cornerText[n] = new osgText::Text;
        m_cornerText[n]->setPosition(bbox.corner(n));
        m_cornerText[n]->setText(std::string(1, (char)'0' + n));
        m_cornerText[n]->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
        m_cornerText[n]->setAxisAlignment(osgText::Text::SCREEN);
        m_cornerText[n]->setAlignment(osgText::Text::CENTER_CENTER);
        m_cornerText[n]->setColor(osg::Vec4(1,1,1,1));
        addDrawable( m_cornerText[n].get() );
    }

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
    m_mainBox->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    m_mainBox->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    applyColorScheme();
}

BoundingBoxGeode::BoundingBoxGeode(const BoundingBoxGeode& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geode(obj, copyop)
    , m_mainBox(new osg::ShapeDrawable(*obj.m_mainBox, copyop))
    , m_colorScheme(obj.m_colorScheme)
{
}

BoundingBoxGeode::~BoundingBoxGeode()
{
}

void BoundingBoxGeode::set(const osg::BoundingBox & bbox)
{
    osg::Vec3d s = bbox._max - bbox._min;
    if(s.valid())
    {
        m_mainBox->setShape(new osg::Box(bbox.center(), s.x(), s.y(), s.z()));
        m_mainBox->dirtyDisplayList();
        m_mainBox->dirtyBound();

        float s_max = std::max(s.x(), std::max(s.y(), s.z()));
        osg::Vec3d s_10(s_max / 10, s_max / 10, s_max / 10);
        for(unsigned n = 0; n < NUM_CORNERS; n++)
        {
            m_cornerBox[n]->setShape(new osg::Box(bbox.corner(n), s_10.x(), s_10.y(), s_10.z()));
            m_cornerBox[n]->dirtyDisplayList();
            m_cornerBox[n]->dirtyBound();

            m_cornerText[n]->setPosition(bbox.corner(n));
        }
    }
}

BoundingBoxGeode::ColorScheme BoundingBoxGeode::colorScheme() const
{ 
    return m_colorScheme;
}

void BoundingBoxGeode::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

void BoundingBoxGeode::applyColorScheme()
{
    osg::Vec4 mainColor, cornerColor, cornerMinMaxColor;
    switch(m_colorScheme)
    {
    case ColorSchemeWhite: 
        mainColor.set(1.0f, 1.0f, 1.0f, 0.5f); 
        cornerColor.set(0.5f, 0.5f, 0.5f, 1.0f); 
        cornerMinMaxColor.set(0.0f, 0.0f, 0.0f, 1.0f); 
        break;
    default:
    case ColorSchemeRed: 
        mainColor.set(1.0f, 0.0f, 0.0f, 0.5f); 
        cornerColor.set(0.0f, 1.0f, 0.0f, 1.0f); 
        cornerMinMaxColor.set(0.0f, 0.0f, 1.0f, 1.0f); 
        break;
    case ColorSchemeGreen: 
        mainColor.set(0.0f, 1.0f, 0.0f, 0.5f); 
        cornerColor.set(0.0f, 0.0f, 1.0f, 1.0f); 
        cornerMinMaxColor.set(1.0f, 0.0f, 0.0f, 1.0f); 
        break;
    case ColorSchemeBlue: 
        mainColor.set(0.0f, 0.0f, 1.0f, 0.5f); 
        cornerColor.set(0.0f, 1.0f, 0.0f, 1.0f); 
        cornerMinMaxColor.set(1.0f, 0.0f, 0.0f, 1.0f); 
        break;
    }
    m_mainBox->setColor(mainColor);
    m_cornerBox[0]->setColor(cornerMinMaxColor);
    m_cornerBox[NUM_CORNERS - 1]->setColor(cornerMinMaxColor);
    for(unsigned n = 1; n < NUM_CORNERS - 1; n++)
        m_cornerBox[n]->setColor(cornerColor);
}

class BoundingBoxGeode::AutoSizeGroup : public osg::Group 
{
public:
    AutoSizeGroup()
        : osg::Group()
        , m_geode(new BoundingBoxGeode)
    {
    }

    virtual void traverse(osg::NodeVisitor& nv)
    {
        if(nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
        {
            osg::BoundingSphere bs;
            for(osg::NodeList::iterator itr=_children.begin();
                itr!=_children.end();
                ++itr)
            {
                (*itr)->accept(nv);
                bs.expandBy((*itr)->getBound());
            }

            if(bs.radius() != m_lastBoundingSphere.radius() || bs.center() != m_lastBoundingSphere.center())
            {
                osg::ComputeBoundsVisitor cbv;
                for(osg::NodeList::iterator itr=_children.begin();
                    itr!=_children.end();
                    ++itr)
                {
                    (*itr)->accept(cbv);
                }
                m_lastBoundingSphere = bs;
                m_lastBoundingBox = cbv.getBoundingBox();
                m_geode->set(m_lastBoundingBox);
            }
        }
        else
            osg::Group::traverse(nv);
        m_geode->accept(nv);
    }

    BoundingBoxGeode * geode() { return m_geode.get(); }
    const BoundingBoxGeode * geode() const { return m_geode.get(); }

private:
    osg::ref_ptr<BoundingBoxGeode>  m_geode;
    osg::BoundingSphere             m_lastBoundingSphere;
    osg::BoundingBox                m_lastBoundingBox;
};

BoundingBoxGeode * BoundingBoxGeode::inject(osg::Group * group)
{
    AutoSizeGroup * autosize = new AutoSizeGroup();
    for( unsigned i=0; i<group->getNumChildren(); ++i )
    {
        autosize->addChild( group->getChild(i) );
    }
    group->removeChildren(0, group->getNumChildren() );
    group->addChild( autosize );
    return autosize->geode();
}

BoundingBoxGeode * BoundingBoxGeode::hook(osg::Node * node)
{
    AutoSizeGroup * autosize = dynamic_cast<AutoSizeGroup *>(node);
    osg::Group * parent = node->getParent(0);
    if(!autosize)
        autosize = dynamic_cast<AutoSizeGroup *>(parent);
    if(!autosize)
    {
        autosize = new AutoSizeGroup();
        autosize->addChild(node);
        osg::Group * parent = node->getParent(0);
        if(parent->removeChild(node))
        {
            parent->addChild(autosize);
        }
    }
    return autosize->geode();
}

bool BoundingBoxGeode::unhook(osg::Node * node)
{
    AutoSizeGroup * autosize = dynamic_cast<AutoSizeGroup *>(node);
    osg::Group * parent = node->getParent(0);
    if(!autosize)
        autosize = dynamic_cast<AutoSizeGroup *>(parent);

    if(autosize)
    {
        osg::Group * original_parent = autosize->getParent(0);
        original_parent->removeChild(autosize);
        original_parent->addChild(node);
        autosize->removeChild(node);
    }
    return (autosize != NULL);
}


PointGeode::PointGeode(const osg::Vec3d & pt, const osg::Vec3d & size, ColorScheme scheme)
    : osg::Geode()
    , m_drawable()
    , m_colorScheme(scheme)
{
    m_drawable = new osg::ShapeDrawable(new osg::Box(pt, size.x(), size.y(), size.z()));
    addDrawable( m_drawable.get() );

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    applyColorScheme();
}

PointGeode::PointGeode(const PointGeode& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geode(obj, copyop)
    , m_drawable(new osg::ShapeDrawable(*obj.m_drawable, copyop))
    , m_colorScheme(obj.m_colorScheme)
{
}

PointGeode::~PointGeode()
{
}


void PointGeode::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

void PointGeode::applyColorScheme()
{
    osg::Vec4 mainColor;
    switch(m_colorScheme)
    {
    case ColorSchemeWhite: 
        mainColor.set(1.0f, 1.0f, 1.0f, 1.0f); 
        break;
    default:
    case ColorSchemeRed: 
        mainColor.set(1.0f, 0.0f, 0.0f, 1.0f); 
        break;
    case ColorSchemeGreen: 
        mainColor.set(0.0f, 1.0f, 0.0f, 1.0f); 
        break;
    case ColorSchemeBlue: 
        mainColor.set(0.0f, 0.0f, 1.0f, 1.0f); 
        break;
    case ColorSchemeMagenta: 
        mainColor.set(1.0f, 0.0f, 1.0f, 1.0f); 
        break;
    case ColorSchemeCyan: 
        mainColor.set(0.0f, 1.0f, 1.0f, 1.0f); 
        break;
    case ColorSchemeYellow: 
        mainColor.set(1.0f, 1.0f, 1.0f, 1.0f); 
        break;
    }
    m_drawable->setColor(mainColor);
}


void PointGeode::set(const osg::Vec3d & pt, const osg::Vec3d & size)
{
    m_drawable->setShape(new osg::Box(pt, size.x(), size.y(), size.z()));
    m_drawable->dirtyDisplayList();
    m_drawable->dirtyBound();
}

PlaneGeode::PlaneGeode(const osg::Plane & plane, ColorScheme scheme)
    : osg::Geode()
    , m_plane(new osg::ShapeDrawable())
    , m_colorScheme(scheme)
{
    set(plane);

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
    getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    addDrawable( m_plane.get() );

    applyColorScheme();
}

PlaneGeode::PlaneGeode(const PlaneGeode& obj,const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geode(obj, copyop)
    , m_plane(new osg::ShapeDrawable(*obj.m_plane, copyop))
{
}

PlaneGeode::~PlaneGeode()
{
}

void PlaneGeode::set(const osg::Plane & plane)
{
    osg::Box * box = new osg::Box(osg::Vec3(), 1000.0, 1000.0, 0.1);
    osg::Vec3d norm = plane.getNormal();
    osg::Quat q;
    q.makeRotate(osg::Vec3d(0,0,1), norm);
    box->setRotation(q);
    m_plane->setShape(box);
    m_plane->dirtyDisplayList();
    m_plane->dirtyBound();
}

PlaneGeode::ColorScheme PlaneGeode::colorScheme() const
{ 
    return m_colorScheme;
}

void PlaneGeode::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

void PlaneGeode::applyColorScheme()
{
    osg::Vec4 c;
    switch(m_colorScheme)
    {
    case ColorSchemeWhite: c.set(1.0f, 1.0f, 1.0f, 0.5f); break;
    case ColorSchemeRed: c.set(1.0f, 0.0f, 0.0f, 0.5f); break;
    case ColorSchemeGreen: c.set(0.0f, 1.0f, 0.0f, 0.5f); break;
    case ColorSchemeBlue: c.set(0.0f, 0.0f, 1.0f, 0.5f); break;
    }
    m_plane->setColor(c);
}


MarkerGeometry::MarkerGeometry(const osg::Vec3 & pos, float length, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_upVector(pos)
    , m_length(length)
    , m_colorScheme(scheme)
{
    m_upVector.normalize();
    setVertexArray(new osg::Vec3Array(2));
    setColorArray(new osg::Vec4Array(2), osg::Array::BIND_PER_VERTEX);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    applyLength();
    applyColorScheme();
}


MarkerGeometry::MarkerGeometry(const osg::Vec3 & pos, const osg::Vec3 & upVector, float length, ColorScheme scheme)
    : osg::Geometry()
    , m_pos(pos)
    , m_upVector(upVector)
    , m_length(length)
    , m_colorScheme(scheme)
{
    setVertexArray(new osg::Vec3Array(2));
    setColorArray(new osg::Vec4Array(2), osg::Array::BIND_PER_VERTEX);
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    applyLength();
    applyColorScheme();
}

MarkerGeometry::MarkerGeometry(const MarkerGeometry& obj, const osg::CopyOp& copyop/* =osg::CopyOp::SHALLOW_COPY */)
    : osg::Geometry(obj, copyop)
    , m_pos(obj.m_pos)
    , m_length(obj.m_length)
    , m_colorScheme(obj.m_colorScheme)
{
}

MarkerGeometry::~MarkerGeometry()
{
}

void MarkerGeometry::applyLength()
{
    osg::Vec3Array * vertices = (osg::Vec3Array *)getVertexArray();
    (*vertices)[0] = m_pos;
    (*vertices)[1] = m_pos + m_upVector * m_length;
    setVertexArray(vertices);
}

void MarkerGeometry::applyColorScheme()
{
    osg::Vec4Array * colors = (osg::Vec4Array *)getColorArray();
    switch (m_colorScheme)
    {
    case ColorSchemePrimary:
    default:
        (*colors)[0] = osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
        (*colors)[1] = osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
        break;
    case ColorSchemeSecondary:
        (*colors)[0] = osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f);
        (*colors)[1] = osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f);
        break;
    }
    setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}

void MarkerGeometry::setLength(float length)
{
    m_length = length;
    applyLength();
}

const osg::Vec3 & MarkerGeometry::position() const
{
    return m_pos;
}

void MarkerGeometry::setPosition(const osg::Vec3 & position)
{
    m_pos = position;
    applyLength();
}

const osg::Vec3 & MarkerGeometry::upVector() const
{
    return m_upVector;
}

void MarkerGeometry::setUpVector(const osg::Vec3 & upVector)
{
    m_upVector = upVector;
    applyLength();
}

void MarkerGeometry::setColorScheme(ColorScheme scheme)
{
    m_colorScheme = scheme;
    applyColorScheme();
}

CenterMarker::CenterMarker(const std::string & name, const osg::Vec3d & center, float radius)
    : geom(nullptr)
    , circle(nullptr)
    , text(nullptr)
{
    osg::Vec3d upVector = center;
    upVector.normalize();

    geom = new MarkerGeometry(osg::Vec3(), upVector, radius);
    circle = new CircleGeometry(osg::Vec3(), upVector, radius);
    text = new osgText::Text;

    setMatrix(osg::Matrixd::translate(center));
    std::stringstream ss;
    ss << name << std::endl << radius;
    text->setPosition(osg::Vec3());
    text->setText(ss.str());
    // osgText::Text turns on depth writing by default, even if you turned it off.
    text->setEnableDepthWrites(false);

    text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
    text->setAxisAlignment(osgText::Text::SCREEN);
    text->setDrawMode(osgText::Text::TEXT);
    text->setAlignment(osgText::Text::CENTER_BOTTOM);
    //text->setFont(m_font.get());
    text->setFontResolution(24, 24);
    text->setCharacterSize(20.0f);
    text->setColor(osg::Vec4(1,1,1,1));
    text->setBackdropType(osgText::Text::OUTLINE);
    text->setBackdropOffset(0.1f, 0.1f);
    text->setBackdropImplementation(osgText::Text::POLYGON_OFFSET);
    text->setBackdropColor(osg::Vec4(0,0,0,1));

    addChild(geom);
    addChild(circle);
    addChild(text);

    osg::StateSet * stateSet = getOrCreateStateSet();

    stateSet->setAttribute(new osg::LineWidth(2.0f));
    stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);

}

namespace {
    bool hasCenterMarker(osg::Group * node)
    {
        bool ret = false;
        for (unsigned n = 0; !ret && n < node->getNumChildren(); ++n)
        {
            osg::Node * child = node->getChild(n);
            if (dynamic_cast<CenterMarker*>(child))
                ret = true;
        }
        return ret;
    }
    bool removeCenterMarker(osg::Group * node)
    {
        bool ret = false;
        for (unsigned n = 0; !ret && n < node->getNumChildren(); ++n)
        {
            osg::Node * child = node->getChild(n);
            if (dynamic_cast<CenterMarker*>(child))
            {
                node->removeChild(n);
                ret = true;
            }
        }
        return ret;
    }
    template<class T>
    bool addCenterMarker(T * node, const osg::Vec3d & center, float radius)
    {
        CenterMarker * marker = nullptr;
        const std::string & name = node->getName();
        std::string::size_type idx = name.rfind('/');
        if (idx != std::string::npos)
            marker = new CenterMarker(name.substr(idx+1), center, radius);
        else
            marker = new CenterMarker(std::string(), center, radius);
        node->addChild(marker);
        return true;
    }
    template<>
    bool addCenterMarker<osg::LOD>(osg::LOD * node, const osg::Vec3d & center, float radius)
    {
        CenterMarker * marker = nullptr;
        const std::string & name = node->getName();
        std::string::size_type idx = name.rfind('/');
        if (idx != std::string::npos)
            marker = new CenterMarker(name.substr(idx + 1), center, radius);
        else
            marker = new CenterMarker(std::string(), center, radius);
        node->addChild(marker, 0, FLT_MAX);
        return true;
    }
    template<class T>
    void toggleCenterMarker(T * node, bool enable)
    {
        bool current = hasCenterMarker(node);
        if (current && !enable)
            removeCenterMarker(node);
        else if (!current && enable)
            addCenterMarker(node, node->getCenter(), node->getRadius());
    }

    std::string use_togglecentermarker(const std::string & uri, bool on)
    {
        if(on)
            return uri + "(1)." TOGGLECENTERMARKER_PL_EXTENSION;
        else
            return uri + "(0)." TOGGLECENTERMARKER_PL_EXTENSION;
    }

    bool isToggleCenterMarker(const std::string & uri)
    {
        return (osgDB::getFileExtension(uri).compare(TOGGLECENTERMARKER_PL_EXTENSION) == 0);
    }

}


QueryCenterMarkerVisitor::QueryCenterMarkerVisitor()
    : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    , _on(false)
{
}


void QueryCenterMarkerVisitor::apply(osg::ProxyNode& node)
{
    for (unsigned n = 0; !_on && n < node.getNumFileNames(); n++)
    {
        const std::string & uri = node.getFileName(n);
        if (isToggleCenterMarker(uri))
            _on = true;
    }
    if (!_on)
        _on = hasCenterMarker(&node);
    if(!_on)
        traverse(node);
}

void QueryCenterMarkerVisitor::apply(osg::Group& node)
{
    if (!_on)
        _on = hasCenterMarker(&node);
    if (!_on)
        traverse(node);
}

void QueryCenterMarkerVisitor::apply(osg::PagedLOD& node)
{
    for (unsigned n = 0; !_on && n < node.getNumFileNames(); n++)
    {
        const std::string & uri = node.getFileName(n);
        if (isToggleCenterMarker(uri))
            _on = true;
    }
    if (!_on)
        _on = hasCenterMarker(&node);
    if (!_on)
        traverse(node);
}

ToggleCenterMarkerVisitor::ToggleCenterMarkerVisitor(bool on)
    : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    , _on(on)
{
}


void ToggleCenterMarkerVisitor::apply(osg::ProxyNode& node)
{
    for (unsigned n = 0; n < node.getNumFileNames(); n++)
    {
        std::string olduri = node.getFileName(n);
        node.setFileName(n, use_togglecentermarker(olduri, _on));
    }
    toggleCenterMarker(&node, _on);
    traverse(node);
}

void ToggleCenterMarkerVisitor::apply(osg::LOD& node)
{
    toggleCenterMarker(&node, _on);
    traverse(node);
}

void ToggleCenterMarkerVisitor::apply(osg::PagedLOD& node)
{
    for (unsigned n = 0; n < node.getNumFileNames(); n++)
    {
        std::string olduri = node.getFileName(n);
        node.setFileName(n, use_togglecentermarker(olduri, _on));
    }
    apply(static_cast<osg::LOD&>(node));
}


struct ToggleCenterMarkerPseudoLoader : public osgDB::ReaderWriter
{
    ToggleCenterMarkerPseudoLoader()
    {
        this->supportsExtension(TOGGLECENTERMARKER_PL_EXTENSION, "sgi_togglecentermarker pseudoloader");
    }

    const char* className()
    {
        return "sgi_togglecentermarker pseudoloader";
    }

    bool acceptsExtension(const std::string& extension) const
    {
        return osgDB::equalCaseInsensitive(extension, TOGGLECENTERMARKER_PL_EXTENSION);
    }

    ReadResult readObject(const std::string& filename, const osgDB::Options* options) const
    {
        return readNode(filename, options);
    }

    ReadResult readNode(const std::string& filename, const osgDB::Options* options) const
    {
        ReadResult ret;
        if (!acceptsExtension(osgDB::getLowerCaseFileExtension(filename)))
            ret = ReadResult::FILE_NOT_HANDLED;
        else
        {
            std::string stripped = osgDB::getNameLessExtension(filename);
            std::string::size_type end = stripped.rfind(')');
            std::string::size_type begin = stripped.rfind('(', end);

            if (end > begin)
            {
                // converts a string to primitive using serialization
                std::string state = stripped.substr(begin + 1, end - begin - 1);
                bool on = (state.compare("1") == 0);

                // chop of state
                stripped.resize(begin);

                ret = osgDB::Registry::instance()->readNode(stripped, options, false);
                if (ret.success() && ret.validNode())
                {
                    ToggleCenterMarkerVisitor tcmv(on);
                    ret.getNode()->accept(tcmv);
                }
            }
            else
                ret = ReadResult::ERROR_IN_READING_FILE;
        }
        return ret;
    }
};

} // namespace osg_plugin
} // namespace sgi

#define REGISTER_OSGPLUGIN_EX(ext, classname, proxyname) \
    extern "C" OSGDB_PLUGIN_EXPORT void osgdb_##ext(void) {} \
    static osgDB::RegisterReaderWriterProxy<classname> g_proxy_##proxyname;

REGISTER_OSGPLUGIN_EX(TOGGLECENTERMARKER_PL_EXTENSION, sgi::osg_plugin::ToggleCenterMarkerPseudoLoader, ToggleCenterMarkerPseudoLoader)
