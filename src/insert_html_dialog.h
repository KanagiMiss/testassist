#ifndef INSERTHTMLDIALOG_H
#define INSERTHTMLDIALOG_H

#include <QDialog>

namespace Ui {
class InsertHtmlDialog;
}

class InsertHtmlDialog : public QDialog
{
    Q_OBJECT

private:
    void groupSelect(int sel);

public:
    explicit InsertHtmlDialog(QWidget *parent = 0);
    ~InsertHtmlDialog();

    bool isInputByHand;
    const QString &htmlText()const{return html;}

private slots:
    void on_insertByHand_clicked();

    void on_buttonBox_accepted();

    void on_insertByFile_clicked();

    void on_browse_clicked();

private:
    Ui::InsertHtmlDialog *ui;
    QString html;
};

#endif // INSERTHTMLDIALOG_H
