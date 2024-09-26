#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->reg_btn,&QPushButton::clicked,this,&LoginDialog::switchRegister);

    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);
}

void LoginDialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"desturt LoginDialog";
    delete ui;
}
