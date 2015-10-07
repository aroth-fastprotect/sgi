#pragma once

#include <QObject>

namespace sgi {

class SGIItemBase;

class ISceneGraphDialogInfo;
class ISceneGraphDialog;
class IObjectLogger;
class IObjectLoggerDialog;
class IObjectLoggerDialogInfo;
class IContextMenu;
class IContextMenuInfo;
class IContextMenuQt;
class IContextMenuInfoQt;
class IImagePreviewDialog;
class IImagePreviewDialogInfo;

class QtProxy : public QObject
{
    Q_OBJECT
protected:
    QtProxy();

public:
    static QtProxy *        instance(bool erase=false);

public:
    ISceneGraphDialog *     showSceneGraphDialog(QWidget *parent, SGIItemBase * item, ISceneGraphDialogInfo * info);
    IObjectLoggerDialog *   showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IObjectLoggerDialogInfo * info);
    IObjectLoggerDialog *   showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IObjectLoggerDialogInfo * info);
    IContextMenu *          createContextMenu(QWidget *parent, SGIItemBase * item, IContextMenuInfo * info);
    IContextMenuQt *        createContextMenu(QWidget *parent, QObject * item, IContextMenuInfoQt * info);
    IImagePreviewDialog *   showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IImagePreviewDialogInfo * info);

private:
    struct JobShowSceneGraphDialog
    {
        JobShowSceneGraphDialog(QWidget *parent_, SGIItemBase * item_, ISceneGraphDialogInfo * info_)
            : retval(NULL), parent(parent_), item(item_), info(info_) {}
        ISceneGraphDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        ISceneGraphDialogInfo * info;
    };
    struct JobShowObjectLoggerDialog
    {
        JobShowObjectLoggerDialog(QWidget *parent_, SGIItemBase * item_, IObjectLoggerDialogInfo * info_)
            : retval(NULL), parent(parent_), item(item_), info(info_) {}
        IObjectLoggerDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        IObjectLoggerDialogInfo * info;
    };
    struct JobShowObjectLoggerDialogForLogger
    {
        JobShowObjectLoggerDialogForLogger(QWidget *parent_, IObjectLogger * logger_, IObjectLoggerDialogInfo * info_)
            : retval(NULL), parent(parent_), logger(logger_), info(info_) {}
        IObjectLoggerDialog * retval;
        QWidget *parent;
        IObjectLogger * logger;
        IObjectLoggerDialogInfo * info;
    };
    struct JobCreateContextMenu
    {
        JobCreateContextMenu(QWidget *parent_, SGIItemBase * item_, IContextMenuInfo * info_)
            : retval(NULL), parent(parent_), item(item_), info(info_) {}
        IContextMenu * retval;
        QWidget *parent;
        SGIItemBase * item;
        IContextMenuInfo * info;
    };
    struct JobCreateContextMenuQt
    {
        JobCreateContextMenuQt(QWidget *parent_, QObject * item_, IContextMenuInfoQt * info_)
            : retval(NULL), parent(parent_), item(item_), info(info_) {}
        IContextMenuQt * retval;
        QWidget *parent;
        QObject * item;
        IContextMenuInfoQt * info;
    };
    struct JobShowImagePreviewDialog
    {
        JobShowImagePreviewDialog(QWidget *parent_, SGIItemBase * item_, IImagePreviewDialogInfo * info_)
            : retval(NULL), parent(parent_), item(item_), info(info_) {}
        IImagePreviewDialog * retval;
        QWidget *parent;
        SGIItemBase * item;
        IImagePreviewDialogInfo * info;
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