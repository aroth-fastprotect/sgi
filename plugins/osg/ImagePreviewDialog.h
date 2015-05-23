#pragma once

#include <sgi/plugins/ImagePreviewDialogBase>

namespace osg {
    class Texture;
    class Image;
    class Camera;
    class View;
}

namespace sgi {
namespace osg_plugin {

class ImagePreviewDialog : public ImagePreviewDialogBase
{
    Q_OBJECT

public:
    ImagePreviewDialog(QWidget * parent, SGIItemBase * item);
    virtual ~ImagePreviewDialog();

private slots:
    void onTextureReady();

signals:
    void textureReady();

public:
    void emitTextureReady(osg::Image * image);

protected:
    virtual void refreshImpl();
    virtual void updateImageAndLabel();

private:
    void renderCameraToImage(osg::Camera * camera);
    void renderTextureToImage(osg::Texture * texture);

    void requestRedraw(osg::Camera * camera);

    osg::ref_ptr<osg::Image> _image;
    osg::ref_ptr<osg::Camera> _textureCamera;
    osg::ref_ptr<osg::View> _textureCameraView;
};

} // namespace osg_plugin
} // namespace sgi
