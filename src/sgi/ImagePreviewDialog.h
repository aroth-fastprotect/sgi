// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once

#include <QDialog>
#include <QLabel>
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
class IContextMenu;
typedef osg::ref_ptr<IContextMenu> IContextMenuPtr;

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT

public:
                            ImagePreviewDialog(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
                            ImagePreviewDialog(SGIItemBase * item, IHostCallback * callback=nullptr, QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    virtual                 ~ImagePreviewDialog();

public:
    IImagePreviewDialog *   dialogInterface() { return _interface; }
    void                    setObject(const SGIHostItemBase * hostitem, IHostCallback * callback);
    void                    setObject(SGIItemBase * item, IHostCallback * callback);
    void                    setObject(SGIItemBase * item, const sgi::Image * image, const std::string & description, IHostCallback * callback);
    void                    setImage(const sgi::Image * image);
    void                    setDescription(const std::string & description);

    SGIItemBase *           item() const;

public slots:
    void setLabel(const QString & label);
    void showBesideParent();
    void onObjectChanged();
    void colorFilterChanged();
    void tabWidgetCurrentChanged(int);
    void openItem();

protected slots:
    void onMouseMoved(float x, float y);

signals:
    void                    triggerOnObjectChanged();
    void                    triggerShow();
    void                    triggerHide();
    void                    triggerReloadStatistics();

protected:
    void refreshImpl();
    void refreshStatistics(const sgi::Image * image);
    void reloadStatistics();

    void                    triggerRepaint();
    SGIItemBase *           getView();

protected:
    virtual void showEvent(QShowEvent * event);
    //void setImage(const QImage & image, const QString & name=QString(), const QString & infoText=QString());
    //void setImage(const QPixmap & pixmap, const QString & name=QString(), const QString & infoText=QString());

private:
    void init();

protected:
    class HostCallback;
    class ImagePreviewDialogImpl;
	class Histogram;

protected:
    SGIItemBasePtr                  _item;
    ConstImagePtr                   _image;
    ImagePtr                        _itemImage;
    ImagePtr                        _workImage;
    SGIPluginHostInterface *        _hostInterface;
    ImagePreviewDialogImpl *        _priv;
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    IImagePreviewDialog *           _interface;
    IHostCallbackPtr                _hostCallback;
    bool                            _firstShow;
};

} // namespace sgi
