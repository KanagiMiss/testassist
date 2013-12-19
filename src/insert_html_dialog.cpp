#include "insert_html_dialog.h"
#include "ui_inserthtmldialog.h"
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QTextCodec>

InsertHtmlDialog::InsertHtmlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertHtmlDialog)
{
    ui->setupUi(this);
    groupSelect(0);
}

InsertHtmlDialog::~InsertHtmlDialog()
{
    delete ui;
}

void InsertHtmlDialog::groupSelect(int sel)
{
    isInputByHand = (sel == 0);
    ui->insertByHand->setChecked(sel == 0);
    ui->textEditHtml->setEnabled(sel == 0);
    ui->insertByFile->setChecked(sel == 1);
    ui->lineEditFilePath->setEnabled(sel == 1);
    ui->browse->setEnabled(sel == 1);
}

void InsertHtmlDialog::on_insertByHand_clicked()
{
    groupSelect(0);
}

void InsertHtmlDialog::on_insertByFile_clicked()
{
    groupSelect(1);
}

void InsertHtmlDialog::on_buttonBox_accepted()
{
    if(isInputByHand)
        html = ui->textEditHtml->toPlainText();
    else{
        QString f = ui->lineEditFilePath->text();
        if (!QFile::exists(f)){
            html.clear();
            return;
        }
        QFile file(f);
        if (!file.open(QFile::ReadOnly)){
            html.clear();
            return;
        }

        QByteArray data = file.readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        html = codec->toUnicode(data);
    }
}


void InsertHtmlDialog::on_browse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("打开html"), QString(), tr("html文件 (*.htm *.html)"));

    if(fileName.isEmpty())
        return;

    ui->lineEditFilePath->setText(fileName);
}
