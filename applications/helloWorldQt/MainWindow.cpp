#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QBuffer>
#include <QLoggingCategory>

#include <QImageReader>

#include <ui_MainWindow.h>

Q_LOGGING_CATEGORY(helloWorldQt, "helloWorldQt", QtDebugMsg)

bool isImageFile(const QString & filename)
{
    QByteArray format = QImageReader::imageFormat(filename);
    return !format.isNull();
}

struct ImageFile {
    ImageFile(const QString & file, const QByteArray & fmt )
        : filename(file), format(fmt) {}
    QString filename;
    QByteArray format;
};

bool sendSGIRawCommand(const QByteArray & cmd)
{
    QBuffer dummyMem;
    QImage load_file_with_sgi;
    dummyMem.setData(cmd);
    return load_file_with_sgi.load(&dummyMem, "sgi_loader");
}

bool openSGIImage(const ImageFile & imgf)
{
    QBuffer dummyMem;
    QImage load_file_with_sgi;
    dummyMem.setData("{\n\"image\": {\n\"filename\": \"" + imgf.filename.toLocal8Bit() + "\",\n\"format\": \"" + imgf.format + "\"}\n}");
    return load_file_with_sgi.load(&dummyMem, "sgi_loader");
}

QByteArray parseSGICommand(const QString & arg)
{
    QByteArray cmd;
    if(arg == "list-plugins")
    {
        cmd = "{\n\"list-plugins\": {}\n}";
    }
    else if(arg == "app")
    {
        cmd = "{\n\"object\": { \"name\":\"app\" }\n}";
    }
    else if(arg == "sgi-info")
    {
        cmd = "{\n\"object\": { \"name\":\"sgi\" }\n}";
    }
    else if (arg == "mainwindow")
    {
        cmd = "{\n\"object\": { \"name\":\"mainwindow\" }\n}";
    }
    return cmd;
}

bool sendSGITextCommand(const QString & cmd)
{
    QByteArray raw = parseSGICommand(cmd);
    if(raw.isNull())
        return false;
    return sendSGIRawCommand(raw);
}


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

void MainWindow::sgiVersionInfo()
{
    sendSGITextCommand("sgi-info");
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

    QList<QByteArray> rawCommands;
    QList<ImageFile> imageFiles;
    bool first = true;
    for(const QString & arg : app.arguments())
    {
        if(first)
        {
            // skip the application name
            first = false;
            continue;
        }
        if(arg.at(0) == '-')
        {
            bool isLongArg = (arg.length() > 1 && arg.at(1) == '-');
            if(isLongArg)
            {
                QString longarg = arg.mid(2);
                QByteArray cmd = parseSGICommand(longarg);
                if(cmd.isNull())
                    qCritical() << "invalid argument" << arg;
                else
                    rawCommands.append(cmd);
            }
            else
            {
                qCritical() << "invalid argument" << arg;
            }
        }
        else
        {
            QFileInfo fi(arg);
            QString filename = fi.absoluteFilePath();
            QByteArray format = QImageReader::imageFormat(filename);
            if(!format.isNull())
                imageFiles.append(ImageFile(filename, format));
        }
    }


    QImage load_sgi;
    QBuffer dummyMem;
    if(load_sgi.load(&dummyMem, "sgi_loader"))
        qWarning(helloWorldQt) << "sgi loaded.";
    else
        qWarning(helloWorldQt) << "failed to load sgi.";

#if 0
    for(const QByteArray & format : QImageReader::supportedImageFormats())
    {
        qWarning(helloWorldQt) << "reader " << format;
    }
#endif

    if(!load_sgi.isNull())
    {
        for(const QByteArray & cmd : rawCommands)
        {
            if(!sendSGIRawCommand(cmd))
                qWarning(helloWorldQt) << "Command" << cmd << "failed";
        }

        for(const ImageFile & imgf : imageFiles)
        {
            if(openSGIImage(imgf))
                qWarning(helloWorldQt) << imgf.filename << " loading...";
        }
    }

    MainWindow * window = new MainWindow(&load_sgi);
    window->show();

    int ret = app.exec();
    delete window;
    return ret;
}
