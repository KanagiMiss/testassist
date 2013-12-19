#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login_dialog.h"
#include "guide_dialog.h"
#include "insert_table_dialog.h"
#include "insert_html_dialog.h"
#include "admin_dialog.h"
#include "testassistance.h"
#include <QTextDocument>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <QFont>
#include <QLabel>
#include <QTextCursor>
#include <QTextList>
#include <QColorDialog>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QPrinter>
#include <QByteArray>
#include <QTextCodec>
#include <QTextImageFormat>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QByteArray>
#include <QTextCodec>
#include <Qfile>
#include <QInputDialog>
#include <QDebug>

const char *TESTASSITANCE_VERSION = "alpha v1.2";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    TestAssistance *pt = TestAssistance::get();
    //main window init
    ui->setupUi(this);
    ui->toolBox->hide();
    if(pt->isUserAdmin()){
        ui->actionAdmin->setEnabled(true);
        ui->mainToolBar->addSeparator();
        ui->mainToolBar->addAction(ui->actionAdmin);
    }
    else
        ui->actionAdmin->setEnabled(false);
    setWindowTitle(tr("[*]自动出卷系统 ") + TESTASSITANCE_VERSION);
    setWindowIcon(QIcon(":/TestAssist.ico"));

    setupFileActions();
    setupEditActions();
    setupTextActions();

    //init editor
    initTextEditor();
    setWindowModified(false);
    setWelcomActions(false);

    //init status
    QLabel *plabel = new QLabel(ui->statusBar);
    plabel->setText("当前用户：" + pt->get()->getUserName() +
                    "    是否为管理员：" + (pt->get()->isUserAdmin()?"yes":"no"));
    ui->statusBar->addWidget(plabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}

void MainWindow::setWelcomActions(bool b){
    ui->actionSave->setEnabled(b);
    ui->actionFileSaveAs->setEnabled(b);
    ui->actionExportPDF->setEnabled(b);
    ui->actionUndo->setEnabled(b);
    ui->actionRedo->setEnabled(b);
    ui->actionCut->setEnabled(b);
    ui->actionCopy->setEnabled(b);
    ui->actionPaste->setEnabled(b);
    ui->actionBold->setEnabled(b);
    ui->actionItalic->setEnabled(b);
    ui->actionUnderline->setEnabled(b);
    ui->actionCenter->setEnabled(b);
    ui->actionLeftAlign->setEnabled(b);
    ui->actionRightAlign->setEnabled(b);
    ui->actionFullJustify->setEnabled(b);
    ui->actionChangeFontColor->setEnabled(b);
    ui->actionSetting->setEnabled(b);
    ui->actionPrint->setEnabled(b);
    ui->actionPrintPreview->setEnabled(b);
    ui->actionZoomIn->setEnabled(b);
    ui->actionZoomOut->setEnabled(b);
    ui->actionSearch->setEnabled(b);
    comboStyle->setEnabled(b);
    comboFont->setEnabled(b);
    comboSize->setEnabled(b);
}

void MainWindow::on_tabs_currentChanged(int index)
{
    if(index == -1)
        return;
    if(ui->tabs->currentWidget()->objectName() == "welcome"){
        ui->toolBox->hide();
        setWelcomActions(false);
    }
    else{
        ui->toolBox->show();
        setWelcomActions(true);

        //set enable
        QTextEdit *pe = textEditor.editor;
        if(pe != NULL){
            ui->actionSave->setEnabled(pe->document()->isModified());
            ui->actionUndo->setEnabled(pe->document()->isUndoAvailable());
            ui->actionRedo->setEnabled(pe->document()->isRedoAvailable());
            ui->actionCut->setEnabled(false);
            ui->actionCopy->setEnabled(false);
        }
    }
}

