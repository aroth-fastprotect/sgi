#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QToolBar;
class QScrollBar;
class Ui_ImagePreviewDialog;
QT_END_NAMESPACE

namespace osg {
    class Texture;
    class Image;
    class Camera;
}

namespace sgi {
namespace osg_plugin {

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT

public:
    ImagePreviewDialog(QWidget * parent, osg::Camera * camera, unsigned buffer);
	ImagePreviewDialog(QWidget * parent, osg::Image * image);
    ImagePreviewDialog(QWidget * parent, osg::Texture * texture);
    virtual ~ImagePreviewDialog();

public:
    ISettingsDialog *   dialogInterface() { return _interface; }

public slots:
    void setLabel(const QString & label);

private slots:
    void open();
    void refresh();
    void save();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
	void load(const QString & filename);
	void load(const QImage * image);
    void load(osg::Image * image);
    void load(osg::Texture * texture);
    void load(osg::Camera * camera);

    void onTextureReady();

signals:
    void textureReady();

public:
    void emitTextureReady(osg::Image * image);

private:
    void init();
    void createToolbar();
    void updateToolbar();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    void updateImageAndLabel();

    void renderCameraToImage(osg::Camera * camera);
    void renderTextureToImage(osg::Texture * texture);

    void requestRedraw(osg::Camera * camera);

private:
    Ui_ImagePreviewDialog *         ui;
    osg::ref_ptr<osg::Image>        _image;
    osg::ref_ptr<osg::Camera>       _camera;
    unsigned                        _buffer;
    osg::ref_ptr<osg::Texture>      _texture;
    osg::ref_ptr<osg::Camera>       _textureCamera;
    osg::ref_ptr<osg::View>         _textureCameraView;
    ISettingsDialogPtr              _interface;
    QToolBar *                      _toolBar;
    double                          _scaleFactor;
    QAction *                       _refreshAction;
    QAction *                       _saveAction;
    QAction *                       _zoomInAction;
    QAction *                       _zoomOutAction;
    QAction *                       _normalSizeAction;
    QAction *                       _fitToWindowAction;
    QString                         _labelText;
};

} // namespace osg_plugin
} // namespace sgi
