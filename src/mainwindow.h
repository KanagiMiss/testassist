#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QHBoxLayout>
#include <vector>
#include <QTextCharFormat>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QAction>
#include <QCloseEvent>
#include <QPrinter>


//text editor info
struct TextEditor{
public:
    //attributes
    QWidget     *tab;
    QTextEdit   *editor;
    QHBoxLayout *layout;

    QString     objName;//ojb name of tab
    QString     fileName;//file name
};

struct PACKEDQUESTIONS;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void setWelcomActions(bool b);
    void setCurrentFileName(const QString &);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    void initTextEditor();

    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    void groupCheck(int a);

    bool maybeSave();

    bool loadFile(const QString &f);

    QString digitToChinese(int d);
    void insertQuestionTypeTitle(const QString &title);
    void formatQuestions(const PACKEDQUESTIONS &qset);

private slots:
    //user define
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void printPreview(QPrinter *printer);

    bool fileSave();
    bool fileSaveAs();

private slots:
    void on_tabs_currentChanged(int index);

    void on_actionFileNew_triggered();

    void on_actionSave_triggered();

    void on_actionFileSaveAs_triggered();

    void on_actionSearch_triggered();

    void on_actionSetting_triggered();

    void on_actionHelp_triggered();

    void on_actionExit_triggered();

    void on_actionGenerate_triggered();

    void on_actionOpenTest_triggered();

    void on_actionAbout_triggered();

    void on_actionBold_triggered();

    void on_actionChangeFontColor_triggered();

    void on_actionLeftAlign_triggered();

    void on_actionRightAlign_triggered();

    void on_actionCenter_triggered();

    void on_actionFullJustify_triggered();

    void on_actionItalic_triggered();

    void on_actionUnderline_triggered();

    void on_actionExportPDF_triggered();

    void on_actionPrint_triggered();

    void on_insertImage_clicked();

    void on_insertTitle_clicked();

    void on_actionPrintPreview_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_insertTable_clicked();

    void on_insetList_clicked();

    void on_insertHtml_clicked();

    void on_actionAdmin_triggered();

private:
    Ui::MainWindow *ui;

    TextEditor textEditor;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QToolBar *tb;
};

#endif // MAINWINDOW_H