void MainWindow::initTextEditor()
{
    //create tab
    QWidget *tab = new QWidget(this);
    QString objName = QString("tab"),
            fileName = "";
    tab->setObjectName(objName);
    //create layout
    QHBoxLayout *layout = new QHBoxLayout(tab);
    layout->setSpacing(0);
    layout->setContentsMargins(11, 11, 11, 11);
    layout->setObjectName(QString("horizontalLayout"));
    layout->setContentsMargins(0, 0, 0, 0);

    //create textedit
    QTextEdit *edit = new QTextEdit(tab);
    edit->setObjectName(QString("textEdit"));

    layout->addWidget(edit);
    ui->tabs->addWidget(tab);

    //slot and signal
    //font change signal
    connect(edit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(edit, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    //modification
    connect(edit->document(), SIGNAL(modificationChanged(bool)),
            ui->actionSave, SLOT(setEnabled(bool)));
    connect(edit->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(edit->document(), SIGNAL(undoAvailable(bool)),
            ui->actionUndo, SLOT(setEnabled(bool)));
    connect(edit->document(), SIGNAL(redoAvailable(bool)),
            ui->actionRedo, SLOT(setEnabled(bool)));

    //redo undo
    connect(ui->actionUndo, SIGNAL(triggered()), edit, SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), edit, SLOT(redo()));

    //cut copy paste
    connect(ui->actionCut, SIGNAL(triggered()), edit, SLOT(cut()));
    connect(ui->actionCopy, SIGNAL(triggered()), edit, SLOT(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), edit, SLOT(paste()));

    connect(edit, SIGNAL(copyAvailable(bool)), ui->actionCut, SLOT(setEnabled(bool)));
    connect(edit, SIGNAL(copyAvailable(bool)), ui->actionCopy, SLOT(setEnabled(bool)));

    textEditor.tab = tab;
    textEditor.editor = edit;
    textEditor.layout = layout;
    textEditor.objName = objName;
    textEditor.fileName = fileName;
}

bool MainWindow::loadFile(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        textEditor.editor->setHtml(str);
    } else {
        str = QString::fromLocal8Bit(data);
        textEditor.editor->setPlainText(str);
    }

    setCurrentFileName(f);
    return true;
}

void MainWindow::insertQuestionTypeTitle(const QString &title)
{
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    cursor.insertText(tr("\n"));
    pEdit->setTextCursor(cursor);

    cursor.insertText(title+" ");
    pEdit->setTextCursor(cursor);

    //set font of title and info
    QFont font = pEdit->font();
    QTextCharFormat fmt;
    font.setBold(true);
    font.setPointSize(13);
    fmt.setFont(font);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(fmt);

    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText("\n");
    pEdit->setTextCursor(cursor);

    //set font of questions
    cursor.insertText(" ");
    font.setBold(false);
    font.setPointSize(10);
    fmt.setFont(font);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(fmt);

    cursor.select(QTextCursor::WordUnderCursor);
    cursor.deletePreviousChar();
    pEdit->setTextCursor(cursor);
}

QString MainWindow::digitToChinese(int d)
{
    switch(d){
    case 0:return tr("零");
    case 1:return tr("一");
    case 2:return tr("二");
    case 3:return tr("三");
    case 4:return tr("四");
    case 5:return tr("五");
    case 6:return tr("六");
    case 7:return tr("七");
    case 8:return tr("八");
    case 9:return tr("九");
    default:return tr("");
    }
}

void MainWindow::formatQuestions(const PACKEDQUESTIONS &qset)
{

    TestAssistance *pt = TestAssistance::get();
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    //set alignment center
    pEdit->setAlignment(Qt::AlignHCenter);
    groupCheck(0);

    //set font of title and info
    QFont font = pEdit->font();
    QTextCharFormat fmt;
    font.setBold(true);
    font.setPointSize(14);
    fmt.setFont(font);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(fmt);

    //insert school name
    cursor.insertText(tr("%1\n").arg(pt->getPaperInfo().strSchool));

    //insert title
    cursor.insertText(tr("“%1”专业（学院） 2011—2012学年第一学期\n").arg(pt->getPaperInfo().major));


    //insert paper info
    cursor.insertText(tr("《%1》期末考试试卷（%2卷） 考核形式（%3卷）\n").arg(pt->getPaperInfo().strCourse)
                      .arg(pt->getPaperInfo().strPaperType).arg(((pt->getPaperInfo().bOpenOrClose)?"开":"闭")));

    //insert author
    cursor.insertText(tr("作者:%1\n").arg(pt->getPaperInfo().strAuthor));

    pEdit->setTextCursor(cursor);

    //set alignment left
    pEdit->setAlignment(Qt::AlignLeft);
    groupCheck(2);

    //get judge
    QString text;
    int type = 0;

    const std::vector<QUESTION> &j = qset.judge;
    if(!j.empty()){
        insertQuestionTypeTitle(tr("%1.判断题").arg(digitToChinese(type+1)));
        type++;

        for(size_t i = 0; i != j.size(); ++i){
            cursor.insertText(tr("%1.").arg(i+1) + j[i].txt + "\n");
            if(!j[i].imgBytes.isEmpty()){
                QImage img;
                img.loadFromData(j[i].imgBytes, "PNG");
                cursor.insertImage(img);
                cursor.insertText("\n\n");
            }
            pEdit->setTextCursor(cursor);
        }
    }

    //get choice
    text.clear();
    const std::vector<QUESTION> &c = qset.choice;
    if(!c.empty()){
        insertQuestionTypeTitle(tr("%1.选择题").arg(digitToChinese(type+1)));
        type++;

        for(size_t i = 0; i != c.size(); ++i){
            cursor.insertText(tr("%1.").arg(i+1) + c[i].txt + "\n");
            if(!c[i].imgBytes.isEmpty()){
                QImage img;
                img.loadFromData(c[i].imgBytes, "PNG");
                cursor.insertImage(img);
                cursor.insertText("\n\n");
            }
            else
                cursor.insertText("\n");
            pEdit->setTextCursor(cursor);
        }
    }

    //get fill
    text.clear();
    const std::vector<QUESTION> &f = qset.fill;
    if(!f.empty()){
        insertQuestionTypeTitle(tr("%1.填空题").arg(digitToChinese(type+1)));
        type++;

        for(size_t i = 0; i != f.size(); ++i){
            cursor.insertText(tr("%1.").arg(i+1) + f[i].txt + "\n");
            if(!f[i].imgBytes.isEmpty()){
                QImage img;
                img.loadFromData(f[i].imgBytes, "PNG");
                cursor.insertImage(img);
                cursor.insertText("\n\n");
            }
            pEdit->setTextCursor(cursor);
        }
    }

    //get calc
    text.clear();
    const std::vector<QUESTION> &l = qset.calc;
    if(!l.empty()){
        insertQuestionTypeTitle(tr("%1.计算题").arg(digitToChinese(type+1)));
        type++;

        for(size_t i = 0; i != l.size(); ++i){
            cursor.insertText(tr("%1.").arg(i+1) + l[i].txt + "\n");
            if(!l[i].imgBytes.isEmpty()){
                QImage img;
                img.loadFromData(l[i].imgBytes, "PNG");
                cursor.insertImage(img);
                cursor.insertText("\n\n");
            }
            pEdit->setTextCursor(cursor);
        }
    }

    //get ans
    text.clear();
    const std::vector<QUESTION> &a = qset.ans;
    if(!a.empty()){
        insertQuestionTypeTitle(tr("%1.问答题").arg(digitToChinese(type+1)));
        type++;

        for(size_t i = 0; i != a.size(); ++i){
            cursor.insertText(tr("%1.").arg(i+1) + a[i].txt + "\n");
            if(!a[i].imgBytes.isEmpty()){
                QImage img;
                img.loadFromData(a[i].imgBytes, "PNG");
                cursor.insertImage(img);
                cursor.insertText("\n\n");
            }
            else
                cursor.insertText("\n");
            pEdit->setTextCursor(cursor);
        }
    }
}

void MainWindow::on_actionFileNew_triggered()
{
    if(!maybeSave())
        return;

    //execute the guidance dialog
    GuideDialog dlg(this);
    if(dlg.exec() != QDialog::Accepted) return;

    switch(dlg.paper_type){
    case GuideDialog::EMPTY:
        textEditor.editor->clear();
        textEditor.fileName = "";
        setWindowTitle(tr("[*]untitle - ") + "自动出卷系统 " + TESTASSITANCE_VERSION);
        break;
    case GuideDialog::OUTLINE:
    {
        textEditor.editor->clear();
        loadFile(tr(":/Documents/test_paper_example.html"));
        textEditor.fileName = "";
        setWindowTitle(tr("[*]untitle - ") + "自动出卷系统 " + TESTASSITANCE_VERSION);
        //set modified
        textEditor.editor->insertPlainText(" ");
        textEditor.editor->textCursor().deletePreviousChar();
        break;
    }
    case GuideDialog::AUTO:
    {
        textEditor.editor->clear();
        textEditor.fileName = "";
        TestAssistance *pt = TestAssistance::get();
        formatQuestions(pt->getQuestions());
        setWindowTitle(tr("[*]untitle - ") + "自动出卷系统 " + TESTASSITANCE_VERSION);
        break;
    }
    }

    ui->tabs->setCurrentWidget(textEditor.tab);
}

void MainWindow::setupFileActions()
{
    ui->actionOpenTest->setShortcut(QKeySequence::Open);
    ui->actionFileNew->setShortcut(QKeySequence::New);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionFileSaveAs->setShortcut(QKeySequence::SaveAs);
}

void MainWindow::setupEditActions()
{
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionPaste->setShortcut(QKeySequence::Paste);
}

void MainWindow::setupTextActions()
{
    //set bold
    QFont bold;
    bold.setBold(true);
    ui->actionBold->setFont(bold);
    //set italic
    QFont italic;
    italic.setItalic(true);
    ui->actionItalic->setFont(italic);
    //set underline
    QFont underline;
    underline.setUnderline(true);
    ui->actionUnderline->setFont(underline);

    //color
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    ui->actionChangeFontColor->setIcon(pix);

    //font format
    tb = new QToolBar(this);
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("Format Actions"));
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
                                                                   .pointSize())));

    //add action bold,italic,underline
    tb->addSeparator();
    tb->addAction(ui->actionBold);
    tb->addAction(ui->actionItalic);
    tb->addAction(ui->actionUnderline);
}

