#include "stdafx.h"
#include <osg/Camera>
#include "ExtraViewDialog.h"

#include <QPushButton>
#include <QTimer>
#include <QResizeEvent>

#include <ui_ExtraViewDialog.h>

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/helpers/osg>
#include <osgViewer/View>
#include <osgViewer/CompositeViewer>

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#endif // SGI_USE_OSGQT

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace osg_plugin {

ViewOSG::ViewOSG(QWidget * parent)
    : QWidget(parent)
{
}

ViewOSG::~ViewOSG()
{
}

void ViewOSG::setCamera(osgViewer::CompositeViewer * viewer, osg::Camera * camera)
{
    _viewer = viewer;
    osg::GraphicsContext * existingContext = camera->getGraphicsContext();
    _gfx = createGraphicsWindow(0, 0, QWidget::width(), QWidget::height(), existingContext);

#ifdef SGI_USE_OSGQT
    osgQt::GraphicsWindowQt* gwq = dynamic_cast<osgQt::GraphicsWindowQt*>(_gfx.get());
    if(gwq)
        _widget = gwq->getGLWidget();
#endif

    QHBoxLayout * l = new QHBoxLayout;
    l->addWidget(_widget);
    setLayout(l);

    _viewCamera = new osg::Camera;
    _viewCamera->setGraphicsContext(_gfx.get());
    _camera = camera;

    _view = new osgViewer::View;
    _view->setCamera(_viewCamera.get());
    _viewer->addView(_view);

    updateCamera();
}

void ViewOSG::updateCamera()
{
    osg::ref_ptr<osg::Camera> camera;
    if (_camera.lock(camera))
    {
        _viewCamera->setViewport(camera->getViewport());
        _viewCamera->setProjectionMatrix(camera->getProjectionMatrix());
        _viewCamera->setViewMatrix(camera->getViewMatrix());
        _viewCamera->setTransformOrder(camera->getTransformOrder());
        _viewCamera->setRenderOrder(camera->getRenderOrder(), camera->getRenderOrderNum());
        _viewCamera->setRenderTargetImplementation(camera->getRenderTargetImplementation(), camera->getRenderTargetFallback());
        _viewCamera->setDrawBuffer(camera->getDrawBuffer());
        _viewCamera->setReadBuffer(camera->getReadBuffer());
        
        _viewCamera->setProjectionResizePolicy(camera->getProjectionResizePolicy());
        _viewCamera->setColorMask(camera->getColorMask());

        _viewCamera->setClearColor(camera->getClearColor());
        _viewCamera->setClearStencil(camera->getClearStencil());
        _viewCamera->setClearAccum(camera->getClearAccum());
        _viewCamera->setClearMask(camera->getClearMask());
        //_viewCamera->setAllowEventFocus(camera->getAllowEventFocus());
        _viewCamera->setAllowEventFocus(false);

        unsigned numChilds = camera->getNumChildren();
        _viewCamera->removeChildren(numChilds, _viewCamera->getNumChildren() - numChilds);
        for (unsigned n = 0; n < numChilds; ++n)
        {
            _viewCamera->setChild(n, camera->getChild(n));
        }
    }
}

osgViewer::GraphicsWindow* ViewOSG::createGraphicsWindow(int x, int y, int w, int h, osg::GraphicsContext * sharedContext, const std::string& name, bool windowDecoration)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->sharedContext = sharedContext;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

#ifdef SGI_USE_OSGQT
    return new osgQt::GraphicsWindowQt(traits.get());
#else
    return nullptr;
#endif
}

void ViewOSG::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    updateCamera();
    _viewer->frame();
}

void ViewOSG::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    _widget->setGeometry(0, 0, event->size().width(), event->size().height());
}

ExtraViewDialog::ExtraViewDialog(QWidget * parent, osg::Camera * camera)
	: QDialog(parent)
	, _camera(camera)
    , _interface(new SettingsDialogImpl(this))
    , _timer(new QTimer(this))
{
	ui = new Ui_ExtraViewDialog;
	ui->setupUi( this );

	//connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
	connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    connect(_timer, SIGNAL(timeout()), this, SLOT(load()));

    osgViewer::View * view = nullptr;
    osgViewer::CompositeViewer * viewer = nullptr;
    osg::Camera * masterCamera = nullptr;
    view = dynamic_cast<osgViewer::View*>(camera->getView());
    if (view)
        viewer = dynamic_cast<osgViewer::CompositeViewer*>(view->getViewerBase());
    else
    {
        for (auto * parent : camera->getParents())
        {
            osg::Camera * nextCamera = osg_helpers::findFirstParentOfType<osg::Camera>(parent);
            if (nextCamera)
            {
                view = dynamic_cast<osgViewer::View*>(nextCamera->getView());
                if (view)
                {
                    masterCamera = nextCamera;
                    viewer = dynamic_cast<osgViewer::CompositeViewer*>(view->getViewerBase());
                    break;
                }
            }
        }
    }

    ui->widget->setCamera(viewer, camera);

	load();
}

ExtraViewDialog::~ExtraViewDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
}

void ExtraViewDialog::showEvent( QShowEvent * event )
{
    QDialog::showEvent(event);
    load();
    _timer->start(1000);
}

void ExtraViewDialog::hideEvent  ( QHideEvent * event )
{
    QDialog::hideEvent(event);
    _timer->stop();
}

void ExtraViewDialog::load()
{

}

void ExtraViewDialog::restoreDefaults()
{
	load();
}

void ExtraViewDialog::save()
{
	apply(true);
	accept();
}

void ExtraViewDialog::apply()
{
	apply(false);
}

void ExtraViewDialog::apply(bool save)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
    if(view)
        view->requestRedraw();
}

void ExtraViewDialog::reject()
{
	QDialog::reject();
}

} // namespace osg_plugin
} // namespace sgi
