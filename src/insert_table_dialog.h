#ifndef INSERTTABLEDIALOG_H
#define INSERTTABLEDIALOG_H

#include <QDialog>

namespace Ui {
class InsertTableDialog;
}

class InsertTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertTableDialog(QWidget *parent = 0);
    ~InsertTableDialog();

    int row()const{return r;}
    int column()const{return c;}

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InsertTableDialog *ui;

    int r,c;
};

#endif // INSERTTABLEDIALOG_H