void MainWindow::on_actionSave_triggered()
{
    fileSave();
}

void MainWindow::on_actionFileSaveAs_triggered()
{
    fileSaveAs();
}

void MainWindow::on_actionSearch_triggered()
{

}

void MainWindow::on_actionSetting_triggered()
{

}

void MainWindow::on_actionHelp_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionGenerate_triggered()
{

}

void MainWindow::on_actionOpenTest_triggered()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("打开文件"),
                                              QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty()){
        loadFile(fn);
        ui->tabs->setCurrentWidget(textEditor.tab);
        setWindowTitle(tr("[*]%1 - ").arg(textEditor.fileName) + "自动出卷系统 " + TESTASSITANCE_VERSION);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("关于 自动出卷系统 %1").arg(QString(TESTASSITANCE_VERSION)),
                             tr("本软件基于Qt 5.1.1开发。\n注意：本软件仅用于学术交流，禁止用于商业用途。\n\n")
                                + "开发者：孟周宇、卢倚霞、陈鑫、石文峰、李骞\n");
}

void MainWindow::on_actionBold_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    pEdit->mergeCurrentCharFormat(format);
}

void MainWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void MainWindow::cursorPositionChanged()
{
    QTextEdit *pEdit = textEditor.editor;
    alignmentChanged(pEdit->alignment());
}

