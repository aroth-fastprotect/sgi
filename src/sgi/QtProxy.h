#pragma once

#include <QObject>

namespace sgi {

class SGIItemBase;

class ISceneGraphDialog;
class IObjectLogger;
class IObjectLoggerDialog;
class IContextMenu;
class IContextMenuQt;
class IImagePreviewDialog;
class IHostCallback;

class QtProxy : public QObject
{
    Q_OBJECT
protected:
    QtProxy();

public:
    static QtProxy *        instance(bool erase=false);

public:
    ISceneGraphDialog *     showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback);
    IObjectLoggerDialog *   showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback);
    IObjectLoggerDialog *   showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback);
    IContextMenu *          createContextMenu(QWidget *parent, SGIItemBase * item, bool onlyRootItem, IHostCallback * callback);
    IContextMenuQt *        createContextMenu(QWidget *parent, QObject * item, bool onlyRootItem, IHostCallback * callback);
    IImagePreviewDialog *   showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback);

private:
    struct JobShowSceneGraphDialog
    {
        JobShowSceneGraphDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
        ISceneGraphDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        IHostCallback * callback;
    };
    struct JobShowObjectLoggerDialog
    {
        JobShowObjectLoggerDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
        IObjectLoggerDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        IHostCallback * callback;
    };
    struct JobShowObjectLoggerDialogForLogger
    {
        JobShowObjectLoggerDialogForLogger(QWidget *parent_, IObjectLogger * logger_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), logger(logger_), callback(callback_) {}
        IObjectLoggerDialog * retval;
        QWidget *parent;
        IObjectLogger * logger;
        IHostCallback * callback;
    };
    struct JobCreateContextMenu
    {
        JobCreateContextMenu(QWidget *parent_, SGIItemBase * item_, bool onlyRootItem_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), item(item_), onlyRootItem(onlyRootItem_), callback(callback_) {}
        IContextMenu * retval;
        QWidget *parent;
        SGIItemBase * item;
        bool onlyRootItem;
        IHostCallback * callback;
    };
    struct JobCreateContextMenuQt
    {
        JobCreateContextMenuQt(QWidget *parent_, QObject * item_, bool onlyRootItem_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), item(item_), onlyRootItem(onlyRootItem_), callback(callback_) {}
        IContextMenuQt * retval;
        QWidget *parent;
        QObject * item;
        bool onlyRootItem;
        IHostCallback * callback;
    };
    struct JobShowImagePreviewDialog
    {
        JobShowImagePreviewDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
            : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
        IImagePreviewDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        IHostCallback * callback;
    };

signals:
    void triggerShowSceneGraphDialog(JobShowSceneGraphDialog * job);
    void triggerShowObjectLoggerDialog(JobShowObjectLoggerDialog * job);
    void triggerShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger * job);
    void triggerCreateContextMenu(JobCreateContextMenu * job);
    void triggerCreateContextMenuQt(JobCreateContextMenuQt * job);
    void triggerShowImagePreviewDialog(JobShowImagePreviewDialog * job);

protected slots:
    void implShowSceneGraphDialog(JobShowSceneGraphDialog * job);
    void implShowObjectLoggerDialog(JobShowObjectLoggerDialog * job);
    void implShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger * job);
    void implCreateContextMenu(JobCreateContextMenu * job);
    void implCreateContextMenuQt(JobCreateContextMenuQt * job);
    void implShowImagePreviewDialog(JobShowImagePreviewDialog * job);
};

} // namespace sgi