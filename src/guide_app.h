#ifndef GUIDEAPP_H
#define GUIDEAPP_H

#include <QApplication>

class GuideApp : public QApplication
{
    Q_OBJECT
public:
    explicit GuideApp(int argc, char *argv[]);
    ~GuideApp();

signals:

public slots:

};

#endif // GUIDEAPP_H
