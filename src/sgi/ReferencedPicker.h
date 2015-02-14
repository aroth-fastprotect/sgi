#pragma once

#include <osgEarth/Pickers>
#include <osgUtil/LineSegmentIntersector>

namespace osgGA {
    class GUIEventAdapter;
}

namespace sgi {

class ReferencedPickerBase : public osg::Referenced
{
public:
    ReferencedPickerBase(osgViewer::GraphicsWindow * gw)
        : osg::Referenced()
        , m_graphicsWindow(gw)
        , m_ea()
        , m_pos()
        , m_thickness(0)
        , m_result(false)
    {
        osgGA::EventQueue * queue = m_graphicsWindow->getEventQueue();
        osgGA::GUIEventAdapter* eventState = queue->getCurrentEventState();
        m_pos = osg::Vec2f(eventState->getX(), eventState->getY());
    }

    ReferencedPickerBase::ReferencedPickerBase(osgGA::GUIEventAdapter * ea)
        : osg::Referenced()
        , m_graphicsWindow(const_cast<osgViewer::GraphicsWindow*>(dynamic_cast<const osgViewer::GraphicsWindow*>(ea->getGraphicsContext())))
        , m_ea(ea)
        , m_pos()
        , m_thickness(0)
        , m_result(false)
    {
        m_pos = osg::Vec2f(m_ea->getX(), m_ea->getY());
    }
    /** dtor */
    virtual ~ReferencedPickerBase()
    {
    }

    bool                        result() const { return m_result; }
    const osg::Vec2f &          position() const { return m_pos; }
    float                       thickness() const { return m_thickness; }

    virtual void                recalc() = 0;
    void                        setThickness(float value) { m_thickness = value; }

protected:
    osgViewer::View *           getView()
    {
        if(m_graphicsWindow)
        {
            osgViewer::GraphicsWindow::Views views;
            m_graphicsWindow->getViews(views);
            
            osgViewer::View * viewptr = views.front();
            return viewptr;
        }
        else
            return NULL;
    }
    const osgViewer::View *     getView() const
    {
        return const_cast<ReferencedPickerBase*>(this)->getView();
    }
    unsigned                    getTraversalMask() const
    {
        unsigned excludedNodeMask = ~0u;
        const osgViewer::View * viewptr = getView();
        return viewptr->getCamera()->getCullMask() & excludedNodeMask;
    }

protected:
    osg::ref_ptr<osgViewer::GraphicsWindow> m_graphicsWindow;
    osg::ref_ptr<osgGA::GUIEventAdapter>    m_ea;
    osg::Vec2f                              m_pos;
    float                                   m_thickness;
    bool                                    m_result;
};

class ReferencedPicker : public ReferencedPickerBase
{
public:
    ReferencedPicker(osgViewer::GraphicsWindow * gw, float hitBufferSize=1.0f)
        : ReferencedPickerBase(gw)
        , m_picker(NULL)
        , m_thickness(hitBufferSize)
    {
        recalc();
    }

    ReferencedPicker::ReferencedPicker(osgGA::GUIEventAdapter * ea, float hitBufferSize=1.0f)
        : ReferencedPickerBase(ea)
        , m_picker(NULL)
        , m_thickness(hitBufferSize)
    {
        recalc();
    }

    /** dtor */
    virtual ~ReferencedPicker()
    {
        if(m_picker)
            delete m_picker;
    }

    typedef osgEarth::PrimitiveIntersector::Intersection  Hit;
    typedef osgEarth::PrimitiveIntersector::Intersections Hits;

    const Hits & hits() const { return m_hits; }
    
    virtual void                recalc()
    {
        if(m_picker)
        {
            delete m_picker;
            m_picker = NULL;
        }

        osg::Node::NodeMask traversalMask = (osg::Node::NodeMask)getTraversalMask();

        m_picker = new osgEarth::Picker(getView(), NULL, traversalMask, m_thickness, osgEarth::Picker::NO_LIMIT);
        m_result = m_picker->pick(m_pos.x(), m_pos.y(), m_hits);
    }

private:
    osgEarth::Picker *          m_picker;
    Hits                        m_hits;
};

class ReferencedLinePicker : public ReferencedPickerBase
{
public:
    ReferencedLinePicker(osgViewer::GraphicsWindow * gw)
        : ReferencedPickerBase(gw)
        , m_picker(NULL)
    {
        recalc();
    }

    ReferencedLinePicker(osgGA::GUIEventAdapter * ea)
        : ReferencedPickerBase(ea)
        , m_picker(NULL)
    {
        recalc();
    }

    /** dtor */
    virtual ~ReferencedLinePicker()
    {
        m_picker = NULL;
    }

    typedef osgUtil::LineSegmentIntersector::Intersection  Hit;
    typedef osgUtil::LineSegmentIntersector::Intersections Hits;
    
    virtual void recalc()
    {
        m_picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, m_pos.x(),m_pos.y());
        osgUtil::IntersectionVisitor iv(m_picker.get());

        osg::Node::NodeMask traversalMask = (osg::Node::NodeMask)getTraversalMask();
        iv.setTraversalMask(traversalMask);

        osgViewer::View * viewptr = getView();
        float local_x, local_y = 0.0;
        const osg::Camera* camera = viewptr->getCameraContainingPosition(m_pos.x(), m_pos.y(), local_x, local_y);
        if ( !camera )
            camera = viewptr->getCamera();
        
        const_cast<osg::Camera*>(camera)->accept(iv);
        
        m_result = m_picker->containsIntersections();
    }

    const Hits & ReferencedLinePicker::hits() const
    {
        return m_picker->getIntersections();
    }
    
private:
    osg::ref_ptr<osgUtil::LineSegmentIntersector> m_picker;
};

} // namespace sgi
