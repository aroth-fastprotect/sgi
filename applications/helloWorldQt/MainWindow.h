#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
                        MainWindow(QImage * image, QWidget * parent=nullptr);
    virtual             ~MainWindow();

public slots:
    void                fileQuit();
    void                sgiVersionInfo();

private:
    Ui::MainWindow *   ui;
};

