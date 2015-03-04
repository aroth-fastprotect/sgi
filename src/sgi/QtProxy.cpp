#include "stdafx.h"
#include "QtProxy.h"

#include <QThread>
#include <QApplication>

#include "SceneGraphDialog.h"
#include "ContextMenu.h"
#include "ObjectLoggerDialog.h"
#include <sgi/helpers/qt>

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

QtProxy::QtProxy()
{
    ensure_QApplication();

    // attach this proxy to the main thread
    moveToThread(QCoreApplication::instance()->thread());

    // and connect the trigger functions (signals) to their
    // implementation function using a BlockingQueuedConnection to
    // transfer the creation of GUI related objects to the main
    // thread and the result back again.
    connect(this, SIGNAL(triggerShowSceneGraphDialog(JobShowSceneGraphDialog *)),
            this, SLOT(implShowSceneGraphDialog(JobShowSceneGraphDialog *)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(triggerShowObjectLoggerDialog(JobShowObjectLoggerDialog *)),
            this, SLOT(implShowObjectLoggerDialog(JobShowObjectLoggerDialog *)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(triggerShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger *)),
            this, SLOT(implShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger *)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(triggerCreateContextMenu(JobCreateContextMenu *)),
            this, SLOT(implCreateContextMenu(JobCreateContextMenu *)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(triggerCreateContextMenuQt(JobCreateContextMenuQt *)),
            this, SLOT(implCreateContextMenuQt(JobCreateContextMenuQt *)),
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

ISceneGraphDialog * QtProxy::showSceneGraphDialog(QWidget *parent, SGIItemBase * item, ISceneGraphDialogInfo * info)
{
    JobShowSceneGraphDialog job(parent, item, info);
    if(QThread::currentThread() == thread())
        implShowSceneGraphDialog(&job);
    else
        emit triggerShowSceneGraphDialog(&job);
    return job.retval;
}

void QtProxy::implShowSceneGraphDialog(JobShowSceneGraphDialog * job)
{
    SceneGraphDialog * qtdialog = new SceneGraphDialog(job->item, job->info, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IObjectLoggerDialog * QtProxy::showObjectLoggerDialog(QWidget *parent, SGIItemBase * item, IObjectLoggerDialogInfo * info)
{
    JobShowObjectLoggerDialog job(parent, item, info);
    if(QThread::currentThread() == thread())
        implShowObjectLoggerDialog(&job);
    else
        emit triggerShowObjectLoggerDialog(&job);
    return job.retval;
}

void QtProxy::implShowObjectLoggerDialog(JobShowObjectLoggerDialog * job)
{
    ObjectLoggerDialog * qtdialog = new ObjectLoggerDialog(job->item, job->info, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IObjectLoggerDialog * QtProxy::showObjectLoggerDialog(QWidget *parent, IObjectLogger * logger, IObjectLoggerDialogInfo * info)
{
    JobShowObjectLoggerDialogForLogger job(parent, logger, info);
    if(QThread::currentThread() == thread())
        implShowObjectLoggerDialogForLogger(&job);
    else
        emit triggerShowObjectLoggerDialogForLogger(&job);
    return job.retval;
}

void QtProxy::implShowObjectLoggerDialogForLogger(JobShowObjectLoggerDialogForLogger * job)
{
    ObjectLoggerDialog * qtdialog = new ObjectLoggerDialog(job->logger, job->info, job->parent);
    job->retval = qtdialog->dialogInterface();
}

IContextMenu * QtProxy::createContextMenu(QWidget *parent, SGIItemBase * item, IContextMenuInfo * info)
{
    JobCreateContextMenu job(parent, item, info);
    if(QThread::currentThread() == thread())
        implCreateContextMenu(&job);
    else
        emit triggerCreateContextMenu(&job);
    return job.retval;
}

void QtProxy::implCreateContextMenu(JobCreateContextMenu * job)
{
    ContextMenu * qtmenu = new ContextMenu(job->item, job->info, job->parent);
    job->retval = qtmenu->menuInterface();
}

IContextMenuQt * QtProxy::createContextMenu(QWidget *parent, QObject * item, IContextMenuInfoQt * info)
{
    JobCreateContextMenuQt job(parent, item, info);
    if(QThread::currentThread() == thread())
        implCreateContextMenuQt(&job);
    else
        emit triggerCreateContextMenuQt(&job);
    return job.retval;
}

void QtProxy::implCreateContextMenuQt(JobCreateContextMenuQt * job)
{
    ContextMenuQt * qtmenu = new ContextMenuQt(job->item, job->info, job->parent);
    job->retval = qtmenu->menuInterface();
}

} // namespace sgi
