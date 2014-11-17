#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdesktopwidget.h"
#include "qpropertyanimation.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <pipeline.h>
#include <sys/types.h>
#include <QMessageBox>
#include <QDialogButtonBox>
#define WIN_GEO this->geometry()

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: #444444;");
    QRect desktop = QApplication::desktop()->availableGeometry();
    int width = desktop.width()*2/3;
    int height = 1;
    int destination_height = 100;
    this->setGeometry(desktop.width()/2-width/2,desktop.height()/2,width,height);
    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);
    animation->setStartValue(this->geometry());
    animation->setEndValue(QRect(WIN_GEO.x(),desktop.height()/2-destination_height/2,WIN_GEO.width(),destination_height));
    animation->setEasingCurve(QEasingCurve::InOutQuint);
    animation->start();
    this->ui->lineEdit->setGeometry(25,25,width-50,50);
    this->ui->lineEdit->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent * keyEvent)
{
    if(keyEvent->key() == Qt::Key_Escape)
    {
        QuitWindow(quit_QUIT);
    }
    else if(keyEvent->key() == Qt::Key_Enter||keyEvent->key() == Qt::Key_Return)
    {
        QuitWindow(quit_EXEC);
    }
}


void MainWindow::QuitWindow(QuitMode quit)
{
    QRect desktop = QApplication::desktop()->availableGeometry();
    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);
    animation->setStartValue(this->geometry());
    animation->setEndValue(QRect(WIN_GEO.x(),desktop.height()/2,WIN_GEO.width(),0));
    animation->setEasingCurve(QEasingCurve::InOutQuint);
    if(quit==quit_QUIT)
    {
        connect(animation,SIGNAL(finished()),this,SLOT(Destroy()));
    }
    else
    {
        connect(animation,SIGNAL(finished()),this,SLOT(Execute()));
    }
    animation->start();
}

void MainWindow::Destroy()
{
    QApplication::exit();
}

void MainWindow::Execute()
{
    int pipefd[2];
    pid_t cpid;
    pipe(pipefd); //Create pipe
    char* bigbuf = new char[10];

    cpid = fork();
    if(cpid == 0)
    {
        QString command = this->ui->lineEdit->text();
        int status = std::system(command.toLocal8Bit().data());
        if(status==0x7f00) //the default 0x7f return code is fucked up, so we have to do this
        {
            printf("Sending ERROR signal\n");
            write(pipefd[1],"ERROR",5); //Send error to main application
        }
        exit(0);
    }
    else
    {
        //close(pipefd[1]); //Close write-end of the pipe.
        char buf;
        int n = 0;
        pid_t d_id = fork();
        if(d_id == 0)
        {
            printf("Sending EOF to main application in 10 seconds...\n");
            sleep(10);
            write(pipefd[1],"!",1); //Send EOF to main application, if it hasn't exited already.
            exit(0);
        }
        while(read(pipefd[0],&buf,1) > 0) //read while EOF
        {
            bigbuf[n++]=buf;
            if(n==9)
            {
                break;
            }
            //write(1,&buf,1);
        }
        bigbuf[n]=0;
        write(1,"\n",1);
        //close(pipefd[0]); //Close read-end of the pipe.
        if(strcmp(bigbuf,"ERROR")==0)
        {
            QMessageBox msgbox;
            msgbox.setText("The specified command failed.");
            msgbox.setStandardButtons(QMessageBox::Ok);
            msgbox.exec();
            printf("ERROR message received; terminating.\n");
        }
        else
        {
            printf("EOF received; terminating.\n");
        }
        QApplication::exit();
    }
}
