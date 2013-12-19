#include "mainwindow.h"
#include "login_dialog.h"
#include "testassistance.h"
#include "guide_app.h"

int main(int argc, char *argv[])
{
    GuideApp a(argc, argv);
    TestAssistance *pt = TestAssistance::get();

    //login
    LoginDialog *l = new LoginDialog;
    l->setModal(true);
    l->exec();
    delete l;
    if(!pt->isUserLogin())
        return TA_ERROR_USERNOTLOGIN;

    MainWindow w;
    w.show();

    return a.exec();
}
