#include "stdafx.h"
#include "QtProxy.h"

#include <QThread>
#include <QApplication>

#include "SceneGraphDialog.h"
#include "ContextMenu.h"
#include "ObjectLoggerDialog.h"
#include "ImagePreviewDialog.h"
#include <sgi/helpers/qt>

#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

using namespace qt_helpers;

namespace {
    static void ensure_QApplication()
    {
        QCoreApplication * app = QApplication::instance();
        if(!app)
        {
            static int argc = 0;
            static char ** argv = NULL;

#ifndef _WIN32
            // Workaround to avoid re-initilaziation of glib
            char* envvar = qstrdup("QT_NO_THREADED_GLIB=1");
            ::putenv(envvar);
#endif

            new QApplication(argc, argv);
        }
    }

}

struct QtProxy::JobShowSceneGraphDialog
{
    JobShowSceneGraphDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
    ISceneGraphDialogPtr retval;
    QWidget *parent;
    SGIItemBase * item;
    IHostCallback * callback;
};
struct QtProxy::JobShowObjectLoggerDialog
{
    JobShowObjectLoggerDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
    IObjectLoggerDialogPtr retval;
    QWidget *parent;
    SGIItemBase * item;
    IHostCallback * callback;
};
struct QtProxy::JobShowObjectLoggerDialogForLogger
{
    JobShowObjectLoggerDialogForLogger(QWidget *parent_, IObjectLogger * logger_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), logger(logger_), callback(callback_) {}
    IObjectLoggerDialogPtr retval;
    QWidget *parent;
    IObjectLogger * logger;
    IHostCallback * callback;
};
struct QtProxy::JobCreateContextMenu
{
    JobCreateContextMenu(QWidget *parent_, SGIItemBase * item_, bool onlyRootItem_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), item(item_), onlyRootItem(onlyRootItem_), callback(callback_) {}
    IContextMenuPtr retval;
    QWidget *parent;
    SGIItemBase * item;
    bool onlyRootItem;
    IHostCallback * callback;
};
struct QtProxy::JobCreateContextMenuQt
{
    JobCreateContextMenuQt(QWidget *parent_, QObject * item_, bool onlyRootItem_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), item(item_), onlyRootItem(onlyRootItem_), callback(callback_) {}
    IContextMenuQtPtr retval;
    QWidget *parent;
    QObject * item;
    bool onlyRootItem;
    IHostCallback * callback;
};
struct QtProxy::JobShowImagePreviewDialog
{
    JobShowImagePreviewDialog(QWidget *parent_, SGIItemBase * item_, IHostCallback * callback_)
        : retval(NULL), parent(parent_), item(item_), callback(callback_) {}
    IImagePreviewDialogPtr retval;
    QWidget *parent;
    SGIItemBase * item;
    IHostCallback * callback;
};

QtProxy::QtProxy()
{
    ensure_QApplication();

    // attach this proxy to the main thread
    moveToThread(QCoreApplication::instance()->thread());

    // and connect the trigger functions (signals) to their
    // implementation function using a BlockingQueuedConnection to
    // transfer the creation of GUI related objects to the main
    // thread and the result back again.
    connect(this, &QtProxy::triggerShowSceneGraphDialog,
            this, &QtProxy::implShowSceneGraphDialog,
            Qt::BlockingQueuedConnection);
    connect(this, &QtProxy::triggerShowObjectLoggerDialog,
            this, &QtProxy::implShowObjectLoggerDialog,
            Qt::BlockingQueuedConnection);
    connect(this, &QtProxy::triggerShowObjectLoggerDialogForLogger,
            this, &QtProxy::implShowObjectLoggerDialogForLogger,
            Qt::BlockingQueuedConnection);
    connect(this, &QtProxy::triggerShowImagePreviewDialog,
            this, &QtProxy::implShowImagePreviewDialog,
            Qt::BlockingQueuedConnection);
    connect(this, &QtProxy::triggerCreateContextMenu,
            this, &QtProxy::implCreateContextMenu,
            Qt::BlockingQueuedConnection);
    connect(this, &QtProxy::triggerCreateContextMenuQt,
            this, &QtProxy::implCreateContextMenuQt,
            Qt::BlockingQueuedConnection);
}

