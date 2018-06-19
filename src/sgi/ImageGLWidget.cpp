#include "stdafx.h"
#include "ImageGLWidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLPixelTransferOptions>
#include <QVector3D>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QLayout>
#include <QDir>

#include <sgi/plugins/SGIImage.h>

#include <QDebug>

#include "Shaders"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ALIGN_SIZE(x,n)       ((size_t)((~(n-1))&((x)+(n-1))))
#define ALIGN_BY_16(x) ALIGN_SIZE(x, 16)
#define ALIGN_BY_8(x) ALIGN_SIZE(x, 8)
#define ALIGN_BY_4(x) ALIGN_SIZE(x, 4)

namespace sgi {

namespace {
    static const GLfloat window_rect_vertexes[] = {
        -1, -1, 0, 0.0, 1.0,
        -1, +1, 0, 0.0, 0.0,
        +1, +1, 0, 1.0, 0.0,
        -1, -1, 0, 0.0, 1.0,
        +1, +1, 0, 1.0, 0.0,
        +1, -1, 0, 1.0, 1.0,
    };
}

ImageGLWidget::ImageGLWidget(QWidget * parent)
    : QOpenGLWidget(parent)
    , _vertex(nullptr)
    , _object(nullptr)
    , _program(nullptr)
    , _texture(nullptr)
    , _image(nullptr)
    , _locationIdPosition(-1)
    , _locationIdTexCoord(-1)
    , _mirrorHorizontal(false)
    , _mirrorVertical(false)
    , _backgroundColor(Qt::black)
{
    // Set OpenGL Version information
    // Note: This format must be set before show() is called.
    QSurfaceFormat format;
#if 0
    if(_useOpenGles)
        format.setRenderableType(QSurfaceFormat::OpenGLES);
    else
        format.setRenderableType(QSurfaceFormat::OpenGL);
    if (_useOpenGl3)
    {
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(3, 0);
    }
    else
    {
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(2, 1);
    }
#else
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
#endif
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    format.setDepthBufferSize(0);
    setFormat(format);

    setUpdateBehavior(NoPartialUpdate);

    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setSizePolicy(sizePolicy);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

ImageGLWidget::~ImageGLWidget()
{
    teardownGL();
}

void ImageGLWidget::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    QOpenGLContext *ctx = context();
    if (!ctx->isValid())
        return;

    connect(ctx, &QOpenGLContext::aboutToBeDestroyed, this, &ImageGLWidget::teardownGL, Qt::DirectConnection);
    //_useOpenGles = (ctx->isOpenGLES() && ctx->format().majorVersion() < 3);

    // Set global information
    glClearColor(_backgroundColor.redF(), _backgroundColor.greenF(), _backgroundColor.blueF(), _backgroundColor.alphaF());

    _errorMessage.clear();

    if(!reloadShaders())
    {
        return;
    }

    _texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    _texture->create();
    _texture->setMinificationFilter(QOpenGLTexture::Linear);
    _texture->setMagnificationFilter(QOpenGLTexture::Linear);
    _texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToBorder);
    _texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToBorder);
    // texture colour sets cubicle colour
    _texture->setBorderColor(0, 0, 0, 255);
    //_texture->setBorderWidth(0);

    bool actualMirrorVertical = _image.valid() && _image->origin() != sgi::Image::OriginBottomLeft ? _mirrorVertical : !_mirrorVertical;

    // Create Buffer (Do not release until VAO is created)
    _vertex = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _vertex->create();
    _vertex->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    _vertex->bind();
    {
        GLfloat v[30];
        Q_ASSERT(sizeof(v) == sizeof(window_rect_vertexes));
        memcpy(v, window_rect_vertexes, sizeof(v));
        for (int i = 0; i < 6; ++i)
        {
            int j = i * 5;
            GLfloat & t_x = v[j + 3];
            GLfloat & t_y = v[j + 4];
            //t_x = window_rect_vertexes[j + 3];
            //t_y = window_rect_vertexes[j + 4];
            if (_mirrorHorizontal)
                t_x = 1.0f - t_x;
            if (actualMirrorVertical)
                t_y = 1.0f - t_y;
        }
        _vertex->allocate(v, sizeof(v));
    }
    

    // Create Vertex Array Object
    _object = new QOpenGLVertexArrayObject;
    _object->create();
    _object->bind();

    _program->enableAttributeArray(_locationIdPosition);
    _program->enableAttributeArray(_locationIdTexCoord);
    _program->setAttributeBuffer(_locationIdPosition, GL_FLOAT /*type*/, 0                   /*offset*/, 3 /*tupleSize*/, 5 * sizeof(GLfloat) /*stride*/);
    _program->setAttributeBuffer(_locationIdTexCoord, GL_FLOAT /*type*/, 3 * sizeof(GLfloat) /*offset*/, 2 /*tupleSize*/, 5 * sizeof(GLfloat) /*stride*/);
    _program->setUniformValue("frame", 0);
    _program->setUniformValue("format", _image.valid() ? (int)_image->format() : -1);
    _program->setUniformValue("texture_scale", 1.0f, 1.0f);
    _program->setUniformValue("pixel_size", 1.0f, 1.0f);

    // Release (unbind) all
    _program->release();
    if(_object)
        _object->release();
    _vertex->release();
    _texture->release();

    setImageImpl(_image.get());
}

