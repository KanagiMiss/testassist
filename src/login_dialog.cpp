#include "login_dialog.h"
#include "ui_logindialog.h"
#include "testassistance.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
    TestAssistance *pt = TestAssistance::get();
    if(pt->getErrorInfo().nErrCode == TA_NOERROR){
        ui->label_status->setText("[+]连接数据库成功");
    }
    else{
        if(pt->getErrorInfo().nErrCode == TA_ERROR_CANNOT_OPEN_DATABASE)
            ui->label_status->setText("[!]无法打开数据库");
        else
            ui->label_status->setText("[!]无法连接数据库");

        ui->pushButton_login->setEnabled(false);
    }
    ui->lineEdit_name->setFocus();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_clear_clicked()
{
    ui->lineEdit_name->setText("");
    ui->lineEdit_pass->setText("");
}

void LoginDialog::on_pushButton_login_clicked()
{
    TestAssistance *pt = TestAssistance::get();
    if(!pt->login(ui->lineEdit_name->text(), ui->lineEdit_pass->text())){
        if(pt->getErrorInfo().nErrCode == TA_ERROR_USER_OR_PASS_INCORRECT)
            ui->label_status->setText("[-]用户名或者密码错误");
        else
            ui->label_status->setText(pt->getErrorInfo().strErrInfo);
    }
    else{
        done(TA_NOERROR);
    }
}
