#include "guide_app.h"
#include "testassistance.h"

GuideApp::GuideApp(int argc, char *argv[]):
    QApplication(argc, argv)
{
    TestAssistance::init();
}

GuideApp::~GuideApp()
{
    TestAssistance::release();
}
