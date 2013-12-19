#ifndef GUIDEDIALOG_H
#define GUIDEDIALOG_H

#include <QDialog>

namespace Ui {
class GuideDialog;
}

struct PAPERINFO;

class GuideDialog : public QDialog
{
    Q_OBJECT

private:
    void groupCheck(int index);
    void openOrClose(int sel);

public:
    enum PAPERTYPE{
        EMPTY,
        OUTLINE,
        AUTO
    }paper_type;

    explicit GuideDialog(QWidget *parent = 0);
    ~GuideDialog();

private slots:
    void on_radioButton_empty_clicked();

    void on_radioButton_outline_clicked();

    void on_radioButton_auto_clicked();

    void on_theme_prev_clicked();

    void on_first_next_clicked();

    void on_theme_next_clicked();

    void on_diff_prev_clicked();

    void on_diff_next_clicked();

    void on_type_prev_clicked();

    void on_type_next_clicked();

    void on_other_prev_clicked();

    void on_other_next_clicked();

    void on_sum_prev_clicked();

    void on_sum_finish_clicked();

    void on_slider_easy_valueChanged(int value);

    void on_slider_normal_valueChanged(int value);

    void on_slider_hard_valueChanged(int value);

    void on_checkBox_judge_stateChanged(int arg1);

    void on_checkBox_choice_stateChanged(int arg1);

    void on_checkBox_fill_stateChanged(int arg1);

    void on_checkBox_calc_stateChanged(int arg1);

    void on_checkBox_ans_stateChanged(int arg1);

    void on_radioButton_open_clicked();

    void on_radioButton_close_clicked();

    void on_comboBox_major_currentTextChanged(const QString &arg1);

    void on_comboBox_grade_currentIndexChanged(int index);

    void on_comboBox_course_currentTextChanged(const QString &arg1);

    void on_pushButton_all_sel_clicked();

    void on_pushButton_all_unsel_clicked();

private:
    Ui::GuideDialog *ui;

    PAPERINFO *info;

    void updateCourses(const QString &major, int grade);
};

#endif // GUIDEDIALOG_H
