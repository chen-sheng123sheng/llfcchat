#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    //并将其父对象设置为 this。这意味着当父窗口被销毁时 LoginDialog 也将被销毁。
    _login_dlg = new LoginDialog(this);

    /*
    * _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    *这行代码调用 setWindowFlags() 方法来设置窗口的标志。这里使用了两个窗口标志：
    *Qt::CustomizeWindowHint：这个标志允许应用程序自定义窗口的非客户区域（如标题栏和边框）。
                            在某些平台上，这可能需要额外的操作系统特定的设置。
    *Qt::FramelessWindowHint：这个标志创建一个没有边框和标题栏的窗口。
                            这通常用于创建自定义的、无边框的窗口，如弹出窗口或特殊的对话框。
    **/
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    //_login_dlg->show();

    //创建注册信号链接
    connect(_login_dlg,&LoginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);
    _reg_dlg = new RegisterDialog(this);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
    // if(_login_dlg){
    //     delete _login_dlg;
    //     _login_dlg = nullptr;
    // }
    // if(_reg_dlg){
    //     delete _reg_dlg;
    //     _reg_dlg = nullptr;
    // }
}

void MainWindow::SlotSwitchReg()
{
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

