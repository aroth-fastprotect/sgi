#include "stdafx.h"
#include "ImagePreviewDialog.h"

#include <ui_ImagePreviewDialog.h>

#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QToolBar>
#include <QScrollBar>
#include <QGLWidget>

#include <osg/Camera>
#include <osg/RenderInfo>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/View>
#include <osgDB/WriteFile>

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/helpers/osg>

#include "FindTopMostNodeOfType"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace osg_plugin {

using namespace sgi::osg_helpers;

namespace {

osg::Image* QImageToOsgImage(const QImage& qimage)
{
    QImage glImage = QGLWidget::convertToGLFormat(qimage);

    unsigned char* data = new unsigned char[glImage.byteCount()];
    for(int i=0; i<glImage.byteCount(); i++)
    {
        data[i] = glImage.bits()[i];
    }

    osg::Image* osgimage = new osg::Image();
    osgimage->setImage(glImage.width(),
                         glImage.height(),
                         1,
                         4,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         data,
                         osg::Image::USE_NEW_DELETE,
                         1);
    return osgimage;
}

/** Capture the frame buffer and write image to disk*/
class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:
    WindowCaptureCallback(GLenum readBuffer, ImagePreviewDialog * dialog):
        _readBuffer(readBuffer),
        _qimage(),
        _dialog(dialog)
        {
        }

    virtual void operator () (osg::RenderInfo& renderInfo) const
        {
            if(_readBuffer == 0)
                return;
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
            glReadBuffer(_readBuffer);
#else
            osg::notify(osg::NOTICE)<<"Error: GLES unable to do glReadBuffer"<<std::endl;
#endif

            osg::ref_ptr<osg::Image> image = new osg::Image;
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
            osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
            if (gc->getTraits())
            {
                GLenum pixelFormat;

                if (gc->getTraits()->alpha)
                    pixelFormat = GL_RGBA;
                else
                    pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
                 if (pixelFormat == GL_RGB)
                 {
                    GLint value;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
    #define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
                    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
                    if ( value != GL_RGB ||
                         value != GL_UNSIGNED_BYTE )
                    {
                        pixelFormat = GL_RGBA;//always supported
                    }
                 }
#endif
//                int width = gc->getTraits()->width;
//                int height = gc->getTraits()->height;
                osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();
                int width = viewport->width();
                int height = viewport->height();

                //std::cout<<"Capture: size="<<width<<"x"<<height<<", format="<<(pixelFormat == GL_RGBA ? "GL_RGBA":"GL_RGB")<<std::endl;


//                GLenum err1 = glGetError();
                image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE);
//                GLenum err = glGetError();
//                int i = err;
            }
/*
            if (image.valid())
            {
                osgImageToQImage(image, &const_cast<WindowCaptureCallback*>(this)->_qimage);
            }
*/
            // mark this as done
            const_cast<WindowCaptureCallback*>(this)->_readBuffer = 0;
            _dialog->emitTextureReady(image.get());
            //_dialog->emitTextureRendered(_qimage);
        }

protected:
    GLenum                      _readBuffer;
    QImage                      _qimage;
    mutable OpenThreads::Mutex  _mutex;
    ImagePreviewDialog *        _dialog;
};

osg::Node * findFirstNode(osg::StateSet * sa)
{
    osg::Node * ret = NULL;
    for(unsigned n = 0; !ret && n < sa->getNumParents(); n++)
    {
        osg::Object * p = sa->getParent(n);
        if(osg::Node * node = dynamic_cast<osg::Node*>(p))
            ret = node;
    }
    return ret;
}

