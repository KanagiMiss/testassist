#include "admin_dialog.h"
#include "ui_admindialog.h"
#include "testassistance.h"
#include <vector>
#include <QTableWidgetItem>
#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QByteArray>
#include <QBuffer>

AdminDialog::AdminDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    readUserData();
    readQuestionData();
}

AdminDialog::~AdminDialog()
{
    delete ui;
}

void AdminDialog::addUserLine(int i, const QString &id, const QString &name, const QString &pass, bool admin)
{
    notUserChange = true;

    QTableWidgetItem *item1 = new QTableWidgetItem(id);
    item1->setFlags(item1->flags() & ~Qt::ItemIsEnabled);
    ui->tableWidget_userinfo->setItem(i, 0, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(name);
    ui->tableWidget_userinfo->setItem(i, 1, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem(pass);
    ui->tableWidget_userinfo->setItem(i, 2, item3);

    QString isAdmin = (admin?"是":"否");
    QTableWidgetItem *item4 = new QTableWidgetItem(isAdmin);
    ui->tableWidget_userinfo->setItem(i, 3, item4);

    notUserChange = false;
}

void AdminDialog::addQuestionLine(int i, const QString &id, const QString &text, const QString &grade,
                                  const QString &major, const QString &course, const QString &cnt,
                                  const QString &hard, const QString &date, const QString &right,
                                  const QString &type, const QString &point, const QByteArray &imgBytes)
{
    notUserChange = true;

    //qno
    QTableWidgetItem *item1 = new QTableWidgetItem(id);
    item1->setFlags(item1->flags() & ~Qt::ItemIsEnabled);
    ui->tableWidget_questions->setItem(i, 0, item1);
    //qtxt
    QTableWidgetItem *item2 = new QTableWidgetItem(text);
    ui->tableWidget_questions->setItem(i, 1, item2);
    //qgrade
    QTableWidgetItem *item3 = new QTableWidgetItem(grade);
    ui->tableWidget_questions->setItem(i, 2, item3);
    //qmajor
    QTableWidgetItem *item4 = new QTableWidgetItem(major);
    ui->tableWidget_questions->setItem(i, 3, item4);
    //qcourse
    QTableWidgetItem *item5 = new QTableWidgetItem(course);
    ui->tableWidget_questions->setItem(i, 4, item5);
    //qcnt
    QTableWidgetItem *item6 = new QTableWidgetItem(cnt);
    ui->tableWidget_questions->setItem(i, 5, item6);
    //qhard
    QTableWidgetItem *item7 = new QTableWidgetItem(hard);
    ui->tableWidget_questions->setItem(i, 6, item7);
    //qdate
    QTableWidgetItem *item8 = new QTableWidgetItem(date);
    ui->tableWidget_questions->setItem(i, 7, item8);
    //qright
    QTableWidgetItem *item9 = new QTableWidgetItem(right);
    ui->tableWidget_questions->setItem(i, 8, item9);
    //qtype
    QTableWidgetItem *item10 = new QTableWidgetItem(type);
    ui->tableWidget_questions->setItem(i, 9, item10);
    //qknowpoint
    QTableWidgetItem *item11 = new QTableWidgetItem(point);
    ui->tableWidget_questions->setItem(i, 10, item11);
    //qimage
    //read image
    if(!imgBytes.isNull()){
        QPixmap pixmap;
        pixmap.loadFromData(imgBytes, "PNG");
        QLabel *item12 = new QLabel;
        item12->setPixmap(pixmap);
        int r = ui->tableWidget_questions->rowHeight(i),
            c = ui->tableWidget_questions->columnWidth(11);
        if(r < pixmap.height())
            ui->tableWidget_questions->setRowHeight(i, pixmap.height());
        if(c < pixmap.width())
            ui->tableWidget_questions->setColumnWidth(11, pixmap.width());
        ui->tableWidget_questions->setCellWidget(i, 11, item12);
    }
    else{
        QTableWidgetItem *item12 = new QTableWidgetItem("无图");
        item12->setFlags(item12->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_questions->setItem(i, 11, item12);
    }
    notUserChange = false;
}

void AdminDialog::readUserData()
{
    TestAssistance *pt = TestAssistance::get();
    if(!pt->packAllUserInfo()){
        QMessageBox::warning(this, tr("错误"),tr("读取用户信息失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }

    const std::vector<USERINFO> &info = pt->getAllUserInfo();
    userNum = info.size();
    ui->tableWidget_userinfo->setRowCount(info.size());

    for(int i = 0; i != ui->tableWidget_userinfo->rowCount(); ++i){
        addUserLine(i, tr("%1").arg(info[i].nId), info[i].strUserName, info[i].strPassword, info[i].isAdmin);
    }
}

void AdminDialog::readQuestionData()
{
    TestAssistance *pt = TestAssistance::get();
    if(!pt->packAllQuestions()){
        QMessageBox::warning(this, tr("错误"),tr("读取试题信息失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }

    const std::vector<QUESTIONINFO> &info = pt->getAllQuestions();
    questionNum = info.size();
    ui->tableWidget_questions->setRowCount(info.size());

    for(int i = 0; i != ui->tableWidget_questions->rowCount(); ++i){
        addQuestionLine(i, tr("%1").arg(info[i].qno), info[i].qtxt, tr("%1").arg(info[i].qgrade), info[i].qmajor,
                        info[i].qcourse, tr("%1").arg(info[i].qcnt),tr("%1").arg(info[i].qhard),tr("%1").arg(info[i].qdate),
                        tr("%1").arg(info[i].qright),tr("%1").arg(info[i].qtype),info[i].qknowpoint,info[i].qimage);
    }
}

void AdminDialog::on_tableWidget_userinfo_itemChanged(QTableWidgetItem *item)
{
    if(notUserChange)
        return;

    TestAssistance *pt = TestAssistance::get();
    USERINFO info;
    int row = item->row();
    info.nId         = ui->tableWidget_userinfo->item(row,0)->text().toInt();
    info.strUserName = ui->tableWidget_userinfo->item(row,1)->text();
    info.strPassword = ui->tableWidget_userinfo->item(row,2)->text();
    info.isAdmin     = ((ui->tableWidget_userinfo->item(row,3)->text() == "是")?true:false);

    if(!pt->updateUserInfo(info))
        QMessageBox::warning(this, tr("错误"),tr("更新记录失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
}

void AdminDialog::on_tableWidget_questions_itemChanged(QTableWidgetItem *item)
{
    if(notUserChange)
        return;

    TestAssistance *pt = TestAssistance::get();
    int r = item->row();
    QTableWidget *ptable =  ui->tableWidget_questions;
    QUESTIONINFO info;
    info.qno        = ptable->item(r,0)->text().toInt();
    info.qtxt       = ptable->item(r,1)->text();
    info.qgrade     = ptable->item(r,2)->text().toInt();
    info.qmajor      = ptable->item(r,3)->text();
    info.qcourse    = ptable->item(r,4)->text();
    info.qcnt       = ptable->item(r,5)->text().toInt();
    info.qhard      = ptable->item(r,6)->text().toInt();
    info.qdate      = ptable->item(r,7)->text().toInt();
    info.qright     = ptable->item(r,8)->text().toInt();
    info.qtype      = ptable->item(r,9)->text().toInt();
    info.qknowpoint = ptable->item(r,10)->text();

    if(!pt->updateQuestionInfo(info))
        QMessageBox::warning(this, tr("错误"),tr("更新记录失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
}

void AdminDialog::on_pushButton_add_user_clicked()
{
    QTableWidget *ptable =  ui->tableWidget_userinfo;
    TestAssistance *pt = TestAssistance::get();
    int row = userNum;

    USERINFO info;
    info.nId = userNum + 1;
    info.strUserName = tr("unname%1").arg(userNum);
    info.strPassword = "unname";
    info.isAdmin = false;
    if(!pt->userRegister(info.strUserName, info.strPassword, info.isAdmin, info.nId)){
        QMessageBox::warning(this, tr("错误"),tr("添加用户失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }
    ptable->setRowCount(userNum+1);
    addUserLine(row, tr("%1").arg(info.nId), info.strUserName, info.strPassword, info.isAdmin);
    ui->tableWidget_userinfo->selectRow(row);
    userNum++;
}

void AdminDialog::on_pushButton_remove_user_clicked()
{
    QTableWidget *ptable =  ui->tableWidget_userinfo;
    TestAssistance *pt = TestAssistance::get();
    if(ptable->selectedItems().empty())
        return;

    //confirm
    if(QMessageBox::Yes != QMessageBox::question(this, tr("注意"), tr("真的要删除记录吗？")))
        return;

    for(int i = 0; i < ptable->rowCount(); ++i)
        if(ptable->item(i, 1)->isSelected()){
            if(!pt->removeUser(ptable->item(i, 0)->text().toInt())){
                QMessageBox::warning(this, tr("错误"),
                                     tr("删除记录失败！\n信息%1").arg(pt->getErrorInfo().strErrInfo));
                return;
            }
            userNum--;
            notUserChange = true;
            ptable->removeRow(i);
            notUserChange = false;
            --i;
        }
}

void AdminDialog::on_pushButton_add_question_clicked()
{
    QTableWidget *ptable =  ui->tableWidget_questions;
    TestAssistance *pt = TestAssistance::get();
    int row = questionNum;


    QUESTIONINFO info;
    info.qno = questionNum + 1;
    info.qtxt = tr("noquestion%1").arg(questionNum);
    info.qcourse = "nocourse";
    info.qmajor = "nomajor";
    info.qknowpoint = "nopoint";
    info.qdate = 0;
    info.qgrade = 0;
    info.qhard = 0;
    info.qright = 0;
    info.qtype = 0;
    info.qcnt = 0;
    if(!pt->addQuestion(info, info.qno)){
        QMessageBox::warning(this, tr("错误"),tr("添加试题失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }
    ptable->setRowCount(questionNum+1);
    addQuestionLine(row, tr("%1").arg(info.qno), info.qtxt, tr("%1").arg(info.qgrade), info.qmajor,
                    info.qcourse, tr("%1").arg(info.qcnt),tr("%1").arg(info.qhard),tr("%1").arg(info.qdate),
                    tr("%1").arg(info.qright),tr("%1").arg(info.qtype),info.qknowpoint,info.qimage);
    ui->tableWidget_questions->selectRow(row);
    questionNum++;
}

void AdminDialog::on_pushButton_remove_question_clicked()
{
    QTableWidget *ptable =  ui->tableWidget_questions;
    TestAssistance *pt = TestAssistance::get();
    if(ptable->selectedItems().empty())
        return;

    //confirm
    if(QMessageBox::Yes != QMessageBox::question(this, tr("注意"), tr("真的要删除记录吗？")))
        return;



    for(int i = 0; i < ptable->rowCount(); ++i)
        if(ptable->item(i, 1)->isSelected()){
            if(!pt->removeQuestion(ptable->item(i, 0)->text().toInt())){
                QMessageBox::warning(this, tr("错误"),
                                     tr("删除记录失败！\n信息%1").arg(pt->getErrorInfo().strErrInfo));
                return;
            }
            questionNum--;
            notUserChange = true;
            ptable->removeRow(i);
            notUserChange = false;
            --i;
        }
}

QString AdminDialog::imgFormat(const QString &fileName)
{
    if(fileName.endsWith(".jpg", Qt::CaseInsensitive) ||
       fileName.endsWith(".jpeg", Qt::CaseInsensitive))
        return tr("JPG");
    if(fileName.endsWith(".png", Qt::CaseInsensitive))
        return tr("PNG");
    if(fileName.endsWith(".bmp", Qt::CaseInsensitive))
        return tr("BMP");
    if(fileName.endsWith(".gif", Qt::CaseInsensitive))
        return tr("GIF");
    return tr("PNG");
}


void AdminDialog::on_tableWidget_questions_cellDoubleClicked(int row, int column)
{
    if(column == 11){
        QString fileName;
        fileName = QFileDialog::getOpenFileName(this, tr("插入图像"),
                                              QString(),
                                              tr("Portable Network Graphics (*.png);;All Files (*)"));
        if(fileName.isEmpty())
            return;
        QPixmap img(fileName);
        QLabel *label = new QLabel;
        label->setPixmap(img);
        int r = ui->tableWidget_questions->rowHeight(row),
            c = ui->tableWidget_questions->columnWidth(column);
        if(r < img.height())
            ui->tableWidget_questions->setRowHeight(row, img.height());
        if(c < img.width())
            ui->tableWidget_questions->setColumnWidth(column, img.width());
        ui->tableWidget_questions->setCellWidget(row, column, label);

        //write to database
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, "PNG"); // writes pixmap into bytes in PNG format

        TestAssistance *pt = TestAssistance::get();
        QTableWidget *ptable =  ui->tableWidget_questions;
        QUESTIONINFO info;
        info.qno        = ptable->item(row,0)->text().toInt();
        info.qtxt       = ptable->item(row,1)->text();
        info.qgrade     = ptable->item(row,2)->text().toInt();
        info.qmajor      = ptable->item(row,3)->text();
        info.qcourse    = ptable->item(row,4)->text();
        info.qcnt       = ptable->item(row,5)->text().toInt();
        info.qhard      = ptable->item(row,6)->text().toInt();
        info.qdate      = ptable->item(row,7)->text().toInt();
        info.qright     = ptable->item(row,8)->text().toInt();
        info.qtype      = ptable->item(row,9)->text().toInt();
        info.qknowpoint = ptable->item(row,10)->text();
        info.qimage     = bytes;

        if(!pt->updateQuestionInfo(info, true))
            QMessageBox::warning(this, tr("错误"),tr("更新记录失败！\n信息：%1").arg(pt->getErrorInfo().strErrInfo));

    }
}