QtProxy * QtProxy::instance(bool erase)
{
    static QtProxy * s_proxy = new QtProxy;
    if(erase)
    {
        delete s_proxy;
        s_proxy = NULL;
    }
    return s_proxy;
}

ISceneGraphDialog * QtProxy::showSceneGraphDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobShowSceneGraphDialog job(parent, item, callback);
    if(QThread::currentThread() == thread())
        implShowSceneGraphDialog(&job);
    else
        emit triggerShowSceneGraphDialog(&job);
    return job.retval.release();
}

void QtProxy::implShowSceneGraphDialog(JobShowSceneGraphDialog * job)
{
    SceneGraphDialog * qtdialog = new SceneGraphDialog(job->item, job->callback, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IObjectLoggerDialog * QtProxy::showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobShowObjectLoggerDialog job(parent, item, callback);
    if(QThread::currentThread() == thread())
        implShowObjectLoggerDialog(&job);
    else
        emit triggerShowObjectLoggerDialog(&job);
    return job.retval.release();
}

void QtProxy::implShowObjectLoggerDialog(JobShowObjectLoggerDialog * job)
{
    ObjectLoggerDialog * qtdialog = new ObjectLoggerDialog(job->item, job->callback, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IObjectLoggerDialog * QtProxy::showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobShowObjectLoggerDialogForLogger job(parent, logger, callback);
    if(QThread::currentThread() == thread())
        implShowObjectLoggerDialogForLogger(&job);
    else
        emit triggerShowObjectLoggerDialogForLogger(&job);
    return job.retval.release();
}

void QtProxy::implShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger * job)
{
    ObjectLoggerDialog * qtdialog = new ObjectLoggerDialog(job->logger, job->callback, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IContextMenu * QtProxy::createContextMenu(QWidget *parent, SGIItemBase * item, bool onlyRootItem, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobCreateContextMenu job(parent, item, onlyRootItem, callback);
    if(QThread::currentThread() == thread())
        implCreateContextMenu(&job);
    else
        emit triggerCreateContextMenu(&job);
    return job.retval.release();
}

void QtProxy::implCreateContextMenu(JobCreateContextMenu * job)
{
    ContextMenu * qtmenu = new ContextMenu(job->item, job->callback, job->onlyRootItem, job->parent);
    job->retval = qtmenu->menuInterface();
}

IContextMenuQt * QtProxy::createContextMenu(QWidget *parent, QObject * item, bool onlyRootItem, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobCreateContextMenuQt job(parent, item, onlyRootItem, callback);
    if(QThread::currentThread() == thread())
        implCreateContextMenuQt(&job);
    else
        emit triggerCreateContextMenuQt(&job);
    return job.retval;
}

void QtProxy::implCreateContextMenuQt(JobCreateContextMenuQt * job)
{
    ContextMenuQt * qtmenu = new ContextMenuQt(job->item, job->callback, job->onlyRootItem, job->parent);
    job->retval = qtmenu->menuInterface();
}

IImagePreviewDialog * QtProxy::showImagePreviewDialog(QWidget *parent, SGIItemBase * item, IHostCallback * callback)
{
    assert(callback);
    if(!parent)
        parent = callback->getFallbackParentWidget();

    JobShowImagePreviewDialog job(parent, item, callback);
    if(QThread::currentThread() == thread())
        implShowImagePreviewDialog(&job);
    else
        emit triggerShowImagePreviewDialog(&job);
    return job.retval.release();
}

void QtProxy::implShowImagePreviewDialog(JobShowImagePreviewDialog * job)
{
    ImagePreviewDialog * qtdialog = new ImagePreviewDialog(job->item, job->callback, job->parent);
    job->retval = qtdialog->dialogInterface();
}

} // namespace sgi
