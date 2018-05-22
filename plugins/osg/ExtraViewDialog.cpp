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
#include "SGIItemOsg"

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace osg_plugin {

ViewOSG::ViewOSG(QWidget * parent)
    : QWidget(parent)
    , _widget(nullptr)
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

    _widget = osg_helpers::getWidgetForGraphicsWindow(_gfx.get());
    if (_widget)
    {

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
}

namespace {

    // Configures a window that lets you see what the RTT camera sees.
    void setupRTTView(osgViewer::View* view, osg::Texture* rttTex)
    {
        view->setCameraManipulator(0L);
        view->getCamera()->setName(rttTex->getName());
        view->getCamera()->setViewport(0, 0, rttTex->getTextureWidth(), rttTex->getTextureHeight());
        view->getCamera()->setClearColor(osg::Vec4(1, 1, 1, 1));
        view->getCamera()->setProjectionMatrixAsOrtho2D(-.5, .5, -.5, .5);
        view->getCamera()->setViewMatrixAsLookAt(osg::Vec3d(0, -1, 0), osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
        view->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);

        osg::Vec3Array* v = new osg::Vec3Array(6);
        (*v)[0].set(-.5, 0, -.5); (*v)[1].set(.5, 0, -.5); (*v)[2].set(.5, 0, .5); (*v)[3].set((*v)[2]); (*v)[4].set(-.5, 0, .5); (*v)[5].set((*v)[0]);

        osg::Vec2Array* t = new osg::Vec2Array(6);
        (*t)[0].set(0, 0); (*t)[1].set(1, 0); (*t)[2].set(1, 1); (*t)[3].set((*t)[2]); (*t)[4].set(0, 1); (*t)[5].set((*t)[0]);

        osg::Geometry* g = new osg::Geometry();
        g->setUseVertexBufferObjects(true);
        g->setUseDisplayList(false);
        g->setVertexArray(v);
        g->setTexCoordArray(0, t);
        g->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6));

        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(g);

        osg::StateSet* stateSet = geode->getOrCreateStateSet();
        stateSet->setDataVariance(osg::Object::DYNAMIC);

        stateSet->setTextureAttributeAndModes(0, rttTex, 1);
        rttTex->setUnRefImageDataAfterApply(false);
        rttTex->setResizeNonPowerOfTwoHint(false);

        stateSet->setMode(GL_LIGHTING, 0);
        stateSet->setMode(GL_CULL_FACE, 0);
        //stateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ZERO), 1);

#if 0
        const char* fs =
            "#version " GLSL_VERSION_STR "\n"
            "void swap(inout vec4 c) { c.rgba = c==vec4(0)? vec4(1) : vec4(vec3((c.r+c.g+c.b+c.a)/4.0),1); }\n";
        osgEarth::Registry::shaderGenerator().run(geode);
        osgEarth::VirtualProgram::getOrCreate(geode->getOrCreateStateSet())->setFunction("swap", fs, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING);
#endif

        view->setSceneData(geode);
    }

}
void ViewOSG::setRTTCamera(osgViewer::CompositeViewer * viewer, osg::Texture * texture)
{
    _viewer = viewer;
    osgViewer::View * firstView = viewer->getNumViews() ? viewer->getView(0) : nullptr;
    osg::Camera * camera = firstView ? firstView->getCamera() : nullptr;
    osg::GraphicsContext * existingContext = camera ? camera->getGraphicsContext() : nullptr;
    _gfx = createGraphicsWindow(0, 0, QWidget::width(), QWidget::height(), existingContext);

    _widget = osg_helpers::getWidgetForGraphicsWindow(_gfx.get());

    if (_widget)
    {
        QHBoxLayout * l = new QHBoxLayout;
        l->addWidget(_widget);
        setLayout(l);

        _viewCamera = new osg::Camera;
        _viewCamera->setGraphicsContext(_gfx.get());
        _camera = _viewCamera;

        _view = new osgViewer::View;
        _view->setCamera(_viewCamera.get());
        setupRTTView(_view.get(), texture);
        _viewer->addView(_view);

        updateCamera();
    }
}

void ViewOSG::updateCamera()
{
    osg::ref_ptr<osg::Camera> camera;
    if (_camera.lock(camera) && camera.get() != _viewCamera.get())
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
    if(_viewer.valid())
        _viewer->frame();
}

void ViewOSG::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(_widget)
        _widget->setGeometry(0, 0, event->size().width(), event->size().height());
}



ExtraViewDialog::ExtraViewDialog(QWidget * parent, SGIItemBase * item, SGIPluginHostInterface * hostInterface)
	: QDialog(parent)
    , _hostInterface(hostInterface)
    , _item(item)
	, _camera(nullptr)
    , _interface(new SettingsDialogImpl(this))
    , _timer(new QTimer(this))
{
	ui = new Ui_ExtraViewDialog;
	ui->setupUi( this );

	//connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &ExtraViewDialog::reject);
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &ExtraViewDialog::restoreDefaults);

    connect(_timer, &QTimer::timeout, this, &ExtraViewDialog::load);


    osgViewer::CompositeViewer * viewer = nullptr;
    osgViewer::View * view = nullptr;
    osg::Camera * masterCamera = nullptr;
    _camera = _item->asCamera();

    if (_camera.valid())
    {
        view = dynamic_cast<osgViewer::View*>(_camera->getView());
        if (view)
            viewer = dynamic_cast<osgViewer::CompositeViewer*>(view->getViewerBase());
        else
        {
            for (auto * parent : _camera->getParents())
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

        ui->widget->setCamera(viewer, _camera);
    }
    else
    {
        osg::Texture * txt = dynamic_cast<osg::Texture*>(static_cast<const SGIItemOsg*>(_item.get())->object());
        if (txt)
        {
            for (osg::StateSet * ss : txt->getParents())
            {
                for (auto * parent : ss->getParents())
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
                if (viewer)
                    break;
            }

            if (viewer)
                ui->widget->setRTTCamera(viewer, txt);
        }
    }

    std::string name;
    _hostInterface->getObjectDisplayName(name, _item.get());
    setWindowTitle(QString::fromStdString(name));

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