void MainWindow::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    ui->actionBold->setChecked(f.bold());
    ui->actionItalic->setChecked(f.italic());
    ui->actionUnderline->setChecked(f.underline());
}

void MainWindow::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->actionChangeFontColor->setIcon(pix);
}

void MainWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        groupCheck(2);
    else if (a & Qt::AlignHCenter)
        groupCheck(0);
    else if (a & Qt::AlignRight)
        groupCheck(1);
    else if (a & Qt::AlignJustify)
        groupCheck(3);
}

void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::textStyle(int styleIndex)
{
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void MainWindow::on_actionChangeFontColor_triggered()
{
    QTextEdit *pEdit = textEditor.editor;
    QColor col = QColorDialog::getColor(pEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void MainWindow::groupCheck(int a)
{
    ui->actionCenter->setChecked(a==0);
    ui->actionRightAlign->setChecked(a==1);
    ui->actionLeftAlign->setChecked(a==2);
    ui->actionFullJustify->setChecked(a==3);
}

void MainWindow::on_actionLeftAlign_triggered()
{
    QTextEdit *pEdit = textEditor.editor;
    pEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    groupCheck(2);
}

void MainWindow::on_actionRightAlign_triggered()
{
    QTextEdit *pEdit = textEditor.editor;
    pEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    groupCheck(1);
}

void MainWindow::on_actionCenter_triggered()
{
    QTextEdit *pEdit = textEditor.editor;
    pEdit->setAlignment(Qt::AlignHCenter);
    groupCheck(0);
}

void MainWindow::on_actionFullJustify_triggered()
{
    QTextEdit *pEdit = textEditor.editor;
    pEdit->setAlignment(Qt::AlignJustify);
    groupCheck(3);
}

bool MainWindow::fileSave()
{
    QTextEdit *pEdit = textEditor.editor;
    QString fileName = textEditor.fileName;
    if (fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(pEdit->document());
    if (success)
        pEdit->document()->setModified(false);
    return success;
}

void MainWindow::setCurrentFileName(const QString &fn)
{
    textEditor.fileName = fn;
}

bool MainWindow::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("另存为"), QString(),
                                              tr("ODF files (*.odt);;HTML-Files "
                                                 "(*.htm *.html);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    if (!(fn.endsWith(".odt", Qt::CaseInsensitive)
          || fn.endsWith(".htm", Qt::CaseInsensitive)
          || fn.endsWith(".html", Qt::CaseInsensitive))) {
        fn += ".odt"; // default
    }
    textEditor.fileName = fn;
    setWindowTitle(tr("[*]%1 - ").arg(fn) + "自动出卷系统 " + TESTASSITANCE_VERSION);
    return fileSave();
}

bool MainWindow::maybeSave()
{
    QTextEdit *pEdit = textEditor.editor;
    if(pEdit == NULL)
        return true;
    if (!pEdit->document()->isModified())
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("自动出卷系统"),
                               tr("文档%1已经被修改。\n"
                                  "想要保存文档内容吗？").arg(
                                   ""),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void MainWindow::on_actionItalic_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_actionUnderline_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_actionExportPDF_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出为pdf",
                                                    QString(), "*.pdf");
    QTextEdit *pEdit = textEditor.editor;
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        pEdit->document()->print(&printer);
    }
}