osg::Node * findFirstNode(osg::StateAttribute * sa)
{
    osg::Node * ret = NULL;
    for(unsigned n = 0; !ret && n < sa->getNumParents(); n++)
    {
        osg::Object * p = sa->getParent(n);
        if(osg::Node * node = dynamic_cast<osg::Node*>(p))
            ret = node;
        else if(osg::StateSet * stateSet = dynamic_cast<osg::StateSet*>(p))
            ret = findFirstNode(stateSet);
    }
    return ret;
}

} // namespace

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, osg::Image * image)
    : QDialog(parent)
    , _image(image)
    , _texture()
    , _interface(new SettingsDialogImpl(this))
    , _scaleFactor(1.0)
{
    ui = new Ui_ImagePreviewDialog;
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
    connect(this, &ImagePreviewDialog::textureRendered, this, &ImagePreviewDialog::onTextureRendered);
    connect(this, &ImagePreviewDialog::textureReady, this, &ImagePreviewDialog::onTextureReady);

    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

	createToolbar();

    refresh();
}

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, osg::Texture * texture)
    : QDialog(parent)
    , _image()
    , _texture(texture)
    , _interface(new SettingsDialogImpl(this))
    , _scaleFactor(1.0)
{
    ui = new Ui_ImagePreviewDialog;
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
	connect(this, &ImagePreviewDialog::textureRendered, this, &ImagePreviewDialog::onTextureRendered);
    connect(this, &ImagePreviewDialog::textureReady, this, &ImagePreviewDialog::onTextureReady);

    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    createToolbar();

    refresh();
}

ImagePreviewDialog::~ImagePreviewDialog()
{
}

void ImagePreviewDialog::setLabel(const QString & label)
{
    _labelText = label;
}

void ImagePreviewDialog::load(const QString & filename)
{
	QImage * img = new QImage(filename);
	if (img->isNull()) 
	{
		QMessageBox::critical(this, windowTitle(), tr("Cannot load %1.").arg(filename));
		delete img;
		return;
	}
	load(img);
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, QImage::Format t)
{
    switch(t)
    {
    case QImage::Format_Invalid: os << "Invalid"; break;
    case QImage::Format_Mono: os << "Mono"; break;
    case QImage::Format_MonoLSB: os << "MonoLSB"; break;
    case QImage::Format_Indexed8: os << "Indexed8"; break;
    case QImage::Format_RGB32: os << "RGB32"; break;
    case QImage::Format_ARGB32: os << "ARGB32"; break;
    case QImage::Format_ARGB32_Premultiplied: os << "ARGB32_Premultiplied"; break;
    case QImage::Format_RGB16: os << "RGB16"; break;
    case QImage::Format_ARGB8565_Premultiplied: os << "ARGB8565_Premultiplied"; break;
    case QImage::Format_RGB666: os << "RGB666"; break;
    case QImage::Format_ARGB6666_Premultiplied: os << "ARGB6666_Premultiplied"; break;
    case QImage::Format_RGB555: os << "RGB555"; break;
    case QImage::Format_ARGB8555_Premultiplied: os << "ARGB8555_Premultiplied"; break;
    case QImage::Format_RGB888: os << "RGB888"; break;
    case QImage::Format_RGB444: os << "RGB444"; break;
    case QImage::Format_ARGB4444_Premultiplied: os << "ARGB4444_Premultiplied"; break;
    default: os << (int)t; break;
    }
    return os;
}

void ImagePreviewDialog::load(const QImage * img)
{
    ui->imageLabel->setText(QString());
	ui->imageLabel->setPixmap(QPixmap::fromImage(*img));

    std::stringstream ss;
    ss << img->width() << "x" << img->height() << "x" << img->depth();
    ss << " [format=" << img->format() << ";colors=" << img->colorCount() << "]";

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("\r\n") + imageInfo);
	_scaleFactor = 1.0;

	_fitToWindowAction->setEnabled(true);
    _saveAction->setEnabled(true);
    _refreshAction->setEnabled(true);
	updateToolbar();

    setWindowTitle(tr("Image Viewer - 0x%1").arg(QString::number((qulonglong)(void*)img, 16)));

	if (!_fitToWindowAction->isChecked())
		ui->imageLabel->adjustSize();
}

