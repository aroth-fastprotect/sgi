#pragma once

#include <osg/Geometry>
#include <osg/Billboard>

namespace osg {
    class ShapeDrawable;
}

namespace osgText {
    class Text;
}

namespace osgUtil {
    class RenderBin;
}

namespace sgi {
namespace osg_plugin {

class AxisGeometry : public osg::Geometry
{
public:
    enum ColorScheme {
        ColorSchemePrimary,
        ColorSchemeSecondary,
        ColorSchemeDefault = ColorSchemePrimary
    };
    
    AxisGeometry(const osg::Vec3 & pos=osg::Vec3(), float length=10.0f, ColorScheme scheme=ColorSchemeDefault);
    AxisGeometry(const osg::Vec3 & pos, const osg::Vec3 & length, ColorScheme scheme=ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    AxisGeometry(const AxisGeometry& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~AxisGeometry();

    META_Object(terra3d-common-osg, AxisGeometry);

public:
    void                    set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side);

    const osg::Vec3 &       length() const { return m_length; }
    void                    setLength(float length);
    void                    setLength(const osg::Vec3 & length);

    ColorScheme             colorScheme() const { return m_colorScheme; }
    void                    setColorScheme(ColorScheme scheme);

protected:
    void                    applyLength();
    void                    applyColorScheme();

protected:
    osg::Vec3               m_pos;
    osg::Vec3               m_length;
    ColorScheme             m_colorScheme;
};

class AxisGeode : public osg::Geode
{
public:
    AxisGeode(const osg::Vec3 & pos=osg::Vec3(), float length=10.0f, AxisGeometry::ColorScheme scheme=AxisGeometry::ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    AxisGeode(const AxisGeode& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~AxisGeode();

    META_Node(terra3d-common-osg, AxisGeode);


public:
    void                    set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side);

    const osg::Vec3 &       length() const;
    void                    setLength(float length);
    void                    setLength(const osg::Vec3 & length);

    AxisGeometry::ColorScheme colorScheme() const;
    void                    setColorScheme(AxisGeometry::ColorScheme scheme);

protected:
    osg::ref_ptr<AxisGeometry>  m_geom;
};

class AxisBillboard : public osg::Billboard
{
public:
    AxisBillboard(const osg::Vec3 & pos = osg::Vec3(), float length=10.0f, AxisGeometry::ColorScheme scheme=AxisGeometry::ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    AxisBillboard(const AxisBillboard& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~AxisBillboard();

    META_Node(terra3d-common-osg, AxisBillboard);

public:
    void                    set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side);

    const osg::Vec3 &       length() const;
    void                    setLength(float length);
    void                    setLength(const osg::Vec3 & length);

    AxisGeometry::ColorScheme colorScheme() const;
    void                    setColorScheme(AxisGeometry::ColorScheme scheme);

protected:
    osg::ref_ptr<AxisGeometry>  m_geom;
};

class AxisGroup : public osg::Group
{
public:
    AxisGroup(const osg::Vec3 & pos = osg::Vec3(), float length=10.0f, AxisGeometry::ColorScheme scheme=AxisGeometry::ColorSchemeDefault, bool autoSize=true);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    AxisGroup(const AxisGroup& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~AxisGroup();

    META_Node(terra3d-common-osg, AxisGroup);

    virtual void            traverse(osg::NodeVisitor& nv);

public:
    void                    set(const osg::Vec3d & up, const osg::Vec3d & front, const osg::Vec3d & side);
    
    bool                    autoSize() const;
    void                    setAutoSize(bool enable);

    const osg::Vec3 &       length() const;
    void                    setLength(float length);
    void                    setLength(const osg::Vec3 & length);

    AxisGeometry::ColorScheme colorScheme() const;
    void                    setColorScheme(AxisGeometry::ColorScheme scheme);

protected:
    osg::ref_ptr<AxisGeode> m_geode;
    bool                    m_autoSize;
};

class LineGeode : public osg::Geode
{
public:
    enum ColorScheme {
        ColorSchemeWhite,
        ColorSchemePinkCyan,
        ColorSchemeRedBlue,
        ColorSchemeRed,
        ColorSchemeGreen,
        ColorSchemeBlue,
        ColorSchemeCyan,
        ColorSchemeMagenta,
        ColorSchemeYellow,
        ColorSchemeDefault = ColorSchemePinkCyan
    };
    
public:
    LineGeode(const osg::Vec3d & a=osg::Vec3d(), const osg::Vec3d & b=osg::Vec3d(), ColorScheme scheme=ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    LineGeode(const LineGeode& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~LineGeode();

    META_Node(terra3d-common-osg, LineGeode);


public:
    void                    set(const osg::Vec3d & a, const osg::Vec3d & b);

    ColorScheme             colorScheme() const;
    void                    setColorScheme(ColorScheme scheme);

protected:
    void                    applyColorScheme();

protected:
    osg::ref_ptr<osg::Geometry>  m_geom;
    ColorScheme             m_colorScheme;
};

class BoundingBoxGeode : public osg::Geode
{
public:
    enum ColorScheme {
        ColorSchemeWhite,
        ColorSchemeRed,
        ColorSchemeGreen,
        ColorSchemeBlue,
        ColorSchemeDefault = ColorSchemeRed
    };
    enum {
        NUM_CORNERS = 8
    };
    
public:
    BoundingBoxGeode(const osg::BoundingBox & bbox=osg::BoundingBox(), ColorScheme scheme=ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    BoundingBoxGeode(const BoundingBoxGeode& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~BoundingBoxGeode();

    META_Node(terra3d-common-osg, BoundingBoxGeode);
    
    void                    set(const osg::BoundingBox & bbox);

    ColorScheme             colorScheme() const;
    void                    setColorScheme(ColorScheme scheme);
    
    static BoundingBoxGeode * inject(osg::Group * group);
    static BoundingBoxGeode * hook(osg::Node * node);
    static bool               unhook(osg::Node * node);

protected:
    void                    applyColorScheme();
    
    class AutoSizeGroup;

protected:
    osg::ref_ptr<osg::ShapeDrawable>    m_mainBox;
    osg::ref_ptr<osg::ShapeDrawable>    m_cornerBox[NUM_CORNERS];
    osg::ref_ptr<osgText::Text>         m_cornerText[NUM_CORNERS];
    ColorScheme                         m_colorScheme;
};

class PointGeode : public osg::Geode
{
public:
    enum ColorScheme {
        ColorSchemeWhite,
        ColorSchemeRed,
        ColorSchemeGreen,
        ColorSchemeBlue,
        ColorSchemeMagenta,
        ColorSchemeCyan,
        ColorSchemeYellow,
        ColorSchemeDefault = ColorSchemeRed
    };
    
public:
    PointGeode(const osg::Vec3d & pt=osg::Vec3d(), const osg::Vec3d & size=osg::Vec3d(1,1,1), ColorScheme scheme=ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    PointGeode(const PointGeode& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~PointGeode();

    META_Node(terra3d-common-osg, PointGeode);
    
    void                    set(const osg::Vec3d & pt, const osg::Vec3d & size=osg::Vec3d(1,1,1));

    ColorScheme             colorScheme() const;
    void                    setColorScheme(ColorScheme scheme);

protected:
    void                    applyColorScheme();

protected:
    osg::ref_ptr<osg::ShapeDrawable>    m_drawable;
    ColorScheme                         m_colorScheme;
};

class PlaneGeode : public osg::Geode
{
public:
    enum ColorScheme {
        ColorSchemeWhite,
        ColorSchemeRed,
        ColorSchemeGreen,
        ColorSchemeBlue,
        ColorSchemeDefault = ColorSchemeRed
    };
    
public:
    PlaneGeode(const osg::Plane & plane=osg::Plane(), ColorScheme scheme=ColorSchemeDefault);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    PlaneGeode(const PlaneGeode& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~PlaneGeode();

    META_Node(terra3d-common-osg, PlaneGeode);

    ColorScheme             colorScheme() const;
    void                    setColorScheme(ColorScheme scheme);
    
    void                    set(const osg::Plane & plane);

protected:
    void                    applyColorScheme();
    
    class AutoSizeGroup;

protected:
    osg::ref_ptr<osg::ShapeDrawable>  m_plane;
    ColorScheme             m_colorScheme;
};

class ToggleCenterMarkerVisitor : public osg::NodeVisitor
{
    bool _on;
public:
    ToggleCenterMarkerVisitor(bool on);

    void apply(osg::ProxyNode& node) override;
    void apply(osg::LOD& node) override;
    void apply(osg::PagedLOD& node) override;
};

} // namespace osg_plugin
} // namespace sgi
