#pragma once

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLShaderProgram;
class QOpenGLTexture;

namespace sgi {

class Image;
typedef osg::ref_ptr<Image> ImagePtr;
typedef osg::ref_ptr<const Image> ConstImagePtr;

class ImageGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ImageGLWidget(QWidget * parent);
    ~ImageGLWidget();
    // OpenGL Events
public:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void setImage(const sgi::Image * image);
    void clearImage();
    const sgi::Image * image() const;
    void reloadImage();

    QSize sizeHint() const override;

    bool hasError() const {
        return !_errorMessage.isEmpty();
    }
    bool setColorFilter(const QString & fragment, const QString & vertex=QString());
    const QString & colorFilterFragment() const;
    const QString & colorFilterVertex() const;

    void setBackgroundColor(const QColor & color);
    const QColor & backgroundColor() const;

    QOpenGLBuffer * vertexBuffer() const { return _vertex; }
    QOpenGLVertexArrayObject * vao() const { return _object; }
    QOpenGLShaderProgram * program() const { return _program; }
    QOpenGLTexture * texture() const { return _texture; }

    void setMirrored(bool horizontal, bool vertical);

protected:
    void teardownGL();
    void paintEvent(QPaintEvent *e) override;

protected:
    void setImageImpl(const sgi::Image * image, bool reset=false);
    bool reloadShaders();

private:
    // OpenGL State Information
    QOpenGLBuffer * _vertex;
    QOpenGLVertexArrayObject * _object;
    QOpenGLShaderProgram *_program;
    QOpenGLTexture * _texture;
    ConstImagePtr _image;
    int _locationIdPosition;
    int _locationIdTexCoord;
    bool _useOpenGl3;
    bool _useOpenGles;
    QString _errorMessage;
    QString _colorFilterVertex;
    QString _colorFilterFragment;
    QColor _backgroundColor;
};


} // namespace sgi