void ImagePreviewDialog::load(const osg::Image * img)
{
    QImage qimg;
    osgImageToQImage(img, &qimg);
    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(QPixmap::fromImage(qimg));

    std::stringstream ss;
    ss << "osg::Image " << img->s() << "x" << img->t() << "x" << img->r();
    ss << " [format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(img->getPixelFormat()) << ";mipmap=" << img->getNumMipmapLevels() << "]";
    ss << "\r\n";
    ss << "Qt " << qimg.width() << "x" << qimg.height() << "x" << qimg.depth();
    ss << " [format=" << qimg.format() << ";colors=" << qimg.colorCount() << "]";

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("\r\n") + imageInfo);
    _scaleFactor = 1.0;

    _fitToWindowAction->setEnabled(true);
    _saveAction->setEnabled(true);
    updateToolbar();

    setWindowTitle(tr("Image Viewer - %1").arg(QString::fromStdString(getObjectNameAndType(img))));

    if (!_fitToWindowAction->isChecked())
        ui->imageLabel->adjustSize();
}

void ImagePreviewDialog::load(const osg::Texture * texture)
{
    renderTextureToQImage(const_cast<osg::Texture*>(texture));
    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(QPixmap());

    std::stringstream ss;
	if(!texture)
		ss << "(null)";
	else
	{
		ss << "osg::Texture " << texture->getTextureWidth() << "x" << texture->getTextureHeight() << "x" << texture->getTextureDepth();
		ss << " [target=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(texture->getTextureTarget());
        ss << ";format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(texture->getInternalFormatMode());
        ss << ";srcFormat=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(texture->getSourceFormat());
        ss << "]";
		ss << "\r\n";
		ss << "Qt N/A";
	}

    QString textureInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(textureInfo);
    else
        ui->label->setText(_labelText + QString("\r\n") + textureInfo);
    _scaleFactor = 1.0;

    _fitToWindowAction->setEnabled(true);
    _saveAction->setEnabled(true);
    _refreshAction->setEnabled(true);
    updateToolbar();

    setWindowTitle(tr("Image Viewer - %1").arg(QString::fromStdString(getObjectNameAndType(texture))));

    if (!_fitToWindowAction->isChecked())
        ui->imageLabel->adjustSize();
}

void ImagePreviewDialog::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open image file"), QDir::currentPath());
    if (!fileName.isEmpty()) {
		load(fileName);
    }
}

void ImagePreviewDialog::save()
{
    if(!_image.valid())
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                    tr("Save image file"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        osgDB::writeImageFile(*_image.get(), fileName.toStdString());
    }
}

void ImagePreviewDialog::zoomIn()
{
    scaleImage(1.25);
}

void ImagePreviewDialog::zoomOut()
{
    scaleImage(0.8);
}

//! [10] //! [11]
void ImagePreviewDialog::normalSize()
//! [11] //! [12]
{
    ui->imageLabel->adjustSize();
    _scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImagePreviewDialog::fitToWindow()
//! [13] //! [14]
{
    bool fitToWindow = _fitToWindowAction->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateToolbar();
}
//! [14]

//! [19]
void ImagePreviewDialog::createToolbar()
//! [19] //! [20]
{
    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);


    _refreshAction = new QAction(tr("&Refresh"), this);
    _refreshAction->setIcon(QIcon::fromTheme("view-refresh"));
    _refreshAction->setShortcut(tr("F5"));
    _refreshAction->setEnabled(false);
    connect(_refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

    _saveAction = new QAction(tr("&Save..."), this);
    _saveAction->setIcon(QIcon::fromTheme("document-save"));
    _saveAction->setShortcut(tr("Ctrl+S"));
    _saveAction->setEnabled(false);
    connect(_saveAction, SIGNAL(triggered()), this, SLOT(save()));

    _zoomInAction = new QAction(tr("Zoom &In (25%)"), this);
    _zoomInAction->setIcon(QIcon::fromTheme("zoom-in"));
    _zoomInAction->setShortcut(tr("Ctrl++"));
    _zoomInAction->setEnabled(false);
    connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    _zoomOutAction = new QAction(tr("Zoom &Out (25%)"), this);
    _zoomOutAction->setIcon(QIcon::fromTheme("zoom-out"));
    _zoomOutAction->setShortcut(tr("Ctrl+-"));
    _zoomOutAction->setEnabled(false);
    connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    _normalSizeAction = new QAction(tr("&Normal Size"), this);
    _normalSizeAction->setIcon(QIcon::fromTheme("zoom-original"));
    _normalSizeAction->setShortcut(tr("Ctrl+O"));
    _normalSizeAction->setEnabled(false);
    connect(_normalSizeAction, SIGNAL(triggered()), this, SLOT(normalSize()));

    _fitToWindowAction = new QAction(tr("&Fit to Window"), this);
    _fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best"));
    _fitToWindowAction->setEnabled(false);
    _fitToWindowAction->setCheckable(true);
    _fitToWindowAction->setShortcut(tr("Ctrl+F"));
    connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    _toolBar->addAction(_refreshAction);
    _toolBar->addAction(_saveAction);
    _toolBar->addAction(_zoomInAction);
    _toolBar->addAction(_zoomOutAction);
    _toolBar->addAction(_normalSizeAction);
    _toolBar->addAction(_fitToWindowAction);
}
//! [20]

//! [21]
void ImagePreviewDialog::updateToolbar()
//! [21] //! [22]
{
    _zoomInAction->setEnabled(!_fitToWindowAction->isChecked());
    _zoomOutAction->setEnabled(!_fitToWindowAction->isChecked());
    _normalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
}
//! [22]

//! [23]
void ImagePreviewDialog::scaleImage(double factor)
//! [23] //! [24]
{
    Q_ASSERT(ui->imageLabel->pixmap());
    _scaleFactor *= factor;
    ui->imageLabel->resize(_scaleFactor * ui->imageLabel->pixmap()->size());

    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);

    _zoomInAction->setEnabled(_scaleFactor < 3.0);
    _zoomOutAction->setEnabled(_scaleFactor > 0.333);
}
//! [24]

