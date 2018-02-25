// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/ReferencedPicker>
#include <osgGA/EventQueue>
#include <osgViewer/View>

namespace sgi {

ReferencedPickerBase::ReferencedPickerBase(const osg::Vec2f & pos, osgViewer::View * view, osg::Node * root, unsigned traversalMask)
    : osg::Referenced()
    , m_pos(pos)
    , m_result(false)
    , m_view(view)
    , m_root(root)
    , m_traversalMask(traversalMask)
{
}

ReferencedPickerBase::ReferencedPickerBase(osgViewer::GraphicsWindow * gw, osg::Node * root, unsigned traversalMask)
    : osg::Referenced()
    , m_pos()
    , m_result(false)
    , m_view()
    , m_root(root)
    , m_traversalMask(traversalMask)
{
    osgGA::EventQueue * queue = gw->getEventQueue();
    osgGA::GUIEventAdapter* eventState = queue->getCurrentEventState();
    m_pos = osg::Vec2f(eventState->getX(), eventState->getY());

    osgViewer::GraphicsWindow::Views views;
    gw->getViews(views);
    osgViewer::View * viewptr = views.front();
    m_view = viewptr;

    if(!m_root.valid())
    {
        m_root = viewptr->getCamera();
        m_traversalMask = viewptr->getCamera()->getCullMask();
    }
}

ReferencedPickerBase::ReferencedPickerBase(const osgGA::GUIEventAdapter & ea, osg::Node * root, unsigned traversalMask)
    : osg::Referenced()
    , m_pos(osg::Vec2f(ea.getX(), ea.getY()))
    , m_result(false)
    , m_view()
    , m_root(root)
    , m_traversalMask(traversalMask)
{
}

ReferencedPickerBase::~ReferencedPickerBase()
{
}

#ifdef SGI_USE_OSGEARTH
ReferencedPicker::ReferencedPicker(const osg::Vec2f & pos, osgViewer::View * view, osg::Node * root, unsigned traversalMask, float hitBufferSize, PickerType::Limit limit)
    : ReferencedPickerBase(pos, view, root, traversalMask)
    , m_picker(NULL)
    , m_thickness(hitBufferSize)
    , m_hits()
    , m_limit(limit)
{
    recalc();
}

ReferencedPicker::ReferencedPicker(osgViewer::GraphicsWindow * gw, osg::Node * root, unsigned traversalMask, float hitBufferSize, PickerType::Limit limit)
    : ReferencedPickerBase(gw, root, traversalMask)
    , m_picker(NULL)
    , m_thickness(hitBufferSize)
    , m_hits()
    , m_limit(limit)
{
    recalc();
}

ReferencedPicker::ReferencedPicker(const osgGA::GUIEventAdapter & ea, osg::Node * root, unsigned traversalMask, float hitBufferSize, PickerType::Limit limit)
    : ReferencedPickerBase(ea, root, traversalMask)
    , m_picker(NULL)
    , m_thickness(hitBufferSize)
    , m_hits()
    , m_limit(limit)
{
    recalc();
}

/** dtor */
ReferencedPicker::~ReferencedPicker()
{
    if(m_picker)
        delete m_picker;
}

void ReferencedPicker::recalc()
{
    if(m_picker)
    {
        delete m_picker;
        m_picker = NULL;
    }

    m_picker = new PickerType(dynamic_cast<osgViewer::View*>(m_view.get()), m_root, m_traversalMask, m_thickness, m_limit);
    m_result = m_picker->pick(m_pos.x(), m_pos.y(), m_hits);
}
#endif // SGI_USE_OSGEARTH

ReferencedLinePicker::ReferencedLinePicker(const osg::Vec2f & pos, osgViewer::View * view, osg::Node * root, unsigned traversalMask)
    : ReferencedPickerBase(pos, view, root, traversalMask)
    , m_picker(NULL)
{
    recalc();
}

ReferencedLinePicker::ReferencedLinePicker(osgViewer::GraphicsWindow * gw, osg::Node * root, unsigned traversalMask)
    : ReferencedPickerBase(gw, root, traversalMask)
    , m_picker(NULL)
{
    recalc();
}

ReferencedLinePicker::ReferencedLinePicker(const osgGA::GUIEventAdapter & ea, osg::Node * root, unsigned traversalMask)
    : ReferencedPickerBase(ea, root, traversalMask)
    , m_picker(NULL)
{
    recalc();
}

/** dtor */
ReferencedLinePicker::~ReferencedLinePicker()
{
    m_picker = NULL;
}

void ReferencedLinePicker::recalc()
{
    m_picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, m_pos.x(),m_pos.y());
    osgUtil::IntersectionVisitor iv(m_picker.get());

    iv.setTraversalMask(m_traversalMask);

    float local_x, local_y = 0.0;
    const osg::Camera* camera = m_view->getCameraContainingPosition(m_pos.x(), m_pos.y(), local_x, local_y);
    if ( !camera )
        camera = m_view->getCamera();

    const_cast<osg::Camera*>(camera)->accept(iv);

    m_result = m_picker->containsIntersections();
}

const ReferencedLinePicker::Hits & ReferencedLinePicker::hits() const
{
    return m_picker->getIntersections();
}

} // namespace sgi
