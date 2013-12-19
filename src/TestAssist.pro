#-------------------------------------------------
#
# Project created by QtCreator 2013-12-06T22:37:36
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestAssist
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    testassistance.cpp \
    guide_app.cpp \
    insert_html_dialog.cpp \
    insert_table_dialog.cpp \
    login_dialog.cpp \
    guide_dialog.cpp \
    admin_dialog.cpp

HEADERS  += mainwindow.h \
    testassistance.h \
    guide_app.h \
    guide_dialog.h \
    insert_html_dialog.h \
    insert_table_dialog.h \
    login_dialog.h \
    admin_dialog.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    guidedialog.ui \
    inserttabledialog.ui \
    inserthtmldialog.ui \
    admindialog.ui

RESOURCES += \
    resource.qrc

OTHER_FILES +=

RC_FILE = TestAssist.rc
