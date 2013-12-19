#include "guide_dialog.h"
#include "ui_guidedialog.h"
#include "testassistance.h"
#include <QMessageBox>
#include <QDebug>

GuideDialog::GuideDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuideDialog),
    info(new PAPERINFO)
{   
    //init info
    info->fEasy = 0.3;
    info->fNormal = 0.4;
    info->fHard = 0.3;

    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
    paper_type = AUTO;
    ui->spinBox_judge->setEnabled(false);
    ui->spinBox_choice->setEnabled(false);
    ui->spinBox_fill->setEnabled(false);
    ui->spinBox_calc->setEnabled(false);
    ui->spinBox_ans->setEnabled(false);
    ui->radioButton_close->setChecked(true);

    TestAssistance *pt = TestAssistance::get();
    //init combo box major
    if(!pt->packAllMajors()){
        QMessageBox::warning(this, tr("错误"), tr("提取专业名称失败。\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }

    const std::vector<QString> &m = pt->getAllMajors();
    for(size_t i = 0; i < m.size(); ++i)
        ui->comboBox_major->addItem(m[i]);

    //init combo box grade
    QStringList l;
    l << "大一" << "大二" << "大三" << "大四";
    ui->comboBox_grade->addItems(l);

    //init combo box type
    QStringList list;
    list << "A" << "B" << "C" << "D" << "E" << "F" << "G";
    ui->comboBox_type->addItems(list);

    //init author
    ui->lineEdit_author->setText(pt->getUserInfo().strUserName);
}

GuideDialog::~GuideDialog()
{
    delete ui;
    delete info;
}

void GuideDialog::groupCheck(int index)
{
    ui->radioButton_empty->setChecked(index == 0);
    ui->radioButton_outline->setChecked(index == 1);
    ui->radioButton_auto->setChecked(index == 2);
}

void GuideDialog::openOrClose(int sel)
{
    ui->radioButton_open->setChecked(sel == 0);
    ui->radioButton_close->setChecked(sel == 1);
}

void GuideDialog::on_radioButton_empty_clicked()
{
    groupCheck(0);
    paper_type = EMPTY;
}

void GuideDialog::on_radioButton_outline_clicked()
{
    groupCheck(1);
    paper_type = OUTLINE;
}

void GuideDialog::on_radioButton_auto_clicked()
{
    groupCheck(2);
    paper_type = AUTO;
}

void GuideDialog::on_first_next_clicked()
{
    if(ui->radioButton_empty->isChecked()){
        ui->textEdit_sum->setText("即将生成一份空试卷。");
        ui->guide->setCurrentWidget(ui->sum);
    }
    else if(ui->radioButton_outline->isChecked())
        ui->guide->setCurrentWidget(ui->other);
    else{
        ui->guide->setCurrentWidget(ui->theme);
    }
}

void GuideDialog::on_theme_prev_clicked()
{
    ui->guide->setCurrentWidget(ui->first);
}



void GuideDialog::on_theme_next_clicked()
{
    if(ui->comboBox_major->currentText().isEmpty() ||
            ui->comboBox_course->currentText().isEmpty()){
        QMessageBox::warning(this, "注意", "请填写完整的试卷信息！");
        return;
    }

    TestAssistance * pt = TestAssistance::get();
    info->major = ui->comboBox_major->currentText().toUtf8().data();
    info->nGrade = ui->comboBox_grade->currentIndex();
    info->strCourse = ui->comboBox_course->currentText().toUtf8().data();
    qDebug() << info->major << info->nGrade << info->strCourse;
    pt->setPaperInfo(*info);

    //init knowledge points
    KPOINT p;
    if(pt->packAllKnowledgePoints())
        p = pt->getAllPoints();

    ui->listWidget_points->clear();
    for(size_t i = 0; i != p.size(); ++i){
        QListWidgetItem * item = new QListWidgetItem(p[i], ui->listWidget_points);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget_points->addItem(item);
    }

    ui->guide->setCurrentWidget(ui->difficulty);
}

void GuideDialog::on_diff_prev_clicked()
{
    ui->guide->setCurrentWidget(ui->theme);
}

void GuideDialog::on_diff_next_clicked()
{
    TestAssistance * pt = TestAssistance::get();
    info->fEasy = static_cast<double>(ui->slider_easy->value())/100.0;
    info->fNormal = static_cast<double>(ui->slider_normal->value())/100.0;
    info->fHard = static_cast<double>(ui->slider_hard->value())/100.0;
    pt->setPaperInfo(*info);

    ui->guide->setCurrentWidget(ui->type);
}

void GuideDialog::on_type_prev_clicked()
{
    ui->guide->setCurrentWidget(ui->difficulty);
}

void GuideDialog::on_type_next_clicked()
{
    TestAssistance * pt = TestAssistance::get();
    QNUM qnum;
    qnum.judge = qnum.choice = qnum.fill = qnum.calc = qnum.ans = 0;
    if(ui->spinBox_judge->isEnabled())
        qnum.judge = ui->spinBox_judge->value();
    if(ui->spinBox_choice->isEnabled())
        qnum.choice = ui->spinBox_choice->value();
    if(ui->spinBox_fill->isEnabled())
        qnum.fill = ui->spinBox_fill->value();
    if(ui->spinBox_calc->isEnabled())
        qnum.calc = ui->spinBox_calc->value();
    if(ui->spinBox_ans->isEnabled())
        qnum.ans = ui->spinBox_ans->value();

    KPOINT points;
    for(int i = 0; i < ui->listWidget_points->count(); ++i)
        if(ui->listWidget_points->item(i)->checkState() == Qt::Checked)
            points.push_back(ui->listWidget_points->item(i)->text());

    info->qNum = qnum;
    info->kpoint = points;

    pt->setPaperInfo(*info);

    ui->guide->setCurrentWidget(ui->other);
}

void GuideDialog::on_other_prev_clicked()
{
    if(ui->radioButton_empty->isChecked() || ui->radioButton_outline->isChecked())
        ui->guide->setCurrentWidget(ui->first);
    else
        ui->guide->setCurrentWidget(ui->type);
}

void GuideDialog::on_other_next_clicked()
{
    TestAssistance * pt = TestAssistance::get();
    info->strAuthor = ui->lineEdit_author->text();
    info->strSchool = ui->lineEdit_school->text();
    info->bOpenOrClose = ui->radioButton_open->isChecked();
    info->strPaperType = ui->comboBox_type->currentText();
    pt->setPaperInfo(*info);

    //pack paper sum
    QString paper_sum;

    QString points;
    for(size_t i = 0; i < info->kpoint.size(); ++i){
        points += info->kpoint[i] + " ";
    }
    QString type;
    if(ui->checkBox_judge->isChecked()) type += "判断题 ";
    if(ui->checkBox_choice->isChecked()) type += "选择题 ";
    if(ui->checkBox_fill->isChecked()) type += "填空题 ";
    if(ui->checkBox_calc->isChecked()) type += "计算题 ";
    if(ui->checkBox_ans->isChecked()) type += "简答题";

    paper_sum += tr("科目名称：%1\n学校名称：%2\n作者：%3\n开卷闭卷：%4\n"
                    "试卷类型：%5\n难度分布：%6\n知识点：%7\n题型：%8").arg(info->strCourse)
            .arg(info->strSchool).arg(info->strAuthor).arg((info->bOpenOrClose?"开卷":"闭卷")).arg(info->strPaperType)
            .arg(tr("%1 %2 %3").arg(info->fEasy).arg(info->fNormal).arg(info->fHard))
            .arg(points).arg(type);

    ui->textEdit_sum->setText(paper_sum);

    ui->guide->setCurrentWidget(ui->sum);
}

void GuideDialog::on_sum_prev_clicked()
{
    if(ui->radioButton_empty->isChecked())
        ui->guide->setCurrentWidget(ui->first);
    else
        ui->guide->setCurrentWidget(ui->other);
}

void GuideDialog::on_sum_finish_clicked()
{
    TestAssistance *pt = TestAssistance::get();
    pt->extractQuestion();
    done(QDialog::Accepted);
}

void GuideDialog::on_slider_easy_valueChanged(int value)
{
    int e = value,
        n = ui->slider_normal->value(),
        h = ui->slider_hard->value();
    if(e + n + h > 100)
        e = 100 - n - h;
    ui->per_easy->setText(tr("%1\%").arg(e));
    ui->slider_easy->setSliderPosition(e);
}

void GuideDialog::on_slider_normal_valueChanged(int value)
{
    int e = ui->slider_easy->value(),
        n = value,
        h = ui->slider_hard->value();
    if(e + n + h > 100)
        n = 100 - e - h;
    ui->per_normal->setText(tr("%1\%").arg(n));
    ui->slider_normal->setSliderPosition(n);
}

void GuideDialog::on_slider_hard_valueChanged(int value)
{
    int e = ui->slider_easy->value(),
        n = ui->slider_normal->value(),
        h = value;
    if(e + n + h > 100)
        h = 100 - e - n;
    ui->per_hard->setText(tr("%1\%").arg(h));
    ui->slider_hard->setSliderPosition(h);
}

void GuideDialog::on_checkBox_judge_stateChanged(int arg1)
{
    ui->spinBox_judge->setEnabled(arg1 == 2);
}

void GuideDialog::on_checkBox_choice_stateChanged(int arg1)
{
    ui->spinBox_choice->setEnabled(arg1 == 2);
}

void GuideDialog::on_checkBox_fill_stateChanged(int arg1)
{
    ui->spinBox_fill->setEnabled(arg1 == 2);
}

void GuideDialog::on_checkBox_calc_stateChanged(int arg1)
{
    ui->spinBox_calc->setEnabled(arg1 == 2);
}

void GuideDialog::on_checkBox_ans_stateChanged(int arg1)
{
    ui->spinBox_ans->setEnabled(arg1 == 2);
}

void GuideDialog::on_radioButton_open_clicked()
{
    openOrClose(0);
}

void GuideDialog::on_radioButton_close_clicked()
{
    openOrClose(1);
}

void GuideDialog::updateCourses(const QString &major, int grade)
{
    ui->comboBox_course->clear();
    TestAssistance *pt = TestAssistance::get();
    //init combo box course
    if(!pt->packCourses(major, grade)){
        QMessageBox::warning(this, tr("错误"), tr("提取课程名称失败。\n信息：%1").arg(pt->getErrorInfo().strErrInfo));
        return;
    }

    const std::vector<QString> &c = pt->getCourses();
    for(size_t i = 0; i < c.size(); ++i)
        ui->comboBox_course->addItem(c[i]);
}

void GuideDialog::on_comboBox_major_currentTextChanged(const QString &arg1)
{
    if(ui->comboBox_grade->currentIndex() == -1)
        return;

    updateCourses(arg1, ui->comboBox_grade->currentIndex());
}

void GuideDialog::on_comboBox_grade_currentIndexChanged(int index)
{
    if(ui->comboBox_major->currentIndex() == -1)
        return;

    updateCourses(ui->comboBox_major->currentText(), index);
}

void GuideDialog::on_comboBox_course_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_paper_name->setText(arg1);
}

void GuideDialog::on_pushButton_all_sel_clicked()
{
    for(int i = 0; i < ui->listWidget_points->count(); ++i){
        ui->listWidget_points->item(i)->setCheckState(Qt::Checked);
    }
}

void GuideDialog::on_pushButton_all_unsel_clicked()
{
    for(int i = 0; i < ui->listWidget_points->count(); ++i){
        ui->listWidget_points->item(i)->setCheckState(
                    ((ui->listWidget_points->
                      item(i)->checkState() == Qt::Checked)?
                         Qt::Unchecked:Qt::Checked));
    }
}
