#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QBuffer>

#include <QImageReader>

#include <ui_MainWindow.h>

MainWindow::MainWindow(QImage * image, QWidget * parent)
    : QMainWindow(parent)
{
    ui = new Ui::MainWindow;
    ui->setupUi( this );

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
    path = QDir::cleanPath(path + "/../pluginsd");
    qDebug() << "addLibraryPath " << path;
    QCoreApplication::addLibraryPath(path);
    QImage load_sgi;
    QBuffer dummyMem;
    if(load_sgi.load(&dummyMem, "sgi_loader"))
        qDebug() << "sgi loaded.";
    else
        qDebug() << "failed to load sgi.";

    foreach(const QByteArray & format, QImageReader::supportedImageFormats())
    {
        qDebug() << "reader " << format;
    }


    MainWindow window(&load_sgi);
    window.show();

    return app.exec();
}