#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>

#include <QImageReader>

#include <ui_MainWindow.h>

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
{
    ui = new Ui::MainWindow;
    ui->setupUi( this );

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
    if(load_sgi.load(QStringLiteral("qapp.sgi_loader"), "sgi_loader"))
        qDebug() << "sgi loaded.";
    else
        qDebug() << "failed to load sgi.";

    foreach(const QByteArray & format, QImageReader::supportedImageFormats())
    {
        qDebug() << "reader " << format;
    }


    MainWindow window;
    window.show();

    return app.exec();
}