#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
                        MainWindow(QImage * image, QWidget * parent=NULL);
    virtual             ~MainWindow();

public slots:
    void                fileQuit();

private:
    Ui::MainWindow *   ui;
};

