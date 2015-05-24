#include "stdafx.h"
#include "ImagePreviewDialog.h"

#include <QGLWidget>

#include <osg/Camera>
#include <osg/RenderInfo>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/View>
#include <osgDB/WriteFile>

#include <sgi/plugins/SGIItemOsg>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/helpers/osg>
#include <sgi/helpers/qt>

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

            osg::ref_ptr<osg::Image> image;
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
            osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
            if (gc->getTraits())
            {
                image = new osg::Image;
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
                int width = gc->getTraits()->width;
                int height = gc->getTraits()->height;
                osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();
                if(viewport)
                {
                    width = viewport->width();
                    height = viewport->height();
                }

                image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE);
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

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, SGIItemBase * item)
    : ImagePreviewDialogBase(parent, item)
{
    connect(this, &ImagePreviewDialog::textureReady, this, &ImagePreviewDialog::onTextureReady);
}

ImagePreviewDialog::~ImagePreviewDialog()
{
}

void ImagePreviewDialog::renderTextureToImage(osg::Texture * texture)
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
    osg::Camera* gcCamera = NULL;
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
        {
            gc = parentCamera->getGraphicsContext();
            gcCamera = parentCamera;
        }

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
    GLenum buffer = 0;
    if(_item->number() == ~0u)
        buffer = gc ? (gc->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT) : GL_BACK;
    else
        buffer = _item->number();
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
        requestRedraw(gcCamera);
    }
}

void ImagePreviewDialog::requestRedraw(osg::Camera * camera)
{
    osg::Camera* parentCamera = camera;
    osg::View * viewBasic = NULL;
    osgViewer::View * view = NULL;
    osg::GraphicsContext * gc = NULL;
    while(parentCamera && !view)
    {
        if(!view)
        {
            viewBasic = parentCamera?parentCamera->getView():NULL;
            if(viewBasic)
                view = dynamic_cast<osgViewer::View*>(viewBasic);
        }
        osg::Camera * nextParentCamera = findFirstParentOfType<osg::Camera>(parentCamera, ~0u, parentCamera);
        if(nextParentCamera == parentCamera)
            break;
        parentCamera = nextParentCamera;
    }
    if(view)
        view->requestRedraw();
}

void ImagePreviewDialog::renderCameraToImage(osg::Camera* camera)
{
    osg::GraphicsContext * gc = camera->getGraphicsContext();
    GLenum buffer = 0;
    if(_item->number() == ~0u)
        buffer = gc ? (gc->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT) : GL_BACK;
    else
        buffer = _item->number();
    camera->setFinalDrawCallback(new WindowCaptureCallback(buffer, this));

    requestRedraw(camera);
}

void ImagePreviewDialog::emitTextureReady(osg::Image * image)
{
    _image = image;
    emit textureReady();
}

void ImagePreviewDialog::onTextureReady()
{
    osg::Camera * camera = getObject<osg::Camera, SGIItemOsg, DynamicCaster>();
    if(_textureCamera.valid())
        _textureCamera->setFinalDrawCallback(NULL);
    else if(camera)
        camera->setFinalDrawCallback(NULL);

    if (_textureCameraView.valid())
    {
        unsigned index = _textureCameraView->findSlaveIndexForCamera(_textureCamera.get());
        if (index < _textureCameraView->getNumSlaves())
            _textureCameraView->removeSlave(index);
    }
    _textureCamera = NULL;
    _textureCameraView = NULL;

    updateImageAndLabel();
}

void ImagePreviewDialog::updateImageAndLabel()
{
    osg::Camera * camera = getObject<osg::Camera, SGIItemOsg, DynamicCaster>();
    osg::Texture * texture = getObject<osg::Texture, SGIItemOsg, DynamicCaster>();
    osg::Image * image = getObject<osg::Image, SGIItemOsg, DynamicCaster>();

    QImage qimg;
    if(image)
        osgImageToQImage(image, &qimg);
    else if(_image.valid())
        osgImageToQImage(_image, &qimg);

    std::string objectName;
    if(texture)
        objectName = getObjectNameAndType(texture, true);
    else if(camera)
        objectName = getObjectNameAndType(camera, true);
    else if(image)
        objectName = getObjectNameAndType(image, true);
    else
        objectName = "NULL";

    QString info;
    std::stringstream ss;
    bool emptyText = true;
    if (texture)
    {
        ss << "osg::Texture " << texture->getTextureWidth() << "x" << texture->getTextureHeight() << "x" << texture->getTextureDepth();
        ss << " [target=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(texture->getTextureTarget());
        ss << ";format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(texture->getInternalFormatMode());
        ss << ";srcFormat=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(texture->getSourceFormat());
        ss << "]";
        emptyText = false;
    }
    if(image)
    {
        if(!emptyText)
            ss << "\r\n";
        ss << "osg::Image " << image->s() << "x" << image->t() << "x" << image->r();
        ss << " [format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(image->getPixelFormat()) << ";mipmap=" << image->getNumMipmapLevels() << "]";
        emptyText = false;
    }
    else if(_image.valid())
    {
        if(!emptyText)
            ss << "\r\n";
        ss << "osg::Image " << _image->s() << "x" << _image->t() << "x" << _image->r();
        ss << " [format=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(_image->getPixelFormat()) << ";mipmap=" << _image->getNumMipmapLevels() << "]";
        emptyText = false;
    }

    ImagePreviewDialogBase::setImage(qimg, qt_helpers::fromLocal8Bit(objectName), qt_helpers::fromLocal8Bit(ss.str()));
}

void ImagePreviewDialog::refreshImpl()
{
    osg::Camera * camera = getObject<osg::Camera, SGIItemOsg, DynamicCaster>();
    osg::Texture * texture = getObject<osg::Texture, SGIItemOsg, DynamicCaster>();
    osg::Image * image = getObject<osg::Image, SGIItemOsg, DynamicCaster>();

    if(texture)
        renderTextureToImage(texture);
    else if(camera)
        renderCameraToImage(camera);
    else if(image)
        (void(0));
    updateImageAndLabel();
}

} // namespace osg_plugin
} // namespace sgi