bool ImageGLWidget::reloadShaders()
{
    if(!_program)
    {
        // Create Shader (Do not release until VAO is created)
        _program = new QOpenGLShaderProgram();
    }
    else
        _program->removeAllShaders();

    bool shaderOk = true;
    details::Shaders pkg;
    pkg.replace("$GLSL_COLOR_FILTER_FRAGMENT", _colorFilterFragment.toStdString());
    pkg.replace("$GLSL_COLOR_FILTER_VERTEX", _colorFilterVertex.toStdString());
    if(!pkg.load( _program, ShaderPackage::ShaderTypeVertex, pkg.ImageGLWidgetVertex ))
        shaderOk = false;
    if(!pkg.load( _program, ShaderPackage::ShaderTypeFragment, pkg.ImageGLWidgetFragment ))
        shaderOk = false;

    if (shaderOk)
    {
        if (!_program->link())
        {
            shaderOk = false;
            qCritical() << "failed to link program";
        }
        else
        {
            _locationIdPosition = _program->attributeLocation("position");
            _locationIdTexCoord = _program->attributeLocation("texCoord");
        }
    }
    if (shaderOk)
    {
        if (!_program->bind())
        {
            shaderOk = false;
            qCritical() << "failed to bind program";
        }
    }

    if (!shaderOk)
    {
        delete _program;
        _program = nullptr;

        // shaders are bad and we cannot get them to work, so we only have the
        // background color to inform the user about the error.
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

        qCritical() << "shader not ok";
    }
    return shaderOk;
}

