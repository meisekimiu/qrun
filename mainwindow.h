#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
        void Destroy();
        void Execute();

protected:
    void keyPressEvent(QKeyEvent *);

private:
        enum QuitMode {quit_QUIT,quit_EXEC};
    Ui::MainWindow *ui;
    void QuitWindow(QuitMode);\
};

#endif // MAINWINDOW_H
