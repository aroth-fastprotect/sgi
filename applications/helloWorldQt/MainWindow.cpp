#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QBuffer>
#include <QLoggingCategory>

#include <QImageReader>

#include <ui_MainWindow.h>

Q_LOGGING_CATEGORY(helloWorldQt, "helloWorldQt", QtDebugMsg)

MainWindow::MainWindow(QImage * image, QWidget * parent)
    : QMainWindow(parent)
{
    ui = new Ui::MainWindow;
    ui->setupUi( this );

    ui->lineEdit->setObjectName(QApplication::translate("MainWindow", "\327\221\327\231\327\252\327\231 \327\224\327\225\327\220 \327\230\327\231\327\250\327\252\327\231", 0));

	ui->noSGIButton->setProperty("sgi_skip_object", QVariant::fromValue(true));
    if(!image || image->isNull())
        ui->image->setText(tr("Failed to load SGI loader"));
    else
        ui->image->setPixmap(QPixmap::fromImage(*image));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fileQuit()
{
    QApplication::quit();
}

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    QString path = QCoreApplication::applicationDirPath();
    path = QDir::cleanPath(path + SGI_QT_PLUGIN_DIR);
    qDebug(helloWorldQt) << "addLibraryPath " << path;
    QCoreApplication::addLibraryPath(path);
    QImage load_sgi;
    QBuffer dummyMem;
    dummyMem.setData("{\n\"image\": {\n\"filename\": \"s:/tmp/logo.png\"\n}\n}");
    if(load_sgi.load(&dummyMem, "sgi_loader"))
        qDebug(helloWorldQt) << "sgi loaded.";
    else
        qDebug(helloWorldQt) << "failed to load sgi.";

    for(const QByteArray & format : QImageReader::supportedImageFormats())
    {
        qDebug(helloWorldQt) << "reader " << format;
    }


    MainWindow * window = new MainWindow(&load_sgi);
    window->show();

    int ret = app.exec();
    delete window;
    return ret;
}