void MainWindow::printPreview(QPrinter *printer)
{
    textEditor.editor->print(printer);
}

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEditor.editor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("打印试卷"));
    if (dlg->exec() == QDialog::Accepted)
        textEditor.editor->print(&printer);
    delete dlg;
}


void MainWindow::on_actionPrintPreview_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::on_insertImage_clicked()
{
    //get select image
    QString fn = QFileDialog::getOpenFileName(this, tr("插入图像"),
                                              QString(),
                                              tr("Joint Photographic Experts Group (*.jpg *.jpeg);;") +
                                              "Portable Network Graphics (*.png);;" +
                                              "Windows Bitmap (*.bmp);;" +
                                              "GIF (*.gif);;" +
                                              "Portable Bitmap (*.pbm);;" +
                                              "Portable Graymap (*.pgm);;" +
                                              "Portable Pixmap (*.ppm);;" +
                                              "X11 Bitmap (*.xbm);;" +
                                              "X11 Pixmap (*.xpm);;" +
                                              "Scalable Vector Graphics (*.svg);;" +
                                              "All Files (*)");

    if (fn.isEmpty())
        return;
    if (!(fn.endsWith(".jpg", Qt::CaseInsensitive)
          || fn.endsWith(".jpeg", Qt::CaseInsensitive)
          || fn.endsWith(".png", Qt::CaseInsensitive)
          || fn.endsWith(".gif", Qt::CaseInsensitive)
          || fn.endsWith(".pbm", Qt::CaseInsensitive)
          || fn.endsWith(".pgm", Qt::CaseInsensitive)
          || fn.endsWith(".ppm", Qt::CaseInsensitive)
          || fn.endsWith(".xbm", Qt::CaseInsensitive)
          || fn.endsWith(".xpm", Qt::CaseInsensitive)
          || fn.endsWith(".svg", Qt::CaseInsensitive))) {
        QMessageBox::warning(this, tr("自动出卷系统"), tr("不支持的文件格式。"));
        return;
    }

    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setName(fn);
    //pEdit->setText();
    cursor.insertImage(imageFormat);
}