//! [25]
void ImagePreviewDialog::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]
void ImagePreviewDialog::renderTextureToQImage(osg::Texture * texture)
{
    Q_ASSERT(texture != NULL);
    float width = texture->getTextureWidth();
    float height = texture->getTextureHeight();
    osg::ref_ptr<osg::Geometry> textureQuad = osg::createTexturedQuadGeometry(osg::Vec3(0.0f,0.0f,0.0f),
                                                                                osg::Vec3(width,0.0f,0.0f),
                                                                                osg::Vec3(0.0f,0.0f,height),
                                                                                0.0f,
                                                                                0.0f,
                                                                                1.0f,
                                                                                1.0f);
    textureQuad->getOrCreateStateSet()->setTextureAttribute(0, texture);

    osg::Node * parentNode = findFirstNode(const_cast<osg::Texture*>(texture));
    osg::Camera* parentCamera = findFirstParentOfType<osg::Camera>(parentNode);
	osg::View * parentViewBasic = NULL;
	osgViewer::View * parentView = NULL;
    osg::GraphicsContext * gc = NULL;
    while(parentCamera && (!parentViewBasic || !gc))
    {
        if(!parentViewBasic)
		{
			parentViewBasic = parentCamera?parentCamera->getView():NULL;
			if(parentViewBasic)
				parentView = dynamic_cast<osgViewer::View*>(parentViewBasic);
		}
        if(!gc)
            gc = parentCamera?parentCamera->getGraphicsContext():NULL;

        osg::Camera * nextParentCamera = findFirstParentOfType<osg::Camera>(parentCamera, ~0u, parentCamera);
        if(nextParentCamera == parentCamera)
            break;
        parentCamera = nextParentCamera;
    }

	if(!gc || !parentView)
		return;

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    camera->setGraphicsContext(gc);

    // set the projection matrix
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0,width,0,height));
    camera->setViewport(new osg::Viewport(0,0,width,height));

    // set the view matrix
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);

    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    GLenum buffer = gc ? (gc->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT) : GL_BACK;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);

    // we don't want the camera to grab event focus from the viewers main camera(s).
    camera->setAllowEventFocus(false);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(textureQuad);

    camera->addChild(geode);

    camera->setFinalDrawCallback(new WindowCaptureCallback(buffer, this));

    _textureCamera = camera;

    if(parentViewBasic)
    {
        _textureCameraView = parentViewBasic;
        parentViewBasic->addSlave(camera.get(), false);
        if(parentView)
            parentView->requestRedraw();
    }
}

