#include "insert_table_dialog.h"
#include "ui_inserttabledialog.h"

InsertTableDialog::InsertTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertTableDialog)
{
    ui->setupUi(this);
}

InsertTableDialog::~InsertTableDialog()
{
    delete ui;
}

void InsertTableDialog::on_buttonBox_accepted()
{
    r = ui->lineNum->value();
    c = ui->columnNum->value();
}