void MainWindow::on_insertTitle_clicked()
{
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    //set alignment center
    pEdit->setAlignment(Qt::AlignHCenter);
    groupCheck(0);

    //insert title
    cursor.insertText("“    ”学院 2011—2012学年第一学期\n");

    //insert paper info
    cursor.insertText("《           》期末考试试卷（  卷） 考核形式（  卷）\n");

    //set alignment left
    pEdit->setAlignment(Qt::AlignLeft);
    groupCheck(2);

    QFont font = pEdit->font();
    font.setBold(true);
    font.setPointSize(16);
    pEdit->setFont(font);
    pEdit->setTextCursor(cursor);
    pEdit->setFocus();
}


void MainWindow::on_actionZoomIn_triggered()
{
    textEditor.editor->zoomIn(2);
}

void MainWindow::on_actionZoomOut_triggered()
{
    textEditor.editor->zoomOut(2);
}

void MainWindow::on_insertTable_clicked()
{
    InsertTableDialog dlg;
    if(dlg.exec() != QDialog::Accepted)
        return;

    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();
    QTextTableFormat format;
    format.setCellSpacing(0);
    format.setCellPadding(5);
    cursor.insertTable(dlg.row(), dlg.column(), format);
    pEdit->setFocus();
}

void MainWindow::on_insetList_clicked()
{
    QInputDialog dlg;
    QStringList items;
    items << tr("○..., ○..., ○...")
          << tr("●..., ●..., ●...")
          << tr("■..., ■..., ■...")
          << tr("1..., 2..., 3...")
          << tr("a..., b..., c...")
          << tr("A..., B..., C...")
          << tr("i..., ii..., iii...")
          << tr("I..., II..., III...");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("请选择列表样式"),
                                         tr("列表前缀:"), items, 0, false, &ok);
    if (!ok || item.isEmpty())
        return;

    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    QTextListFormat listFormat;
    if(item == tr("○..., ○..., ○..."))
        listFormat.setStyle(QTextListFormat::ListCircle);
    else if(item == tr("●..., ●..., ●..."))
        listFormat.setStyle(QTextListFormat::ListDisc);
    else if(item == tr("■..., ■..., ■..."))
        listFormat.setStyle(QTextListFormat::ListSquare);
    else if(item == tr("1..., 2..., 3..."))
        listFormat.setStyle(QTextListFormat::ListDecimal);
    else if(item == tr("a..., b..., c..."))
        listFormat.setStyle(QTextListFormat::ListLowerAlpha);
    else if(item == tr("A..., B..., C..."))
        listFormat.setStyle(QTextListFormat::ListUpperAlpha);
    else if(item == tr("i..., ii..., iii..."))
        listFormat.setStyle(QTextListFormat::ListLowerRoman);
    else
        listFormat.setStyle(QTextListFormat::ListUpperRoman);

    cursor.insertList(listFormat);
    pEdit->setFocus();
}

void MainWindow::on_insertHtml_clicked()
{
    QTextEdit *pEdit = textEditor.editor;
    QTextCursor cursor = pEdit->textCursor();

    InsertHtmlDialog dlg(this);
    if(dlg.exec() != QDialog::Accepted)
        return;
    cursor.insertHtml(dlg.htmlText());
}

void MainWindow::on_actionAdmin_triggered()
{
    AdminDialog dlg(this);
    dlg.exec();
}
