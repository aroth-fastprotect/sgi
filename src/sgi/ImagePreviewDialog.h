// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include <QDialog>
#include "sgi/plugins/ImagePreviewDialog"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QToolBar;
class QScrollBar;
class QIcon;
class QShowEvent;
class Ui_ImagePreviewDialog;
QT_END_NAMESPACE

namespace sgi {

class Image;
typedef osg::ref_ptr<Image> ImagePtr;
class SGIPluginHostInterface;

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT

public:
                            ImagePreviewDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
                            ImagePreviewDialog(SGIItemBase * item, IImagePreviewDialogInfo * info=NULL, QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual                 ~ImagePreviewDialog();

public:
    IImagePreviewDialog *   dialogInterface() { return _interface; }
    void                    setObject(const SGIHostItemBase * hostitem, IImagePreviewDialogInfo * info);
    void                    setObject(SGIItemBase * item, IImagePreviewDialogInfo * info);
    void                    setObject(SGIItemBase * item, const sgi::Image * image, const std::string & description, IImagePreviewDialogInfo * info);
    void                    setImage(const sgi::Image * image);
    void                    setDescription(const std::string & description);

    SGIItemBase *           item() const;

    bool                    showSceneGraphDialog(SGIItemBase * item);
    bool                    showSceneGraphDialog(const SGIHostItemBase * hostitem);

    bool                    showObjectLoggerDialog(SGIItemBase * item);
    bool                    showObjectLoggerDialog(const SGIHostItemBase * hostitem);

public slots:
    void setLabel(const QString & label);

signals:
    void                    triggerOnObjectChanged();
    void                    triggerShow();
    void                    triggerHide();

protected:
    void updateImageAndLabel();
    void refreshImpl();
    bool openImpl(const QString & filename);
    bool saveImpl(const QString & filename);

    void                    setNodeInfo(const SGIItemBase * item);

    void                    triggerRepaint();
    SGIItemBase *           getView();

protected:
    virtual void showEvent(QShowEvent * event);
    void setImage(const QImage & image, const QString & name=QString(), const QString & infoText=QString());
    void setImage(const QPixmap & pixmap, const QString & name=QString(), const QString & infoText=QString());

private:
    void init();

protected:
    class ContextMenuCallback;
    class ImagePreviewDialogImpl;

protected:
    SGIItemBasePtr                  _item;
    ConstImagePtr                   _image;
    SGIPluginHostInterface *        _hostInterface;
    ImagePreviewDialogImpl *        _priv;
    IImagePreviewDialogPtr          _interface;
    osg::ref_ptr<IImagePreviewDialogInfo> _info;
    osg::ref_ptr<IContextMenu>          _contextMenu;
    osg::ref_ptr<ContextMenuCallback>   _contextMenuCallback;

};

} // namespace sgi