void ImagePreviewDialog::emitTextureReady(osg::Image * image)
{
    _image = image;
    emit textureReady();
}

void ImagePreviewDialog::emitTextureRendered(QImage qimg)
{
	emit textureRendered(qimg);
}

void ImagePreviewDialog::onTextureReady()
{
    if(_textureCamera.valid())
        _textureCamera->setFinalDrawCallback(NULL);

    if (_textureCameraView.valid())
    {
        unsigned index = _textureCameraView->findSlaveIndexForCamera(_textureCamera.get());
        if (index < _textureCameraView->getNumSlaves())
            _textureCameraView->removeSlave(index);
    }
    _textureCamera = NULL;
    _textureCameraView = NULL;

    QImage qimg;
    osgImageToQImage(_image.get(), &qimg);
    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(QPixmap::fromImage(qimg));

    std::stringstream ss;
    if (!_texture.valid())
        ss << "(null)";
    else
    {
        ss << "osg::Texture " << _texture->getTextureWidth() << "x" << _texture->getTextureHeight() << "x" << _texture->getTextureDepth();
        ss << " [target=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getTextureTarget());
        ss << ";format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getInternalFormatMode());
        ss << ";srcFormat=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getSourceFormat());
        ss << "]";
        ss << "\r\n";
        ss << "Qt " << qimg.width() << "x" << qimg.height() << "x" << qimg.depth();
        ss << " [format=" << qimg.format() << ";colors=" << qimg.colorCount() << "]";
    }

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("\r\n") + imageInfo);
    _scaleFactor = 1.0;

    _fitToWindowAction->setEnabled(true);
    _saveAction->setEnabled(true);
    _refreshAction->setEnabled(true);
    updateToolbar();

    setWindowTitle(tr("Image Viewer - %1").arg(QString::fromStdString(getObjectNameAndType(_texture.get()))));

    if (!_fitToWindowAction->isChecked())
        ui->imageLabel->adjustSize();

}

void ImagePreviewDialog::onTextureRendered(QImage qimg)
{
	if(_textureCamera.valid())
		_textureCamera->setFinalDrawCallback(NULL);

	if (_textureCameraView.valid())
	{
		unsigned index = _textureCameraView->findSlaveIndexForCamera(_textureCamera.get());
		if (index < _textureCameraView->getNumSlaves())
			_textureCameraView->removeSlave(index);
	}
	_textureCamera = NULL;
	_textureCameraView = NULL;

    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(QPixmap::fromImage(qimg));

	std::stringstream ss;
	if (!_texture.valid())
		ss << "(null)";
	else
	{
		ss << "osg::Texture " << _texture->getTextureWidth() << "x" << _texture->getTextureHeight() << "x" << _texture->getTextureDepth();
		ss << " [target=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getTextureTarget());
		ss << ";format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getInternalFormatMode());
		ss << ";srcFormat=" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(_texture->getSourceFormat());
		ss << "]";
		ss << "\r\n";
		ss << "Qt " << qimg.width() << "x" << qimg.height() << "x" << qimg.depth();
		ss << " [format=" << qimg.format() << ";colors=" << qimg.colorCount() << "]";
	}

    QString imageInfo = QString::fromStdString(ss.str());
    if(_labelText.isEmpty())
        ui->label->setText(imageInfo);
    else
        ui->label->setText(_labelText + QString("\r\n") + imageInfo);
    _scaleFactor = 1.0;

    _fitToWindowAction->setEnabled(true);
    _saveAction->setEnabled(true);
    updateToolbar();

    setWindowTitle(tr("Image Viewer - %1").arg(QString::fromStdString(getObjectNameAndType(_texture.get()))));

    if (!_fitToWindowAction->isChecked())
        ui->imageLabel->adjustSize();
}

void ImagePreviewDialog::refresh()
{
    if(_texture.valid())
        load(_texture.get());
    else if(_image.valid())
        load(_image.get());
    else
        setWindowTitle(tr("Image Viewer"));
}

} // namespace osg_plugin
} // namespace sgi