void ImageGLWidget::resizeGL(int width, int height)
{
    // Currently we are not handling width/height changes.
    if (!_errorMessage.isEmpty())
    {
        QRect r(0,0, ALIGN_BY_8(width), ALIGN_BY_8(height));
        QImage image(r.size(), QImage::Format_RGB888);
        QPainter painter(&image);
        painter.fillRect(r, Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(r, _errorMessage, QTextOption(Qt::AlignCenter));
        painter.end();
    }
}

void ImageGLWidget::paintGL()
{
    if (!_texture)
        return;

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    _vertex->bind();
    if(_object)
        _object->bind();
    _texture->bind();
    _program->bind();
    _program->enableAttributeArray(_locationIdPosition);
    _program->enableAttributeArray(_locationIdTexCoord);
    _program->setAttributeBuffer(_locationIdPosition, GL_FLOAT /*type*/, 0                   /*offset*/, 3 /*tupleSize*/, 5 * sizeof(GLfloat) /*stride*/);
    _program->setAttributeBuffer(_locationIdTexCoord, GL_FLOAT /*type*/, 3 * sizeof(GLfloat) /*offset*/, 2 /*tupleSize*/, 5 * sizeof(GLfloat) /*stride*/);

    glEnable(GL_BLEND);

    const unsigned numTrianglePoints = 6;
    glDrawArrays(GL_TRIANGLES, 0, numTrianglePoints);
    _program->disableAttributeArray(_locationIdPosition);
    _program->disableAttributeArray(_locationIdTexCoord);

    _program->release();
    _texture->release();
    if(_object)
        _object->release();
    _vertex->release();
}

void ImageGLWidget::teardownGL()
{
    // need to activate the context so we can destroy the textures and resources gracefully.
    makeCurrent();

    // Actually destroy our OpenGL information
    if (_texture)
        _texture->destroy();
    if (_object)
        _object->destroy();
    if (_vertex)
        _vertex->destroy();
    delete _texture;
    delete _object;
    delete _vertex;
    delete _program;
    _texture = nullptr;
    _object = nullptr;
    _vertex = nullptr;
    _program = nullptr;
}

void ImageGLWidget::paintEvent(QPaintEvent *event)
{
    QOpenGLContext * ctx = context();
    if (ctx && ctx->isValid())
    {
        if (_program)
            QOpenGLWidget::paintEvent(event);
        else
        {
            /*
            QPainter painter(this);
            painter.fillRect(event->rect(), Qt::black);
            painter.setPen(Qt::white);
            if(_errorMessage.isEmpty())
                painter.drawText(event->rect(), tr("Unable to create OpenGL shaders."), QTextOption(Qt::AlignCenter));
            else
                painter.drawText(event->rect(), _errorMessage, QTextOption(Qt::AlignCenter));
                */
        }
    }
    else
    {
        QPainter painter(this);
        painter.fillRect(event->rect(), Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(event->rect(), tr("Unable to create OpenGL context."), QTextOption(Qt::AlignCenter));
    }
}

QSize ImageGLWidget::sizeHint() const
{
    if (!_image.valid() || _image->empty())
        return QOpenGLWidget::sizeHint();

    return QSize((int)_image->width(), (int)_image->height());
}

const sgi::Image * ImageGLWidget::image() const
{
    return _image.get();
}

void ImageGLWidget::clearImage()
{
    _image = nullptr;
}

void ImageGLWidget::reloadImage()
{
    if (!_texture)
        return;

    makeCurrent();
    setImageImpl(_image.get());

    doneCurrent();
    // trigger a repaint because we changed the widgets content outside repaintGL
    update();
}

void ImageGLWidget::setImage(const sgi::Image * image)
{
    if (!_texture || !_errorMessage.isEmpty())
    {
        _image = image;
        return;
    }

    makeCurrent();
    setImageImpl(image);

    // now release the old frame data pointer and hold the new one
    _image = image;
    reloadVertexData();

    doneCurrent();
    // trigger a repaint because we changed the widgets content outside repaintGL
    update();
}

void ImageGLWidget::setImageImpl(const sgi::Image * image, bool reset)
{
    Q_ASSERT(_texture != nullptr);
    if(image)
        qWarning() << "setImageImpl" << image->format() << image->dataType() << image->width() << image->height() << image->data() << image->length();
    else
        qWarning() << "setImageImpl" << 0;

    QOpenGLTexture::TextureFormat textureFormat = QOpenGLTexture::NoFormat;
    QOpenGLTexture::PixelFormat pixelFormat = QOpenGLTexture::NoSourceFormat;
    int textureWidth = 0;
    int textureHeight = 0;
    if(image)
    {
        switch(image->format())
        {
        case Image::ImageFormatBGR24:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGB8_UNorm;
            pixelFormat = QOpenGLTexture::BGR;
            break;
        case Image::ImageFormatRGB24:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGB8_UNorm;
            pixelFormat = QOpenGLTexture::RGB;
            break;
        case Image::ImageFormatBGRA32:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA8_UNorm;
            pixelFormat = QOpenGLTexture::BGRA;
            break;
        case Image::ImageFormatRGBA32:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA8_UNorm;
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        case Image::ImageFormatABGR32:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA8_UNorm;
            pixelFormat = QOpenGLTexture::BGRA;
            break;
        case Image::ImageFormatARGB32:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA8_UNorm;
            pixelFormat = QOpenGLTexture::BGRA;
            break;
        case Image::ImageFormatYUV444:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        case Image::ImageFormatYUV420:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight() + (image->allocatedHeight() / 2);
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        case Image::ImageFormatYUV422:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight() + image->allocatedHeight();
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        case Image::ImageFormatYUYV:
        case Image::ImageFormatUYVY:
            textureWidth = image->allocatedWidth() / 2;
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA8_UNorm;
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        case Image::ImageFormatIndexed8:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        case Image::ImageFormatDXT1:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGB_DXT1;
            pixelFormat = QOpenGLTexture::RGB;
            break;
        case Image::ImageFormatDXT1Alpha:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA_DXT1;
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        case Image::ImageFormatDXT3:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA_DXT3;
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        case Image::ImageFormatDXT5:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::RGBA_DXT5;
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        case Image::ImageFormatGray:
        case Image::ImageFormatRed:
        case Image::ImageFormatGreen:
        case Image::ImageFormatBlue:
        case Image::ImageFormatAlpha:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        default:
            textureWidth = image->allocatedWidth();
            textureHeight = image->allocatedHeight();
            textureFormat = QOpenGLTexture::R8_UNorm;
            pixelFormat = QOpenGLTexture::Red;
            break;
        }
        if (!_image.valid() || reset || image->format() != _image->format() || image->hscale() != _image->hscale() || image->vscale() != _image->vscale())
        {
            Q_ASSERT(_program != nullptr);
            _program->bind();
            float vscale = image->vscale();
            float hscale = image->hscale();
            float vpixel_size = image->verticalPixelSize();
            float hpixel_size = image->horizontalPixelSize();
            qWarning() << "frame format changed" << (_image.valid()?_image->format():-1) << image->format();
            qWarning() << "frame hscale changed" << (_image.valid()?_image->hscale():0.0f) << hscale;
            qWarning() << "frame vscale changed" << (_image.valid()?_image->vscale():0.0f) << vscale;

            _program->setUniformValue("format", (int)image->format());
            _program->setUniformValue("texture_scale", hscale, vscale);
            _program->setUniformValue("pixel_size", hpixel_size, vpixel_size);
            _program->release();
        }

    }

    if (textureFormat != 0)
    {
        if (_texture->width() != textureWidth || _texture->height() != textureHeight || _texture->format() != textureFormat)
        {
            qWarning() << "texture size/format changed" << textureWidth << textureHeight << textureFormat;
            //        _texture->setData(0, pixelFormat, QOpenGLTexture::UInt8, (const void*)nullptr, &uploadOptions);
            _texture->destroy();
            _texture->create();
            _texture->bind();
            _texture->setFormat(textureFormat);
            _texture->setSize(textureWidth, textureHeight);
            _texture->allocateStorage(pixelFormat, QOpenGLTexture::UInt8);
        }
        else
            _texture->bind();

        // Upload pixel data with byte alignment
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        _texture->setData(0, pixelFormat, QOpenGLTexture::UInt8, image->data(), &uploadOptions);
        _texture->release();
    }
}

bool ImageGLWidget::setColorFilter(const QString & fragment, const QString & vertex)
{
    _colorFilterFragment = fragment;
    _colorFilterVertex = vertex;

    if (!_texture || !_errorMessage.isEmpty())
        return true;

    makeCurrent();
    bool ret = reloadShaders();
    setImageImpl(_image.get(), true);

    doneCurrent();
    // trigger a repaint because we changed the widgets content outside repaintGL
    update();

    return ret;
}

const QString & ImageGLWidget::colorFilterFragment() const
{
    return _colorFilterFragment;
}

const QString & ImageGLWidget::colorFilterVertex() const
{
    return _colorFilterVertex;
}

void ImageGLWidget::setBackgroundColor(const QColor & color)
{
    _backgroundColor = color;

    if (!_texture || !_errorMessage.isEmpty())
        return;

    makeCurrent();
    glClearColor(_backgroundColor.redF(), _backgroundColor.greenF(), _backgroundColor.blueF(), _backgroundColor.alphaF());
    _texture->setBorderColor(_backgroundColor.red(), _backgroundColor.green(), _backgroundColor.blue(), _backgroundColor.alpha());

    doneCurrent();
    // trigger a repaint because we changed the widgets content outside repaintGL
    update();
}

const QColor & ImageGLWidget::backgroundColor() const
{
    return _backgroundColor;
}

void ImageGLWidget::reloadVertexData()
{
    bool actualMirrorVertical = _image.valid() && _image->origin() != sgi::Image::OriginBottomLeft ? _mirrorVertical : !_mirrorVertical;

    _object->bind();
    _vertex->bind();
    GLfloat v[30];
    memcpy(v, window_rect_vertexes, sizeof(v));
    for (int i = 0; i < 6; ++i)
    {
        int j = i * 5;
        GLfloat & t_x = v[j + 3];
        GLfloat & t_y = v[j + 4];
        //t_x = window_rect_vertexes[j + 3];
        //t_y = window_rect_vertexes[j + 4];
        if (_mirrorHorizontal)
            t_x = 1.0f - t_x;
        if (actualMirrorVertical)
            t_y = 1.0f - t_y;
    }
    _vertex->write(0, v, sizeof(v));

    _vertex->release();
    _object->release();
}

void ImageGLWidget::setMirrored(bool horizontal, bool vertical)
{
    if (_mirrorHorizontal == horizontal && _mirrorVertical == vertical)
        return;

    _mirrorHorizontal = horizontal;
    _mirrorVertical = vertical;

    if (!_object)
        return;

    makeCurrent();
    reloadVertexData();

    doneCurrent();
    // trigger a repaint because we changed the widgets content outside repaintGL
    update();
}

void ImageGLWidget::mouseMoveEvent(QMouseEvent *ev)
{
    QOpenGLWidget::mouseMoveEvent(ev);

    float x = float(ev->x()) / float(width());
    float y = float(ev->y()) / float(height());

    emit mouseMoved(x, y);
}

void ImageGLWidget::resizeEvent(QResizeEvent *event)
{
    int pheight = parentWidget()->height();
    QSize s(_image.valid() ? static_cast<int>(_image->width()) : 0, _image.valid() ? static_cast<int>(_image->height()) : 0);
    s.scale(event->size(), Qt::KeepAspectRatio);
    s = QLayout::closestAcceptableSize(this, s);
    QRect imageRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, s, rect());
    int top = (pheight / 2) - (imageRect.height() / 2);
    imageRect.moveTop(top);
    setGeometry(imageRect);

    QOpenGLWidget::resizeEvent(event);
}

} // namespace sgi
