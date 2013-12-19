#ifndef ADMIN_DIALOG_H
#define ADMIN_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <vector>
#include <QString>

namespace Ui {
class AdminDialog;
}

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(QWidget *parent = 0);
    ~AdminDialog();



private slots:

    void on_tableWidget_userinfo_itemChanged(QTableWidgetItem *item);

    void on_pushButton_add_user_clicked();

    void on_pushButton_remove_user_clicked();

    void on_pushButton_add_question_clicked();

    void on_pushButton_remove_question_clicked();

    void on_tableWidget_questions_itemChanged(QTableWidgetItem *item);

    void on_tableWidget_questions_cellDoubleClicked(int row, int column);

private:
    Ui::AdminDialog *ui;

    bool notUserChange;
    int  userNum;
    int  questionNum;

    void addUserLine(int i, const QString &id, const QString &name, const QString &pass, bool admin);
    void addQuestionLine(int i, const QString &id, const QString &text, const QString &grade, const QString &major,
                         const QString &course, const QString &cnt, const QString &hard, const QString &date,
                         const QString &right, const QString &type, const QString &point, const QByteArray &imgBytes);
    void readUserData();
    void readQuestionData();
    QString imgFormat(const QString &fileName);
};

#endif // ADMIN_DIALOG_H
